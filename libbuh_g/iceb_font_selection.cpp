/*$Id: iceb_font_selection.c,v 1.8 2013/08/19 06:50:09 sasa Exp $*/
/*05.12.2014	24.05.2010	Белых А.И.	iceb_font_selection.c
Фыбор фонтов
*/
#include <pwd.h>
#include <errno.h>
#include "iceb_libbuh.h"

void iceb_font_selection(GtkWidget *wpredok)
{


GtkWidget *dialog= gtk_font_chooser_dialog_new(gettext("Выбор шрифта"),GTK_WINDOW(wpredok));



if(gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
  {
   gchar *font=gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
   printf("%s-%s\n",__FUNCTION__,font);

   struct passwd *rek_login;
   rek_login=getpwuid(getuid());

   class iceb_u_str imaf_nast;
   imaf_nast.plus(rek_login->pw_dir);
//   imaf_nast.plus(G_DIR_SEPARATOR_S,".gtkrc-2.0");
   imaf_nast.plus(G_DIR_SEPARATOR_S,".gtk.css");

   FILE *ff;

   if((ff = fopen(imaf_nast.ravno(),"w")) == NULL)
    {
     iceb_u_str repl;
     char bros[312];
     
     sprintf(bros,"%s:%d !",gettext("Ошибка открытия файла"),errno);
     repl.plus_ps(bros);
     

     repl.plus(strerror(errno));

     iceb_menu_soob(&repl,NULL);
     return;
    }

   fprintf(ff,"\
   style \"gtk-default\"\n\
   {\n\
    font_name = \"%s\"\n\
   }\n\
   class \"*\" style \"gtk-default\"\n",font);

   fclose(ff);


   g_free(font);
  }
                    
gtk_widget_destroy (dialog);


}


#if 0
###################################################################3
void  iceb_font_selection_out(GtkWidget *widget,GtkWidget *hrift)
{
gtk_widget_destroy(hrift);
}
void  iceb_font_selection_ok(GtkWidget *widget,GtkWidget *hrift)
{

gchar *name_font=gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(hrift));
//gchar *name_font=gtk_font_chooser_get_font(GTK_FONT_CHOOSER(hrift));

//printf("Имя шрифта=%s\n",name_font);
//if(gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(hrift),name_font) == TRUE)
// printf("Шрифт найден\n");

struct passwd *rek_login;
rek_login=getpwuid(getuid());

class iceb_u_str imaf_nast;
imaf_nast.plus(rek_login->pw_dir);
imaf_nast.plus(G_DIR_SEPARATOR_S,".gtkrc-2.0");

FILE *ff;

if((ff = fopen(imaf_nast.ravno(),"w")) == NULL)
 {
  iceb_u_str repl;
  char bros[312];
  
  sprintf(bros,"%s:%d !",gettext("Ошибка открытия файла"),errno);
  repl.plus_ps(bros);
  

  repl.plus(strerror(errno));

  iceb_menu_soob(&repl,NULL);
  return;
 }

fprintf(ff,"\
style \"gtk-default\"\n\
{\n\
 font_name = \"%s\"\n\
}\n\
class \"*\" style \"gtk-default\"\n",name_font);

fclose(ff);

gtk_widget_destroy(hrift);
}

/*******************************/

void iceb_font_selection(GtkWidget *wpredok)
{
GtkWidget *hrift=gtk_font_selection_dialog_new(gettext("Выбор шрифта"));

g_signal_connect(hrift,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(hrift,"destroy",G_CALLBACK(gtk_main_quit),NULL);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(hrift),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(hrift),TRUE);
 }


g_signal_connect(gtk_font_selection_dialog_get_ok_button(GTK_FONT_SELECTION_DIALOG(hrift)), "clicked",G_CALLBACK(iceb_font_selection_ok),hrift);

g_signal_connect(gtk_font_selection_dialog_get_cancel_button(GTK_FONT_SELECTION_DIALOG(hrift)), "clicked",G_CALLBACK(iceb_font_selection_out),hrift);

gtk_widget_show(hrift);
//gtk_widget_show(GTK_FONT_SELECTION_DIALOG(hrift)->apply_button);
gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

gtk_rc_reparse_all (); /*Читаем настройки всех файлов*/

}
###########################################3
#endif