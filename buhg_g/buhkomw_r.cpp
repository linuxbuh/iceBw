/*$Id: buhkomw_r.c,v 1.33 2014/07/31 07:08:24 sasa Exp $*/
/*23.05.2016	07.06.2005	Белых А.И. 	buhkomw_r.c
Расчёт прогноза реализации 
*/
#include <errno.h>
#include "buhg_g.h"
#include "buhkomw.h"

class buhkomw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class buhkomw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  buhkomw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhkomw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhkomw_r_data *data);
gint buhkomw_r1(class buhkomw_r_data *data);
void  buhkomw_r_v_knopka(GtkWidget *widget,class buhkomw_r_data *data);

int prov_kom(class iceb_u_spisok *koment,char *row,int kol_koment);
void buhkom_it(int metka_i,double deb,double kre,FILE *ff);
void buhkom_it_dk(int metka_i,double deb,FILE *ff);
void buhkom_sh_kre(char *shet,char *naim,FILE *ff);
void buhkom_sh(const char *shet,const char *naim,FILE *ff);
void buhkom_sh_deb(const char *shet,const char *naim,FILE *ff);
void buhkom_sh_kre(const char *shet,const char *naim,FILE *ff);
void buhkomw_ri(double dbt,double krt,int metka,FILE *ff1);
void	buhkomw_rs(FILE *ff1);

extern SQL_baza bd;



