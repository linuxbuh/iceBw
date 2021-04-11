/*$Id: iceb_salorok_r.c,v 1.24 2013/09/26 09:47:05 sasa Exp $*/
/*13.01.2018	02.03.2008	Белых А.И.	iceb_salorok_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "iceb_libbuh.h"
#include "iceb_salorok.h"
#define NAIM_SHRIFT "naim_shrift"

class iceb_salorok_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class iceb_salorok_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  iceb_salorok_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   iceb_salorok_r_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_salorok_r_data *data);
gint iceb_salorok_r1(class iceb_salorok_r_data *data);
void  iceb_salorok_r_v_knopka(GtkWidget *widget,class iceb_salorok_r_data *data);

void iceb_spkw(short dk,short mk,short gk,const char *shet,const char *naimsh,const char *kontr,const char *naikontr,double suma,FILE *ff_spr,GtkWidget *wpredok);


extern SQL_baza bd;

extern iceb_u_str shrift_ravnohir;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int iceb_salorok_r(class iceb_salorok_data *datark,GtkWidget *wpredok)
{
char strsql[1024];
int gor=0,ver=0;
class iceb_u_str repl;
class iceb_salorok_r_data data;
data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
   gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт сальдо по контрагенту"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_salorok_r_key_press),&data);

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

repl.plus(gettext("Расчёт сальдо по контрагенту"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,wpredok));
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
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне."));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(iceb_salorok_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)iceb_salorok_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_salorok_r_v_knopka(GtkWidget *widget,class iceb_salorok_r_data *data)
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

gboolean   iceb_salorok_r_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_salorok_r_data *data)
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
/*********************/
/*Вывод общего итога в акт сверки*/
/*********************/
void obit_ac(double db,double kr,FILE *ff1)
{
double          brr=0.;

fprintf(ff1,"----------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%10.2f %10.2f\n",iceb_u_kolbait(31,gettext("Оборот общий")),gettext("Оборот общий"),db,kr);
if(db >= kr)
 {
  brr=db-kr;
  fprintf(ff1,"%*s:%10s\n",iceb_u_kolbait(31,gettext("Сальдо общее")),gettext("Сальдо общее"),iceb_u_prnbr(brr));
 }
else
 {
  brr=kr-db;
  fprintf(ff1,"%*s:%10s %10s\n",iceb_u_kolbait(31,gettext("Сальдо общее")),gettext("Сальдо общее")," ",iceb_u_prnbr(brr));
 }

}

/**************/
/*Вывод сальдо*/
/**************/
void salorkslw(double dd, double kk,double ddn,double kkn,
double debm,double krem,
FILE *ff1,FILE *ffact)
{

double brr=dd+ddn;
double krr=kk+kkn;
fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");

if(debm != 0. || krem != 0.)
 {
  fprintf(ff1,"\
%*s:%12s",
  iceb_u_kolbait(37,gettext("Оборот за месяц")),gettext("Оборот за месяц"),iceb_u_prnbr(debm));

  fprintf(ff1," %12s\n",iceb_u_prnbr(krem));
  if(ffact != NULL)
   fprintf(ffact,"%*s %10.2f %10.2f| |\n",
   iceb_u_kolbait(31,gettext("Оборот за месяц")),gettext("Оборот за месяц"),debm,krem);
 }

fprintf(ff1,"%*s:%12s",
iceb_u_kolbait(37,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(dd));

fprintf(ff1," %12s\n",iceb_u_prnbr(kk));

if(ffact != NULL)
 fprintf(ffact,"%*s %10.2f %10.2f| |\n",
 iceb_u_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),dd,kk);

fprintf(ff1,"%*s:%12s",
iceb_u_kolbait(37,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),iceb_u_prnbr(brr));
fprintf(ff1," %12s\n",iceb_u_prnbr(brr));

if(ffact != NULL)
 fprintf(ffact,"%*s %10.2f %10.2f| |\n",
 iceb_u_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),brr,krr);

if(dd+ ddn >= kk+kkn)
 {
  brr=(dd+ddn)-(kk+kkn);
  fprintf(ff1,"%*s:%12s\n",
  iceb_u_kolbait(37,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),iceb_u_prnbr(brr));

  if(ffact != NULL)
   fprintf(ffact,"%*s %10.2f %10s| |\n",
   iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),brr," ");
 }
