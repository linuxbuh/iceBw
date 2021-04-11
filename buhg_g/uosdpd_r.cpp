/*$Id: uosdpd_r.c,v 1.17 2013/09/26 09:46:56 sasa Exp $*/
/*23.06.2020	13.01.2008	Белых А.И.	uosdpd_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "uosdpd.h"

class uosdpd_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class uosdpd_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  uosdpd_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosdpd_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosdpd_r_data *data);
gint uosdpd_r1(class uosdpd_r_data *data);
void  uosdpd_r_v_knopka(GtkWidget *widget,class uosdpd_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;




extern int      kol_strok_na_liste;

int uosdpd_r(class uosdpd_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class uosdpd_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать движение по документам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uosdpd_r_key_press),&data);

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

repl.plus(gettext("Распечатать движение по документам"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(), iceb_get_pnk("00",0,wpredok));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(uosdpd_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);

gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)uosdpd_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosdpd_r_v_knopka(GtkWidget *widget,class uosdpd_r_data *data)
{
if(data->kon_ras == 1) 
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosdpd_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosdpd_r_data *data)
{
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}
/**************/
/*Шапка файлов*/
/***************/
void	sapdvos(FILE *ff,const char *kodolz,const char *podrz,const char *kodop,
int vidop,const char *grupabuh,const char *grupanal,const char *shetu_p,const char *in_nom)
{


if(kodolz[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Мат.-ответственный"),kodolz);
if(podrz[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),podrz);
if(kodop[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Код операции"),kodop);
if(vidop != 0)
  fprintf(ff,"%s: %d\n",gettext("Документы"),vidop);
if(grupabuh[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Группа бухгалтерского учёта"),grupabuh);
if(grupanal[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Группа налогового учёта"),grupanal);
if(shetu_p[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Счёт учёта"),shetu_p);
if(in_nom[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Инвентарный номер"),in_nom);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Дата докум. |Инвент. |  Наименование      |  Налоговый учёт       |Бухгалтерский учёт     |Ном. |Код. |Подр-|Мат. |Группа|Группа|\n\
            | номер  |                    |Бал.стоим. |   Износ   |Бал.стоим. |   Износ   |докум|опер.|аздел|отвец|бух.уч|нал.уч|\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
}


/******************************************/
/******************************************/

gint uosdpd_r1(class uosdpd_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,rowtmp;
class SQLCURSOR cur,curtmp;

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");


int kolstr=0;
SQLCURSOR curr;
sprintf(strsql,"select datd,tipz,innom,nomd,podr,kodol,bs,iz,kodop,\
bsby,izby from Uosdok1 where datd >= '%d-%d-%d' and datd <= '%d-%d-%d' \
order by datd asc",
gn,mn,dn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_tmptab tabtmp;
const char *imatmptab={"uosdpd"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));
sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
hnaby char(24) not null,\
hna char(24) not null,\
datd DATE NOT NULL,\
vidopt char(8) not null,\
innom int not null,\
naim char(112) not null,\
bsn double(16,2) not null,\
izn double(16,2) not null,\
bsb double(16,2) not null,\
izb double(16,2) not null,\
nomd char(32) not null,\
kodop char(32) not null,\
podr int not null,\
kodol int not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  
/**************
char imafsort[64];
sprintf(imafsort,"dvos%d.tmp",getpid());
FILE *ffsort;
if((ffsort = fopen(imafsort,"w")) == NULL)
 {
  iceb_er_op_fil(imafsort,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
**************/
char imaf[64];
sprintf(imaf,"dvos%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(gettext("Распечатать движение по документам"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff);

sapdvos(ff,data->rk->mat_ot.ravno(),data->rk->podr.ravno(),data->rk->kod_op.ravno(),data->rk->prih_rash,data->rk->grup_bu.ravno(),data->rk->grup_nu.ravno(),data->rk->shetu.ravno(),data->rk->in_nom.ravno());

float kolstr1=0.;
short d,m,g;
int tipz=0;
char		vidopt[5];
double		bsn,izn,bsb,izb;
double		ibsn=0.,iizn=0.,ibsb=0.,iizb=0.;
double		ibsnp=0.,iiznp=0.,ibsbp=0.,iizbp=0.;
double		ibsnr=0.,iiznr=0.,ibsbr=0.,iizbr=0.;
double		ibsngr=0.,iizngr=0.,ibsbgr=0.,iizbgr=0.;
double		ibsngro=0.,iizngro=0.,ibsbgro=0.,iizbgro=0.;
class iceb_u_str naim;
SQL_str row1;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->podr.ravno(),row[4],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->mat_ot.ravno(),row[5],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kod_op.ravno(),row[8],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->in_nom.ravno(),row[2],0,0) != 0)
    continue;
   
  if(data->rk->prih_rash == '2')
    if(row[1][0] == '1')
     continue;

  if(data->rk->prih_rash == 1)
    if(row[1][0] == '2')
     continue;

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  class poiinpdw_data rekin;
  poiinpdw(atol(row[2]),m,g,&rekin,data->window);

  if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),1,0) != 0)
       continue;
  if(iceb_u_proverka(data->rk->grup_bu.ravno(),rekin.hnaby.ravno(),0,0) != 0)
       continue;
  if(iceb_u_proverka(data->rk->grup_nu.ravno(),rekin.hna.ravno(),0,0) != 0)
       continue;

  tipz=atoi(row[1]);
  if(tipz == 1)
   strcpy(vidopt,"+");
  if(tipz == 2)
   strcpy(vidopt,"-");
   
  bsn=atof(row[6]);  
  izn=atof(row[7]);  
  bsb=atof(row[9]);  
  izb=atof(row[10]);  
  naim.new_plus("");
  sprintf(strsql,"select naim from Uosin where innom=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    naim.new_plus(row1[0]);

  fprintf(ff,"%02d.%02d.%d %s %-8s %-*.*s %11.2f %11.2f %11.2f %11.2f \
%-*s %-*s %-5s %-5s %-*s %-*s\n",
  d,m,g,vidopt,row[2],
  iceb_u_kolbait(20,naim.ravno()),iceb_u_kolbait(20,naim.ravno()),naim.ravno(),
  bsn,izn,bsb,izb,
  iceb_u_kolbait(5,row[3]),row[3],
  iceb_u_kolbait(5,row[8]),row[8],
  row[4],row[5],
  iceb_u_kolbait(6,rekin.hnaby.ravno()),rekin.hnaby.ravno(),
  iceb_u_kolbait(6,rekin.hna.ravno()),rekin.hna.ravno());

  for(int nom=20; nom < iceb_u_strlen(naim.ravno()); nom+=20)
   {
    fprintf(ff,"%21s %-*.*s\n",
    "",
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim.ravno())),
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim.ravno())),
    iceb_u_adrsimv(nom,naim.ravno()));
   }

  sprintf(strsql,"insert into %s values ('%s','%s','%04d-%02d-%02d','%s',%s,'%s',%.2f,%.2f,%.2f,%.2f,'%s','%s',%s,%s)",
  imatmptab,
  rekin.hnaby.ravno(),rekin.hna.ravno(),g,m,d,vidopt,row[2],naim.ravno(),bsn,izn,bsb,izb,
  row[3],row[8],row[4],row[5]);

  iceb_sql_zapis(strsql,1,0,data->window);    


  ibsn+=bsn;
  iizn+=izn;
  ibsb+=bsb;
  iizb+=izb;
  if(tipz == 1)
   {
    ibsnp+=bsn;
    iiznp+=izn;
    ibsbp+=bsb;
    iizbp+=izb;
   }  
  if(tipz == 2)
   {
    ibsnr+=bsn;
    iiznr+=izn;
    ibsbr+=bsb;
    iizbr+=izb;
   }  
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(42,gettext("Итого")),gettext("Итого"),ibsn,iizn,ibsb,iizb);

fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnp,iiznp,ibsbp,iizbp);

fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnr,iiznr,ibsbr,iizbr);

fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);

sprintf(strsql,"%s :%11.2f %11.2f %11.2f %11.2f\n",gettext("Итого"),ibsn,iizn,ibsb,iizb);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"\n%s+:%11.2f %11.2f %11.2f %11.2f\n",gettext("Итого"),ibsnp,iiznp,ibsbp,iizbp);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"%s :%11.2f %11.2f %11.2f %11.2f\n",gettext("Итого"),ibsnr,iiznr,ibsbr,iizbr);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

