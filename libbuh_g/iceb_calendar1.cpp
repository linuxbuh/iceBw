/*$Id: iceb_calendar1.c,v 1.14 2013/08/25 08:27:05 sasa Exp $*/
/*03.06.2016	23.11.2006	Белых А.И.	iceb_calendar1.c
Меню для выбора месяц.год
Возвращаем 0-если выбрали дату
           1-не выбрали
*/
#include "iceb_libbuh.h"

enum
{
 FK10,
 KOL_F_KL
};

enum
{
 COL_DATA,
 NUM_COLUMNS
};

class  iceb_calendar1_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *spin_enter;
  GtkWidget *knopka[KOL_F_KL];
  class iceb_u_str name_window;
  short     kl_shift; //0-отжата 1-нажата  
  int       metka_voz;  //0-выбрали 1-нет  


  class iceb_u_str datav; //Выбранная дата
  short god;
  short mes;
  //Конструктор
  iceb_calendar1_data()
   {
    metka_voz=1;
    kl_shift=0;
    window=treeview=NULL;
   }      
 };

void iceb_calendar1_vibor(GtkTreeSelection *selection,class iceb_calendar1_data *data);
void iceb_calendar1_create_list(class iceb_calendar1_data *data);
void iceb_calendar1_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class iceb_calendar1_data *data);
void iceb_calendar1_spin_enter(GtkSpinButton *widget,class iceb_calendar1_data *data);
gboolean   iceb_calendar1_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_calendar1_data *data);
void  iceb_calendar1_knopka(GtkWidget *widget,class iceb_calendar1_data *data);


int iceb_calendar1(class iceb_u_str *mes_god,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
char bros[512];
class  iceb_calendar1_data data;
data.name_window.plus(__FUNCTION__);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW(data.window),-1,350);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Выбор даты"));

gtk_window_set_title (GTK_WINDOW (data.window),bros);
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_calendar1_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), vbox1);



data.label_kolstr=gtk_label_new (gettext("Выбор даты"));


gtk_box_pack_start (GTK_BOX (vbox1),data.label_kolstr,FALSE, FALSE, 0);

short dt;
if(mes_god->getdlinna() > 1)
 {
  iceb_u_rsdat1(&data.mes,&data.god,mes_god->ravno());
 }
else
 iceb_u_poltekdat(&dt,&data.mes,&data.god);

data.spin_enter=gtk_spin_button_new_with_range(data.god-1000,data.god+1000,1);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(data.spin_enter),(gfloat)data.god);
g_signal_connect(data.spin_enter,"value-changed",G_CALLBACK(iceb_calendar1_spin_enter),&data);

gtk_box_pack_start (GTK_BOX (vbox1),data.spin_enter,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox1), data.sw, TRUE, TRUE, 0);


//Кнопки

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],FALSE,FALSE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(iceb_calendar1_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);

iceb_calendar1_create_list (&data);

gtk_widget_show_all (data.window);

gtk_main();
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.metka_voz == 0)
 mes_god->new_plus(data.datav.ravno());
return(data.metka_voz);

}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void iceb_calendar1_create_list (class iceb_calendar1_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
char strsql[512];

//printf("iceb_calendar1_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(iceb_calendar1_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(iceb_calendar1_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_INT);

for(int ii=0; ii < 12 ; ii++)
 {
  sprintf(strsql,"%02d.%d",ii+1,data->god);
//  printf("%s\n",strsql);
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATA,strsql,
  NUM_COLUMNS,ii+1,
  -1);
  
 }
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),
-1,gettext("Дата"), renderer,"text", COL_DATA,NULL);
int kk=data->mes-1;
iceb_snanomer(12,&kk,data->treeview);
gtk_widget_show (data->treeview);

}

/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void iceb_calendar1_vibor(GtkTreeSelection *selection,class iceb_calendar1_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;

gtk_tree_model_get(model,&iter,COL_DATA,&kod,-1);

data->datav.new_plus(kod);

g_free(kod);


}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void iceb_calendar1_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class iceb_calendar1_data *data)
{
data->metka_voz=0;
iceb_sizww(data->name_window.ravno(),data->window);

gtk_widget_destroy(data->window);


}
/********************************/
/*Реакция на изменение года*/
/******************************/

void iceb_calendar1_spin_enter(GtkSpinButton *widget,class iceb_calendar1_data *data)
{

data->god=(short)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(data->spin_enter));

iceb_calendar1_create_list (data);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_calendar1_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_calendar1_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_calendar1_knopka(GtkWidget *widget,class iceb_calendar1_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {    
  case FK10:
    data->metka_voz=1;
    iceb_sizww(data->name_window.ravno(),data->window);
    gtk_widget_destroy(data->window);
    return;
 }
}
