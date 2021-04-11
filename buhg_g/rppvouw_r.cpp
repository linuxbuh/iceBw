/*$Id: rppvouw_r.c,v 1.26 2013/09/26 09:46:55 sasa Exp $*/
/*05.03.2020	01.02.2005	Белых А.И.	rppvouw_r.c
Расчёт 
*/
#include <errno.h>
#include "buhg_g.h"
#include "rppvouw.h"

class rppvouw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class rppvouw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  rppvouw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rppvouw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rppvouw_r_data *data);
gint rppvouw_r1(class rppvouw_r_data *data);
void  rppvouw_r_v_knopka(GtkWidget *widget,class rppvouw_r_data *data);

void gsapr(short,short,short,short,short,short,int*,int*,int,const char*,FILE*);
void rasid(double,short,short,short,short,short,short,int,const char*,int*,int*,FILE*);
void rasio(const char*,double,short,short,short,short,short,short,int,const char*,const char*,class iceb_u_spisok *mso,class iceb_u_double *mspo,int*,int*,FILE*);
void rasis(const char*,double,short,short,short,short,short,short,int,const char*,const char*,class iceb_u_spisok *mso,class iceb_u_double *mspo,class iceb_u_spisok *mss,class iceb_u_double *msps,int*,int*,FILE*);
void rasoi(class iceb_u_spisok *mssi,class iceb_u_double *mspsi,FILE*);
void rppvou1_sap(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl1,const char *naiskl,FILE *ff);
void rppvouw_r_rp(const char *podr,const char *kontr,int prihr,const char *kodop,const char *shet,int *kst,FILE *ff);


extern SQL_baza bd;




extern double	nds1;
extern double   okrg1;
extern double	okrcn;
extern int kol_strok_na_liste;

int rppvouw_r(class rppvouw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class rppvouw_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать реестр проводок по видам операций"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rppvouw_r_key_press),&data);

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

repl.plus(gettext("Распечатать реестр проводок по видам операций"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rppvouw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);

gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rppvouw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rppvouw_r_v_knopka(GtkWidget *widget,class rppvouw_r_data *data)
{
//printf("rppvouw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rppvouw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rppvouw_r_data *data)
{
// printf("rppvouw_r_key_press\n");
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

gint rppvouw_r1(class rppvouw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

struct tm *bf;
bf=localtime(&vremn);
short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"select datp,sh,shk,nomd,pod,deb,datd,komen,kodkon,oper,tz from Prov \
where val=0 and \
datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and kto='%s' and deb != 0. \
order by tz asc,oper asc,pod asc,kodkon asc,nomd asc",
gn,mn,dn,gk,mk,dk,ICEB_MP_USLUGI);

SQLCURSOR cur;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    

/*Сортировка через временную таблицу делается так как сортировать нужно по коду контрагента
а он может отсутствовать в проводке*/

class iceb_tmptab tabtmp;
const char *imatmptab={"rppvouw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
datp DATE not null,\
sh char(32) not null,\
shk char(32) not null,\
nomd char(32) not null,\
pod int not null,\
deb double(14,2) not null,\
datd DATE not null,\
komen char(255) not null,\
kodkon char(32) not null,\
oper char(32) not null,\
tz int not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

sprintf(strsql,"%s %d.%d.%d %s %d.%d.%d\n",gettext("Период с"),dn,mn,gn,gettext("по"),
dk,mk,gk);

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

float kolstr1=0.;
SQL_str row;
SQL_str row1;
SQLCURSOR cur1;
class iceb_u_str kontr("");
short tz=0;
class iceb_u_str kprr1("");
class iceb_u_str kprr("");
class iceb_u_str nomdok("");
class iceb_u_str koment("");

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->podr.ravno(),row[4],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[1],0,0) != 0)
    continue;
   
  sprintf(strsql,"select tp,datd,kontr,kodop,nomnn from Usldokum where datd='%s' and \
podr=%s and nomd='%s'",row[6],row[4],row[3]);
/*  printw("strsql=%s\n",strsql);*/
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"Не найден документ N %s Подразделение %s Дата=%s\n",
    row[3],row[4],row[6]);

    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

    sprintf(strsql,"%s %s %s %s\n",gettext("Проводка"),row[1],row[2],row[3]);

    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
    continue;
   }
  tz=atoi(row1[0]);
  if(data->rk->pr_ras != 0 && data->rk->pr_ras != tz)
   continue;

  if(iceb_u_proverka(data->rk->kontr.ravno(),row1[2],0,0) != 0)
    continue;
   
  if(iceb_u_proverka(data->rk->kodop.ravno(),row1[3],0,0) != 0)
    continue;

  kontr.new_plus(row1[2]);    
  kprr.new_plus(row1[3]);    

  nomdok.new_plus(row[3]);
  nomdok.plus(" ",row1[4]);
  koment.new_plus(row[7]);
  
  sprintf(strsql,"insert into %s values ('%s','%s','%s','%s',%s,%s,'%s','%s','%s','%s',%s)",
  imatmptab,
  row[0],row[1],row[2],nomdok.ravno_filtr(),row[4],row[5],row[5],koment.ravno_filtr(),kontr.ravno(),kprr.ravno(),row[10]);
  iceb_sql_zapis(strsql,1,0,data->window);    

 }

