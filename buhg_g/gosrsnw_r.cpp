/*$Id: gosrsnw_r.c,v 1.20 2014/07/31 07:08:25 sasa Exp $*/
/*19.06.2019	10.03.2006	Белых А.И.	gosrsnw_r.c
Расчёт журнала ордера с развёрнутым сальдо
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "go.h"

class gosrsnw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class go_rr *rk;
  int metkasort;
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  gosrsnw_r_data()
   {
    metkasort=0;
    voz=1;
    kon_ras=1;
   }
 };
gboolean   gosrsnw_r_key_press(GtkWidget *widget,GdkEventKey *event,class gosrsnw_r_data *data);
gint gosrsnw_r1(class gosrsnw_r_data *data);
void  gosrsnw_r_v_knopka(GtkWidget *widget,class gosrsnw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;
extern short	startgodb; /*Стартовый год*/
extern short	vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/

int gosrsnw_r(class go_rr *datark,int metkasort,GtkWidget *wpredok)
{
char strsql[1024];
iceb_u_str repl;
class gosrsnw_r_data data;
int gor=0;
int ver=0;

data.rk=datark;
data.metkasort=metkasort;
data.name_window.plus(__FUNCTION__);


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт журнал-ордера по счёту"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(gosrsnw_r_key_press),&data);

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

repl.plus(gettext("Расчёт журнал-ордера по счёту"));
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
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(gosrsnw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)gosrsnw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  gosrsnw_r_v_knopka(GtkWidget *widget,class gosrsnw_r_data *data)
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

gboolean   gosrsnw_r_key_press(GtkWidget *widget,GdkEventKey *event,class gosrsnw_r_data *data)
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

/****************************/
/*Шапка журнала ордера*/
/*****************************/
void gosrsn_hapgo(short dn,short mn,short gn,
short dk,short mk,short gk,
int *kollist,
int *kolstr,
FILE *ff)
{
*kollist+=1;
*kolstr+=6;
fprintf(ff,"%*s%d\n",iceb_u_kolbait(100,gettext("Лист N")),gettext("Лист N"),*kollist);
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("  Код     |    Наименование    | C а л ь д о  на  %02d.%02d.%d |   Счёт   |      Оборот за период       | С а л ь д о  на  %02d.%02d.%d |\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff,gettext("контраген-|    контрагента     ------------------------------|кореспон- |------------------------------------------------------------\n"));
fprintf(ff,gettext("   та     |                    |   Дебет      |   Кредит     | дент     |   Дебет      |    Кредит    |   Дебет      |   Кредит     |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n");

}
/************************************/
/*Шапка конечного сальдо*/
/**********************************/
void gossrsn_sapsk(short dk,short gk,short mk,int *kollist_sk,int *kolstr_sk,FILE *ff)
{
*kollist_sk+=1;
*kolstr_sk+=6;
fprintf(ff,"%*s%d\n",iceb_u_kolbait(68,gettext("Лист N")),gettext("Лист N"),*kollist_sk);

fprintf(ff,"\
----------------------------------------------------------------------------------\n");

fprintf(ff,gettext("  Код     |    Наименование    | C а л ь д о  на  %02d.%02d.%d |                    \n"),
dk,mk,gk);
fprintf(ff,gettext("контраген-|    контрагента     -------------------------------     Т е л е ф о н  \n"));
fprintf(ff,gettext("   та     |                    |   Дебет      |   Кредит     |                    \n"));

fprintf(ff,"\
----------------------------------------------------------------------------------\n");
}
/******************/
/*Счётчик строк*/
/*****************/
void gosrsn_ss(int metka, /*0-оборотный баланс  1-журнал ордер*/
short dn,short mn,short gn,
short dk,short mk,short gk,
int *kollist,int *kolstr,FILE *ff)
{
*kolstr+=1;

if(*kolstr <= kol_strok_na_liste)
  return;

fprintf(ff,"\f");
*kolstr=1;

if(metka == 0)
  sapgorsw(dn,mn,gn,dk,mk,gk,kollist,kolstr,ff);
if(metka == 1)
  gosrsn_hapgo(dn,mn,gn,dk,mk,gk,kollist,kolstr,ff);
if(metka == 2)
  gossrsn_sapsk(dk,gk,mk,kollist,kolstr,ff);
 
}

/**************************************/
/*Распечатка свода по счетам корреспондентам для журнала ордера*/
/**************************************************************/

void gosrsn_itoggo(const char *shet,
short dn,short mn,short gn, //Дата начала периода
short dk,short mk,short gk, //Дата конца периода
const char *naim_shet,
class iceb_u_spisok *oss,
const char *imatmptab_hax,
FILE *ff,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];
struct  tm      *bf;
time_t vrem=time(NULL);
bf=localtime(&vrem);

fprintf(ff,"\f\%s\n\n\
%s %s %s\n\
%s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s %s - %d:%d\n",
iceb_get_pnk("00",0,wpredok),
gettext("Счёт"),
shet,naim_shet,
gettext("Всего по счетам корреспондентам."),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,gettext("  Счёт    |   Наименование счета         |   Дебет       |   Кредит      |\n"));
fprintf(ff,"\
--------------------------------------------------------------------------\n");
double ideb=0.;
double ikre=0.;
double deb=0.;
double kre=0.;
class iceb_u_str shet_kor("");
int kolstr=0;
for(int nomer_sheta=0; nomer_sheta < oss->kolih(); nomer_sheta++)
 {
  deb=kre=0.;
  sprintf(strsql,"select * from %s where shet='%s'",imatmptab_hax,oss->ravno(nomer_sheta));

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
   }

  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    deb+=atof(row[2]);
    kre+=atof(row[3]);
   }

  sprintf(strsql,"select nais from Plansh where ns='%s'",oss->ravno(nomer_sheta));

  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_beep();
    sprintf(strsql,"%s %s !",gettext("Не найден счет"),oss->ravno(nomer_sheta));
    iceb_menu_soob(strsql,wpredok);
    continue;
   }
  strncpy(strsql,row[0],sizeof(strsql)-1);

  fprintf(ff,"%-*s %-*.*s %15s",
  iceb_u_kolbait(10,oss->ravno(nomer_sheta)),oss->ravno(nomer_sheta),
  iceb_u_kolbait(30,strsql),iceb_u_kolbait(30,strsql),strsql,
  iceb_u_prnbr(deb));
  
  fprintf(ff," %15s\n",iceb_u_prnbr(kre));
  ideb+=deb;
  ikre+=kre;

 }
fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,"%*s %15s",iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(ideb));

fprintf(ff," %15s\n",iceb_u_prnbr(ikre));

}
/************************/
/*проверка контрагента*/
/************************/
int gorsrs_provk(const char *kod_k,
double debs,
double kres,
double debp,
double krep,
const char *imatmptab_hax,
GtkWidget *wpredok)
{
char strsql[512];

if(fabs(debs - kres) < 0.009)
 if(fabs(debp) < 0.01 && fabs(krep) < 0.009)
  {

   sprintf(strsql,"select * from %s where kod_kontr='%s'",imatmptab_hax,kod_k);
   if(iceb_sql_readkey(strsql,wpredok) == 0)
     return(1);

  }
return(0);
}

/*********************/
/*Распечатка по дням*/
/***********************/
void gorsrsn_podn(const char *shet,
const char *naim_shet,
short dn,short mn,short gn, //Дата начала периода
short dk,short mk,short gk, //Дата конца периода
class iceb_u_spisok *ssd, /*Список счетов дебета*/
class iceb_u_spisok *ssk, /*Список счетов кредита*/
double debs,double kres,
double debp,double krep,
double debk,double krek,
int metka_f, /*0-A4 1-A3*/
const char *kod_val,
const char *imatab,
char *imaf,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];

class iceb_fopen ff;

if(ff.start(imaf,"w",wpredok) != 0)
 return;

int kolih_deb=ssd->kolih();
int kolih_kre=ssk->kolih();


class iceb_u_str liniq;
for(int nomer=0; nomer < kolih_deb+2; nomer++)
 liniq.plus("-----------");


class iceb_u_spisok dni;
int kolih_dni=iceb_u_period(dn,mn,gn,dk,mk,gk,0);
short d=dn,m=mn,g=gn;

for(int nomer=0; nomer < kolih_dni; nomer++)
 {
  sprintf(strsql,"%02d.%02d.%04d",d,m,g);
  dni.plus(strsql);
  iceb_u_dpm(&d,&m,&g,1);
 }

class iceb_u_double den_shet_suma;
den_shet_suma.make_class(kolih_dni*kolih_deb);
class iceb_u_str den_mes_god("");
class iceb_u_str shet_d("");
int nomer_sheta=0;


double deb=0.,kre=0.;

d=dn;
m=mn;
g=gn;
int kolstr=0;
for(int nomer=0; nomer < kolih_dni; nomer++)
 {
//  sprintf(strsql,"select * from %s where data='%04d-%02d-%02d' and deb <> 0.",imatab,g,m,d);
  sprintf(strsql,"select * from %s where data='%04d-%02d-%02d'",imatab,g,m,d);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
   }
  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {

    deb=atof(row[2]);
    nomer_sheta=ssd->find_r(row[1]);
    den_shet_suma.plus(deb,nomer*kolih_deb+nomer_sheta);
   }

  iceb_u_dpm(&d,&m,&g,1);
 }


struct  tm      *bf;
time_t tmmn=time(NULL);
bf=localtime(&tmmn);


fprintf(ff.ff,"\x1B\x33%c",30); /*Изменение межстрочного интервала*/

fprintf(ff.ff,"%s\n%s %s %s",iceb_get_pnk("00",0,wpredok),gettext("Журнал-ордер по счёту"),shet,naim_shet);
if(kod_val[0] != '\0')
 fprintf(ff.ff," %s:%s",gettext("Валюта"),kod_val);
 
sprintf(strsql,"\n%s %s %s %d.%d.%d%s %s: %d:%d",
gettext("Оборот по дебету счета"),shet,
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

buhdenshetw(strsql,&dni,ssd,&den_shet_suma,metka_f,ff.ff);
iceb_podpis(ff.ff,wpredok);


/************************************************************/




den_shet_suma.free_class();
den_shet_suma.make_class(kolih_dni*kolih_kre);

d=dn;
m=mn;
g=gn;
for(int nomer=0; nomer < kolih_dni; nomer++)
 {
  sprintf(strsql,"select * from %s where data='%04d-%02d-%02d'",imatab,g,m,d);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
   }
  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    
    kre=atof(row[3]);
    nomer_sheta=ssk->find_r(row[1]);
    den_shet_suma.plus(kre,nomer*kolih_kre+nomer_sheta);
   }

  iceb_u_dpm(&d,&m,&g,1);
 }

fprintf(ff.ff,"\f");
fprintf(ff.ff,"%s\n%s %s %s",iceb_get_pnk("00",0,wpredok),gettext("Журнал-ордер по счёту"),shet,naim_shet);

if(kod_val[0] != '\0')
 fprintf(ff.ff," %s:%s",gettext("Валюта"),kod_val);

sprintf(strsql,"\n%s %s %s %d.%d.%d%s %s: %d:%d",
gettext("Оборот по кредиту счета"),shet,
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);


buhdenshetw(strsql,&dni,ssk,&den_shet_suma,metka_f,ff.ff);


fprintf(ff.ff,"\
        		   %s           %s\n",
gettext("Дебет"),
gettext("Кредит"));

fprintf(ff.ff,"\
%*s:%15.2f %15.2f\n",
iceb_u_kolbait(18,gettext("Сальдо начальное")),gettext("Сальдо начальное"),debs,kres);

fprintf(ff.ff,"\
%*s:%15.2f %15.2f\n",
iceb_u_kolbait(18,gettext("Оборот за период")),gettext("Оборот за период"),
debp,krep);

