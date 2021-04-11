/*$Id: iceb_ponp.c,v 1.21 2013/08/25 08:27:07 sasa Exp $*/
/*21.05.2016	22.09.2003	Белых А.И.	iceb_ponp.c
Просмотр очереди на печать
*/
#include <errno.h>
#include "iceb_libbuh.h"
#define   KOLKNOP 3

struct iceb_ponp_data
 {
  GtkWidget *knopka[KOLKNOP];
  GtkWidget *window;
  GtkWidget *treeview;
  int kolzap;  
 };

gboolean   iceb_ponp_key_press(GtkWidget *,GdkEventKey *,struct iceb_ponp_data *);
void       iceb_ponp_knopka(GtkWidget *,struct iceb_ponp_data *);


void  iceb_ponp(const char* lpq,const char *lprm,GtkWidget *wpredok)
{
FILE *ff;
char imaf[64];
char bros[1024];
iceb_ponp_data  data;

printf("%s\n",__FUNCTION__);
memset(&data,'\0',sizeof(data));

sprintf(imaf,"iceb_ponp%d.tmp",getpid());

sprintf(bros,"%s > %s",lpq,imaf);
printf("%s-%s\n",__FUNCTION__,bros);

if(system(bros) != 0)
 {
  class iceb_u_str soob(__FUNCTION__);
  soob.plus("-");
  soob.plus(gettext("Не выполнена команда"));
  soob.ps_plus(bros);
  iceb_menu_soob(soob.ravno(),wpredok);
 }


if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_u_str SP;
  sprintf(bros,"%s: %d !",gettext("Ошибка открытия файла"),errno);
  SP.plus_ps(bros);
  SP.plus(imaf);
  iceb_menu_soob(&SP,NULL);
  return;
 }

unlink(imaf);

class iceb_u_spisok SP;

while(fgets(bros,sizeof(bros),ff) != NULL)
 {
  printf("%s",bros);
  iceb_u_ps_minus(bros);
  SP.plus(bros);
 }
fclose(ff);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal( GTK_WINDOW(data.window ) ,TRUE ); 

sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Просмотр очереди на печать."));
gtk_window_set_title(GTK_WINDOW(data.window),bros);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_ponp_key_press),&data);

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
GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);
gtk_container_add(GTK_CONTAINER(data.window), vbox);
gtk_widget_show(vbox);

GtkWidget *scrolled_window=gtk_scrolled_window_new(NULL, NULL);


gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);

gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);

data.treeview = gtk_tree_view_new();

gtk_container_add (GTK_CONTAINER (scrolled_window), data.treeview);
gtk_widget_show (data.treeview);

//g_signal_connect(data.treeview,"row_activated",G_CALLBACK(kontr_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data.treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
//g_signal_connect(selection,"changed",G_CALLBACK(kontr_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data.treeview)),GTK_SELECTION_SINGLE);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);




GtkListStore *model=NULL;
GtkTreeIter iter;

model = gtk_list_store_new (2,G_TYPE_STRING,G_TYPE_INT);
data.kolzap=0;
for(int i=0; i < SP.kolih(); i++)
 {
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,0,SP.ravno(i),1,data.kolzap,-1);

  data.kolzap++;
 }
gtk_tree_view_set_model (GTK_TREE_VIEW(data.treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));



GtkCellRenderer *renderer;
GtkTreeViewColumn *column;

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Очередь на печать"),renderer, "text",0,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
//gtk_tree_view_column_set_sort_column_id (column,0); Разрешить сортировку по колонке
gtk_tree_view_append_column (GTK_TREE_VIEW(data.treeview), column);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(hbox), data.knopka[2], TRUE, TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[2],gettext("Закрыть окно"));
g_signal_connect(data.knopka[2], "clicked",G_CALLBACK(iceb_ponp_knopka),&data);
gtk_widget_set_name(data.knopka[2],"10");
gtk_widget_grab_focus(data.knopka[2]);

gtk_widget_show_all(data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_ponp_knopka(GtkWidget *widget,struct iceb_ponp_data *data)
{
//char    bros[512];

gchar *knop=(gchar*)gtk_widget_get_name(widget);
//g_print("iceb_ponp_knopka knop=%s\n",knop);

switch (atoi(knop))
 {
  case 2:

    return;  

  case 10:
    gtk_widget_destroy(data->window);
    return;
 }

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_ponp_key_press(GtkWidget *widget,GdkEventKey *event,struct iceb_ponp_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    gtk_widget_destroy(widget);
    return(FALSE);
 }

return(TRUE);

}
