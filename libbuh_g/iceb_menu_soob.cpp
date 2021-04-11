/*$Id: iceb_menu_soob.c,v 1.19 2013/08/25 08:27:07 sasa Exp $*/
/*11.11.2008	19.08.2003	Белых А.И.	iceb_menu_soob.c
Прогамма вывода сообщения на экран
*/
#include "iceb_libbuh.h"

void  iceb_menu_soob1(const char *soob,GtkWidget *wpredok);
void  iceb_menu_soob2(iceb_u_spisok *soob,GtkWidget *wpredok);
void  iceb_menu_soob_f10(GtkWidget *widget,GtkWidget *data);



void  iceb_menu_soob(iceb_u_spisok *soob,GtkWidget *wpredok)
 {
  iceb_menu_soob2(soob,wpredok);
 }

void  iceb_menu_soob(iceb_u_str *soob,GtkWidget *wpredok)
 {
  iceb_menu_soob1(soob->ravno(),wpredok);
 }

void  iceb_menu_soob(const char *soob,GtkWidget *wpredok)
 {
  iceb_menu_soob1(soob,wpredok);
 }

/***************************************/

void  iceb_menu_soob2(iceb_u_spisok *soob,GtkWidget *wpredok)
{
char bros[512];
GtkWidget *dialog;
GtkWidget *label;
GtkWidget *ok_button;
GtkWidget *vbox;
int       i=0;
int kolstr=soob->kolih();

dialog = gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_resizable(GTK_WINDOW(dialog),FALSE); /*запрет на изменение размеров окна*/
gtk_container_set_border_width(GTK_CONTAINER(dialog),10);

gtk_window_set_position( GTK_WINDOW( dialog ), GTK_WIN_POS_CENTER );                      

sprintf(bros,"%s %s!",iceb_get_namesystem(),gettext("Внимание"));

gtk_window_set_title (GTK_WINDOW (dialog),bros);

ok_button = gtk_button_new_with_label(gettext("Выход"));
	
g_signal_connect(dialog,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(dialog,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(dialog,"key_press_event",G_CALLBACK(iceb_vihod),&i);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(dialog),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog),TRUE);
 }

vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(dialog),vbox);
for(int i=0; i < kolstr; i++)
 {
  label = gtk_label_new (soob->ravno(i));
  gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);
 }

gtk_box_pack_start((GtkBox*)vbox,ok_button,FALSE,FALSE,0);

gtk_widget_grab_focus(ok_button);
g_signal_connect( ok_button, "clicked",G_CALLBACK( iceb_menu_soob_f10),dialog);

gtk_window_set_modal( GTK_WINDOW( dialog ) ,TRUE ); 

gtk_widget_show_all( dialog );

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}
/***************************************/

void  iceb_menu_soob1(const char *soob,GtkWidget *wpredok)
{
char bros[512];
GtkWidget *dialog;
GtkWidget *label;
GtkWidget *ok_button;
GtkWidget *vbox;
int       i=0;
class iceb_u_str  utf;

//Для символов в UTF кодировке нужно больше места
utf.plus(soob);

dialog = gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_resizable(GTK_WINDOW(dialog),FALSE); /*запрет на изменение размеров окна*/
//dialog = gtk_window_new( GTK_WINDOW_POPUP );
// dialog = gtk_dialog_new();
gtk_container_set_border_width(GTK_CONTAINER(dialog),10);

gtk_window_set_position( GTK_WINDOW( dialog ), GTK_WIN_POS_CENTER );                      

sprintf(bros,"%s %s!",iceb_get_namesystem(),gettext("Внимание"));

gtk_window_set_title (GTK_WINDOW (dialog),bros);

label = gtk_label_new (utf.ravno());
ok_button = gtk_button_new_with_label(gettext("Выход"));
	
g_signal_connect(dialog,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
//if(level == 0)
  g_signal_connect(dialog,"destroy",G_CALLBACK(gtk_main_quit),NULL);
//g_signal_connect(dialog,"key_press_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect_after(dialog,"key_press_event",G_CALLBACK(iceb_vihod),&i);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(dialog),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog),TRUE);
 }

vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(dialog),vbox);
gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);

gtk_box_pack_start((GtkBox*)vbox,ok_button,FALSE,FALSE,0);

gtk_widget_grab_focus(ok_button);
g_signal_connect(ok_button , "clicked",G_CALLBACK( iceb_menu_soob_f10 ),dialog);

gtk_window_set_modal( GTK_WINDOW( dialog ) ,TRUE ); 

gtk_widget_show_all( dialog );
gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}
/*****************************/
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_menu_soob_f10(GtkWidget *widget,GtkWidget *data)
{
 gtk_widget_destroy(data);
}