fprintf(ff.ff,"\
%*s:%15.2f %15.2f\n",
iceb_u_kolbait(18,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
debk,krek);

fprintf(ff.ff,"\n%s__________________________\n",
gettext("Главный бухгалтер"));

ff.end();

}
/***********************************************/
/* запись в промежуточную таблицу*/
/********************************************/
int gosrs_zvt(int per,const char *kod_kontr,double deb,double kre,const char *imatabl,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;



sprintf(strsql,"select debs,kres,debp,krep from %s where kod_kontr='%s'",imatabl,kod_kontr);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  class iceb_u_str naim("");
  class iceb_u_str grup("");
  
  int kod_kontr_int=atoi(kod_kontr);
  sprintf(strsql,"select naikon,grup from Kontragent where kodkon='%s'",kod_kontr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    naim.new_plus(row[0]);
    grup.new_plus(row[1]);
   }

  if(per == 0)
   sprintf(strsql,"insert into %s values ('%s',%.2f,%.2f,0.,0.,%d,'%s','%s',-1)",imatabl,kod_kontr,deb,kre,kod_kontr_int,naim.ravno_filtr(),grup.ravno_filtr());
  if(per == 1)
   sprintf(strsql,"insert into %s values ('%s',0.,0.,%.2f,%.2f,%d,'%s','%s',-1)",imatabl,kod_kontr,deb,kre,kod_kontr_int,naim.ravno_filtr(),grup.ravno_filtr());
 }
else
 {

  double debz=0.;
  double krez=0.;

  if(per == 0)
   {
  
    debz=deb+atof(row[0]);
    krez=kre+atof(row[1]);

    sprintf(strsql,"update %s \
set \
debs=%.2f,\
kres=%.2f \
where kod_kontr='%s'",imatabl,debz,krez,kod_kontr);
   }

  if(per == 1)
   {
    debz=deb+atof(row[2]);
    krez=kre+atof(row[3]);
   sprintf(strsql,"update %s \
set \
debp=%.2f,\
krep=%.2f \
where kod_kontr='%s'",imatabl,debz,krez,kod_kontr);
   }
 }
//printw("%s-%s\n",__FUNCTION__,strsql);
//OSTANOV();
if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return(1);

return(0);
}
/********************************************/
/*запись в в таблицу по дням*/
/*********************************************/
int gosrs_zvt_den(short d,short m,short g,const char *shet,double deb,double kre,const char *imatabl,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;



sprintf(strsql,"select deb,kre from %s where data='%04d-%02d-%02d' and shet='%s'",imatabl,g,m,d,shet);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  sprintf(strsql,"insert into %s values ('%04d-%02d-%02d','%s',%.2f,%.2f)",imatabl,g,m,d,shet,deb,kre);
 }
else
 {

  double debz=deb+atof(row[0]);
  double krez=kre+atof(row[1]);


  sprintf(strsql,"update %s \
set \
deb=%.2f,\
kre=%.2f \
where data='%04d-%02d-%02d' and shet='%s'",imatabl,debz,krez,g,m,d,shet);

 }

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return(1);

return(0);
}

/********************************************/
/*запись в в таблицу для шахматки*/
/*********************************************/
int gosrs_zvt_hax(const char *kod_kontr,const char *shet,double deb,double kre,const char *imatabl,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;



sprintf(strsql,"select deb,kre from %s where kod_kontr='%s' and shet='%s'",imatabl,kod_kontr,shet);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  sprintf(strsql,"insert into %s values ('%s','%s',%.2f,%.2f)",imatabl,kod_kontr,shet,deb,kre);
 }
else
 {

  double debz=deb+atof(row[0]);
  double krez=kre+atof(row[1]);


  sprintf(strsql,"update %s \
set \
deb=%.2f,\
kre=%.2f \
where kod_kontr='%s' and shet='%s'",imatabl,debz,krez,kod_kontr,shet);

 }

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return(1);

return(0);
}
/****************************************/
/*пронумерование строк */
/**********************************/
int gosrsn_ps(const char *imatmptab,const char *str_sort,GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
int nomer_str=0;

//sprintf(strsql,"select kod_kontr from %s where debp <> 0. or krep <> 0. %s",imatmptab,str_sort);
sprintf(strsql,"select kod_kontr from %s %s",imatmptab,str_sort);

if(cur.make_cursor(&bd,strsql) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"update %s \
set \
ns=%d \
where kod_kontr='%s'",imatmptab,nomer_str++,row[0]);
  iceb_sql_zapis(strsql,1,0,wpredok);
  
 }
 
return(0);
}


/******************************************/
/******************************************/

gint gosrsnw_r1(class gosrsnw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[2048];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
SQLCURSOR cur,cur1;

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);

 }
short godn=startgodb;

if(startgodb == 0 || startgodb > gn)
 godn=gn;


sprintf(strsql,"%s %s\n%d.%d.%d%s => %d.%d.%d%s %d %s:%s\n",
gettext("Счёт"),
data->rk->shet.ravno(),
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."),
kol_strok_na_liste,
gettext("Валюта"),data->rk->kod_val.ravno());

iceb_printw(strsql,data->buffer,data->view);


char str_sort[1024];
int kolstr=0;
double deb=0.,kre=0.;
class iceb_u_spisok ssd; /*Список счетов дебета*/
class iceb_u_spisok ssk; /*Список счетов кредита*/
class iceb_u_spisok oss; /*Общий список счетов*/



class iceb_u_double hach_kontr_shet_deb; /*шахматка контрагент-счёт по дебету*/
class iceb_u_double hach_kontr_shet_kre; /*шахматка контрагент-счёт по кредиту*/


class iceb_getkue_dat getval(data->rk->kod_val.ravno(),data->window);


int vds=0;
/*Узнаём наименование счёта*/
class iceb_u_str naim_shet("");
sprintf(strsql,"select nais,vids from Plansh where ns='%s'",data->rk->shet.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  naim_shet.new_plus(row[0]);
  vds=atoi(row[1]);
 }