iceb_podpis(ff,data->window);

fclose(ff);


sprintf(strsql,"select * from %s order by hnaby asc,datd asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafgrbuh[64];
sprintf(imafgrbuh,"dvosbh%d.lst",getpid());

if((ff = fopen(imafgrbuh,"w")) == NULL)
 {
  iceb_er_op_fil(imafgrbuh,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(gettext("Распечатать движение по документам"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff);

fprintf(ff,"%s\n",gettext("Сортировка по группам бухгалтерского учёта"));

sapdvos(ff,data->rk->mat_ot.ravno(),data->rk->podr.ravno(),data->rk->kod_op.ravno(),data->rk->prih_rash,data->rk->grup_bu.ravno(),data->rk->grup_nu.ravno(),data->rk->shetu.ravno(),data->rk->in_nom.ravno());
char grbuhz[64];
char grbuh[64];
char		grnal[64],grnalz[64];
char		invnom[64];
char 		nalbs[64];
char		naliz[64];
char 		buhbs[64];
char		buhiz[64];
char		nomd[64];
char		kodoper[64];
char		podr[64];
char		matot[64];
memset(grbuhz,'\0',sizeof(grbuhz));

double ibsnpo=0.,iiznpo=0.,ibsbpo=0.,iizbpo=0.;
double ibsnro=0.,iiznro=0.,ibsbro=0.,iizbro=0.;
ibsnp=iiznp=ibsbp=iizbp=0.;
ibsnr=iiznr=ibsbr=iizbr=0.;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
//  iceb_u_polen(stroka,grbuh,sizeof(grbuh),1,'|');
  strncpy(grbuh,rowtmp[0],sizeof(grbuh)-1);
  if(iceb_u_SRAV(grbuhz,grbuh,0) != 0)
   {
    if(grbuhz[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Итого по группе"),grbuhz);
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(42,strsql),strsql,ibsngr,iizngr,ibsbgr,iizbgr);
      
      ibsngro+=ibsngr;
      iizngro+=iizngr;
      ibsbgro+=ibsbgr;
      iizbgro+=iizbgr;
      
      fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnp,iiznp,ibsbp,iizbp);

      fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnr,iiznr,ibsbr,iizbr);

      fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n\n",
      iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);
     }
    strcpy(grbuhz,grbuh);
    ibsngr=iizngr=ibsbgr=iizbgr=0.;
    ibsnp=iiznp=ibsbp=iizbp=0.;
    ibsnr=iiznr=ibsbr=iizbr=0.;
   }
/************
  iceb_u_polen(stroka,grnal,sizeof(grnal),2,'|');
  iceb_u_polen(stroka,datadok,sizeof(datadok),3,'|');
  iceb_u_rsdat(&d,&m,&g,datadok,2);
  iceb_u_polen(stroka,vidopt,sizeof(vidopt),4,'|');
  iceb_u_polen(stroka,invnom,sizeof(invnom),5,'|');
  iceb_u_polen(stroka,&naim,6,'|');
  iceb_u_polen(stroka,nalbs,sizeof(nalbs),7,'|');
  iceb_u_polen(stroka,naliz,sizeof(naliz),8,'|');
  iceb_u_polen(stroka,buhbs,sizeof(buhbs),9,'|');
  iceb_u_polen(stroka,buhiz,sizeof(buhiz),10,'|');
  iceb_u_polen(stroka,nomd,sizeof(nomd),11,'|');
  iceb_u_polen(stroka,kodoper,sizeof(kodoper),12,'|');
  iceb_u_polen(stroka,podr,sizeof(podr),13,'|');
  iceb_u_polen(stroka,matot,sizeof(matot),14,'|');
***************/
  strncpy(grnal,rowtmp[1],sizeof(grbuh)-1);
  iceb_u_rsdat(&d,&m,&g,rowtmp[2],2);
  strncpy(vidopt,rowtmp[3],sizeof(vidopt)-1);
  strncpy(invnom,rowtmp[4],sizeof(invnom)-1);
  naim.new_plus(rowtmp[5]);
  strncpy(nalbs,rowtmp[6],sizeof(nalbs)-1);
  strncpy(naliz,rowtmp[7],sizeof(naliz)-1);
  strncpy(buhbs,rowtmp[8],sizeof(buhbs)-1);
  strncpy(buhiz,rowtmp[9],sizeof(buhiz)-1);
  strncpy(nomd,rowtmp[10],sizeof(nomd)-1);
  strncpy(kodoper,rowtmp[11],sizeof(kodoper)-1);
  strncpy(podr,rowtmp[12],sizeof(podr)-1);
  strncpy(matot,rowtmp[13],sizeof(matot)-1);

  fprintf(ff,"%02d.%02d.%d %s %-8s %-*.*s %11s %11s %11s %11s \
%-*s %-*s %-5s %-5s %-6s %-6s\n",
  d,m,g,vidopt,invnom,
  iceb_u_kolbait(20,naim.ravno()),iceb_u_kolbait(20,naim.ravno()),naim.ravno(),
  nalbs,naliz,buhbs,buhiz,
  iceb_u_kolbait(5,nomd),nomd,
  iceb_u_kolbait(5,kodoper),kodoper,
  podr,matot,
  grbuh,
  grnal);

  for(int nom=20; nom < iceb_u_strlen(naim.ravno()); nom+=20)
   {
    fprintf(ff,"%21s %-*.*s\n",
    "",
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim.ravno())),
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim.ravno())),
    iceb_u_adrsimv(nom,naim.ravno()));
   }
  
  ibsngr+=atof(nalbs);
  iizngr+=atof(naliz);
  ibsbgr+=atof(buhbs);
  iizbgr+=atof(buhiz);
  if(vidopt[0] == '+')
   {
    ibsnp+=atof(nalbs);
    iiznp+=atof(naliz);
    ibsbp+=atof(buhbs);
    iizbp+=atof(buhiz);

    ibsnpo+=atof(nalbs);
    iiznpo+=atof(naliz);
    ibsbpo+=atof(buhbs);
    iizbpo+=atof(buhiz);
   }  
  if(vidopt[0] == '-')
   {
    ibsnr+=atof(nalbs);
    iiznr+=atof(naliz);
    ibsbr+=atof(buhbs);
    iizbr+=atof(buhiz);

    ibsnro+=atof(nalbs);
    iiznro+=atof(naliz);
    ibsbro+=atof(buhbs);
    iizbro+=atof(buhiz);
   }  

 }