else
 {
  brr=(kk+kkn)-(dd+ddn);
  fprintf(ff1,"%*s:%12s %12s\n",
  iceb_u_kolbait(37,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",iceb_u_prnbr(brr));


  if(ffact != NULL)
   fprintf(ffact,"%*s %10s %10.2f| |\n",
   iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",brr);

 }
}
/*********************************/
/*Концовка акта сверки           */
/*********************************/

void	konact(FILE *ff)
{
fprintf(ff,"\n");
fprintf(ff,gettext("\
После отражения в учете указанных дополнительных проводок сальдо на ________________________\n\
выражается в сумме__________________________________________________________________________\n\
____________________________________________________________________________________________\n\
                  (сумма прописью и цифрами)\n\
____________________________________________________________________________________________\n\
                   (Указать наименование учереждения)\n\n"));

fprintf(ff,"\
%*s________________________    %*s________________________\n\n",
iceb_u_kolbait(25,gettext("Руководитель")),
gettext("Руководитель"),
iceb_u_kolbait(25,gettext("Руководитель")),
gettext("Руководитель"));


fprintf(ff,"\
%*s________________________    %*s________________________\n",
iceb_u_kolbait(25,gettext("Главный бухгалтер")),
gettext("Главный бухгалтер"),
iceb_u_kolbait(25,gettext("Главный бухгалтер")),
gettext("Главный бухгалтер"));
                                     
fprintf(ff,"\n");

SQL_str row;
SQLCURSOR cur;
class iceb_u_str telef("");
if(iceb_sql_readkey("select telef from Kontragent where kodkon='00'",&row,&cur,NULL) == 1)
 telef.new_plus(row[0]);

fprintf(ff,gettext("\
Вышлите нам, пожалуйста, один экземпляр акта с Вашими подписью и печатью.\n\
В случае несоответствия сумм, просим выслать расшифровку или провести сверку\n\
по телефону"));

fprintf(ff," %s\n",telef.ravno()); 

fprintf(ff,gettext("\
Просим в течении 5 дней вернуть подписанный акт.\n\
При невозвращении акта, сумма будет зачтена, как согласованная."));

iceb_podpis(ff,NULL);

}


/*************************************/
/*Получение сальдо по заданному счету*/
/*************************************/
void saloro1(const char sh[],short dn,short mn,short gn,
short dk,short mk,short gk,const char kor[],const char nai[],FILE *ff1,
FILE *ff4,FILE *ffact,
double *db,double *kr,
short mks, //0-все проводки 1-без взаимно кореспондирующих
class iceb_u_spisok *sheta_srs,class iceb_u_double *mdo,
const char *kodgr,
const char *naimgr,
const char *imatmptab,
GtkWidget *view,
GtkTextBuffer *buffer,
FILE *ff_act,
FILE *ff_act_bi,
FILE *ff_spr,
GtkWidget *wpredok)
{
time_t          tmm;
struct  tm      *bf;
double          ddn,kkn;
short           mvs;
short           d,m,g;
class iceb_u_str shh("");
double		dd,kk;
int		nm;
long		kolstr;
SQL_str         row;
SQLCURSOR       cur;
char		strsql[1024];
class iceb_u_str naish("");
short		godn;
short		mes=0,god=0;
short		shet=0;
double		deb=0.,kre=0.;
double		debm=0.,krem=0.;
int		val=0;
class iceb_u_str koment;
short	startgodb=0; /*Стартовый год*/


if(iceb_poldan("Стартовый год",strsql,"nastrb.alx",wpredok) == 0)
 startgodb=atoi(strsql);

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

shh.new_plus(sh);


sprintf(strsql,"select nais,stat from Plansh where ns='%s'",sh);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  naish.new_plus(row[0]);
  val=atoi(row[1]);
 }
if(val == 1)
 val=-1;
 
ddn=kkn=0.;
dd=kk=0.;
sprintf(strsql,"select deb,kre from Saldo where kkk='1' and gs=%d and \
ns='%s' and kodkon='%s'",godn,shh.ravno(),kor);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
//if((kolstr=static_cast<int>(cur.make_cursor(&bd,strsql))) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  ddn+=atof(row[0]);
  kkn+=atof(row[1]);
 }
if(ddn > kkn)
 {
  ddn=ddn-kkn; 
  kkn=0.;
 }
else
 {
  kkn=kkn-ddn;
  ddn=0;
 }
time(&tmm);
bf=localtime(&tmm);

fprintf(ff1,"%s\n\
%s: %s %s\n\
%s %s %s %s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
iceb_get_pnk("00",0,wpredok),
gettext("Счет"),sh,naish.ravno(),
gettext("Распечатка проводок"),
gettext("для контрагента"),
kor,nai,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(kodgr[0] != '\0')
 fprintf(ff1,"%s:%s %s\n",gettext("Группа"),kodgr,naimgr);
 
fprintf(ff1,"\
--------------------------------------------------------------------------------------------\n");

fprintf(ff1,gettext("\
  Дата    |Сче.к.|Записал|Номер док. |    Дебет   |   Кредит   |    К о м е н т а р и й    |\n"));
/********
                                                                123456789012345678901234567
*********/

fprintf(ff1,"\
--------------------------------------------------------------------------------------------\n");

fprintf(ff4,"%s\n\
%s: %s %s\n\
%s %s %s %s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
iceb_get_pnk("00",0,wpredok),
gettext("Счет"),sh,naish.ravno(),
gettext("Распечатка проводок"),
gettext("для контрагента"),
kor,nai,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff4,"\
--------------------------------------------------------------------------------------------\n");

fprintf(ff4,gettext("\
  Дата    |Сче.к.|Записал|Номер док. |    Дебет   |   Кредит   |    К о м е н т а р и й    |\n"));

fprintf(ff4,"\
--------------------------------------------------------------------------------------------\n");

iceb_zagsh(sh,ffact,wpredok);

class akt_sverki akt_svr;
akt_svr.shet.new_plus(sh);

mvs=0;
g=godn;
d=1;
m=1;
int nomer=0;
int kolbsn37=iceb_u_kolbait(37,gettext("Сальдо начальное"));
int kolbsn20=iceb_u_kolbait(20,gettext("Сальдо начальное"));
while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0 && mvs == 0)
   {
    nm=sheta_srs->find(sh);
    akt_svr.dn=d;
    akt_svr.mn=m;
    akt_svr.gn=g;

    if(ddn > kkn)    
     {
      akt_svr.start_saldo_deb=ddn-kkn;
      ddn=ddn-kkn;
      mdo->plus(ddn,nm*2);
      kkn=0.;

      sprintf(strsql,"%20s\n",iceb_u_prnbr(ddn));
      iceb_printw(strsql,buffer,view,NAIM_SHRIFT);

      fprintf(ff1,"%*s:%12s\n",kolbsn37,gettext("Сальдо начальное"),iceb_u_prnbr(ddn));
      
      fprintf(ff4,"%*s:%12s\n",kolbsn37,gettext("Сальдо начальное"),iceb_u_prnbr(ddn));

      fprintf(ffact,"%02d.%02d.%4d %-*.*s %10.2f %10s| |\n",
      d,m,g,kolbsn20,kolbsn20,gettext("Сальдо начальное"),ddn," ");
      fprintf(ffact,"\
------------------------------------------------------ ---------------------------------------------------\n");

     }
    else
     {
      akt_svr.start_saldo_kre=kkn-ddn;
      kkn=kkn-ddn;
      mdo->plus(kkn,nm*2+1);
      ddn=0.;

      sprintf(strsql,"%20s %20s\n"," ",iceb_u_prnbr(kkn));
      iceb_printw(strsql,buffer,view,NAIM_SHRIFT);

      fprintf(ff1,"%*s:%12s %12s\n",kolbsn37,
      gettext("Сальдо начальное")," ",iceb_u_prnbr(kkn));

      fprintf(ff4,"%*s:%12s %12s\n",kolbsn37,
      gettext("Сальдо начальное")," ",iceb_u_prnbr(kkn));

      fprintf(ffact,"%02d.%02d.%4d %-*.*s %10s %10.2f| |\n",
      d,m,g,kolbsn20,kolbsn20,gettext("Сальдо начальное")," ",kkn);
       fprintf(ffact,"\
------------------------------------------------------ ---------------------------------------------------\n");
     }
      mvs=1;
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"select * from Prov \
where val=%d and datp='%04d-%02d-%02d' and sh='%s' and kodkon='%s'",
  val,g,m,d,shh.ravno(),kor);
//  if((kolstr=static_cast<int>(cur.make_cursor(&bd,strsql))) < 0)
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    iceb_u_dpm(&d,&m,&g,1);
    continue;
   }
  
  if(kolstr == 0)
   {
    iceb_u_dpm(&d,&m,&g,1);
    continue;
   }
  shet=0;
  
  while(cur.read_cursor(&row) != 0)
   {
    if(mks == 1)
     if(iceb_u_SRAV(sh,row[3],1) == 0 || sheta_srs->find(row[3]) == 0)
       continue;
    deb=atof(row[9]);
    kre=atof(row[10]);


    if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0)
    if(iceb_u_sravmydat(dk,mk,gk,d,m,g) >= 0)
     {
      if(shet == 0)
       {
        if(iceb_u_sravmydat(1,m,g,1,mes,god) != 0)
         {
          if(mes != 0)
           {
            salorkslw(dd,kk,ddn,kkn,debm,krem,ff1,ffact);
            debm=krem=0.;
           }
          mes=m;
          god=g;
         }
        shet=1;
       }
      dd+=deb;
      kk+=kre;
      debm+=deb;
      krem+=kre;

      int kbsh=iceb_u_kolbait(6,row[3]);
      int kbkto=iceb_u_kolbait(3,row[5]);
      int kbnomd=iceb_u_kolbait(11,row[6]);
      int kbkom=iceb_u_kolbait(27,row[13]);
      
      fprintf(ff1,"%02d.%02d.%4d %-*s %-*s%4s %-*s %12s",
      d,m,g,kbsh,row[3],kbkto,row[5],row[11],kbnomd,row[6],iceb_u_prnbr(deb));

      fprintf(ff1," %12s %-*.*s\n",iceb_u_prnbr(kre),kbkom,kbkom,row[13]);
      for(int nom=27; nom < iceb_u_strlen(row[13]); nom+=27)
        fprintf(ff1,"%63s %.*s\n","",iceb_u_kolbait(27,iceb_u_adrsimv(nom,row[13])),iceb_u_adrsimv(nom,row[13]));

      fprintf(ff4,"%02d.%02d.%4d %-*s %-*s%4s %-*s %12s",
      d,m,g,kbsh,row[3],kbkto,row[5],row[11],kbnomd,row[6],iceb_u_prnbr(deb));

      fprintf(ff4," %12s %-*.*s\n",iceb_u_prnbr(kre),kbkom,kbkom,row[13]);
      for(int nom=27; nom < iceb_u_strlen(row[13]); nom+=27)
        fprintf(ff4,"%63s %.*s\n","",iceb_u_kolbait(27,iceb_u_adrsimv(nom,row[13])),iceb_u_adrsimv(nom,row[13]));



      koment.new_plus(row[13]);

      sprintf(strsql,"insert into %s values ('%s','%s','%04d-%02d-%02d','%s','%s',%s,%s,'%s',%s,%s)",
      imatmptab,
      row[2],row[6],g,m,d,row[3],row[5],row[9],row[10],koment.ravno_filtr(),row[7],row[15]);
             
      iceb_sql_zapis(strsql,1,1,wpredok);    

      koment.new_plus(row[6]);
      koment.plus(" ",row[13]);

      kbkom=iceb_u_kolbait(20,koment.ravno());      
      fprintf(ffact,"%02d.%02d.%4d %-*.*s %10.2f %10.2f| |\n",
      d,m,g,kbkom,kbkom,koment.ravno(),deb,kre);

      //заряжаем массивы
      sprintf(strsql,"%d.%d.%d|%s",d,m,g,row[6]);
      if(row[6][0] == '\0')
         nomer=-1;
      else
         nomer=akt_svr.data_nomd.find(strsql);

      if(nomer < 0)
       {
        akt_svr.data_nomd.plus(strsql);
        if(iceb_u_strstrm(koment.ravno(),"-ПДВ") == 1) //есть образец в строке
         {
          iceb_u_cpbstr(&akt_svr.koment,koment.ravno(),"-ПДВ");
         }
        else
          akt_svr.koment.plus(koment.ravno());
       }
      akt_svr.suma_deb.plus(deb,nomer);
      akt_svr.suma_kre.plus(kre,nomer);

      fprintf(ffact,"\
------------------------------------------------------ ---------------------------------------------------\n");

     }


    if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0)
     {
      ddn+=deb;
      kkn+=kre;
     }

   }
  iceb_u_dpm(&d,&m,&g,1);
 }