class iceb_tmptab tabtmp;
const char *imatmptab={"gosrs"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kod_kontr char(32) not null DEFAULT '',\
debs double(16,2) not null DEFAULT 0.,\
kres double(16,2) not null DEFAULT 0.,\
debp double(16,2) not null DEFAULT 0.,\
krep double(16,2) not null DEFAULT 0.,\
kod_kontr_int INT NOT NULL DEFAULT 0,\
nm CHAR(255) NOT NULL DEFAULT '',\
gr INT NOT NULL DEFAULT 0,\
ns INT NOT NULL DEFAULT -1,\
index(nm),\
index(gr),\
unique(kod_kontr)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  





/*Узнаем начальное сальдо по каждой организации*/
sprintf(strsql,"%s\n",gettext("Вычисляем стартовое сальдо по всем контрагентам"));
iceb_printw(strsql,data->buffer,data->view);

if(vds == 0 || vplsh == 1)
 sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d \
and ns like '%s%%'",godn,data->rk->shet.ravno());

if(vds == 1 && vplsh == 0)
 sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d and ns='%s'",godn,data->rk->shet.ravno());

//printw("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
sprintf(strsql,"%04d-01-01",godn);

class iceb_u_str data_saldo(strsql);

int nomer_kontr=0;


if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(data->rk->kodgr.ravno()[0] != '\0')
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),row[0]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    if(iceb_u_proverka(data->rk->kodgr.ravno(),row1[0],0,0) != 0)
      continue;  
   }

  deb=getval.toue(row[1],data_saldo.ravno());
  kre=getval.toue(row[2],data_saldo.ravno());
  

 
  gosrs_zvt(0,row[0],deb,kre,imatmptab,data->window); /*запись в таблицу*/

 }


class iceb_tmptab tabtmp_den;
const char *imatmptab_den={"gosrsden"};
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
data DATE not null DEFAULT '0000-00-00',\
shet char(64) NOT NULL DEFAULT '',\
deb double(16,2) not null DEFAULT 0.,\
kre double(16,2) not null DEFAULT 0.,\
unique(data,shet)) ENGINE = MYISAM",imatmptab_den);


if(tabtmp_den.create_tab(imatmptab_den,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

class iceb_tmptab tabtmphax;
const char *imatmptab_hax={"gosrshax"};
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kod_kontr char(64) not null DEFAULT '',\
shet char(64) NOT NULL DEFAULT '',\
deb double(16,2) not null DEFAULT 0.,\
kre double(16,2) not null DEFAULT 0.,\
unique(kod_kontr,shet)) ENGINE = MYISAM",imatmptab_hax);


if(tabtmphax.create_tab(imatmptab_hax,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  


/*смотрим проводки заполняем массивы*/
int nomer_mes_v_per=-1;
short d=1,m=1,g=godn;
int kolih_den=iceb_u_period(1,1,godn,dk,mk,gk,0);
float kolstr1=0;
short mes_zap=0;

while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  
//  strzag(LINES-1,0,kolih_den,++kolstr1);
  iceb_pbar(data->bar,kolih_den,++kolstr1);    

  if(vds == 0 || vplsh == 1)
   sprintf(strsql,"select sh,shk,kodkon,deb,kre,datp from Prov \
 where val=0 and datp='%d-%02d-%02d' and sh like '%s%%'",g,m,d,data->rk->shet.ravno());

  if(vds == 1 && vplsh == 0)
   sprintf(strsql,"select sh,shk,kodkon,deb,kre,datp from Prov \
 where val=0 and datp='%d-%02d-%02d' and sh='%s'",g,m,d,data->rk->shet.ravno());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0) /*период*/
  if(mes_zap != m)
   {
    nomer_mes_v_per++;
    mes_zap=m;
   }            
  
  while(cur.read_cursor(&row) != 0)
   {
//    printw("%02d.%02d.%04d|%6s|%6s|%6s|%10s|%10s|\n",d,m,g,row[0],row[1],row[2],row[3],row[4]);
//    refresh();
    
    if(row[2] == NULL || row[2][0] == '\0')
     {
      class iceb_u_str rep;
      sprintf(strsql,"%d.%d.%d|%6s|%6s|%6s|%10s|%10s|",d,m,g,row[0],row[1],row[2],row[3],row[4]);
      rep.plus(strsql);
      
      rep.ps_plus(gettext("В проводке нет кода контрагента !"));
      iceb_menu_soob(rep.ravno(),data->window);
      continue;
     }

    if(data->rk->kodgr.ravno()[0] != '\0')
     {
      sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[2]);
      if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),row[2]);
        iceb_menu_soob(strsql,data->window);
        continue;
       }
      if(iceb_u_proverka(data->rk->kodgr.ravno(),row1[0],0,0) != 0)
        continue;  
     }



    deb=getval.toue(row[3],row[5]);
    kre=getval.toue(row[4],row[5]);

    if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0) /*До периода*/
     {
      gosrs_zvt(0,row[2],deb,kre,imatmptab,data->window); /*запись в таблицу*/
     }
    else /*Период*/
     {
      if(oss.find_r(row[1]) < 0)
         oss.plus(row[1]);

      gosrs_zvt(1,row[2],deb,kre,imatmptab,data->window); /*запись в таблицу*/

      gosrs_zvt_hax(row[2],row[1],deb,kre,imatmptab_hax,data->window);

      if(fabs(deb) > 0.009)
       {
        if(ssd.find_r(row[1]) < 0)
         ssd.plus(row[1]);
       }
      else
       {
        if(ssk.find_r(row[1]) < 0)
          ssk.plus(row[1]);
       }
      gosrs_zvt_den(d,m,g,row[1],deb,kre,imatmptab_den,data->window);
     }
   }

  iceb_u_dpm(&d,&m,&g,1);
 }


int kolih_kontr=0;


if(data->metkasort == 0) /*по коду контрагента в символьном виде*/
 sprintf(str_sort,"order by kod_kontr asc");

if(data->metkasort == 1) /*по коду контрагента в цифровом виде*/
 sprintf(str_sort,"order by kod_kontr_int asc");