sprintf(strsql,"select * from %s order by tz asc,oper asc,pod asc,kodkon asc,nomd asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }




class iceb_u_spisok mssi;
class iceb_u_spisok mssio;
class iceb_u_double mspsi;
class iceb_u_double mspsio;

class iceb_u_spisok mso; //Массив счетов по организации
class iceb_u_double mspo;
class iceb_u_spisok mss; //Массив счетов по организации
class iceb_u_double msps;

char imaf[64];

sprintf(imaf,"uslrp%d.lst",getpid());

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kst=0;
int sli=0;

rppvouw_r_rp(data->rk->podr.ravno(),data->rk->kontr.ravno(),data->rk->pr_ras,data->rk->kodop.ravno(),data->rk->shet.ravno(),&kst,ff);



int skl=0;
class iceb_u_str kpos("");
class iceb_u_str nomdokv("");
class iceb_u_str naik("");
class iceb_u_str naimpr("");
class iceb_u_str naiskl("");


short tipzz=0,kom=0;
kolstr1=0.;
int klr=0;
double itd=0.;
class iceb_u_str kome("");
int nomer=0;
char orgn[64];
int skl1=0;
class iceb_u_str prih_ras("");
short dp,mp,gp;
short dd,md,gd;
double deb=0.;
class iceb_u_str shett("");
class iceb_u_str shettz("");
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&dp,&mp,&gp,row[0],2);

  shett.new_plus(row[1]);
  shett.plus(",",row[2]);
  nomdok.new_plus(row[3]);
  skl1=atoi(row[4]);
  deb=atof(row[5]);
  iceb_u_rsdat(&dd,&md,&gd,row[6],2);
  kome.new_plus(row[7]);

  strncpy(orgn,row[8],sizeof(orgn)-1);

  kprr.new_plus(row[9]);
  tz=atoi(row[10]);
     
  if(iceb_u_SRAV(kprr.ravno(),kprr1.ravno(),0) != 0 || tipzz != tz)
   {
    if(kprr1.ravno()[0] != '\0' )
     {
      rasis(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),naik.ravno(),&mso,&mspo,&mss,&msps,&sli,&kst,ff);
      itd=0.;
      rasoi(&mssi,&mspsi,ff);

      fprintf(ff,"\f");
      kst=0;
     }
    skl=0;
    nomdokv.new_plus("");
    kpos.new_plus("");

    /*Определяем наименоввание операции*/
    if(tz == 1)
     sprintf(strsql,"select naik from Usloper1 where kod='%s'",kprr.ravno());
    if(tz == 2)
     sprintf(strsql,"select naik from Usloper2 where kod='%s'",kprr.ravno());
    
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"Не нашли операцию %s",kprr.ravno());
      iceb_menu_soob(strsql,data->window);
      naimpr.new_plus("");
     }
    else
      naimpr.new_plus(row1[0]);
    
    kprr1.new_plus(kprr.ravno());
    tipzz=tz;

    sprintf(strsql,"%d/%s %s\n",tz,kprr.ravno(),naimpr.ravno());
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

    if(tz == 1)
     prih_ras.new_plus(gettext("Приход"));
    if(tz == 2)
     prih_ras.new_plus(gettext("Расход"));

    /*Определяем наименование подразделения*/
     sprintf(strsql,"select naik from Uslpodr where kod=%d",
     skl1);
    
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),skl1);
       iceb_menu_soob(strsql,data->window);
      naiskl.new_plus("");
     }
    else
      naiskl.new_plus(row1[0]);

    fprintf(ff,"%s\n",gettext("Учёт услуг"));
    fprintf(ff,"%s\n\
%s %s - \"%s\" %s %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %d:%d\n",
iceb_get_pnk("00",0,data->window),gettext("Реестр проводок по виду операции"),
kprr.ravno(),naimpr.ravno(),prih_ras.ravno(),
gettext("за период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);
    kst+=3;
    klr+=sli;
    sli=0;
    rppvou1_sap(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);

   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
      rasis(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),naik.ravno(),&mso,&mspo,&mss,&msps,&sli,&kst,ff);
      itd=0.;
      klr+=sli;
      sli=0;
      kst=0;
      fprintf(ff,"\f");
     }
    shettz.new_plus("");
    nomdokv.new_plus("");
    kpos.new_plus("");
    kom=1;

    /*Определяем наименование подразделения*/
     sprintf(strsql,"select naik from Uslpodr where kod=%d",
     skl1);
    
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),skl1);
       iceb_menu_soob(strsql,data->window);
      naiskl.new_plus("");
     }
    else
      naiskl.new_plus(row1[0]);

    sprintf(strsql,"%s: %d %s\n",gettext("Подразделение"),skl1,naiskl.ravno());
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
    skl=skl1;
   }

  if(iceb_u_SRAV(kpos.ravno(),orgn,0) != 0)
   {
    if(kpos.ravno()[0] != '\0')
     {
      rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),naik.ravno(),&mso,&mspo,&sli,&kst,ff);
      itd=0.;
     }
    nomdokv.new_plus("");
    shettz.new_plus("");
    kom=1;

    /*Читаем наименование организации*/
    if(atof(orgn) == 0. && iceb_u_pole(orgn,strsql,1,'-') == 0 && orgn[0] == '0')
      kontr.new_plus(strsql);
    else
      kontr.new_plus(orgn);

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr.ravno());
       iceb_menu_soob(strsql,data->window);
      naik.new_plus("");
     }
    else
      naik.new_plus(row1[0]);

    sprintf(strsql,gettext("Контрагент %s\n"),orgn);
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
    kpos.new_plus(orgn);
   }

  if(iceb_u_SRAV(nomdok.ravno(),nomdokv.ravno(),0) != 0)
   {
    if(nomdokv.getdlinna() > 1)
     {
      rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),&sli,&kst,ff);
      itd=0.;
     }
   nomdokv.new_plus(nomdok.ravno());
   }



   /*Записываем в массив организаций*/
  if((nomer=mso.find(shett.ravno())) < 0)
      mso.plus(shett.ravno());
  mspo.plus(deb,nomer);
   
   /*Записываем в массив по подразделению*/
  if((nomer=mss.find(shett.ravno())) < 0)
      mss.plus(shett.ravno());
  msps.plus(deb,nomer);

   /*Записываем в массив по подразделениям */
  if((nomer=mssi.find(shett.ravno())) < 0)
      mssi.plus(shett.ravno());
  mspsi.plus(deb,nomer);

   /*Записываем в массив по подразделениям и операциям*/
  if((nomer=mssio.find(shett.ravno())) < 0)
      mssio.plus(shett.ravno());
  mspsio.plus(deb,nomer);

   gsapr(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);

   sprintf(strsql,"%02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %15.2f\n",
   dp,mp,gp,dd,md,gd,
   iceb_u_kolbait(11,nomdok.ravno()),nomdok.ravno(),
   iceb_u_kolbait(11,shett.ravno()),shett.ravno(),
   deb);
   iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

   if(kom == 1)
     fprintf(ff,"\
%-*s %-*.*s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %20s %.*s\n",
     iceb_u_kolbait(7,orgn),orgn,
     iceb_u_kolbait(40,naik.ravno()),iceb_u_kolbait(40,naik.ravno()),naik.ravno(),
     dd,md,gd,dp,mp,gp,
     iceb_u_kolbait(11,nomdok.ravno()),nomdok.ravno(),
     iceb_u_kolbait(11,shett.ravno()),shett.ravno(),
     iceb_u_prnbr(deb),
     iceb_u_kolbait(13,kome.ravno()),
     kome.ravno());

   if(kom == 0)
     fprintf(ff,"\
%48s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %20s %.*s\n",
     " ",dd,md,gd,dp,mp,gp,
     iceb_u_kolbait(11,nomdok.ravno()),nomdok.ravno(),
     iceb_u_kolbait(11,shett.ravno()),shett.ravno(),
     iceb_u_prnbr(deb),
     iceb_u_kolbait(13,kome.ravno()),
     kome.ravno());

   itd+=deb;
   kom=0;

 }
