/*$Id: saldozrw_r.c,v 1.13 2013/09/26 09:46:55 sasa Exp $*/
/*23.05.2016	10.03.2006	Белых А.И.	saldozrw_r.c
Расчёт отчёта 
*/
#include <math.h>
#include "buhg_g.h"

class saldozrw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  short godk;
  short godn;  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  saldozrw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   saldozrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class saldozrw_r_data *data);
gint saldozrw_r1(class saldozrw_r_data *data);
void  saldozrw_r_v_knopka(GtkWidget *widget,class saldozrw_r_data *data);


extern SQL_baza bd;
extern short startgodz;

int saldozrw_r(short godn,short godk,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class saldozrw_r_data data;

data.godk=godk;
data.godn=godn;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Перенос сальдо для подсистемы \"Зароботная плата\""));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(saldozrw_r_key_press),&data);

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

repl.plus(gettext("Перенос сальдо для подсистемы \"Зароботная плата\""));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(saldozrw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)saldozrw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldozrw_r_v_knopka(GtkWidget *widget,class saldozrw_r_data *data)
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

gboolean   saldozrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class saldozrw_r_data *data)
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

/**********************************/

gint saldozrw_r1(class saldozrw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;


sprintf(strsql,"Чистим %d.\n",data->godk);
iceb_printw(strsql,data->buffer,data->view);

/*Чистим год куда будут записаны сальдо*/
sprintf(strsql,"delete from Zsal where god=%d",data->godk);
iceb_sql_zapis(strsql,0,0,data->window);

int kolstr=0;
sprintf(strsql,"select tabn,fio from Kartb");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

float kolstr1=0;
double saldi=0.;
double sald=0.;
double saldb=0.;
long ttt=0;
/*Оператору может быть нужно перенести повторно сальдо с любого года на любой*/
short zapstargod=startgodz; /*Запоминаем установленный в системе стартовый год*/
startgodz=data->godn;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  ttt=atol(row[0]);
  sald=zarsaldw(1,1,data->godk,ttt,&saldb,data->window);

  if(fabs(sald) < 0.009)
     continue;

  sprintf(strsql,"%6s %-*.*s %8.2f\n",
  row[0],
  iceb_u_kolbait(30,row[1]),
  iceb_u_kolbait(30,row[1]),
  row[1],
  sald);
  iceb_printw(strsql,data->buffer,data->view);

  saldi+=sald;

  sprintf(strsql,"insert into Zsal values (%d,%ld,%.2f,%d,%ld,%.2f)",
  data->godk,ttt,sald,iceb_getuid(data->window),vremn,saldb);

  iceb_sql_zapis(strsql,0,0,data->window);

 }
startgodz=zapstargod; /*Возвращаем стартовый год*/

sprintf(strsql,"Итого %.2f\n",saldi);
iceb_printw(strsql,data->buffer,data->view);



short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
if(data->godk <= gt)
 {
  sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='zarnast.alx' and str like 'Стартовый год|%d%%'",
  data->godk,data->godn);

  if(iceb_sql_zapis(strsql,1,0,data->window) == 0)
   startgodz=data->godk;
 }





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}









