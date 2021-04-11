/*$Id: prognozw_r.c,v 1.21 2014/02/28 05:21:00 sasa Exp $*/
/*23.05.2016	01.05.2005	Белых А.И. 	prognozw_r.c
Расчёт прогноза реализации 
*/
#include <errno.h>
#include "buhg_g.h"
#include "prognozw.h"

class prognozw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class prognozw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  prognozw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   prognozw_r_key_press(GtkWidget *widget,GdkEventKey *event,class prognozw_r_data *data);
gint prognozw_r1(class prognozw_r_data *data);
void  prognozw_r_v_knopka(GtkWidget *widget,class prognozw_r_data *data);

void		rasnak(int skl,short d,short m,short g,char nn[],
double *suma,double *sumabn,double *sp,double *spbn,
short vt,double *sumkor,int lnds,FILE *ff,FILE *ffnp,double *sumsnds,
GtkWidget *wpredok);

void  sapitsh(FILE *ff);
void  rasoi(iceb_u_spisok *mssi,iceb_u_double *mspsi,FILE *ff);
void            rasis(char orgn[],double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,char naiskl[],char naik[],
iceb_u_spisok *mso,
iceb_u_double *mspo,
iceb_u_spisok *mss,
iceb_u_double *msps,
int*,int*,
FILE *ff);
void            rasid(double itd,short dn,short mn,short gn,short dk,
short mk,short gk,int skl1,char naiskl[],int*,int*,FILE *ff);

void            gsapr(short dn,short mn,short gn,short dk,short mk,
short gk,int *sl,int *kst,int skl1,char naiskl[],FILE *ff);

void rppvo1(int skl1,char *naiskl,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
FILE *ff);

void rasmasop(iceb_u_spisok *spopr,iceb_u_double *mkm,iceb_u_double *mkm1_1r,iceb_u_double *mkm1_1k,
short tip,FILE *ff,class prognozw_r_data *data);


extern SQL_baza bd;

int prognozw_r(class prognozw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class prognozw_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать прогноз реализации товаров"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(prognozw_r_key_press),&data);

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

repl.plus(gettext("Распечатать прогноз реализации товаров"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(prognozw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)prognozw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  prognozw_r_v_knopka(GtkWidget *widget,class prognozw_r_data *data)
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

gboolean   prognozw_r_key_press(GtkWidget *widget,GdkEventKey *event,class prognozw_r_data *data)
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

/******************************************/
/******************************************/

gint prognozw_r1(class prognozw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

SQLCURSOR cur;
SQL_str row;
class iceb_u_str naiskl("");

sprintf(strsql,"select naik from Sklad where kod=%d",data->rk->sklad.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код склада"),data->rk->sklad.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
naiskl.new_plus(row[0]);
/*Расчитываем количество дней в периоде*/

sprintf(strsql,"%d.%d.%d %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
iceb_printw(strsql,data->buffer,data->view);

int kold=iceb_u_period(dn,mn,gn,dk,mk,gk,0);

if(kold == 0)
 {
  iceb_menu_soob(gettext("Количество дней в периоде равно нолю"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kolstr2;
sprintf(strsql,"select kodm,naimat,kodgr from Material order by kodgr,naimat asc");
if((kolstr2=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



if(kolstr2 == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);

 }

struct tm *bf;
bf=localtime(&vremn);
char imaf[64];

sprintf(imaf,"prognoz%d.lst",getpid());

FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Прогноз реализации"));

sprintf(strsql,"%s %d.%d.%d %s %d.%d.%d %s %d %s %d\n",
gettext("Период с"),
dn,mn,gn,
gettext("по"),
dk,mk,gk,
gettext("Склад"),
data->rk->sklad.ravno_atoi(),
gettext("Количество дней"),
kold);

iceb_printw(strsql,data->buffer,data->view);



fprintf(ff,"\
%s\n\n%s %d %s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s - %d:%d\n",
iceb_get_pnk("00",0,data->window),
gettext("Склад"),
data->rk->sklad.ravno_atoi(),naiskl.ravno(),
gettext("Расчёт прогнозируемой реализации на основании реализации\nза период c"),
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

fprintf(ff,gettext("Эталонный период %d дней.\n"),kold);
fprintf(ff,gettext("Расчётный период %d дней.\n"),data->rk->kolden.ravno_atoi());
fprintf(ff,"\
-----------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 К/м | Наименование товара    | Остаток |Реализация|Средняя|Кол. |   Дата   |Спрогнозированый\n"));
fprintf(ff,gettext("\
     |                        |         |за период |за день|дней |          |завоз товара\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------------------------\n");
float kolstr1=0;
int kodm=0;
class iceb_u_str naimat("");
double ost;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
SQLCURSOR cur1;
SQL_str row1;
int kolstr;
double real;
double srzd;
int kdn;
short d,m,g;
char bros[512];
char koldneih[100];

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr2,++kolstr1);    
  
  kodm=atoi(row[0]);
  naimat.new_plus(row[1]);
  if(iceb_u_proverka(data->rk->kodmat.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kodgrmat.ravno(),row[2],0,0) != 0)
    continue;
  ost=ostdok1w(dt,mt,mt,data->rk->sklad.ravno_atoi(),kodm);


  sprintf(strsql,"select kolih from Dokummat1 where \
kodm=%d and sklad=%d and tipz=%d and \
datd >= '%d-%02d-%02d' and datd <= '%d-%02d-%02d'",
  kodm,data->rk->sklad.ravno_atoi(),2,gn,mn,dn,gk,mk,dk);
/*
  printw("strsql=%s\n",strsql);
*/    
  if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }
/*    printw("kolstr=%ld\n",kolstr);*/
  if(kolstr == 0)
    continue;

  real=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
/*  
    printw("%s %s\n",row1[0],row1[1]);
    refresh();
*/
    real+=atof(row1[0]);
   }

  
  srzd=real/kold;

  kdn=0;
  if(srzd > 0.)
    kdn=(int)(ost/srzd);        

  d=dt; m=mt; g=gt;
  for(int i=0; i < kdn; i++)
    iceb_u_dpm(&d,&m,&g,1);

  sprintf(strsql,"%-5d %-*.*s %9.9g %-10d %02d.%02d.%d\n",
  kodm,
  iceb_u_kolbait(24,naimat.ravno()),
  iceb_u_kolbait(24,naimat.ravno()),
  naimat.ravno(),
  ost,kdn,d,m,g);
  
  iceb_printw(strsql,data->buffer,data->view);
  
  if(kdn > 0)
    sprintf(bros,"%02d.%02d.%d",d,m,g);
  else
    strcpy(bros,"**********");

  koldneih[0]='\0';
  if(data->rk->kolden.ravno_atoi() != 0)
    sprintf(koldneih,"%.10g",data->rk->kolden.ravno_atoi()*srzd);
        
  fprintf(ff,"%-5d %-*.*s %9.9g %10.10g %7.2f %-5d %s %s\n",
  kodm,
  iceb_u_kolbait(24,naimat.ravno()),
  iceb_u_kolbait(24,naimat.ravno()),
  naimat.ravno(),
  ost,real,srzd,kdn,bros,koldneih);
    
 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------\n");
iceb_podpis(ff,data->window);

fclose(ff);

iceb_ustpeh(imaf,3,data->window);









gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
