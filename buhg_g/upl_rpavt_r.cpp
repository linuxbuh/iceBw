/*$Id: upl_rpavt_r.c,v 1.15 2013/09/26 09:46:58 sasa Exp $*/
/*19.06.2020	31.03.2008	Белых А.И.	upl_rpavt_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "upl_rpavt.h"

class upl_rpavt_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class upl_rpavt_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  upl_rpavt_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   upl_rpavt_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_rpavt_r_data *data);
gint upl_rpavt_r1(class upl_rpavt_r_data *data);
void  upl_rpavt_r_v_knopka(GtkWidget *widget,class upl_rpavt_r_data *data);

double upl_ptw(short dn,short mn,short gn,short dk,short mk,short gk,char *kod_avt,char *kod_vod,GtkWidget *wpredok);


extern SQL_baza bd;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int upl_rpavt_r(class upl_rpavt_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class upl_rpavt_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка реестра путевых листов по автомобилям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(upl_rpavt_r_key_press),&data);

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

repl.plus(gettext("Распечатка реестра путевых листов по автомобилям"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(upl_rpavt_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)upl_rpavt_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upl_rpavt_r_v_knopka(GtkWidget *widget,class upl_rpavt_r_data *data)
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

gboolean   upl_rpavt_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_rpavt_r_data *data)
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
/*************************/
/*Смотрим от дня последней путёвки в периоде до конца заданного периода - не было ли передач топлива*/
/********************************************/
double uplrpla_pt(short dpp,short mpp,short gpp,
short dk,short mk,short gk,
const char *kodavt,
GtkWidget *wpredok)
{
SQL_str row;
SQLCURSOR cur;
int kolstr;
char strsql[512];
double suma=0.;
//short d,m,g;
double isuma=0.;

sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd >= '%d-%d-%d' \
and datd <= '%d-%d-%d' and kap=%s and tz=1 and ka <> '%s'",
gpp,mpp,dpp,gk,mk,dk,kodavt,kodavt);

//printw("strsql=%s\n",strsql);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s\n",row[0],row[1],row[2]);  
  suma=atof(row[2]);
  isuma+=suma;
/*****************
  rsdat(&d,&m,&g,row[0],2);
  
  fprintf(ff_vse,"%*s %02d.%02d.%04d %118s %8.8g %7.7g\n",
  iceb_tu_kolbait(7,row[1]),row[1],d,m,g," ",suma,ost2-isuma);
*************/
 }

return(isuma);

}

/*******************************/
/*Смотрим былали передача топлива между предыдущей путёвкой и настоящей*/
/****************************************/
double uplrpla_per_top_avt(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodavt,
GtkWidget *wpredok)
{
SQL_str row;
SQLCURSOR cur;
int kolstr;
char strsql[512];
double suma=0.;
//short d,m,g;
double isuma=0.;


sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd >= '%d-%d-%d' and \
datd < '%d-%d-%d' and kap=%s and tz=1 and ka <> '%s'",
gn,mn,dn,gk,mk,dk,kodavt,kodavt);

//printw("strsql=%s\n",strsql);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s\n",row[0],row[1],row[2]);  
  suma=atof(row[2]);
  isuma+=suma;
/***********
  rsdat(&d,&m,&g,row[0],2);
  
  fprintf(ff_vse,"%*s %02d.%02d.%04d %118s %8.8g %7.7g\n",
  iceb_tu_kolbait(7,row[1]),row[1],d,m,g," ",suma,ost2-isuma);
****************/
 }

return(isuma);

}


/******************************************/
/******************************************/

gint upl_rpavt_r1(class upl_rpavt_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1,row2;
class SQLCURSOR cur,cur1,cur2;


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

iceb_printw(strsql,data->buffer,data->view);





int kolstr=0;

sprintf(strsql,"select distinct ka from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d'",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Нет записей по заданным реквизитам поиска !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

class iceb_u_spisok avtomobil;

float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kod_avt.ravno(),row[0],0,0) != 0)
   continue;

  avtomobil.plus(row[0]);

 }

char imaf[64];
FILE *ff;

