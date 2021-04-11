/*$Id: iceb_file_selection.c,v 1.11 2013/11/24 08:25:08 sasa Exp $*/
/*14.11.2013	15.11.2004	Белых А.И.	iceb_file_selection.c
Просмотр текущего каталога и выбор файлов
*/

#include "iceb_libbuh.h"

int  iceb_file_selection(class iceb_u_str *imaf_v,GtkWidget *wpredok)
{
/*************
GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File",
          GTK_WINDOW(wpredok),
          GTK_FILE_CHOOSER_ACTION_OPEN,
          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
          NULL);
***********************/

GtkWidget *dialog = gtk_file_chooser_dialog_new (gettext("Поиск файла"),
          GTK_WINDOW(wpredok),
//          GTK_FILE_CHOOSER_ACTION_OPEN,
          GTK_FILE_CHOOSER_ACTION_SAVE,
          gettext("Выбрать"),GTK_RESPONSE_ACCEPT,
          gettext("Выход"),GTK_RESPONSE_CANCEL,
          NULL);

if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
 {
  char *filename;
  filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
  imaf_v->new_plus(filename);
  g_free (filename);
 }

gtk_widget_destroy (dialog);

return(0);
}
