/*$Id: iceb_menu_danet.c,v 1.16 2013/08/25 08:27:06 sasa Exp $*/
/*03.05.2010	11.09.2003	Белых А.И.	iceb_menu_danet.c
Программа выдачи запроса и выбора Да или Нет
*/
#include "iceb_libbuh.h"

struct danet
 {
  GtkWidget *window;
  int knopka;
 };

void knda(GtkWidget*,struct danet*);
void knnet(GtkWidget*,struct danet*);


/*************************************************/

int  iceb_menu_danet(const char *soob,int knopka,GtkWidget *wpredok)
 {
  class iceb_u_str repl;
  repl.plus(soob);
  return(iceb_menu_danet(&repl,knopka,wpredok));  
 }
/******************************************/

int  iceb_menu_danet(class iceb_u_spisok *soob,int knopka,GtkWidget *wpredok)
{
class iceb_u_str titl;
GtkWidget *label;
GtkWidget *ok_button;
GtkWidget *cancel_button;
danet data;
data.knopka=knopka;

/*Избавляемся от перевода строки*/
if(iceb_u_polen(soob->ravno(0),&titl,1,'\n') != 0)
 titl.new_plus(soob->ravno(0));

data.window=gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/
//gtk_window_set_position( GTK_WINDOW( data.window ), ICEB_POS_CENTER );                      
gtk_container_set_border_width( GTK_CONTAINER( data.window ), 10 );

class iceb_u_str repl(iceb_get_namesystem());
repl.plus(" ",titl.ravno());

gtk_window_set_title (GTK_WINDOW (data.window),repl.ravno());

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

g_signal_connect(data.window, "delete_event",G_CALLBACK(gtk_widget_destroy), NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

ok_button = gtk_button_new_with_label(gettext("Да") );
cancel_button = gtk_button_new_with_label(gettext( "Нет") );


GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(data.window),vbox);
int kolstr=soob->kolih();
for(int i=0; i < kolstr; i++)
 {
  label = gtk_label_new (soob->ravno(i));
  gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);
 }

GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous(GTK_BOX(hbox),TRUE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(vbox),hbox);

//gtk_box_pack_start((GtkBox*)hbox,ok_button,FALSE,FALSE,0);
//gtk_box_pack_start((GtkBox*)hbox,cancel_button,FALSE,FALSE,0);
gtk_box_pack_start((GtkBox*)hbox,ok_button,TRUE,TRUE,0);
gtk_box_pack_start((GtkBox*)hbox,cancel_button,TRUE,TRUE,0);

/* Ensure that the dialog box is destroyed when the user clicks ok. */
 
g_signal_connect( ok_button , "clicked",
                           G_CALLBACK( knda ),&data);

g_signal_connect( cancel_button , "clicked",
                           G_CALLBACK( knnet),&data );

gtk_window_set_modal( GTK_WINDOW( data.window ) ,TRUE ); 
if(knopka == 1)
  gtk_widget_grab_focus(ok_button);
if(knopka == 2)
  gtk_widget_grab_focus(cancel_button);

gtk_window_set_position( GTK_WINDOW( data.window ), ICEB_POS_CENTER );                      
gtk_widget_show_all( data.window );
gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.knopka);

}




/***************************************************/
/***************************************************/
int  iceb_menu_danet(class iceb_u_str *soob,int knopka,GtkWidget *wpredok)
{
class iceb_u_str titl;
GtkWidget *label;
GtkWidget *ok_button;
GtkWidget *cancel_button;
danet data;
data.knopka=knopka;
class iceb_u_str utf;
utf.plus(soob->ravno());

/*Избавляемся от перевода строки*/
if(iceb_u_polen(soob->ravno(),&titl,1,'\n') != 0)
 titl.new_plus(soob->ravno());

data.window=gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/
//gtk_window_set_position( GTK_WINDOW( data.window ), ICEB_POS_CENTER );                      
gtk_container_set_border_width( GTK_CONTAINER( data.window ), 10 );

class iceb_u_str repl(iceb_get_namesystem());
repl.plus(" ",titl.ravno());

gtk_window_set_title (GTK_WINDOW (data.window),repl.ravno());


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

g_signal_connect(data.window, "delete_event",G_CALLBACK(gtk_widget_destroy), NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

label = gtk_label_new (utf.ravno());
ok_button = gtk_button_new_with_label(gettext("Да") );
cancel_button = gtk_button_new_with_label(gettext( "Нет") );


GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(data.window),vbox);
gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);

GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous(GTK_BOX(hbox),TRUE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(vbox),hbox);

//gtk_box_pack_start((GtkBox*)hbox,ok_button,FALSE,FALSE,0);
//gtk_box_pack_start((GtkBox*)hbox,cancel_button,FALSE,FALSE,0);
gtk_box_pack_start((GtkBox*)hbox,ok_button,TRUE,TRUE,0);
gtk_box_pack_start((GtkBox*)hbox,cancel_button,TRUE,TRUE,0);

/* Ensure that the dialog box is destroyed when the user clicks ok. */
 
g_signal_connect( ok_button , "clicked",
                           G_CALLBACK( knda ),&data);

g_signal_connect( cancel_button , "clicked",
                           G_CALLBACK( knnet),&data );

gtk_window_set_modal( GTK_WINDOW( data.window ) ,TRUE ); 
if(knopka == 1)
  gtk_widget_grab_focus(ok_button);
if(knopka == 2)
  gtk_widget_grab_focus(cancel_button);

gtk_window_set_position( GTK_WINDOW( data.window ), ICEB_POS_CENTER );
gtk_widget_show_all( data.window );

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.knopka);

}
/*******************/
void knda(GtkWidget *widget,struct danet *data)
{
data->knopka=1;
gtk_widget_destroy(data->window);
}
/*******************/
void knnet(GtkWidget *widget,struct danet *data)
{
data->knopka=2;
gtk_widget_destroy(data->window);
}
