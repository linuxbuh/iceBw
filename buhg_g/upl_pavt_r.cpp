/*$Id: upl_pavt_r.c,v 1.18 2013/09/26 09:46:58 sasa Exp $*/
/*23.05.2016	31.03.2008	Белых А.И.	upl_pavt_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "upl_pavt.h"

class upl_pavt_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class upl_pavt_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  upl_pavt_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   upl_pavt_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_pavt_r_data *data);
gint upl_pavt_r1(class upl_pavt_r_data *data);
void  upl_pavt_r_v_knopka(GtkWidget *widget,class upl_pavt_r_data *data);

double upl_ptw(short dn,short mn,short gn,short dk,short mk,short gk,const char *kod_avt,const char *kod_vod,GtkWidget *wpredok);


extern SQL_baza bd;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int upl_pavt_r(class upl_pavt_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class upl_pavt_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка оборотной ведомости движения топлива по автомобилям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(upl_pavt_r_key_press),&data);

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

repl.plus(gettext("Распечатка оборотной ведомости движения топлива по автомобилям"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(upl_pavt_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)upl_pavt_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upl_pavt_r_v_knopka(GtkWidget *widget,class upl_pavt_r_data *data)
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

gboolean   upl_pavt_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_pavt_r_data *data)
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
/**************************/
/*Шапка                   */
/**************************/

void hpov(short dn,short mn,short gn,
short dk,short mk,short gk,
FILE *ff)
{
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
           Автомобиль          |      Водитель      | Остаток  |           Движение топлива за период      | Остаток  |\n\
      Марка    |Государс. номер|                    |%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------\n");


}
/******************/
/*Надпись         */
/******************/