rasis(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),naik.ravno(),&mso,&mspo,&mss,&msps,&sli,&kst,ff);
rasoi(&mssi,&mspsi,ff);
int kssio=mssio.kolih();
class iceb_u_str shett1("");
double k1=0.;
if(data->rk->kodop.ravno()[0] == '\0' || ( data->rk->kodop.ravno()[0] != '\0' && iceb_u_polen(data->rk->kodop.ravno(),&shett1,2,',') == 0))
 {
  k1=0.;
  fprintf(ff,"\n==================================================\n\
%s\n",gettext("Общий итог по всем операциям:"));
  for(int i=0; i< kssio ;i++)
   {
    fprintf(ff,"%*s: %20s\n",iceb_u_kolbait(15,mssio.ravno(i)),mssio.ravno(i),iceb_u_prnbr(mspsio.ravno(i)));
    k1+=mspsio.ravno(i);
   }
  fprintf(ff,"%*s: %20s\n",iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));
 }

sprintf(strsql,"\n%s\n",gettext("Общий итог по всем операциям:"));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

char imaf_oi[64];
sprintf(imaf_oi,"rpoi%d.lst",getpid());
FILE *ff1;
if((ff1 = fopen(imaf_oi,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_oi,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
fprintf(ff1,"%s\n",gettext("Учёт услуг"));
iceb_u_zagolov(gettext("Реестр проводок по видам операций"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff1);

rppvouw_r_rp(data->rk->podr.ravno(),data->rk->kontr.ravno(),data->rk->pr_ras,data->rk->kodop.ravno(),data->rk->shet.ravno(),&kst,ff1);

fprintf(ff1,"%s\n",gettext("Общий итог по всем операциям:"));

for(int i=0; i< kssio ;i++)
 {
  sprintf(strsql,"%*s: %20s\n",iceb_u_kolbait(15,mssio.ravno(i)),mssio.ravno(i),iceb_u_prnbr(mspsio.ravno(i)));
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
  fprintf(ff1,"%*s: %20s\n",iceb_u_kolbait(15,mssio.ravno(i)),mssio.ravno(i),iceb_u_prnbr(mspsio.ravno(i)));
 }

iceb_podpis(ff1,data->window);
fclose(ff1);


fclose(ff);


data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Реестр проводок по видам операций"));

data->rk->imaf.plus(imaf_oi);
data->rk->naimf.plus(gettext("Общие итоги"));

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
/*******/
/*Шапка*/
/*******/
void  gsapr(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl1,const char *naiskl,FILE *ff)
{
*kst+=1;

if(*kst <=  kol_strok_na_liste)
 return;

fprintf(ff,"\f");
*kst=1;
rppvou1_sap(dn,mn,gn,dk,mk,gk,sl,kst,skl1,naiskl,ff);


}

/*******************************/
/*Распечатка итога по документа*/
/*******************************/
void rasid(double itd,short dn,short mn,short gn,short dk,short mk,short gk,int skl1,const char *naiskl,int *sli,int *kst,FILE *ff)
{
 gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
 fprintf(ff,"%*s: %20s\n",iceb_u_kolbait(94,gettext("Итого по документу")),gettext("Итого по документу"),iceb_u_prnbr(itd));
}

/*******************************/
/*Распечатка итога по организации*/
/*******************************/
void rasio(const char *orgn,double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,const char *naiskl,const char *naik,
class iceb_u_spisok *mso,
class iceb_u_double *mspo,
int *sli,
int *kst,
FILE *ff)
{
short           i;
class iceb_u_str shett1("");
double          bb;

rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,sli,kst,ff);

/*Распечатываем результаты по организации*/
int kso=mso->kolih();
bb=0.;
if(kso > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%s \"%s\":\n",gettext("Итого по контрагенту"),naik);
 }
/*printw("kso- %d\n",kso);*/

for(i=0; i< kso ;i++)
 {
  shett1.new_plus(mso->ravno(i));

  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%*s: %20s\n",iceb_u_kolbait(15,shett1.ravno()),shett1.ravno(),iceb_u_prnbr(mspo->ravno(i)));

  bb+=mspo->ravno(i);
 }
if(kso > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%*s: %20s\n",iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));

  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
 }
mspo->free_class();
mso->free_class();
}
/*******************************/
/*Распечатка итога по подразделению*/
/*******************************/
void rasis(const char *orgn,double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,const char *naiskl,const char *naik,
class iceb_u_spisok *mso,
class iceb_u_double *mspo,
class iceb_u_spisok *mss,
class iceb_u_double *msps,
int *sli,
int *kst,
FILE *ff)
{
short           i;
double          bb;

rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,naik,mso,mspo,sli,kst,ff);

/*Распечатываем результаты по подразделению*/
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%s \"%s\":\n",gettext("Итого по подразделению"),naiskl);
bb=0.;
int kss=mss->kolih();
for(i=0; i< kss ;i++)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%*s: %20s\n",iceb_u_kolbait(15,mss->ravno(i)),mss->ravno(i),iceb_u_prnbr(msps->ravno(i)));
  bb+=msps->ravno(i);
 }
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%*s: %20s\n",iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"\
...............................................................................................................................\n");