sprintf(strsql,"%20s",iceb_u_prnbr(dd));
iceb_printw(strsql,buffer,view,NAIM_SHRIFT);

sprintf(strsql," %20s\n",iceb_u_prnbr(kk));
iceb_printw(strsql,buffer,view,NAIM_SHRIFT);

salorkslw(dd,kk,ddn,kkn,debm,krem,ff1,ffact);
salorkslw(dd,kk,ddn,kkn,0.,0.,ff4,NULL);

iceb_spkw(dk,mk,gk,sh,naish.ravno(),kor,nai,kk+kkn-dd-ddn,ff_spr,wpredok);
fprintf(ff_spr,"\n\n");

*db+=dd+ddn;
*kr+=kk+kkn;

iceb_akt_sv(&akt_svr,ff_act,ff_act_bi);

}
/*********************/
/*Выдача общего итога*/
/*********************/
void  obit(double db,double kr,
short mt, //0-пром. итог 1- общий
int metkascr, //0-выводить на экран 1-нет
FILE *ff1,
GtkWidget *view,
GtkTextBuffer *buffer)
{
double          brr;
char strsql[1024];

if(mt == 1 && metkascr == 0)
 {  
  sprintf(strsql,"--------------------------------------------\n");
  iceb_printw(strsql,buffer,view,NAIM_SHRIFT);
 }

if(mt == 1)
 {
  fprintf(ff1,"\
---------------------------------------------------------------------------------------\n\
%*s:%12s",iceb_u_kolbait(37,gettext("Общий итог")),gettext("Общий итог"),iceb_u_prnbr(db));
  fprintf(ff1," %12s\n",iceb_u_prnbr(kr));
 }

if(db >= kr)
 {
  brr=db-kr;
  if(metkascr == 0)
   {
    sprintf(strsql,"%20s\n",iceb_u_prnbr(brr));
    iceb_printw(strsql,buffer,view,NAIM_SHRIFT);
   }
  if(mt == 1)
      fprintf(ff1,"%38s%12s\n"," ",iceb_u_prnbr(brr));
 }
else
 {
  brr=kr-db;
  if(metkascr == 0)
   {
    sprintf(strsql,"%20s %20s\n"," ",iceb_u_prnbr(brr));
    iceb_printw(strsql,buffer,view,NAIM_SHRIFT);
   }
  if(mt == 1)
      fprintf(ff1,"%38s%12s %12s\n"," "," ",iceb_u_prnbr(brr));
 }

}
/*********************/
/*Выдача итога в файл*/
/*********************/
void		vsali(double debs,double kres,short nm,short met,FILE *ff1,class iceb_u_double *mdo)
{
double		brr;

if(met == 1)
 {
  fprintf(ff1,"\n\
---------------------------------------------------------------------------------------\n\
%*s:%12s",iceb_u_kolbait(32,gettext("Общий итог")),gettext("Общий итог"),iceb_u_prnbr(debs));

  fprintf(ff1," %12s\n",iceb_u_prnbr(kres));

  if(debs >= kres)
   {
    brr=debs -kres;
    fprintf(ff1,"%33s%12s\n"," ",iceb_u_prnbr(brr));
   }
  else
   {
    brr=kres-debs;
    fprintf(ff1,"%33s%12s %12s\n"," "," ",iceb_u_prnbr(brr));
   }

  return;
 }

fprintf(ff1,"\
---------------------------------------------------------------------------------------\n\
%*s:%12s",iceb_u_kolbait(32,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(debs));

fprintf(ff1," %12s\n",iceb_u_prnbr(kres));

brr=debs+mdo->ravno(nm*2);
fprintf(ff1,"%*s:%12s",iceb_u_kolbait(32,gettext("Сальдо развернутое")),gettext("Сальдо развернутое"),iceb_u_prnbr(brr));

brr=kres+mdo->ravno(nm*2+1);
fprintf(ff1," %12s\n",iceb_u_prnbr(brr));
 
if(mdo->ravno(nm*2)+ debs >= mdo->ravno(nm*2+1)+kres)
 {
  brr=mdo->ravno(nm*2)+ debs - (mdo->ravno(nm*2+1)+kres);
  fprintf(ff1,"%*s:%12s\n",iceb_u_kolbait(32,gettext("Сальдо конечное")),gettext("Сальдо конечное"),iceb_u_prnbr(brr));
 }
else
 {
  brr=mdo->ravno(nm*2+1)+kres-(mdo->ravno(nm*2)+ debs) ;
  fprintf(ff1,"%*s:%12s %12s\n",iceb_u_kolbait(32,gettext("Сальдо конечное")),gettext("Сальдо конечное")," ",iceb_u_prnbr(brr));
 }

}
/**********************************************/
/*Взять из шапки документа вид договора*/
/********************************************/
const char *salorok_gvd(const char *podsistema,short dd,short md,short gd,int podr,int tz,const char *nom_dok,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
static class iceb_u_str vid_dog;

vid_dog.new_plus("");

if(iceb_u_SRAV(podsistema,ICEB_MP_MATU,0) == 0) /*подсистема материальный учёт*/
 {
  
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=7",
  gd,podr,nom_dok);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   vid_dog.new_plus(row[0]);
 }

if(iceb_u_SRAV(podsistema,ICEB_MP_USLUGI,0) == 0) /*подсистема учёт услуг*/
 {
  
  sprintf(strsql,"select uslpr from Usldokum where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
  gd,md,dd,podr,nom_dok,tz);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   vid_dog.new_plus(row[0]);
 }


return(vid_dog.ravno());
}

