/*$Id: udgodsw_r.c,v 1.5 2013/09/26 09:46:56 sasa Exp $*/
/*23.05.2016	18.11.2012	Белых А.И.	udgodsw_r.c
Расчёт отчёта 
*/
#include "buhg_g.h"

class udgodsw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  short god_ud;  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  udgodsw_r_data()
   {
    voz=1;
    kon_ras=1;
    god_ud=0;
   }
 };
gboolean   udgodsw_r_key_press(GtkWidget *widget,GdkEventKey *event,class udgodsw_r_data *data);
gint udgodsw_r1(class udgodsw_r_data *data);
void  udgodsw_r_v_knopka(GtkWidget *widget,class udgodsw_r_data *data);


extern SQL_baza bd;

int udgodsw_r(short god_ud,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class udgodsw_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);
data.god_ud=god_ud;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Удаление документов за год"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(udgodsw_r_key_press),&data);

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

repl.plus(gettext("Удаление документов за год"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(udgodsw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)udgodsw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  udgodsw_r_v_knopka(GtkWidget *widget,class udgodsw_r_data *data)
{
//printf("udgodsw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   udgodsw_r_key_press(GtkWidget *widget,GdkEventKey *event,class udgodsw_r_data *data)
{
// printf("udgodsw_r_key_press\n");
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

gint udgodsw_r1(class udgodsw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;


int		metkazap=0;
strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur);
if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  

sprintf(strsql,"VERSION:%s\n",row[0]);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s %d\n",gettext("Удаляем документы за год"),data->god_ud);
iceb_printw(strsql,data->buffer,data->view);



sprintf(strsql,"%s Uddok1\n",gettext("Удаляем записи в таблице"));
iceb_printw(strsql,data->buffer,data->view);


sprintf(strsql,"delete from Uddok1 where datd >= '%04d-01-01' and datd <= '%04d-12-31'",data->god_ud,data->god_ud);

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s Uddok\n",gettext("Удаляем записи в таблице"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Uddok where datd >= '%04d-01-01' and datd <= '%04d-12-31'",data->god_ud,data->god_ud);
iceb_sql_zapis(strsql,1,0,data->window);





sprintf(strsql,"%s Uddok\n",gettext("Оптимизация таблицы"));
iceb_printw(strsql,data->buffer,data->view);


memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Uddok");
if(metkazap == 1)
 {
  iceb_sql_zapis(strsql,1,0,data->window);
 }
else
  iceb_sql_readkey(strsql,data->window);



sprintf(strsql,"%s Uddok1\n",gettext("Оптимизация таблицы"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Uddok1");
if(metkazap == 1)
 {
  iceb_sql_zapis(strsql,1,0,data->window);
 }
else
  iceb_sql_readkey(strsql,data->window);





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
