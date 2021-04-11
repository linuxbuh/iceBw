/*$Id: iceb_runsystem.c,v 1.12 2013/08/25 08:27:07 sasa Exp $*/
/*29.08.2006 	15.04.2004	Белых А.И.	iceb_runsystem.c
Запуск вспомогательных программ
*/
#include "iceb_libbuh.h"

class runsystem_data
 {
  public:
  
  GtkWidget *window;
  GtkWidget *label;
  
  iceb_u_str komanda;
  int voz;  
  

 };
gint runsystem1(class runsystem_data *data);


int iceb_runsystem(char *komanda,char *koment,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str soob;

runsystem_data data;

data.komanda.plus(komanda);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_default_size (GTK_WINDOW  (data.window),300,-1);


sprintf(strsql,"%s %s",iceb_get_namesystem(),koment);
gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

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

soob.new_plus(gettext("Ждите !!!"));
GtkWidget *label=gtk_label_new(soob.ravno());
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

soob.new_plus(koment);
soob.ps_plus(komanda);

label=gtk_label_new(soob.ravno());
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

gtk_widget_show_all(vbox);


g_idle_add((GSourceFunc)runsystem1,&data);

gtk_widget_show_all(data.window);
//gtk_widget_realize(data.window);
//gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR_GDITE));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);

}

gint runsystem1(class runsystem_data *data)
{
g_usleep(100000);
iceb_refresh();
data->voz=system(data->komanda.ravno());

gtk_widget_destroy(data->window);

return(FALSE);

}