/*********************************/
/*Работа с отсортированным файлом*/
/*********************************/
void potf(const char kor[],const char nai[],short dn,short mn,short gn,
short dk,short mk,short gk,const char *imaf1,const char *imaf2,class iceb_u_spisok *sheta_srs,class iceb_u_double *mdo,
const char *imatmptab,
GtkWidget *wpredok)
{
time_t          tmm;
struct  tm      *bf;
FILE		*ff1;
short		d,m,g;
class iceb_u_str sh(""),shz("");
class iceb_u_str shk("");
class iceb_u_str nn(""),nnz("");
double		deb,deb1=0.,debo,debs;
double		kre,kre1=0.,kreo,kres;
class iceb_u_str kom(""),komz("");
class iceb_u_str kto("");
short		kop;
int		nm;
class iceb_u_str naish("");
char		strsql[512];
SQL_str         row;
char		bros[512];

time(&tmm);
bf=localtime(&tmm);


class SQLCURSOR curtmp;
SQL_str rowtmp;

sprintf(strsql,"select * from %s order by sh asc,datp asc,nomd asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }

fprintf(ff1,"%s\n",gettext("Проводки по номерам документов просуммированы"));

fprintf(ff1,"%s\n\
%s %s %s %s\n\
     %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
iceb_get_pnk("00",0,wpredok),
gettext("Распечатка проводок"),
gettext("для контрагента"),
kor,nai,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

shz.new_plus("");
nnz.new_plus("");
kre=deb=debo=kreo=debs=kres=0.;
nm=kop=0;
SQLCURSOR curr;
int podrz=0;
int tipz=0;
short ddz=0,mdz=0,gdz=0;
class iceb_u_str podsistemaz("");
class iceb_u_str nom_dokz("");
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  sh.new_plus(rowtmp[0]);
  iceb_u_rsdat(&d,&m,&g,rowtmp[2],2);
  kom.new_plus(rowtmp[7]);
  nn.new_plus(rowtmp[1]);
  shk.new_plus(rowtmp[3]);
  kto.new_plus(rowtmp[4]);
  deb1=atof(rowtmp[5]);
  kre1=atof(rowtmp[6]);
 
  if(iceb_u_SRAV(nn.ravno(),nnz.ravno(),0) != 0)
   {
    sprintf(bros,"%s %s:",gettext("Итог по документу"),nnz.ravno());
    fprintf(ff1,"%*s %12s",iceb_u_kolbait(32,bros),bros,iceb_u_prnbr(deb));
    fprintf(ff1," %12s %s\n\n",iceb_u_prnbr(kre),salorok_gvd(podsistemaz.ravno(),ddz,mdz,gdz,podrz,tipz,nom_dokz.ravno(),wpredok));
    kop=0;
    nnz.new_plus(nn.ravno());
    deb=kre=0.;
   }

  if(iceb_u_SRAV(sh.ravno(),shz.ravno(),0) != 0)
   {
    if(shz.ravno()[0] != '\0')
     {
      if(deb != 0. || kre != 0.)
       {
        sprintf(bros,"%s %s:",gettext("Итог по документу"),nnz.ravno());
        fprintf(ff1,"%*s %12s",iceb_u_kolbait(32,bros),bros,iceb_u_prnbr(deb));
        fprintf(ff1," %12s %s\n\n",iceb_u_prnbr(kre),salorok_gvd(podsistemaz.ravno(),ddz,mdz,gdz,podrz,tipz,nom_dokz.ravno(),wpredok));
        deb=kre=0.;
        
       }
      vsali(debs,kres,nm,0,ff1,mdo);
      debs=kres=0.;
     }

    nm=sheta_srs->find(sh.ravno());
    debo+=mdo->ravno(nm*2);
    kreo+=mdo->ravno(nm*2+1);


    sprintf(strsql,"select nais from Plansh where ns='%s'",sh.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      naish.new_plus(row[0]);
    else
      naish.new_plus("");
        
    fprintf(ff1,"\n%s: %s %s\n",gettext("Счёт"),sh.ravno(),naish.ravno());

    fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");
    fprintf(ff1,gettext("\
  Дата    |  Счёт  | Номер |Подс|   Дебет    |   Кредит   |      Коментарий           |\n\
          |        |докумен|ист.|            |            |     Вид договора          |\n"));
    fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");

    if(mdo->ravno(nm*2) > mdo->ravno(nm*2+1) )
      {
       fprintf(ff1,"%*s:%12s\n",iceb_u_kolbait(32,gettext("Сальдо начальное")),
       gettext("Сальдо начальное"),iceb_u_prnbr(mdo->ravno(nm*2)));
      }
     else
      {
       fprintf(ff1,"%*s:%12s %12s\n",iceb_u_kolbait(32,gettext("Сальдо начальное")),
       gettext("Сальдо начальное")," ",iceb_u_prnbr(mdo->ravno(nm*2+1)));
      }

    shz.new_plus(sh.ravno());
   }
  iceb_u_rsdat(&ddz,&mdz,&gdz,rowtmp[2],2);
  nom_dokz.new_plus(rowtmp[1]);
  podrz=atoi(rowtmp[8]); /*именно здесь*/
  tipz=atoi(rowtmp[9]);
  podsistemaz.new_plus(rowtmp[4]);

  fprintf(ff1,"%02d.%02d.%4d %-*s %-*s %-*s %12s",
  d,m,g,
  iceb_u_kolbait(8,shk.ravno()),shk.ravno(),
  iceb_u_kolbait(7,nn.ravno()),nn.ravno(),
  iceb_u_kolbait(4,kto.ravno()),kto.ravno(),
  iceb_u_prnbr(deb1));

  fprintf(ff1," %12s %.*s\n",iceb_u_prnbr(kre1),iceb_u_kolbait(27,kom.ravno()),kom.ravno());

  for(int nom=27; nom < iceb_u_strlen(kom.ravno()); nom+=27)
        fprintf(ff1,"%58s %.*s\n","",iceb_u_kolbait(27,iceb_u_adrsimv(nom,kom.ravno())),iceb_u_adrsimv(nom,kom.ravno()));

  if(nn.ravno()[0] != '\0' )
   {
    kop++;
   }
  deb+=deb1;
  kre+=kre1;
  debs+=deb1;
  kres+=kre1;
  debo+=deb1;
  kreo+=kre1;
 }
sprintf(bros,"%s %s:",gettext("Итог по документу"),nnz.ravno());
fprintf(ff1,"%*s %12s",iceb_u_kolbait(32,bros),bros,iceb_u_prnbr(deb));
fprintf(ff1," %12s %s\n\n",iceb_u_prnbr(kre),salorok_gvd(podsistemaz.ravno(),ddz,mdz,gdz,podrz,tipz,nom_dokz.ravno(),wpredok));

vsali(debs,kres,nm,0,ff1,mdo);
vsali(debo,kreo,nm,1,ff1,mdo);
iceb_podpis(ff1,wpredok);

fclose(ff1);


sprintf(strsql,"select * from %s order by sh asc,datp asc,koment asc,nomd asc,shk asc",imatmptab);

if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }




if((ff1 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,wpredok);
  return;
 }

fprintf(ff1,"%s\n",gettext("Проводки по комментариям просуммированы"));

fprintf(ff1,"%s\n\
%s %s %s %s\n\
     %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
iceb_get_pnk("00",0,wpredok),
gettext("Распечатка проводок"),
gettext("для контрагента"),
kor,nai,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

nnz.new_plus("");
shz.new_plus("");
komz.new_plus("");
kre=deb=debo=kreo=debs=kres=0.;
kop=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  sh.new_plus(rowtmp[0]);
  iceb_u_rsdat(&d,&m,&g,rowtmp[2],2);
  kom.new_plus(rowtmp[7]);
  nn.new_plus(rowtmp[1]);
  shk.new_plus(rowtmp[3]);
  kto.new_plus(rowtmp[4]);
  deb1=atof(rowtmp[5]);
  kre1=atof(rowtmp[6]);
       
  if(iceb_u_SRAV(kom.ravno(),komz.ravno(),0) != 0)
   {
    if(komz.getdlinna() > 1)
     {
      sprintf(bros,"%s-%.*s:",gettext("Итог по комментарию"),
      iceb_u_kolbait(10,komz.ravno()),
      komz.ravno());

      fprintf(ff1,"%*s %12s",iceb_u_kolbait(32,bros),bros,iceb_u_prnbr(deb));
      fprintf(ff1," %12s\n\n",iceb_u_prnbr(kre));
     }
    kop=0;
    komz.new_plus(kom.ravno());
    deb=kre=0.;
   }

  if(iceb_u_SRAV(sh.ravno(),shz.ravno(),0) != 0)
   {
    if(shz.ravno()[0] != '\0')
     {
      vsali(debs,kres,nm,0,ff1,mdo);
      debs=kres=0.;
     }

    nm=sheta_srs->find(sh.ravno());
    debo+=mdo->ravno(nm*2);
    kreo+=mdo->ravno(nm*2+1);
    

    sprintf(strsql,"select nais from Plansh where ns='%s'",sh.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      naish.new_plus(row[0]);
    else
      naish.new_plus("");

    fprintf(ff1,"\n%s %s %s\n",gettext("Счёт"),sh.ravno(),naish.ravno());

    fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");
    fprintf(ff1,gettext("\
  Дата    |  Счёт  | Номер |Подс|   Дебет    |   Кредит   |      Коментарий           |\n\
          |        |докумен|ист.|            |            |                           |\n"));
    fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");

    if(mdo->ravno(nm*2) > mdo->ravno(nm*2+1))    
       fprintf(ff1,"%*s:%12s\n",iceb_u_kolbait(32,gettext("Сальдо начальное")),gettext("Сальдо начальное"),iceb_u_prnbr(mdo->ravno(nm*2)));
     else
       fprintf(ff1,"%*s:%12s %12s\n",iceb_u_kolbait(32,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",iceb_u_prnbr(mdo->ravno(nm*2+1)));
    shz.new_plus(sh.ravno());
   }

  fprintf(ff1,"%02d.%02d.%4d %-*s %-*s %-*s %12s",
  d,m,g,
  iceb_u_kolbait(8,shk.ravno()),shk.ravno(),
  iceb_u_kolbait(7,nn.ravno()),nn.ravno(),
  iceb_u_kolbait(4,kto.ravno()),kto.ravno(),
  iceb_u_prnbr(deb1));

  fprintf(ff1," %12s %.*s\n",iceb_u_prnbr(kre1),iceb_u_kolbait(27,kom.ravno()),kom.ravno());

  for(int nom=27; nom < iceb_u_strlen(kom.ravno()); nom+=27)
        fprintf(ff1,"%58s %.*s\n","",iceb_u_kolbait(27,iceb_u_adrsimv(nom,kom.ravno())),iceb_u_adrsimv(nom,kom.ravno()));

  if(kom.ravno()[0] != '\0' )
    kop++;
  deb+=deb1;
  kre+=kre1;
  debo+=deb1;
  kreo+=kre1;
  debs+=deb1;
  kres+=kre1;
  
 }
sprintf(bros,"%s %.*s:",gettext("Итог по комментарию"),
iceb_u_kolbait(10,komz.ravno()),
komz.ravno());
fprintf(ff1,"%*s %12s",iceb_u_kolbait(32,bros),bros,iceb_u_prnbr(deb));
fprintf(ff1," %12s\n\n",iceb_u_prnbr(kre));
vsali(debs,kres,nm,0,ff1,mdo);
vsali(debo,kreo,nm,1,ff1,mdo);

iceb_podpis(ff1,wpredok);
fclose(ff1);

}



/******************************************/
/******************************************/

gint iceb_salorok_r1(class iceb_salorok_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
class SQLCURSOR cur,cur1;
short		godn;
short	startgodb=0; /*Стартовый год*/



gtk_text_buffer_create_tag(data->buffer,NAIM_SHRIFT,"family","monospace",NULL); /*назначаем шрифт*/

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(iceb_poldan("Стартовый год",strsql,"nastrb.alx",data->window) == 0)
 startgodb=atoi(strsql);

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(strsql,data->buffer,data->view,NAIM_SHRIFT);


class iceb_u_str kodgr("");
class iceb_u_str naimgr("");
class iceb_u_str nai("");
sprintf(strsql,"select naikon,grup from Kontragent where kodkon='%s'",data->rk->kontr.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  nai.new_plus(row[0]);
  kodgr.new_plus(row[1]);
 }
if(kodgr.ravno()[0] != '\0')
 {
  sprintf(strsql,"select naik from Gkont where kod=%s",kodgr.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   naimgr.new_plus(row[0]);
 }
 

/*Просматриваем план счетов, находим все счета с развернутым
сальдо и заряжаем в массив*/
time_t tmm;
time(&tmm);
int kolstr=0;


class iceb_u_spisok sheta_srs; /*Список счетов с развёрнутым сальдо*/
/*Смотрим по каким счетам введено сальдо*/
sprintf(strsql,"select ns from Saldo where kkk=1 and gs=%d and kodkon='%s'",
godn,data->rk->kontr.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 {
  sheta_srs.plus(row[0]);
 }

/*Смотрим по каким счетам были сделаны проводки*/
sprintf(strsql,"select distinct sh from Prov where datp >= '%04d-1-1' and \
datp <= '%04d-%02d-%02d' and kodkon='%s'",godn,gk,mk,dk,data->rk->kontr.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select saldo from Plansh where ns='%s'",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    sprintf(strsql,"%s %s",gettext("Не найден счёт в плане счетов!"),row[0]);
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  if(atoi(row1[0]) != 3)
   continue;  
  if(sheta_srs.find(row[0]) < 0)
    sheta_srs.plus(row[0]);
 }



class iceb_u_str sh("");
int ks=sheta_srs.kolih();
if(ks == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

sprintf(strsql,"%s\n",gettext("Расчёт сальдо по всем счетам с развернутым сальдо"));
iceb_printw(strsql,data->buffer,data->view,NAIM_SHRIFT);


sprintf(strsql,"%s:%d\n",gettext("Количество счетов с развернутым сальдо"),ks);
iceb_printw(strsql,data->buffer,data->view,NAIM_SHRIFT);

for(int ii=0; ii < ks ; ii++)
 {
  sprintf(strsql," %s",sheta_srs.ravno(ii));
  iceb_printw(strsql,data->buffer,data->view,NAIM_SHRIFT);
 }
iceb_printw("\n",data->buffer,data->view);


class iceb_u_double mdo;
mdo.make_class(ks*2);

char imaf_spr[64];
sprintf(imaf_spr,"sprk%d.lst",getpid());
FILE *ff_spr;
if((ff_spr = fopen(imaf_spr,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_spr,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf[64];
FILE *ff1;
sprintf(imaf,"sl%d.lst",getpid());
if((ff1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


class iceb_tmptab tabtmp;
const char *imatmptab={"salorok"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(24) not null,\
nomd char(24) not null,\
datp DATE NOT NULL,\
shk char(24) not null,\
kto char(8) not null,\
deb double(14,2) not null,\
kre double(14,2) not null,\
koment char(112) not null,\
pod INT NOT NULL DEFAULT 0,\
tz TINYINT NOT NULL DEFAULT 0,\
INDEX(sh,datp)) ENGINE = MYISAM",imatmptab);



if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  



char imaf1[64];
sprintf(imaf1,"sla%d.lst",getpid());

char imaf2[64];

sprintf(imaf2,"slb%d.lst",getpid());

char imaf4[64];
FILE *ff4;

sprintf(imaf4,"slbc%d.lst",getpid());
if((ff4 = fopen(imaf4,"w")) == NULL)
 {
  iceb_er_op_fil(imaf4,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafacts[64];
FILE *ffact;

sprintf(imafacts,"acts%d.lst",getpid());
if((ffact = fopen(imafacts,"w")) == NULL)
 {
  iceb_er_op_fil(imafacts,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_acts[64];

sprintf(imaf_acts,"asv%d.lst",getpid());
FILE *ff_act;
if((ff_act = fopen(imaf_acts,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_acts,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_acts_bi[64];

sprintf(imaf_acts_bi,"asvbi%d.lst",getpid());
FILE *ff_act_bi;
if((ff_act_bi = fopen(imaf_acts_bi,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_acts_bi,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_zagacts(dn,mn,gn,dk,mk,gk,data->rk->kontr.ravno(),nai.ravno(),ffact);
iceb_zagacts(dn,mn,gn,dk,mk,gk,data->rk->kontr.ravno(),nai.ravno(),ff_act);
iceb_zagacts(dn,mn,gn,dk,mk,gk,data->rk->kontr.ravno(),nai.ravno(),ff_act_bi);
int kolshet=0;
float kolstr1=0.;
double db=0.,kr=0.;
for(int i=0; i < ks; i++)
 {
  iceb_pbar(data->bar,ks,++kolstr1);    
  sh.new_plus(sheta_srs.ravno(i));
  
  if(iceb_u_proverka(data->rk->shet.ravno(),sh.ravno(),1,0) != 0)
    continue;

  /*Проверяем есть ли код организации в списке данного счета*/

  sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",sh.ravno(),data->rk->kontr.ravno());
  if(iceb_sql_readkey(strsql,data->window) != 1)
    continue;

  sprintf(strsql,"%s\n",sh.ravno());
  iceb_printw(strsql,data->buffer,data->view,NAIM_SHRIFT);
  kolshet++;  
  double db1=0.,kr1=0.;
  saloro1(sh.ravno(),dn,mn,gn,dk,mk,gk,data->rk->kontr.ravno(),nai.ravno(),ff1,ff4,ffact,&db1,&kr1,data->rk->metka_prov,&sheta_srs,&mdo,kodgr.ravno(),naimgr.ravno(),imatmptab,data->view,data->buffer,ff_act,ff_act_bi,ff_spr,data->window);
  obit(db1,kr1,0,0,ff1,data->view,data->buffer);
  obit(db1,kr1,0,1,ff4,data->view,data->buffer);
  db+=db1; kr+=kr1;
 }
obit(db,kr,1,0,ff1,data->view,data->buffer);
obit(db,kr,1,1,ff4,data->view,data->buffer);
iceb_podpis(ff1,data->window);
iceb_podpis(ff4,data->window);
fclose(ff1);
fclose(ff4);
//Концовка распечатки
if(kolshet > 1)
 {
  obit_ac(db,kr,ffact);
  obit_ac(db,kr,ff_act);
  obit_ac(db,kr,ff_act_bi);
 }
konact(ffact);
fclose(ffact);
konact(ff_act);
fclose(ff_act);
konact(ff_act_bi);
fclose(ff_act_bi);
fclose(ff_spr);


potf(data->rk->kontr.ravno(),nai.ravno(),dn,mn,gn,dk,mk,gk,imaf1,imaf2,&sheta_srs,&mdo,imatmptab,data->window);





data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Распечатка проводок з дневными итогами"));

data->rk->imaf.plus(imaf1);
data->rk->naim.plus(gettext("Проводки по контрагенту с итогами по документам"));

data->rk->imaf.plus(imaf2);
data->rk->naim.plus(gettext("Проводки по контрагенту с итогами по комментариям"));

data->rk->imaf.plus(imaf4);
data->rk->naim.plus(gettext("Проводки по контрагенту с конечным сальдо"));

data->rk->imaf.plus(imafacts);
data->rk->naim.plus(gettext("Акт сверки по контрагенту"));

data->rk->imaf.plus(imaf_acts);
data->rk->naim.plus(gettext("Акт сверки с общими суммами по документам"));

data->rk->imaf.plus(imaf_acts_bi);
data->rk->naim.plus(gettext("Акт сверки с общими суммами по документам без месячных итогов"));

data->rk->imaf.plus(imaf_spr);
data->rk->naim.plus(gettext("Справка про сальдо по контаргенту"));

for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),1,data->window);



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен."));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
