/*$Id: clearzarw_r.c,v 1.14 2014/02/28 05:20:58 sasa Exp $*/
/*23.05.2016	06.11.2008	Белых А.И.	clearzarw_r.c
Удаление записей
*/
#include "buhg_g.h"

class clearzarw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  short mu;
  short gu;
  int kom;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  clearzarw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   clearzarw_r_key_press(GtkWidget *widget,GdkEventKey *event,class clearzarw_r_data *data);
gint clearzarw_r1(class clearzarw_r_data *data);
void  clearzarw_r_v_knopka(GtkWidget *widget,class clearzarw_r_data *data);


extern SQL_baza bd;

int clearzarw_r(int kom,short mu,short gu,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
char strsql[512];
class iceb_u_str repl;
class clearzarw_r_data data;

data.mu=mu;
data.gu=gu;
data.kom=kom;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Удаление записей"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(clearzarw_r_key_press),&data);

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

repl.plus(gettext("Удаление записей"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(clearzarw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)clearzarw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  clearzarw_r_v_knopka(GtkWidget *widget,class clearzarw_r_data *data)
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

gboolean   clearzarw_r_key_press(GtkWidget *widget,GdkEventKey *event,class clearzarw_r_data *data)
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

gint clearzarw_r1(class clearzarw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;



 
strcpy(strsql,"select VERSION()");
iceb_sql_readkey(strsql,&row,&cur,NULL);
int metkazap=0;
if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
printf("VERSION:%s\n",row[0]);

if(data->kom == 0)
  sprintf(strsql,"%s %d%s\n",
  gettext("Удаление записей за"),
  data->gu,gettext("г."));
if(data->kom == 1)
  sprintf(strsql,"%s %d.%d%s\n",
  gettext("Удаление записей за"),
  data->mu,data->gu,gettext("г."));
if(data->kom == 2)
  sprintf(strsql,"%s.\n",
  gettext("Удаление записей с нулевым значением"));

iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s\n",gettext("Удаляем записи начислений и удержаний"));

iceb_printw(strsql,data->buffer,data->view);
if(data->kom == 0)
 sprintf(strsql,"delete from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31'",data->gu,data->mu,data->gu,data->mu);
if(data->kom == 1)
 sprintf(strsql,"delete from Zarp where datz >= '%04d-01-01' and \
datz <= '%04d-12-31'",data->gu,data->gu);

if(data->kom == 2)
 sprintf(strsql,"delete from Zarp where datz < '%04d-%d-01' and suma=0.",data->gu,data->mu);

iceb_sql_zapis(strsql,1,0,data->window);

int kolud=sql_rows(&bd);
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);

iceb_printw(strsql,data->buffer,data->view);

if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Zarp");
  if(metkazap == 1)
   {
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  else
    iceb_sql_readkey(strsql,data->window);
 }

if(data->kom == 2)
 {
  sprintf(strsql,"%s: %d",gettext("Количество удаленных записей"),kolud);
  iceb_menu_soob(strsql,data->window);
  goto kon;
 } 

sprintf(strsql,"%s\n",gettext("Удаляем записи в вспомогательной таблице"));
iceb_printw(strsql,data->buffer,data->view);

if(data->kom == 0)
 sprintf(strsql,"delete from Zarn where god = %d and mes=%d",
 data->gu,data->mu);
if(data->kom == 1)
 sprintf(strsql,"delete from Zarn where god=%d",
 data->gu);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);

sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(strsql,data->buffer,data->view);

if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Zarn");
  if(metkazap == 1)
   {
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  else
    iceb_sql_readkey(strsql,data->window);
 }

sprintf(strsql,"%s\n",gettext("Удаляем записи начислений на фонд оплаты труда"));

iceb_printw(strsql,data->buffer,data->view);

if(data->kom == 0)
 sprintf(strsql,"delete from Zarsocz where datz >= '%d-%d-01' and datz <= '%d-%d-31'",
 data->gu,data->mu,data->gu,data->mu);

if(data->kom == 1)
 sprintf(strsql,"delete from Zarsocz where datz >= '%d-01-01' and datz <= '%d-12-31'",
 data->gu,data->gu);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(strsql,data->buffer,data->view);

if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Zarsocz");
  if(metkazap == 1)
   {
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  else
    iceb_sql_readkey(strsql,data->window);
 }

sprintf(strsql,"%s\n",gettext("Удаляем записи отработанного времени"));
iceb_printw(strsql,data->buffer,data->view);

if(data->kom == 0)
 sprintf(strsql,"delete from Ztab where god=%d and mes=%d",
 data->gu,data->mu);
if(data->kom == 1)
 sprintf(strsql,"delete from Ztab where god=%d",
 data->gu);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(strsql,data->buffer,data->view);

if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Ztab");
  if(metkazap == 1)
   {
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  else
    iceb_sql_readkey(strsql,data->window);
 }

sprintf(strsql,"%s\n",gettext("Удаляем записи протоколов хода расчёта"));
iceb_printw(strsql,data->buffer,data->view);

if(data->kom == 0)
 sprintf(strsql,"delete from Zarpr where dr='%04d-%02d-01'",data->gu,data->mu);
if(data->kom == 1)
 sprintf(strsql,"delete from Zarpr where dr >= '%04d-01-01' and dr <= '%04d-12-01'",data->gu,data->gu);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);

sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(strsql,data->buffer,data->view);

if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);
  
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Zarpr");
  if(metkazap == 1)
   {
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  else
    iceb_sql_readkey(strsql,data->window);
 }


if(data->kom == 1)
 {
  sprintf(strsql,"%s\n",gettext("Удаляем записи сальдо"));

  iceb_printw(strsql,data->buffer,data->view);
  sprintf(strsql,"delete from Zsal where god=%d",data->gu);

  iceb_sql_zapis(strsql,1,0,data->window);

  kolud=sql_rows(&bd);
  sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);

  iceb_printw(strsql,data->buffer,data->view);
  if(kolud > 0)
   { 
    sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
    iceb_printw(strsql,data->buffer,data->view);
    memset(strsql,'\0',sizeof(strsql));
    sprintf(strsql,"optimize table Zsal");
    if(metkazap == 1)
     {
      iceb_sql_zapis(strsql,1,0,data->window);
     }
    else
      iceb_sql_readkey(strsql,data->window);
   }
 }




kon:;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