if(data->metkasort == 2) /*по наименованию*/
 sprintf(str_sort,"order by nm asc");

if(data->metkasort == 3) /*по группе контрагента*/
 sprintf(str_sort,"order by gr asc,nm asc");

//sprintf(strsql,"select kod_kontr from %s where debp <> 0. or krep <> 0. %s",imatmptab,str_sort);
sprintf(strsql,"select kod_kontr from %s %s",imatmptab,str_sort);
kolih_kontr=iceb_sql_readkey(strsql,data->window);

int kolih_shet=oss.kolih();
int nomer_sheta=0;
/*заряжаем шахматки*/
hach_kontr_shet_deb.make_class(kolih_kontr*kolih_shet);
hach_kontr_shet_kre.make_class(kolih_kontr*kolih_shet);


gosrsn_ps(imatmptab,str_sort,data->window);


sprintf(strsql,"select * from %s order by kod_kontr asc",imatmptab_hax);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 {
  deb=atof(row[2]);
  kre=atof(row[3]);  
  sprintf(strsql,"select ns from %s where kod_kontr='%s'",imatmptab,row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {

    printf("%s-%s\n",__FUNCTION__,strsql);   
    iceb_menu_soob(gettext("Не найден номер строки"),data->window);

    continue;
   }
  nomer_kontr=atoi(row1[0]);

  nomer_sheta=oss.find_r(row[1]);

  hach_kontr_shet_deb.plus(deb,nomer_kontr*kolih_shet+nomer_sheta);

  hach_kontr_shet_kre.plus(kre,nomer_kontr*kolih_shet+nomer_sheta);
 }
 
sprintf(strsql,"%s\n",gettext("Вертикальна форма распечатки по контрагентам"));

iceb_printw(strsql,data->buffer,data->view);


int metka_naim_shet=0;
memset(strsql,'\0',sizeof(strsql));
if(iceb_poldan("Печатать наименование счёта корреспондента в журналах-ордерах для счетов с развёрнутым сальдо",strsql,"nastrb.alx",data->window) == 0)
 if(iceb_u_SRAV("Вкл",strsql,1) == 0)
  metka_naim_shet=1;

char imaf_obor[64];
char imaf_gur[64];
char imaf_sk[64];

sprintf(imaf_obor,"obor%d.lst",getpid());
sprintf(imaf_gur,"gurrs%d.lst",getpid());
sprintf(imaf_sk,"salk%d.lst",getpid());

class iceb_fopen ff_obor;
if(ff_obor.start(imaf_obor,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_fopen ff_gur;
if(ff_gur.start(imaf_gur,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf_gur_csv[64];
sprintf(imaf_gur_csv,"go%d.csv",getpid());
class iceb_fopen ff_gur_csv;
if(ff_gur_csv.start(imaf_gur_csv,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 
class iceb_fopen ff_sk;
if(ff_sk.start(imaf_sk,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 
struct  tm      *bf;
time_t tmm=time(NULL);

bf=localtime(&tmm);
fprintf(ff_obor.ff,"%s\n\n\
			      %s %s %s\n\
				  %s %d.%d.%d%s %s %d.%d.%d%s",
iceb_get_pnk("00",0,data->window),
gettext("Оборот по счёту"),
data->rk->shet.ravno(),naim_shet.ravno(),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

fprintf(ff_gur_csv.ff,"%s\n\n\
%s %s %s\n\
%s %d.%d.%d%s %s %d.%d.%d%s",
iceb_get_pnk("00",0,data->window),
gettext("Журнал ордер по счёту"),
data->rk->shet.ravno(),naim_shet.ravno(),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

if(data->rk->kod_val.ravno()[0] != '\0')
 {
  fprintf(ff_obor.ff," %s:%s",gettext("Валюта"),data->rk->kod_val.ravno());
  fprintf(ff_gur_csv.ff," %s:%s",gettext("Валюта"),data->rk->kod_val.ravno());
 }
fprintf(ff_obor.ff,"\n\
			      %s %d.%d.%d%s  %s: %d.%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);


fprintf(ff_gur_csv.ff,"\n\
%s %d.%d.%d  %s - %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff_gur_csv.ff,"%s|%s|\
%s %d.%d.%d|%s %d.%d.%d|\
%s|%s|%s|\
%s %d.%d.%d|%s %d.%d.%d|\n",
gettext("Код контрагента"),
gettext("Наименование контрагента"),
gettext("Дебет на"),dn,mn,gn,
gettext("Кредит на"),dn,mn,gn,
gettext("Счёт корреспондент"),
gettext("Дебет за период"),
gettext("Кредит за период"),
gettext("Дебет на"),dk,mk,gk,
gettext("Кредит на"),dk,mk,gk);

int kolstr_obor=5; 

fprintf(ff_gur.ff,"%s\n\n\
			       %s %s %s\n\
				  %s %d.%d.%d%s %s %d.%d.%d%s",
iceb_get_pnk("00",0,data->window),
gettext("Журнал ордер по счёту"),
data->rk->shet.ravno(),naim_shet.ravno(),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));


if(data->rk->kod_val.ravno()[0] != '\0')
 fprintf(ff_gur.ff," %s:%s",gettext("Валюта"),data->rk->kod_val.ravno());
 
fprintf(ff_gur.ff,"\n\
			       %s %d.%d.%d  %s - %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

int kolstr_gur=5;

fprintf(ff_sk.ff,"%s\n\n\
%s %d.%d.%d%s %s %s %s",
iceb_get_pnk("00",0,data->window),
gettext("Конечное сальдо на"),
dk,mk,gk,
gettext("г."),
gettext("по счету"),
data->rk->shet.ravno(),naim_shet.ravno());

if(data->rk->kod_val.ravno()[0] != '\0')
 fprintf(ff_sk.ff," %s:%s",gettext("Валюта"),data->rk->kod_val.ravno());
 
fprintf(ff_sk.ff,"\n\
%s %d.%d.%d%s  %s: %d.%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

int kolstr_sk=4;

int klst=1;
if(data->rk->kodgr.ravno()[0] != '\0')
 {
  fprintf(ff_obor.ff,"%s: %s\n",gettext("Список контрагентов"),data->rk->kodgr.ravno());
  iceb_printcod(ff_obor.ff,"Gkont","kod","naik",0,data->rk->kodgr.ravno(),&klst);

  fprintf(ff_gur_csv.ff,"%s: %s\n",gettext("Список контрагентов"),data->rk->kodgr.ravno());
  iceb_printcod(ff_gur_csv.ff,"Gkont","kod","naik",0,data->rk->kodgr.ravno(),&klst);

  klst=0;
  fprintf(ff_gur.ff,"%s: %s\n",gettext("Список контрагентов"),data->rk->kodgr.ravno());
  iceb_printcod(ff_gur.ff,"Gkont","kod","naik",0,data->rk->kodgr.ravno(),&klst);

  klst=0;
  fprintf(ff_sk.ff,"%s: %s\n",gettext("Список контрагентов"),data->rk->kodgr.ravno());
  iceb_printcod(ff_sk.ff,"Gkont","kod","naik",0,data->rk->kodgr.ravno(),&klst);
 }

kolstr_gur+=klst;
kolstr_obor+=klst;
kolstr_sk+=klst;
int kollist_gur=0;
int kollist_obor=0;
int kollist_sk=0;
/*Шапка оборотного баланса*/
sapgorsw(dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);
/*Шапка журнал-ордера*/
gosrsn_hapgo(dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
/*Шапка конечного сальдо*/
gossrsn_sapsk(dk,gk,mk,&kollist_sk,&kolstr_sk,ff_sk.ff);

//int metka_prop=0;
class iceb_u_str kod_kontr;
class iceb_u_str naim_kontr;
class iceb_u_str telef;
int grupa=0;
int grupaz=0;
double   sdgr=0.,skgr=0.,sdngr=0.,skngr=0.,dnsgr=0.,knsgr=0.;
double idebs=0.,ikres=0.;
double idebp=0.,ikrep=0.;
double idebk=0.,ikrek=0.;

sprintf(strsql,"%s=%d\n",gettext("Количество контрагентов за период"),kolih_kontr);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select * from %s %s",imatmptab,str_sort);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
sprintf(strsql,"%s=%d\n",gettext("Общее количество контрагентов"),kolstr);
iceb_printw(strsql,data->buffer,data->view);

double debs=0.,kres=0.,debp=0.,krep=0.;
nomer_kontr=0;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%6s %10s %10s %10s %10s\n",row[0],row[1],row[2],row[3],row[4]);  
//  OSTANOV();
  nomer_kontr++;

    
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  debs=atof(row[1]);
  kres=atof(row[2]);
  debp=atof(row[3]);
  krep=atof(row[4]);
  
  if(gorsrs_provk(row[0],debs,kres,debp,krep,imatmptab_hax,data->window) != 0)
   continue;

  sprintf(strsql,"select kodkon,naikon,telef,grup from Kontragent where kodkon='%s'",row[0]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    fprintf(ff_obor.ff,"\n\n%s %s!!!\n\n",gettext("Не найден код контрагента"),row[0]);
    fprintf(ff_gur.ff,"\n\n%s %s!!!\n\n",gettext("Не найден код контрагента"),row[0]);
    sprintf(strsql,"%s %s",gettext("Не найден код контрагента"),row[0]);
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  naim_kontr.new_plus(row1[1]);
  telef.new_plus(row1[2]);
  grupa=atoi(row1[3]); 

  if(data->metkasort == 3)
   if(grupaz != grupa)
    {
     if(grupaz != 0)
      {
     gosrsn_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);
     fprintf(ff_obor.ff,"\
 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
     class iceb_u_str naimgrup("");       
     sprintf(strsql,"select naik from Gkont where kod=%d",grupaz);
     if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
         naimgrup.new_plus(row1[0]);

     gosrsn_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);

     sprintf(strsql,"%s %d %s",gettext(" Итого по группе"),grupaz,naimgrup.ravno());
     fprintf(ff_obor.ff,"\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
     iceb_u_kolbait(31,strsql),iceb_u_kolbait(31,strsql),strsql,dnsgr,knsgr,sdngr,skngr,sdgr,skgr);

     /*****************************************/

     gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
     fprintf(ff_gur.ff,"\
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");


     sprintf(strsql,"%s %d %s",gettext(" Итого по группе"),grupaz,naimgrup.ravno());

     gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
     fprintf(ff_gur.ff,"\
%-*.*s %14.2f %14.2f %10s %14.2f %14.2f %14.2f %14.2f\n",
     iceb_u_kolbait(31,strsql),iceb_u_kolbait(31,strsql),strsql,dnsgr,knsgr," ",sdngr,skngr,sdgr,skgr);
      }

     sdgr=skgr=sdngr=skngr=dnsgr=knsgr=0.;
     grupaz=grupa;
    }

  deb=kre=0.;
/*************
  if(debs.ravno(nomer_kontr) > kres.ravno(nomer_kontr))
   deb=debs.ravno(nomer_kontr)-kres.ravno(nomer_kontr);
  else
   kre=kres.ravno(nomer_kontr)-debs.ravno(nomer_kontr);
****************/
  if(debs > kres)
   deb=debs-kres;
  else
   kre=kres-debs;

  gosrsn_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);
  gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
  gosrsn_ss(2,dn,mn,gn,dk,mk,gk,&kollist_sk,&kolstr_sk,ff_sk.ff);
  if(deb > 0.)
   {
    idebs+=deb;
    dnsgr+=deb;
    fprintf(ff_obor.ff,"%*s %-*.*s %14.2f %14s %14.2f %14.2f ",
    iceb_u_kolbait(10,row[0]),row[0],
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    deb," ",debp,krep);

    fprintf(ff_gur.ff,"%*s %-*.*s %14.2f %14s %10s %14.2f %14.2f ",
    iceb_u_kolbait(10,row[0]),row[0],
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    deb," "," ",debp,krep);

    fprintf(ff_gur_csv.ff,"%s|%s|%.2f|%s|%s|%.2f|%.2f|",
    row[0],
    naim_kontr.ravno(),
    deb," "," ",debp,krep);
   }
  else
   {
    ikres+=kre;
    knsgr+=kre;
    fprintf(ff_obor.ff,"%*s %-*.*s %14s %14.2f %14.2f %14.2f ",
    iceb_u_kolbait(10,row[0]),row[0],
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    " ",kre,debp,krep);

    fprintf(ff_gur.ff,"%*s %-*.*s %14s %14.2f %10s %14.2f %14.2f ",
    iceb_u_kolbait(10,row[0]),row[0],
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    " ",kre," ",debp,krep);

    fprintf(ff_gur_csv.ff,"%s|%s|%s|%.2f|%s|%.2f|%.2f|",
    row[0],
    naim_kontr.ravno(),
    " ",kre," ",debp,krep);
   }


  idebp+=debp;
  ikrep+=krep;

  deb=kre=0.;
  if(debs+debp > kres+krep)
   deb=debs+debp - (kres+krep);
  else
   kre=kres+krep-(debs+debp);

  if(deb > 0.)
   {
    idebk+=deb;
    sdgr+=deb;
    fprintf(ff_obor.ff,"%14.2f\n",deb);
    fprintf(ff_gur.ff,"%14.2f\n",deb);
    fprintf(ff_gur_csv.ff,"%.2f|\n",deb);

    fprintf(ff_sk.ff,"%*s %-*.*s %14.2f %14s %s\n",
    iceb_u_kolbait(10,row[0]),row[0],
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    deb," ",telef.ravno());


   }
  else
   {
    fprintf(ff_obor.ff,"%14s %14.2f\n"," ",kre);
    fprintf(ff_gur.ff,"%14s %14.2f\n"," ",kre);
    fprintf(ff_gur_csv.ff,"%s|%.2f|\n"," ",kre);

    fprintf(ff_sk.ff,"%*s %-*.*s %14s %14.2f %s\n",
    iceb_u_kolbait(10,row[0]),row[0],
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    " ",kre,telef.ravno());
    ikrek+=kre;
    skgr+=kre;
   }

  if(iceb_u_strlen(naim_kontr.ravno()) > 20)
   {
    gosrsn_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);
    fprintf(ff_obor.ff,"%-10s %.*s\n",
    " ",
    iceb_u_kolbait(110,iceb_u_adrsimv(20,naim_kontr.ravno())),
    iceb_u_adrsimv(20,naim_kontr.ravno()));

    gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
    fprintf(ff_gur.ff,"%-10s %.*s\n",
    " ",
    iceb_u_kolbait(120,iceb_u_adrsimv(20,naim_kontr.ravno())),
    iceb_u_adrsimv(20,naim_kontr.ravno()));

    gosrsn_ss(2,dn,mn,gn,dk,mk,gk,&kollist_sk,&kolstr_sk,ff_sk.ff);
    fprintf(ff_sk.ff,"%-10s %.*s\n",
    " ",
    iceb_u_kolbait(70,iceb_u_adrsimv(20,naim_kontr.ravno())),
    iceb_u_adrsimv(20,naim_kontr.ravno()));
   }

 for(int nomer_shet=0; nomer_shet < kolih_shet; nomer_shet++)
  {
   int nom_kontr_zp=atoi(row[8]);
   if(nom_kontr_zp == -1)
    break;
   deb=hach_kontr_shet_deb.ravno(nom_kontr_zp*kolih_shet+nomer_shet);
   kre=hach_kontr_shet_kre.ravno(nom_kontr_zp*kolih_shet+nomer_shet);
   
   if(fabs(deb) < 0.009 && fabs(kre) < 0.009)
    continue;
   if(metka_naim_shet == 1)
     {
      class iceb_u_str naim_shet_kor("");
      sprintf(strsql,"select nais from Plansh where ns='%s'",oss.ravno(nomer_shet));
      if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
       naim_shet_kor.new_plus(row1[0]);

      int razmer_naim=71-iceb_u_strlen(oss.ravno(nomer_shet));

      sprintf(strsql,"%.*s %s",
      iceb_u_kolbait(razmer_naim,naim_shet_kor.ravno()),
      naim_shet_kor.ravno(),oss.ravno(nomer_shet));

     }
   else
      sprintf(strsql,"%s",oss.ravno(nomer_shet));
      
   gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);

   fprintf(ff_gur.ff,"%*.*s %14.2f %14.2f\n",
   iceb_u_kolbait(72,strsql),iceb_u_kolbait(72,strsql),strsql,deb,kre);

   fprintf(ff_gur_csv.ff,"||||%s|%.2f|%.2f|\n",
   strsql,deb,kre);


  }

  gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
  fprintf(ff_gur.ff,"\
.....................................................................................................................................\n");

  sdngr+=debp;
  skngr+=krep;
 }

if(data->metkasort == 3)
  {
     gosrsn_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);
     fprintf(ff_obor.ff,"\
 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
     class iceb_u_str naimgrup("");       
     sprintf(strsql,"select naik from Gkont where kod=%d",grupa);
     if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
         naimgrup.new_plus(row1[0]);

     gosrsn_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);

     sprintf(strsql,"%s %d %s",gettext(" Итого по группе"),grupa,naimgrup.ravno());
     fprintf(ff_obor.ff,"\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
     iceb_u_kolbait(31,strsql),iceb_u_kolbait(31,strsql),strsql,
     dnsgr,knsgr,sdngr,skngr,sdgr,skgr);

     /*****************************************/

     gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
     fprintf(ff_gur.ff,"\
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");


     sprintf(strsql,"%s %d %s",gettext(" Итого по группе"),grupa,naimgrup.ravno());

     gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
     fprintf(ff_gur.ff,"\
%-*.*s %14.2f %14.2f %10s %14.2f %14.2f %14.2f %14.2f\n",
     iceb_u_kolbait(31,strsql),iceb_u_kolbait(31,strsql),strsql,
     dnsgr,knsgr," ",sdngr,skngr,sdgr,skgr);

  }

fprintf(ff_obor.ff,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
iceb_u_kolbait(31,gettext(" И т о г о")),iceb_u_kolbait(31,gettext(" И т о г о")),gettext(" И т о г о"),idebs,ikres,idebp,ikrep,idebk,ikrek);

fprintf(ff_gur_csv.ff,"\
|%s|%.2f|%.2f|%s|%.2f|%.2f|%.2f|%.2f|\n",
gettext(" И т о г о"),idebs,ikres," ",idebp,ikrep,idebk,ikrek);

fprintf(ff_obor.ff,"\n%s__________________________\n",
gettext("Главный бухгалтер"));

iceb_podpis(ff_obor.ff,data->window);
ff_obor.end();

iceb_podpis(ff_gur_csv.ff,data->window);
ff_gur_csv.end();


fprintf(ff_gur.ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n\
%-*.*s %14.2f %14.2f %10s %14.2f %14.2f %14.2f %14.2f\n",
iceb_u_kolbait(31,gettext(" И т о г о")),iceb_u_kolbait(31,gettext(" И т о г о")),gettext(" И т о г о"),idebs,ikres," ",idebp,ikrep,idebk,ikrek);

gosrsn_itoggo(data->rk->shet.ravno(),dn,mn,gn,dk,mk,gk,naim_shet.ravno(),&oss,imatmptab_hax,ff_gur.ff,data->window);

fprintf(ff_gur.ff,"\n%s__________________________\n",
gettext("Главный бухгалтер"));
iceb_podpis(ff_gur.ff,data->window);

ff_gur.end();

fprintf(ff_sk.ff,"\
----------------------------------------------------------------------------------\n");
fprintf(ff_sk.ff,"%-*.*s %14.2f %14.2f\n",
iceb_u_kolbait(31,gettext(" И т о г о")),iceb_u_kolbait(31,gettext(" И т о г о")),gettext(" И т о г о"),idebk,ikrek);

fprintf(ff_sk.ff,"\n%s__________________________\n",
gettext("Главный бухгалтер"));
iceb_podpis(ff_sk.ff,data->window);
ff_sk.end();



/*Распечатка по дням*/
sprintf(strsql,"%s\n",gettext("Распечатка по дням в формате А4"));
iceb_printw(strsql,data->buffer,data->view);

/*распечатка в формате А4*/
char imaf_dn[64];
sprintf(imaf_dn,"gopd%d.lst",getpid());
gorsrsn_podn(data->rk->shet.ravno(),naim_shet.ravno(),dn,mn,gn,dk,mk,gk,&ssd,&ssk,idebs,ikres,idebp,ikrep,idebk,ikrek,0,data->rk->kod_val.ravno(),imatmptab_den,imaf_dn,data->window);

sprintf(strsql,"%s\n",gettext("Распечатка по дням в формате А3"));
iceb_printw(strsql,data->buffer,data->view);
/*распечатка в формате А3*/
char imaf_dn_a3[64];
sprintf(imaf_dn_a3,"gopda%d.lst",getpid());
gorsrsn_podn(data->rk->shet.ravno(),naim_shet.ravno(),dn,mn,gn,dk,mk,gk,&ssd,&ssk,idebs,ikres,idebp,ikrep,idebk,ikrek,1,data->rk->kod_val.ravno(),imatmptab_den,imaf_dn_a3,data->window);

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace",NULL);


sprintf(strsql,"%29s %*s %*s\n","",
iceb_u_kolbait(20,gettext("Дебет")),
gettext("Дебет"),
iceb_u_kolbait(20,gettext("Кредит")),
gettext("Кредит"));

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"%*s %02d.%02d.%04d:%20s",iceb_u_kolbait(18,gettext("Сальдо на")),gettext("Сальдо на"),dn,mn,gn,iceb_u_prnbr(idebs));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql," %20s\n",iceb_u_prnbr(ikres));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"%*s:%20s",iceb_u_kolbait(29,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(idebp));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql," %20s\n",iceb_u_prnbr(ikrep));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"%*s %02d.%02d.%04d:%20s",iceb_u_kolbait(18,gettext("Сальдо на")),gettext("Сальдо на"),dk,mk,gk,iceb_u_prnbr(idebk));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql," %20s\n",iceb_u_prnbr(ikrek));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

if(idebk - ikrek > 0)
 {
  sprintf(strsql,"%*s:%20s\n",iceb_u_kolbait(29,gettext("Сальдо свёрнутое")),gettext("Сальдо свёрнутое"),iceb_u_prnbr(idebk-ikrek));
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
 }
else
 {
  sprintf(strsql,"%*s:%20s %20s\n",iceb_u_kolbait(29,gettext("Сальдо свёрнутое")),gettext("Сальдо свёрнутое"),"",iceb_u_prnbr(ikrek-idebk));
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
 }

data->rk->imaf.new_plus(imaf_obor);

sprintf(strsql,"%s %s",gettext("Оборотный баланс по счёту"),data->rk->shet.ravno());
data->rk->naimf.new_plus(strsql);

data->rk->imaf.plus(imaf_gur);
sprintf(strsql,"%s %s",gettext("Журнал-ордер по счёту"),data->rk->shet.ravno());
data->rk->naimf.plus(strsql);

data->rk->imaf.plus(imaf_sk);
sprintf(strsql,"%s %s",gettext("Конечное сальдо по счёту"),data->rk->shet.ravno());
data->rk->naimf.plus(strsql);


data->rk->imaf.plus(imaf_dn);
data->rk->naimf.plus(gettext("Распечатка по дням А4"));

data->rk->imaf.plus(imaf_dn_a3);
data->rk->naimf.plus(gettext("Распечатка по дням А3"));

for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),3,data->window);

data->rk->imaf.plus(imaf_gur_csv);
data->rk->naimf.plus(gettext("Журнал для загрузки в электронные таблицы"));




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