int buhkomw_r(class buhkomw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class buhkomw_r_data data;
int gor=0;
int ver=0;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать отчёт по комментариям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(buhkomw_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Распечатать отчёт по комментариям"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,data.window));
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(buhkomw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)buhkomw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhkomw_r_v_knopka(GtkWidget *widget,class buhkomw_r_data *data)
{
//printf("buhkomw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   buhkomw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhkomw_r_data *data)
{
// printf("buhkomw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}

/******************************************/
/******************************************/

gint buhkomw_r1(class buhkomw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

class iceb_u_spisok koment;
class iceb_u_str bros("");
int nomer=0;
float kolstr1=0.;
class iceb_u_spisok sh_shk_kom;
double deb,kre;
class iceb_u_double debet;
class iceb_u_double kredit;
SQL_str row,rowtmp;
SQLCURSOR cur,curtmp; 
char zaprostmp[512];
class iceb_tmptab tabtmp;
const char *imatmptab={"buhkomw"};
int kolstrtmp=0;
short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
sprintf(strsql,"%d.%d.%d => %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
iceb_printw(strsql,data->buffer,data->view);

SQL_str row_alx;
class SQLCURSOR cur_alx;
const char *imaf_alx={"buhkom.alx"};
int kolstr=0;
/*Открываем файл настройки индексации*/
sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдены настройки"),imaf_alx);
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],&bros,1,'|') != 0) 
   continue;
  koment.plus(bros.ravno());
 }
 

int kol_koment=koment.kolih();
if(kol_koment == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной настройки !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"select sh,shk,deb,kre,komen from Prov \
where val=0 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d' and komen <> ''",gn,mn,dn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено проводок !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
while(cur.read_cursor(&row) != 0)
 {
  
  iceb_pbar(data->bar,kolstr,++kolstr1);
  
  if(iceb_u_proverka(data->rk->shet.ravno(),row[0],0,0) != 0)
    continue;
  
  if((nomer=prov_kom(&koment,row[4],kol_koment)) < 0)
   continue;

  deb=atof(row[2]);
  kre=atof(row[3]);
  
  sprintf(strsql,"%s|%s|%s",row[0],koment.ravno(nomer),row[1]);
  if((nomer=sh_shk_kom.find(strsql)) < 0)
    sh_shk_kom.plus(strsql);
  debet.plus(deb,nomer);
  kredit.plus(kre,nomer);    
 }

int kol_shet=sh_shk_kom.kolih();
if(kol_shet == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной проводки с указанными комментариями!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(32) not null,\
kom char(112) not null,\
shk char(32) not null,\
deb double(12,2) not null,\
kre double(12,2) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  
class iceb_u_str sh("");
class iceb_u_str kom("");
class iceb_u_str shk("");
for(int i=0; i < kol_shet ; i++)
 {
  iceb_u_pole(sh_shk_kom.ravno(i),&sh,1,'|');
  iceb_u_pole(sh_shk_kom.ravno(i),&kom,2,'|');
  iceb_u_pole(sh_shk_kom.ravno(i),&shk,3,'|');
  
  sprintf(strsql,"insert into %s values ('%s','%s','%s',%.2f,%.2f)",
  imatmptab,
  sh.ravno(),
  kom.ravno_filtr(),
  shk.ravno(),
  debet.ravno(i),
  kredit.ravno(i));
  
  iceb_sql_zapis(strsql,1,0,data->window);    

 }

sprintf(strsql,"select * from %s order by sh asc,kom asc",imatmptab);
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_str shet("");
class iceb_u_str shet_z("");
class iceb_u_str shetk("");
class iceb_u_str komentar("");
class iceb_u_str komentar_z("");
double ideb=0.,ikre=0.;
double idebk=0.,ikrek=0.;
class iceb_u_str naim("");

char imaf[64];
sprintf(imaf,"kom%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Свод по комментариям (дебет и кредит)"));

iceb_u_zagolov(gettext("Отчёт покомментариям"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff);

if(data->rk->shet.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());


//распечатываем дебет и кредит
idebk=ikrek=0.;
ideb=ikre=0.;
while(curtmp.read_cursor(&rowtmp) != 0)
 {

  shet.new_plus(rowtmp[0]);
  komentar.new_plus(rowtmp[1]);
  shetk.new_plus(rowtmp[2]);
  deb=atof(rowtmp[3]);
  kre=atof(rowtmp[4]);

  if(iceb_u_SRAV(komentar_z.ravno(),komentar.ravno(),0) != 0)
   {
    if(komentar_z.ravno()[0] != '\0')
      buhkom_it(0,idebk,ikrek,ff);
    idebk=ikrek=0.;
    komentar_z.new_plus(komentar.ravno());
   }

  if(iceb_u_SRAV(shet.ravno(),shet_z.ravno(),0) != 0)
   {
    if(shet_z.ravno()[0] != '\0')
     {
      if(idebk != 0. || ikrek != 0.)
       {
        buhkom_it(0,idebk,ikrek,ff);
        idebk=ikrek=0.;
       }
      buhkom_it(1,ideb,ikre,ff);
     }
    naim.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);

    buhkom_sh(shet.ravno(),naim.ravno(),ff);
     
    ideb=ikre=0.;
    shet_z.new_plus(shet.ravno());
   }

  fprintf(ff,"%-*s %-*s %10.2f %10.2f\n",
  iceb_u_kolbait(10,komentar.ravno()),komentar.ravno(),
  iceb_u_kolbait(7,shetk.ravno()),shetk.ravno(),
  deb,kre);
  
  ideb+=deb;
  ikre+=kre;
  idebk+=deb;
  ikrek+=kre;

 }
buhkom_it(0,idebk,ikrek,ff);
buhkom_it(1,ideb,ikre,ff);

iceb_podpis(ff,data->window);
fclose(ff);


char imaf_deb[64];
sprintf(imaf_deb,"komd%d.lst",getpid());
if((ff = fopen(imaf_deb,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_deb,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imaf_deb);
data->rk->naimf.plus(gettext("Свод покомментариям (дебет)"));
iceb_u_zagolov(gettext("Отчёт покомментариям"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff);

if(data->rk->shet.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());


shet_z.new_plus("");
komentar_z.new_plus("");
//распечатываем дебет
curtmp.poz_cursor(0);
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  deb=atof(rowtmp[3]);
  if(deb == 0.)
   continue;

  shet.new_plus(rowtmp[0]);
  komentar.new_plus(rowtmp[1]);
  shetk.new_plus(rowtmp[2]);
    
  if(iceb_u_SRAV(komentar_z.ravno(),komentar.ravno(),0) != 0)
   {
    if(komentar_z.ravno()[0] != '\0')
      buhkom_it_dk(0,idebk,ff);
    idebk=ikrek=0.;
    komentar_z.new_plus(komentar.ravno());
   }

  if(iceb_u_SRAV(shet.ravno(),shet_z.ravno(),0) != 0)
   {
    if(shet_z.ravno()[0] != '\0')
     {
      if(idebk != 0. || ikrek != 0.)
       {
        buhkom_it_dk(0,idebk,ff);
        idebk=ikrek=0.;
       }
      buhkom_it_dk(1,ideb,ff);
     }
    naim.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);

    buhkom_sh_deb(shet.ravno(),naim.ravno(),ff);
     
    ideb=ikre=0.;
    shet_z.new_plus(shet.ravno());
   }

  fprintf(ff,"%-*s %-*s %10.2f\n",
  iceb_u_kolbait(10,komentar.ravno()),komentar.ravno(),
  iceb_u_kolbait(7,shetk.ravno()),shetk.ravno(),deb);
  
  ideb+=deb;
  idebk+=deb;

 }
buhkom_it_dk(0,idebk,ff);
buhkom_it_dk(1,ideb,ff);


iceb_podpis(ff,data->window);
fclose(ff);


char imaf_kre[64];
sprintf(imaf_kre,"komk%d.lst",getpid());
if((ff = fopen(imaf_kre,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_kre,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imaf_kre);
data->rk->naimf.plus(gettext("Свод покомментариям (кредит)"));
iceb_u_zagolov(gettext("Отчёт покомментариям"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff);

if(data->rk->shet.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());


shet_z.new_plus("");
komentar_z.new_plus("");
//распечатываем кредит
curtmp.poz_cursor(0);
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  kre=atof(rowtmp[4]);
  if(kre == 0.)
   continue;

  shet.new_plus(rowtmp[0]);
  komentar.new_plus(rowtmp[1]);
  shetk.new_plus(rowtmp[2]);
    
  if(iceb_u_SRAV(komentar_z.ravno(),komentar.ravno(),0) != 0)
   {
    if(komentar_z.ravno()[0] != '\0')
      buhkom_it_dk(0,ikrek,ff);
    ikrek=0.;
    komentar_z.new_plus(komentar.ravno());
   }

  if(iceb_u_SRAV(shet.ravno(),shet_z.ravno(),0) != 0)
   {
    if(shet_z.ravno()[0] != '\0')
     {
      if(ikrek != 0.)
       {
        buhkom_it_dk(0,ikrek,ff);
        ikrek=0.;
       }
      buhkom_it_dk(1,ikre,ff);
     }
    naim.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);

    buhkom_sh_kre(shet.ravno(),naim.ravno(),ff);
     
    ikre=0.;
    shet_z.new_plus(shet.ravno());
   }

  fprintf(ff,"%-*s %-*s %10.2f\n",
  iceb_u_kolbait(10,komentar.ravno()),komentar.ravno(),
  iceb_u_kolbait(7,shetk.ravno()),shetk.ravno(),kre);
  
  ikre+=kre;
  ikrek+=kre;

 }
buhkom_it_dk(0,ikrek,ff);
buhkom_it_dk(1,ikre,ff);


iceb_podpis(ff,data->window);
fclose(ff);




//Распечатываем проводки


char imaf_prov[64];
sprintf(imaf_prov,"komp%d.lst",getpid());
if((ff = fopen(imaf_prov,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prov,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imaf_prov);
data->rk->naimf.plus(gettext("Отчёт по комментариям"));
iceb_u_zagolov(gettext("Отчёт по комментариям"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff);

if(data->rk->shet.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());

short d,m,g;
class iceb_u_str naikon("");
SQL_str row1;
SQLCURSOR cur1; 
kolstr1=0;
ideb=ikre=0.;
shet_z.new_plus("");
komentar_z.new_plus("");
class iceb_u_str komentar_prov("");
curtmp.poz_cursor(0);

while(curtmp.read_cursor(&rowtmp) != 0)
 {
  iceb_pbar(data->bar,kol_shet,++kolstr1);
  
  shet.new_plus(rowtmp[0]);
  komentar.new_plus(rowtmp[1]);
  shetk.new_plus(rowtmp[2]);

  if(iceb_u_SRAV(komentar_z.ravno(),komentar.ravno(),0) != 0)
   {
    if(komentar_z.ravno()[0] != '\0')
      buhkomw_ri(idebk,ikrek,0,ff);
    idebk=ikrek=0.;
    komentar_z.new_plus(komentar.ravno());
   }

  if(iceb_u_SRAV(shet.ravno(),shet_z.ravno(),0) != 0)
   {
    if(shet_z.ravno()[0] != '\0')
     {
      if(idebk != 0. || ikrek != 0.)
       {
        buhkomw_ri(idebk,ikrek,0,ff);
        idebk=ikrek=0.;
       }
      buhkomw_ri(ideb,ikre,1,ff);
     }
    naim.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);

    fprintf(ff,"\n%s %s\n",shet.ravno(),naim.ravno());
    buhkomw_rs(ff);
     
    ideb=ikre=0.;
    shet_z.new_plus(shet.ravno());
   }
    
  sprintf(strsql,"select * from Prov \
where val=0 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d' and sh='%s' and shk='%s'  and komen <> ''",gn,mn,dn,gk,mk,dk,shet.ravno(),shetk.ravno());
  int kolstr;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  if(kolstr == 0)
   continue;

  while(cur.read_cursor(&row) != 0)
   {
    if(row[13][0] == '\0')
     continue;
    if(iceb_u_strstrm(row[13],komentar.ravno()) == 0)
      continue;
    iceb_u_rsdat(&d,&m,&g,row[1],2);
    deb=atof(row[9]);
    kre=atof(row[10]);
    ideb+=deb;
    ikre+=kre;
    idebk+=deb;
    ikrek+=kre;
    
    naikon.new_plus("");
    if(row[4][0] != '\0')
     {  
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
      row[4]);
      int i;
      if((i=sql_readkey(&bd,strsql,&row1,&cur1)) != 1)
       {
        printf("%02d.%02d.%d %-5s %-5s %-5s %-3s %-4s %10.2f %10.2f %s%-10.10s\n\
  Error %d\n",      
        d,m,g,row[2],row[3],row[4],row[5],row[6],
        deb,kre,row[8],row[13],i);

        sprintf(strsql,"%s %s",gettext("Не найден код контрагента"),row[4]);
        iceb_menu_soob(strsql,data->window);

       }
      else
       naikon.new_plus(row1[0]);
     }       
    sozkom(&komentar_prov,row[13],row[5],row[8]);

    fprintf(ff,"%02d.%02d.%4d %-*s %-*s %-*s %-3s%4s %-*s %10s",
    d,m,g,
    iceb_u_kolbait(6,row[2]),row[2],
    iceb_u_kolbait(6,row[3]),row[3],
    iceb_u_kolbait(5,row[4]),row[4],
    row[5],row[11],
    iceb_u_kolbait(10,row[6]),row[6],
    iceb_u_prnbr(deb));

    fprintf(ff," %10s %*s %-*s %-*s %s\n",
    iceb_u_prnbr(kre),
    iceb_u_kolbait(4,row[16]),row[16],
    iceb_u_kolbait(4,row[8]),row[8],
    iceb_u_kolbait(32,komentar_prov.ravno()),komentar_prov.ravno(),
    naikon.ravno());
   }   
 }
buhkomw_ri(idebk,ikrek,0,ff);
buhkomw_ri(ideb,ikre,1,ff);


iceb_podpis(ff,data->window);
fclose(ff);


for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
/**************************/
/*Проверкакомментария*/
/***************************/
//Возвращаем номеркомментария или -1 
int prov_kom(class iceb_u_spisok *koment,char *row,int kol_koment)
{
if(row[0] == '\0')
 return(-1);
for(int i=0; i < kol_koment;i++)
 {
//printw("проверяем %s | %s\n",row,koment->ravno(i));
  if(iceb_u_strstrm(row,koment->ravno(i)) != 0)
    return(i);
 }    
return(-1);

}
/***************************/
/*шапка*/
/**************************/

void buhkom_sh(const char *shet,const char *naim,FILE *ff)
{

fprintf(ff,"\n%s %s\n",shet,naim);
fprintf(ff,"\
-----------------------------------------\n");
fprintf(ff,gettext("\
Комментарий| Счёт  | Дебет    |  Кредит  |\n"));
fprintf(ff,"\
-----------------------------------------\n");

}
/***************************/
/*шапка*/
/**************************/

void buhkom_sh_deb(const char *shet,const char *naim,FILE *ff)
{

fprintf(ff,"\n%s %s\n",shet,naim);
fprintf(ff,"\
------------------------------\n");
fprintf(ff,gettext("\
Комментарий| Счёт  | Дебет    |\n"));
fprintf(ff,"\
------------------------------\n");

}
/***************************/
/*шапка*/
/**************************/

void buhkom_sh_kre(const char *shet,const char *naim,FILE *ff)
{

fprintf(ff,"\n%s %s\n",shet,naim);
fprintf(ff,"\
------------------------------\n");
fprintf(ff,gettext("\
Комментарий| Счёт  |  Кредит  |\n"));
fprintf(ff,"\
------------------------------\n");

}
/************************/
/*выдача итога*/
/*************************/
void buhkom_it(int metka_i,double deb,double kre,FILE *ff)
{
if(metka_i == 0)
  fprintf(ff,"%*s %10.2f %10.2f\n",
  iceb_u_kolbait(18,gettext("Итого")),gettext("Итого"),deb,kre);

if(metka_i == 1)
  fprintf(ff,"%*s %10.2f %10.2f\n",
  iceb_u_kolbait(18,gettext("Итого по счёту")),gettext("Итого по счёту"),deb,kre);
}

/************************/
/*выдача итога*/
/*************************/
void buhkom_it_dk(int metka_i,double deb,FILE *ff)
{
if(metka_i == 0)
  fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(18,gettext("Итого")),gettext("Итого"),deb);

if(metka_i == 1)
  fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(18,gettext("Итого по счёту")),gettext("Итого по счёту"),deb);
}
/********************************/
/*Распечатка итога*/
/********************************/

void buhkomw_ri(double dbt,double krt,int metka,FILE *ff1)
{
double	brr;

fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------------------------\n");

if(metka == 0)
  fprintf(ff1,"%*s:",iceb_u_kolbait(46,gettext("Итого по комментарию")),gettext("Итого по комментарию"));
if(metka == 1)
  fprintf(ff1,"%*s:",iceb_u_kolbait(46,gettext("Итого по счёту")),gettext("Итого по счёту"));

fprintf(ff1,"%13s",iceb_u_prnbr(dbt));
fprintf(ff1," %10s\n",iceb_u_prnbr(krt));

if(dbt >= krt)
 {
  brr=dbt-krt;
  fprintf(ff1,"\
%50s%10s\n"," ",iceb_u_prnbr(brr));
 }
else
 {
  brr=krt-dbt;
  fprintf(ff1,"\
%50s%10s %10s\n"," "," ",iceb_u_prnbr(brr));
 }
fprintf(ff1,"\n");

}
/*******/
/*Шапка*/
/*******/
void	buhkomw_rs(FILE *ff1)
{

fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,gettext("\
  Дата    | Счёт.|Сч.к-т|Конт.| Кто   |Номер док.|   Дебет  |  Кредит  |КЭКЗ|К.о.|      К о м е н т а р и й       |Наименование контрагента|\n"));

fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------------------------\n");

}