sprintf(strsql,"%s %s",gettext("Итого по группе"),grbuhz);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(42,strsql),strsql,ibsngr,iizngr,ibsbgr,iizbgr);

      ibsngro+=ibsngr;
      iizngro+=iizngr;
      ibsbgro+=ibsbgr;
      iizbgro+=iizbgr;

      fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnp,iiznp,ibsbp,iizbp);

      fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnr,iiznr,ibsbr,iizbr);

      fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n\n",
      iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(42,gettext("Итого по всем группам")),gettext("Итого по всем группам"),ibsngro,iizngro,ibsbgro,iizbgro);

fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnpo,iiznpo,ibsbpo,iizbpo);

fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnro,iiznro,ibsbro,iizbro);

fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnpo-ibsnro,iiznpo-iiznro,ibsbpo-ibsbro,iizbpo-iizbro);

iceb_podpis(ff,data->window);

fclose(ff);

sprintf(strsql,"select * from %s order by hna asc,datd asc",imatmptab);

kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafgrnal[64];
sprintf(imafgrnal,"dvosnl%d.lst",getpid());

if((ff = fopen(imafgrnal,"w")) == NULL)
 {
  iceb_er_op_fil(imafgrnal,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(gettext("Распечатать движение по документам"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff);

fprintf(ff,"%s\n",gettext("Сортировка по группам налогового учёта"));

sapdvos(ff,data->rk->mat_ot.ravno(),data->rk->podr.ravno(),data->rk->kod_op.ravno(),data->rk->prih_rash,data->rk->grup_bu.ravno(),data->rk->grup_nu.ravno(),data->rk->shetu.ravno(),data->rk->in_nom.ravno());
memset(grnalz,'\0',sizeof(grnalz));

ibsngr=iizngr=ibsbgr=iizbgr=0.;
ibsngro=iizngro=ibsbgro=iizbgro=0.;
ibsnpo=0.,iiznpo=0.,ibsbpo=0.,iizbpo=0.;
ibsnro=0.,iiznro=0.,ibsbro=0.,iizbro=0.;
ibsnp=iiznp=ibsbp=iizbp=0.;
ibsnr=iiznr=ibsbr=iizbr=0.;

while(curtmp.read_cursor(&rowtmp) != 0)
 {
/*********
  iceb_u_polen(stroka,grbuh,sizeof(grbuh),1,'|');

  iceb_u_polen(stroka,grnal,sizeof(grnal),2,'|');
****************/
  strncpy(grbuh,rowtmp[0],sizeof(grbuh)-1);
  strncpy(grnal,rowtmp[1],sizeof(grbuh)-1);
  
  if(iceb_u_SRAV(grnalz,grnal,0) != 0)
   {
    if(grnalz[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Итого по группе"),grnalz);
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(42,strsql),strsql,ibsngr,iizngr,ibsbgr,iizbgr);

      ibsngro+=ibsngr;
      iizngro+=iizngr;
      ibsbgro+=ibsbgr;
      iizbgro+=iizbgr;

      fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnp,iiznp,ibsbp,iizbp);

      fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnr,iiznr,ibsbr,iizbr);

      fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n\n",
      iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);
     }
    strcpy(grnalz,grnal);
    ibsngr=iizngr=ibsbgr=iizbgr=0.;
    ibsnp=iiznp=ibsbp=iizbp=0.;
    ibsnr=iiznr=ibsbr=iizbr=0.;
   }
/***************
  iceb_u_polen(stroka,datadok,sizeof(datadok),3,'|');
  iceb_u_rsdat(&d,&m,&g,datadok,2);
  iceb_u_polen(stroka,vidopt,sizeof(vidopt),4,'|');
  iceb_u_polen(stroka,invnom,sizeof(invnom),5,'|');
  iceb_u_polen(stroka,&naim,6,'|');
  iceb_u_polen(stroka,nalbs,sizeof(nalbs),7,'|');
  iceb_u_polen(stroka,naliz,sizeof(naliz),8,'|');
  iceb_u_polen(stroka,buhbs,sizeof(buhbs),9,'|');
  iceb_u_polen(stroka,buhiz,sizeof(buhiz),10,'|');
  iceb_u_polen(stroka,nomd,sizeof(nomd),11,'|');
  iceb_u_polen(stroka,kodoper,sizeof(kodoper),12,'|');
  iceb_u_polen(stroka,podr,sizeof(podr),13,'|');
  iceb_u_polen(stroka,matot,sizeof(matot),14,'|');
***************/
  iceb_u_rsdat(&d,&m,&g,rowtmp[2],2);
  strncpy(vidopt,rowtmp[3],sizeof(vidopt)-1);
  strncpy(invnom,rowtmp[4],sizeof(invnom)-1);
  naim.new_plus(rowtmp[5]);
  strncpy(nalbs,rowtmp[6],sizeof(nalbs)-1);
  strncpy(naliz,rowtmp[7],sizeof(naliz)-1);
  strncpy(buhbs,rowtmp[8],sizeof(buhbs)-1);
  strncpy(buhiz,rowtmp[9],sizeof(buhiz)-1);
  strncpy(nomd,rowtmp[10],sizeof(nomd)-1);
  strncpy(kodoper,rowtmp[11],sizeof(kodoper)-1);
  strncpy(podr,rowtmp[12],sizeof(podr)-1);
  strncpy(matot,rowtmp[13],sizeof(matot)-1);
  
  fprintf(ff,"%02d.%02d.%d %s %-8s %-*.*s %11s %11s %11s %11s \
%-*s %-*s %-5s %-5s %-6s %-6s\n",
  d,m,g,vidopt,invnom,
  iceb_u_kolbait(20,naim.ravno()),iceb_u_kolbait(20,naim.ravno()),naim.ravno(),
  nalbs,naliz,buhbs,buhiz,
  iceb_u_kolbait(5,nomd),nomd,
  iceb_u_kolbait(5,kodoper),kodoper,
  podr,matot,grbuh,grnal);

  for(int nom=20; nom < iceb_u_strlen(naim.ravno()); nom+=20)
   {
    fprintf(ff,"%21s %-*.*s\n",
    "",
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim.ravno())),
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim.ravno())),
    iceb_u_adrsimv(nom,naim.ravno()));
   }

  ibsngr+=atof(nalbs);
  iizngr+=atof(naliz);
  ibsbgr+=atof(buhbs);
  iizbgr+=atof(buhiz);
  if(vidopt[0] == '+')
   {
    ibsnp+=atof(nalbs);
    iiznp+=atof(naliz);
    ibsbp+=atof(buhbs);
    iizbp+=atof(buhiz);

    ibsnpo+=atof(nalbs);
    iiznpo+=atof(naliz);
    ibsbpo+=atof(buhbs);
    iizbpo+=atof(buhiz);
   }  
  if(vidopt[0] == '-')
   {
    ibsnr+=atof(nalbs);
    iiznr+=atof(naliz);
    ibsbr+=atof(buhbs);
    iizbr+=atof(buhiz);

    ibsnro+=atof(nalbs);
    iiznro+=atof(naliz);
    ibsbro+=atof(buhbs);
    iizbro+=atof(buhiz);
   }  

 }
