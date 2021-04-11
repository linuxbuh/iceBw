/*$Id: iceb_about.c,v 1.10 2013/12/31 11:49:18 sasa Exp $*/
/*28.04.2020	14.04.2010	Белых А.И.	iceb_about.c
получение информации о системе
*/
#include        "iceb_libbuh.h"

extern char *version;

/*************************/

void iceb_about()
{

GtkWidget *about=gtk_about_dialog_new();
g_signal_connect(about,"response",G_CALLBACK(gtk_widget_destroy),NULL);

gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about),"iceBw");

class iceb_u_str stroka(gettext("Версия"));
stroka.plus(":",version);
gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about),stroka.ravno());

gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about),"http://iceb.net.ua"); /*Устанавливаем сылку на сайт*/

stroka.new_plus(gettext("Сайт разработчика"));
stroka.plus(" ","http://iceb.net.ua");
gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about),stroka.ravno()); /*Устанавливам текст на ссылке*/

gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about),gettext("Бухгалтерский учёт"));
gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about),"Copyright © 1992-2020 Aleksandr Belykh");

const gchar *avtor[]={"Aleksandr Belykh","<belin1957@gmail.com>","http://iceb.net.ua","tel:+380676654526",NULL};
 
gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about),avtor);


gtk_dialog_run(GTK_DIALOG(about));

}