void uplovdt_n(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodvod,
const char *kodavt,
const char *podr,
const char *nadp,
FILE *ff,
GtkWidget *wpredok)
{
iceb_zagolov(nadp,dn,mn,gn,dk,mk,gk,ff,wpredok);

if(kodvod[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Водитель"),kodvod);
if(kodavt[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Автомобиль"),kodavt);
if(podr[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Подразделение"),podr);
}


/******************************************/
/******************************************/

gint upl_pavt_r1(class upl_pavt_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;


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
//Определяем список автомобилей

sprintf(strsql,"select distinct kv,ka from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' order by ka,kv asc",gn,mn,dn,gk,mk,dk);

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
class iceb_u_spisok KVKA;  //код водителя|код автомобиля
class iceb_u_int kod_avt;  //коды автомобилей
#define OST_AVTOMOBIL 5
class iceb_u_double ost_avt[OST_AVTOMOBIL]; //остатки по автомобилям

class iceb_u_int kod_vod;  //коды водителей
#define OST_VODITEL 5
class iceb_u_double ost_vod[OST_VODITEL]; //остатки по водителям


float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kod_vod.ravno(),row[0],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->kod_avt.ravno(),row[1],0,0) != 0)
   continue;

  sprintf(strsql,"%s|%s",row[0],row[1]);
  KVKA.plus(strsql);
  
  if(kod_avt.find(atoi(row[1])) < 0)
   kod_avt.plus(atoi(row[1]),-1);

  if(kod_vod.find(atoi(row[0])) < 0)
   kod_vod.plus(atoi(row[0]),-1);
 }

kolstr1=kod_avt.kolih();

for(int i=0 ; i < OST_AVTOMOBIL; i++)
 ost_avt[i].make_class(kolstr1);

kolstr1=kod_vod.kolih();

for(int i=0 ; i < OST_VODITEL; i++)
 ost_vod[i].make_class(kolstr1);

char imaf[64];
FILE *ff;

sprintf(imaf,"rpl%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


uplovdt_n(dn,mn,gn,dk,mk,gk,data->rk->kod_vod.ravno(),data->rk->kod_avt.ravno(),data->rk->kod_pod.ravno(),gettext("Оботротная ведомость движения топлива по автомобилям"),ff,data->window);



hpov(dn,mn,gn,dk,mk,gk,ff);

int kol=KVKA.kolih();
class iceb_u_str kodvd("");
class iceb_u_str kodav("");
class iceb_u_str naimvod("");
class iceb_u_str naimavt("");
class iceb_u_str rnavt("");
double kolih=0.;


double dvig[4];
memset(&dvig,'\0',sizeof(dvig));
double idvig[4];
memset(&idvig,'\0',sizeof(idvig));
int nomer_avt=0;
class iceb_u_str nomdok("");
double peredano=0.;
double iperedano=0.;
float kolstr2=0;
for(int nomer=0; nomer < kol; nomer++)
 {

  iceb_u_polen(KVKA.ravno(nomer),&kodvd,1,'|');
  iceb_u_polen(KVKA.ravno(nomer),&kodav,2,'|');

  sprintf(strsql,"\n%s=%s %s=%s\n",
  gettext("Код водителя"),
  kodvd.ravno(),
  gettext("Код автомобиля"),
  kodav.ravno());

  iceb_printw(strsql,data->buffer,data->view);

  iceb_pbar(data->bar,kolstr,++kolstr2);    
  
  //Читаем водителя
  naimvod.new_plus(kodvd.ravno());
  sprintf(strsql,"select naik from Uplouot where kod=%s",kodvd.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)  
   {
    naimvod.plus(" ",row[0]);
   }
  //Читаем автомобиль
  naimavt.new_plus(kodav.ravno());
  sprintf(strsql,"select nomz,naik from Uplavt where kod=%s",kodav.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)  
   {
    rnavt.new_plus(row[0]);
    naimavt.plus(" ",row[1]);
   }
  else
   rnavt.new_plus("");
   
  sprintf(strsql,"select kp,tz,kolih,datd,nomd,nz,kap from Upldok1 where datd >= '%d-%d-%d' \
and datd <= '%d-%d-%d' and kv=%s and ka=%s order by datd,nomd asc",
  gn,mn,dn,gk,mk,dk,kodvd.ravno(),kodav.ravno());

  if((kolstr1=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  memset(&dvig,'\0',sizeof(dvig));


  nomdok.new_plus("");
  int sh_str=0;
  while(cur.read_cursor(&row) != 0)
   {
    if(sh_str == 0)
     {
      dvig[0]=ostvaw(dn,mn,gn,kodvd.ravno(),kodav.ravno(),atoi(row[5]),NULL,NULL,NULL,data->window);
      sh_str=1;
     }
    if(iceb_u_proverka(data->rk->kod_pod.ravno(),row[0],0,0) != 0)
     continue;
    kolih=atof(row[2]);               
    if(row[1][0] == '1')
     {
      if(atoi(row[6]) != 0)
        dvig[1]+=kolih;      
      else
        dvig[2]+=kolih;      
     }
    else
     {
      dvig[3]+=kolih;      
     }
    if(iceb_u_SRAV(nomdok.ravno(),row[4],0) != 0)
     {
      nomdok.new_plus(row[4]); 
     }    
   }

  peredano=upl_ptw(dn,mn,gn,dk,mk,gk,kodav.ravno(),kodvd.ravno(),data->window);
  iperedano+=peredano;
  
  kolih=dvig[0]+dvig[1]+dvig[2]-dvig[3]-peredano;

  nomer_avt=kod_avt.find(kodav.ravno_atoi());
  
  ost_avt[0].plus(dvig[0],nomer_avt);
  ost_avt[1].plus(dvig[1],nomer_avt);
  ost_avt[2].plus(dvig[2],nomer_avt);
  ost_avt[3].plus(dvig[3],nomer_avt);
  ost_avt[4].plus(peredano,nomer_avt);

  nomer_avt=kod_vod.find(kodvd.ravno_atoi());

  ost_vod[0].plus(dvig[0],nomer_avt);
  ost_vod[1].plus(dvig[1],nomer_avt);
  ost_vod[2].plus(dvig[2],nomer_avt);
  ost_vod[3].plus(dvig[3],nomer_avt);
  ost_vod[4].plus(peredano,nomer_avt);

  
  fprintf(ff,"%-*.*s %-*.*s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  iceb_u_kolbait(15,naimavt.ravno()),iceb_u_kolbait(15,naimavt.ravno()),naimavt.ravno(),
  iceb_u_kolbait(15,rnavt.ravno()),iceb_u_kolbait(15,rnavt.ravno()),rnavt.ravno(),
  iceb_u_kolbait(20,naimvod.ravno()),iceb_u_kolbait(20,naimvod.ravno()),naimvod.ravno(),
  dvig[0],dvig[1],dvig[2],dvig[3],peredano,kolih);

  
  idvig[0]+=dvig[0];
  idvig[1]+=dvig[1];
  idvig[2]+=dvig[2];
  idvig[3]+=dvig[3];


 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------\n");

kolih=idvig[0]+idvig[1]+idvig[2]-idvig[3]-iperedano;

fprintf(ff,"\
%*s:%10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(52,gettext("Итого")),gettext("Итого"),
idvig[0],idvig[1],idvig[2],idvig[3],iperedano,kolih);


iceb_podpis(ff,data->window);
fclose(ff);



//Распечатка только по автомобилям
char imaf_avt[64];

sprintf(imaf_avt,"ostavt%d.lst",getpid());

if((ff = fopen(imaf_avt,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_avt,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

uplovdt_n(dn,mn,gn,dk,mk,gk,data->rk->kod_vod.ravno(),data->rk->kod_avt.ravno(),data->rk->kod_pod.ravno(),gettext("Оботротная ведомость движения топлива по автомобилям"),ff,data->window);




fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код |           Автомобиль          | Остаток  |    Движение топлива за период             | Остаток  |\n\
авт.|      Марка    |Государс. номер|%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
dn,mn,gn,dk,mk,gk);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");




kol=kod_avt.kolih();
int kavt;

for(int nomer=0; nomer < kol; nomer++)
 {
  kavt=kod_avt.ravno(nomer);
  //Читаем автомобиль
  
  sprintf(strsql,"select nomz,naik from Uplavt where kod=%d",kavt);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)  
   {
    rnavt.new_plus(row[0]);
    naimavt.new_plus(row[1]);
   }
  else
   {
    rnavt.new_plus("");
    naimavt.new_plus("");
   }
  kolih=ost_avt[0].ravno(nomer)+ost_avt[1].ravno(nomer)+ost_avt[2].ravno(nomer)-\
  ost_avt[3].ravno(nomer)-ost_avt[4].ravno(nomer);

  fprintf(ff,"%4d %-*s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  kavt,
  iceb_u_kolbait(15,naimavt.ravno()),naimavt.ravno(),
  iceb_u_kolbait(15,rnavt.ravno()),iceb_u_kolbait(15,rnavt.ravno()),rnavt.ravno(),
  ost_avt[0].ravno(nomer),
  ost_avt[1].ravno(nomer),
  ost_avt[2].ravno(nomer),
  ost_avt[3].ravno(nomer),
  ost_avt[4].ravno(nomer),
  kolih);


 }


iceb_podpis(ff,data->window);
fclose(ff);


//Распечатка только по водителям
char imaf_vod[64];

sprintf(imaf_vod,"ostvod%d.lst",getpid());

if((ff = fopen(imaf_vod,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_vod,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

uplovdt_n(dn,mn,gn,dk,mk,gk,data->rk->kod_vod.ravno(),data->rk->kod_avt.ravno(),data->rk->kod_pod.ravno(),gettext("Оботротная ведомость движения топлива по водителям"),ff,data->window);


fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код |       Фамилия водителя        | Остаток  |           Движение топлива за период      | Остаток  |\n\
вод.|                               |%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");



kol=kod_vod.kolih();

for(int nomer=0; nomer < kol; nomer++)
 {
  kavt=kod_vod.ravno(nomer);

  //Читаем водителя
  sprintf(strsql,"select naik from Uplouot where kod=%d",kavt);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)  
   {
    naimvod.new_plus(row[0]);
   }
  else
    naimvod.new_plus("");
  kolih=ost_vod[0].ravno(nomer)+ost_vod[1].ravno(nomer)+ost_vod[2].ravno(nomer)-ost_vod[3].ravno(nomer)-ost_vod[4].ravno(nomer);

  fprintf(ff,"%4d %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  kavt,
  iceb_u_kolbait(31,naimvod.ravno()),iceb_u_kolbait(31,naimvod.ravno()),naimvod.ravno(),
  ost_vod[0].ravno(nomer),
  ost_vod[1].ravno(nomer),
  ost_vod[2].ravno(nomer),
  ost_vod[3].ravno(nomer),
  ost_vod[4].ravno(nomer),
  kolih);


 }


iceb_podpis(ff,data->window);
fclose(ff);



data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Оборотная ведомость движения топлива по водителях и автомобилях"));

data->rk->imaf.plus(imaf_avt);
data->rk->naim.plus(gettext("Оборотная ведомость движения топлива по автомобилях"));


data->rk->imaf.plus(imaf_vod);
data->rk->naim.plus(gettext("Оборотная ведомость движения топлива по водителям"));



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
