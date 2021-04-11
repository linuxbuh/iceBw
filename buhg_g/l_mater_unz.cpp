/*$Id: l_mater_unz.c,v 1.19 2013/08/25 08:26:37 sasa Exp $*/
/*15.12.2018	18.05.2004	Белых А.И.	l_mater_unz.c
Удаление не используемых записей кодов материалов
*/
#include "buhg_g.h"

class mater_udnz_data
 {
  public:
  SQLCURSOR cur;
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *bar;
  GtkWidget *label;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  gfloat kolstr1; //Количество прочитанных строк
  int    kolstr;  //Количество строк в курсоре
  int koludk;
  time_t vremn;
  
  //Конструктор
  mater_udnz_data()
   {
    koludk=0;
    kolstr1=0.;
   }
 };
gint   mater_udnz11(class mater_udnz_data *data);
void  mater_r_v_knopka(GtkWidget *widget,class mater_udnz_data *data);
gboolean   mater_r_key_press(GtkWidget *widget,GdkEventKey *event,class mater_udnz_data *data);

extern SQL_baza	bd;

void  l_mater_udnz(GtkWidget *wpredok)
{
int gor=0;
int ver=0;
char strsql[512];
class mater_udnz_data data;
data.name_window.plus(__FUNCTION__);


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
//gtk_window_set_default_size (GTK_WINDOW(data.window),400,300);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Удаление неиспользуемых кодов материалов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

//g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

GtkWidget *label=gtk_label_new(gettext("Удаление неиспользуемых кодов материалов"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна
gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(mater_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);


g_idle_add((GSourceFunc)mater_udnz11,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  mater_r_v_knopka(GtkWidget *widget,class mater_udnz_data *data)
{
 iceb_sizww(data->name_window.ravno(),data->window);
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   mater_r_key_press(GtkWidget *widget,GdkEventKey *event,class mater_udnz_data *data)
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
/**********************************/
gint   mater_udnz11(class mater_udnz_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur1;
//GtkTextIter iter;

//printf("mater_udnz11 %f\n",data->kolstr1);

if(data->kolstr1 == 0)
 {
  time(&data->vremn);
  
  sprintf(strsql,"select kodm,naimat from Material order by kodm asc");

  if((data->kolstr=data->cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  if(data->kolstr == 0)
   return(FALSE);
 }

while(data->cur.read_cursor(&row) != 0)
 {
  iceb_u_str soob;
  iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

//  iceb_printw(soob.ravno(),data->buffer,data->view);

  if(mater_pvu(0,row[0],data->window) != 0)
    return(TRUE);

  sprintf(strsql,"%-5s %-*.*s %s\n",
  row[0],
  iceb_u_kolbait(30,row[1]),
  iceb_u_kolbait(30,row[1]),
  row[1],
  gettext("Удаляем"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"delete from Material where kodm = %s",row[0]);
  if(iceb_sql_zapis(strsql,1,0,data->window) == ER_DBACCESS_DENIED_ERROR) //нет полномочий для выполнения этой операции
   return(FALSE);

  return(TRUE);
 }

iceb_printw_vr(data->vremn,data->buffer,data->view);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

return(FALSE);
}