sprintf(imaf,"rplpa%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_pl[64];
FILE *ff_pl;

sprintf(imaf_pl,"rplpap%d.lst",getpid());

if((ff_pl = fopen(imaf_pl,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_pl,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_str kodav("");
class iceb_u_str naimavt("");
class iceb_u_str rnavt("");
int kol=avtomobil.kolih();
float kolstr2=0;
for(int nomer=0; nomer < kol; nomer++)
 {
//  strzag(LINES-1,0,kol,++kolstr2);
  iceb_pbar(data->bar,kol,++kolstr2);    

  kodav.new_plus(avtomobil.ravno(nomer));

  class iceb_u_spisok KZMT; //Код заправки|марка топлива
  class iceb_u_double SMT;

  //Читаем автомобиль
  rnavt.new_plus("");
  naimavt.new_plus("");
  sprintf(strsql,"select nomz,naik from Uplavt where kod=%s",kodav.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)  
   {
    rnavt.new_plus(row[0]);
    naimavt.new_plus(row[1]);
   }

  if(nomer != 0)
    fprintf(ff,"\f");

  iceb_zagolov(gettext("Реестр путевых листов по автомобилям"),dn,mn,gn,dk,mk,gk,ff,data->window);

  fprintf(ff,"\n\
%s %s %s %s %s\n",
  gettext("Автомобиль"),
  kodav.ravno(),
  naimavt.ravno(),
  gettext("регистрационный номер"),
  rnavt.ravno());

  fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("\
Номер  |  Дата    |Показ.спидом.|Пробег|Остаток|Приня- |Получе-|Затраты|Переда-|Остаток|\n\
п.листа|          |выезд |возвр.|      |       |  то   |  но   |       |  но   |       |\n"));
//1        2         3      4      5      6        7         8     9      10     11    
  fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

   iceb_zagolov(gettext("Путевые листы"),dn,mn,gn,dk,mk,gk,ff_pl,data->window);

   fprintf(ff_pl,"\n\
%s: %s %s %s %s\n",
  gettext("Автомобиль"),
  kodav.ravno(),
  naimavt.ravno(),
  gettext("регистрационный номер"),
  rnavt.ravno());




  sprintf(strsql,"select datd,kp,nomd,psv,psz,kv,nz,otmet,denn,denk,vremn,vremk,nomb from Upldok where \
datd >= '%d-%d-%d' and datd <= '%d-%d-%d' and ka=%s order by datd,nomd asc",
  gn,mn,dn,gk,mk,dk,kodav.ravno()); 

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  
  sprintf(strsql,"\n%s=%s\n",
  gettext("Код автомобиля"),
  kodav.ravno());
  
  iceb_printw(strsql,data->buffer,data->view);

  short dd=0,md=0,gd=0;  
  short dp=0,mp=0,gp=0;  
  int nom_zap=0;
  double iperedano=0.;
  double ost1=0.,ost2=0.;
  double iprin=0.;
  double ipoluh=0.;
  double irash=0.;
  int itogo_probeg=0;
  class iceb_u_str naimvod("");
  while(cur.read_cursor(&row) != 0)
   {
    if(iceb_u_proverka(data->rk->kod_pod.ravno(),row[1],0,0) != 0)
     continue;
    if(iceb_u_proverka(data->rk->kod_vod.ravno(),row[5],0,0) != 0)
     continue;
   //Читаем водителя
   naimvod.new_plus("");
   sprintf(strsql,"select naik from Uplouot where kod=%s",row[5]);
   if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)  
     naimvod.new_plus(row1[0]);

    iceb_u_rsdat(&dd,&md,&gd,row[0],2);
    nom_zap=atoi(row[6]);

    sprintf(strsql,"%02d.%02d.%d %2s %s\n",dd,md,gd,row[1],row[2]);
  
    iceb_printw(strsql,data->buffer,data->view);
/************************
   Несколько передач топлива с одной машины на другие в течение одного дня не должно быть
   а если будут то пусть она их все покажет после первого путевого листа в общей сумме
*********************/
    if(dp != 0 && iceb_u_sravmydat(dp,mp,gp,dd,md,gd) != 0)
     {
      //смотрим былали передача топлива между двумя путевыми листами
      iperedano+=uplrpla_per_top_avt(dp,mp,gp,dd,md,gd,kodav.ravno(),data->window);
     }
    dp=dd;
    mp=md;
    gp=gd;

    sprintf(strsql,"select kodsp,kodtp,kolih,tz,kap,ka from Upldok1 where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);

    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
      continue;
     }
    ost1=ostvaw(dd,md,gd,"",kodav.ravno(),nom_zap,NULL,NULL,NULL,data->window);

    fprintf(ff_pl,"\n%s:%02d.%02d.%d %s:%s\n",gettext("Дата"),dd,md,gd,gettext("Номер"),row[2]);

    fprintf(ff_pl,"%s:%.10g %s:%.10g %s:%.10g\n",
    gettext("Показания спидометра при выезде"),atof(row[3]),
    gettext("Показания спидометра при возвращении"),atof(row[4]),
    gettext("Пробег"),atof(row[4])-atof(row[3]));

    fprintf(ff_pl,"%s:%10s %s:%s\n",gettext("Дата выезда"),iceb_u_datzap(row[8]),gettext("Время выезда"),row[10]);    
    fprintf(ff_pl,"%s:%10s %s:%s\n",gettext("Дата возвращения"),iceb_u_datzap(row[9]),gettext("Время возвращения"),row[11]);    
    fprintf(ff_pl,"%s:%10s %s:%s\n",gettext("Номер бланка"),row[12],gettext("Отметки"),row[7]);
    fprintf(ff_pl,"%s:%s %s\n",gettext("Водитель"),row[5],naimvod.ravno());
    fprintf(ff_pl,"%*.*s:%10.10g\n",
    iceb_u_kolbait(10,gettext("Сальдо")),
    iceb_u_kolbait(10,gettext("Сальдо")),
    gettext("Сальдо"),
    ost1);

    fprintf(ff_pl,"\
--------------------------------------\n");
    fprintf(ff_pl,"\
+/-|Код т.|Количество|Поставщик\n");
/*
123 123456 1234567890 
*/
    fprintf(ff_pl,"\
--------------------------------------\n");

    int probeg=0;
    double iprobeg=0.;
    double kolih=0.;
    double kolihi=0.;
    double kolspis=0;
    double prin=0.,poluh=0.;    
    int nomer1=0;
    char plus_minus[16];
    while(cur1.read_cursor(&row1) != 0)
     {
//      printw("*****%s %s %s %s %s %s\n",row1[0],row1[1],row1[2],row1[3],row1[4],row1[5]);
      if(row1[3][0] == '1')
       {
        strcpy(plus_minus,"+");     
        
        kolih=atof(row1[2]);

        if(atoi(row1[4]) != 0)
         prin+=kolih;
        else
         poluh+=kolih;
         
        kolihi+=kolih;
        sprintf(strsql,"%s|%s",row1[0],row1[1]);

        if((nomer1=KZMT.find(strsql)) < 0)
          KZMT.plus(strsql);
        SMT.plus(kolih,nomer1);
       }
      else
       {
        strcpy(plus_minus,"-");     
        kolspis+=atof(row1[2]);
       }
     
      memset(strsql,'\0',sizeof(strsql));
      if(atoi(row1[0]) != 0)
       {
        if(atoi(row1[4]) == 0)
          sprintf(strsql,"select naik from Uplouot where kod=%s and mt=0",row1[0]); //заправка
        else
          sprintf(strsql,"select naik from Uplouot where kod=%s and mt=1",row1[0]); //водитель

        if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) == 1)
          sprintf(strsql,"%s %s",row1[0],row2[0]);
        else
          sprintf(strsql,"%s",row1[0]);
       }

      fprintf(ff_pl,"%3s %*s %10.10g %s\n",
      plus_minus,
      iceb_u_kolbait(6,row1[1]),
      row1[1],
      atof(row1[2]),
      strsql);    

     }
    iprin+=prin;
    ipoluh+=poluh;
    
    ost2=ost1+kolihi-kolspis;
    ost2=iceb_u_okrug(ost2,0.001);

    fprintf(ff_pl,"\
--------------------------------------\n");


    fprintf(ff_pl,"%*.*s:%10.10g\n",
    iceb_u_kolbait(10,gettext("Сальдо")),
    iceb_u_kolbait(10,gettext("Сальдо")),
    gettext("Сальдо"),
    ost2);
    
    iprobeg+=atof(row[6]);
    irash+=kolspis;



    probeg=atoi(row[4])-atoi(row[3]);
    itogo_probeg+=probeg;
    


    fprintf(ff,"%*s %02d.%02d.%d %6s %6s \
%6d %7.7g %7.7g %7.7g %7.7g %7s %7.7g %s\n",
    iceb_u_kolbait(7,row[2]),row[2],
    dd,md,gd,
    row[3],
    row[4],
    probeg,
    ost1,
    prin,
    poluh,
    kolspis,
    " ",
    ost2,
    row[7]);

   fprintf(ff,"\
 ----------------------------------------------------------------------------------------\n");
   fprintf(ff,"%*s %6d %7s %7.7g %7.7g %7.7g %7.7g\n",
   iceb_u_kolbait(32,gettext("Итого")),gettext("Итого"),
   itogo_probeg,
   "",
   iprin,
   ipoluh,
   irash,
   iperedano);


     fprintf(ff_pl,"%s\n",gettext("Расчёт списания топлива по нормам"));
     fprintf(ff_pl,"\
---------------------------------------------------------------------------------------------------------------------------------------\n");
     fprintf(ff_pl,gettext("\
Код н.с.|  Наименование нормы списания |Ед.изм. |Количество|Норма списан.|Затраты по норме|Затраты по факту| Разница  |Коментарий\n"));
 /*************
 12345678 123456789012345678901234567890 12345678 1234567890 1234567890123 1234567890123456 1234567890123456 1234567890
 *******************/
     fprintf(ff_pl,"\
---------------------------------------------------------------------------------------------------------------------------------------\n");

     /*смотрим расчёт списания*/
     sprintf(strsql,"select knst,kl,nst,zn,zf,ei,kom from Upldok3 where datd='%s' and kp=%s \
   and nomd='%s'",row[0],row[1],row[2]);

     if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
         iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
     
     class iceb_u_str naim_nst("");
     
     double itogo_zt[3];
     memset(itogo_zt,'\0',sizeof(itogo_zt));
     
     while(cur1.read_cursor(&row1) != 0)
      {
       naim_nst.new_plus("");
       sprintf(strsql,"select naik from Uplnst where kod='%s'",row1[0]);
       if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) == 1)
        naim_nst.new_plus(row2[0]);    
       fprintf(ff_pl,"%-*s %-*.*s %-*s %10.10g %13.13g %16.16g %16.16g %10.10g %s\n",
       iceb_u_kolbait(8,row1[0]),
       row1[0],
       iceb_u_kolbait(30,naim_nst.ravno()),
       iceb_u_kolbait(30,naim_nst.ravno()),
       naim_nst.ravno(),
       iceb_u_kolbait(8,row1[5]),
       row1[5],
       atof(row1[1]),
       atof(row1[2]),
       atof(row1[3]),
       atof(row1[4]),
       atof(row1[4])-atof(row1[3]),
       row1[6]);    

       itogo_zt[0]+=atof(row1[3]);
       itogo_zt[1]+=atof(row1[4]);
       itogo_zt[2]+=atof(row1[4])-atof(row1[3]);
      }
     fprintf(ff_pl,"\
---------------------------------------------------------------------------------------------------------------------------------------\n");

     fprintf(ff_pl,"%*s:%16.16g %16.16g %10.10g\n",
     iceb_u_kolbait(73,gettext("Итого")),
     gettext("Итого"),
     itogo_zt[0],itogo_zt[1],itogo_zt[2]);
    
   }

  if(dd != 0)
   iperedano+=uplrpla_pt(dd,md,gd,dk,mk,gk,kodav.ravno(),data->window);
   
    



  int kol1=KZMT.kolih();
  class iceb_u_str kodvd("");

  fprintf(ff,"\
-----------------------------------------------------\n");
  fprintf(ff,gettext("\
Код|Наименование поставщика|Марка топлива|Количество|\n"));
  fprintf(ff,"\
-----------------------------------------------------\n");

  for(int nomer1=0; nomer1 < kol1; nomer1++)
    {
     iceb_u_polen(KZMT.ravno(nomer1),&kodvd,1,'|');
     iceb_u_polen(KZMT.ravno(nomer1),&kodav,2,'|');

     //Читаем поставщика
     naimvod.new_plus("");
     sprintf(strsql,"select naik from Uplouot where kod=%s",kodvd.ravno());
     if(iceb_sql_readkey(strsql,&row,&cur1,data->window) == 1)  
       naimvod.new_plus(row[0]);

     fprintf(ff,"%3s %-*.*s %-13.13s %10.10g\n",
     kodvd.ravno(),
     iceb_u_kolbait(23,naimvod.ravno()),iceb_u_kolbait(23,naimvod.ravno()),naimvod.ravno(),
     kodav.ravno(),SMT.ravno(nomer1));  
    }

  fprintf(ff,"\
-----------------------------------------------------\n");

  fprintf(ff,"\n%s__________________\n",gettext("Бухгалтер"));


 }

iceb_podpis(ff,data->window);
fclose(ff);
iceb_podpis(ff_pl,data->window);
fclose(ff_pl);







data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Реестр путевых листов по автомобилям"));
data->rk->imaf.plus(imaf_pl);
data->rk->naim.plus(gettext("Путевые листы"));

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
