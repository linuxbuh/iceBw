/*$Id: iceb_gdite.c,v 1.17 2013/08/25 08:27:05 sasa Exp $*/
/*07.05.2015	21.08.2006	Белых А.И.	iceb_gdite.c
Окно с текстом Ждите !!!
*/

#include  "iceb_libbuh.h"



void iceb_gdite(class iceb_gdite_data *data,int metka_bar,GtkWidget *wpredok)
{

data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
gtk_window_set_resizable(GTK_WINDOW(data->window),FALSE); /*запрет на изменение размеров окна*/
gtk_window_set_deletable(GTK_WINDOW(data->window),FALSE); /*Выключить кнопку close в рамке окна*/

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data->window),GTK_WINDOW(wpredok));

 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data->window),TRUE);

/*Окно поверх всех окон*/
/*если окно будет поверх всех окон то сообщения об ошибках будут под ним*/
//gtk_window_set_keep_above(GTK_WINDOW(data->window),TRUE);

char bros[312];
sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Ждите !"));

gtk_window_set_title (GTK_WINDOW (data->window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data->window), 5);


GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data->window), vbox);


//GtkWidget *label=gtk_label_new(gettext("Ждите !"));
GtkWidget *label=gtk_label_new("");
iceb_label_set_text_color(label,gettext("Ждите !"),"#F90101");

gtk_widget_set_size_request(GTK_WIDGET(label),300,-1);
gtk_box_pack_start(GTK_BOX (vbox), label, TRUE, TRUE, 0);


if(metka_bar == 0)
 {
  data->bar=gtk_progress_bar_new();



  gtk_box_pack_start (GTK_BOX (vbox), data->bar, FALSE, FALSE, 2);
 }
//gtk_window_set_default_size (GTK_WINDOW  (data->window),300,-1);

gtk_window_set_position( GTK_WINDOW(data->window),ICEB_POS_CENTER);
gtk_widget_show_all(data->window);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));

iceb_refresh();

}
