/*$Id: spdnr_r.c,v 1.9 2013/08/13 06:10:25 sasa Exp $*/
/*24.05.2016	25.12.2007	Белых А.И.	spdnr_r.c
Расчёт формы 1ДФ
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "i_rest.h"


class spdnr_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class spdnr_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  spdnr_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   spdnr_r_key_press(GtkWidget *widget,GdkEventKey *event,class spdnr_r_data *data);
gint spdnr_r1(class spdnr_r_data *data);
void  spdnr_r_v_knopka(GtkWidget *widget,class spdnr_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *imabaz;


int spdnr_r(GtkWidget *wpredok)
{
char strsql[500];
iceb_u_str repl;
class spdnr_r_data data;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",name_system,gettext("Получение списка рождённых в текущий и следующий день"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(spdnr_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Получение списка рождённых в текущий и следующий день"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),imabaz,iceb_get_pnk("00",0,NULL));
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



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(spdnr_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)spdnr_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  spdnr_r_v_knopka(GtkWidget *widget,class spdnr_r_data *data)
{
printf("spdnr_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   spdnr_r_key_press(GtkWidget *widget,GdkEventKey *event,class spdnr_r_data *data)
{
 printf("spdnr_r_key_press\n");
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

gint spdnr_r1(class spdnr_r_data *data)
{
//struct  tm      *bf;
time_t vremn;
time(&vremn);
//bf=localtime(&vremn);
char strsql[1000];
class iceb_clock sss(data->window);



short dt,mt,gt;
short dt1,mt1,gt1;
iceb_u_poltekdat(&dt,&mt,&gt);
dt1=dt;
mt1=mt;
gt1=gt;
iceb_u_dpm(&dt1,&mt1,&gt1,1);

sprintf(strsql,"%02d.%02d.%d -> %02d.%02d.%d\n",dt,mt,gt,dt1,mt1,gt1);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s:\n",gettext("Клиенты"));
iceb_printw(strsql,data->buffer,data->view);
int kolstr=0;

class SQLCURSOR cur;
SQL_str row;
sprintf(strsql,"select kod,fio,denrog from Taxiklient where denrog >= '%04d-%02d-%02d' and denrog <= '%04d-%02d-%02d' \
order by denrog asc",gt,mt,dt,gt1,mt1,dt1);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  data->voz=1;
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  sprintf(strsql,"%s %-10s %s\n",iceb_u_datzap(row[2]),row[0],row[1]);
  iceb_printw(strsql,data->buffer,data->view);
 }

sprintf(strsql,"\n%s:\n",gettext("Служащие"));
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"select tabn,denrog from Kartb where denrog >= '%04d-%02d-%02d' and denrog <= '%04d-%02d-%02d' \
order by denrog asc",gt,mt,dt,gt1,mt1,dt1);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  data->voz=1;
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  sprintf(strsql,"%s %-10s %s\n",iceb_u_datzap(row[2]),row[0],row[1]);
  iceb_printw(strsql,data->buffer,data->view);
 }
 
data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
