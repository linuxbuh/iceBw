/*$Id: iceb_dir_select.c,v 1.4 2013/08/13 06:10:09 sasa Exp $*/
/*08.05.2015	01.04.2010	Белых А.И.	iceb_dir_select.c
Выбор нужного каталога
*/
#include "iceb_libbuh.h"

int iceb_dir_select(class iceb_u_str *imadir,const char *titl,GtkWidget *wpredok)
{
int voz=1;
GtkWidget *dialog;

dialog=gtk_file_chooser_dialog_new(titl,
GTK_WINDOW(wpredok),
GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
gettext("Выход"),GTK_RESPONSE_CANCEL,
gettext("Выбрать"),GTK_RESPONSE_ACCEPT,
NULL);
/****************************
dialog=gtk_file_chooser_dialog_new(titl,
GTK_WINDOW(wpredok),
GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
GTK_STOCK_OPEN,GTK_RESPONSE_ACCEPT,
NULL);
**********************/
if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
 {
  char *filname;
  filname=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
  imadir->new_plus(filname);
  g_free(filname);
  voz=0;  
 }
gtk_widget_destroy(dialog);
return(voz);
}