sprintf(strsql,"%s %s",gettext("Итого по группе"),grnalz);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(42,strsql),strsql,ibsngr,iizngr,ibsbgr,iizbgr);

      ibsngro+=ibsngr;
      iizngro+=iizngr;
      ibsbgro+=ibsbgr;
      iizbgro+=iizbgr;

      fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnp,iiznp,ibsbp,iizbp);

      fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
      iceb_u_kolbait(41,gettext("Итого по группе")),gettext("Итого по группе"),ibsnr,iiznr,ibsbr,iizbr);

      fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n\n",
      iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnp-ibsnr,iiznp-iiznr,ibsbp-ibsbr,iizbp-iizbr);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(42,gettext("Итого по всем группам")),gettext("Итого по всем группам"),ibsngro,iizngro,ibsbgro,iizbgro);

fprintf(ff,"\n%*s+:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnpo,iiznpo,ibsbpo,iizbpo);

fprintf(ff,"%*s-:%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),ibsnro,iiznro,ibsbro,iizbro);

fprintf(ff,"%*s :%11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(41,gettext("Разница")),gettext("Разница"),ibsnpo-ibsnro,iiznpo-iiznro,ibsbpo-ibsbro,iizbpo-iizbro);

iceb_podpis(ff,data->window);

fclose(ff);
//fclose(ffsort);


//unlink(imafsort);

data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Реестр документов движения основных средств"));

data->rk->imaf.plus(imafgrbuh);
data->rk->naim.plus(gettext("Реестр документов с сортировкой по группам бух. учёта"));

data->rk->imaf.plus(imafgrnal);
data->rk->naim.plus(gettext("Реестр документов с сортировкой по группам налогового учёта"));

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