msps->free_class();
mss->free_class();

}

/*************************/
/*Распечатка общего итога*/
/*************************/
void            rasoi(class iceb_u_spisok *mssi,class iceb_u_double *mspsi,FILE *ff)
{
double          k1;
short           i;

k1=0.;
fprintf(ff,"\n%s:\n",gettext("Общий итог"));
int kssi=mssi->kolih();
for(i=0; i< kssi ;i++)
 {
  fprintf(ff,"%*s: %20s\n",iceb_u_kolbait(15,mssi->ravno(i)),mssi->ravno(i),iceb_u_prnbr(mspsi->ravno(i)));
  k1+=mspsi->ravno(i);
 }
fprintf(ff,"%*s: %20s\n",iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));
mspsi->free_class();
mssi->free_class();

}
/***********************/
/*Шапка*/
/***************/
void rppvou1_sap(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl1,const char *naiskl,FILE *ff)
{

*sl+=1;
*kst+=4;

fprintf(ff,"\
%s N%-2d %-30s %s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %30s%sN%d\n",
gettext("Подразделение"),
 skl1,naiskl,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
"",gettext("Лист"),
*sl);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext(" 	 Контрагент                              |Дата док. |Дата пров.|N документа|  Счета   |       Сумма        |Комментарий\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------\n");

}
/***********************************/
/*Реквизиты поиска*/
/*************************/
void rppvouw_r_rp(const char *podr,const char *kontr,int prihr,const char *kodop,const char *shet,int *kst,FILE *ff)
{
if(podr[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Код подразделения"),podr);
 }
if(kontr[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Контрагент"),kontr);
 }
if(prihr != 0)
 {
  *kst+=1;

  if(prihr == 1)
   fprintf(ff,"%s:+\n",gettext("Вид операции"));
  if(prihr == 2)
   fprintf(ff,"%s:-\n",gettext("Вид операции"));
 }
if(kodop[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Код операции"),kodop);
 }
if(shet[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Счёт"),shet);
 }
}
