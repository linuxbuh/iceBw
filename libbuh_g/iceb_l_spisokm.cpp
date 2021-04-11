/*$Id: iceb_l_spisokm.c,v 1.23 2013/08/25 08:27:06 sasa Exp $*/
/*28.02.2016	12.04.2004	Белых А.И.	iceb_l_spisokm.c
список коментариев
Возвращаем номер выбранной строки или -1 если ничего не выбрали

*/
#include       "iceb_libbuh.h"

enum
 {
  VIBOR,
  VIHOD,
  KOLSTRM
 };


class  spisokm_data
 {
  public:

  iceb_u_str imafv;

  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *vwindow;
  GtkWidget *pwindow;
  GtkWidget *knopka[KOLSTRM];
  class iceb_u_str name_window;
  
  short     kl_shift; //0-отжата 1-нажата  
  short     metkazapisi; //0-новая запись 1-корректировка
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  short     metka_poi;  //0-без поиска 1-с поиском
  
  class SYSTEM_NAST sysnast;

  iceb_u_spisok *kol1;
  iceb_u_spisok *kol2;
  iceb_u_spisok *naim_kol;
  iceb_u_str *naim_menu;
  int   voz;
    
  //Конструктор
  spisokm_data()
   {
    snanomer=0;
    kl_shift=metkazapisi=metka_poi=0;
    vwindow=pwindow=window=treeview=NULL;
    voz=-1;
   }      
 };

extern SQL_baza	bd;


enum
{
 COL_KOL1,
 COL_KOL2,
 NUM_COLUMNS
};

gboolean   spisokm_key_press(GtkWidget *widget,GdkEventKey *event,class spisokm_data *data);
void spisokm_vibor(GtkTreeSelection *selection,class spisokm_data *data);
void spisokm_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class spisokm_data *data);
void  spisokm_knopka(GtkWidget *widget,class spisokm_data *data);
void spisokm_add_columns (GtkTreeView *treeview,iceb_u_spisok*);
void spisokm_create_list (class spisokm_data *data);


int   iceb_l_spisokm(iceb_u_spisok *kol1,
iceb_u_spisok *kol2,
iceb_u_spisok *naim_kol,
iceb_u_str *naim_menu,
int nom_str, /*номер строки на которую нужно стать*/
GtkWidget *wpredok)
{
int gor=0;
int ver=0;

class spisokm_data data;
char bros[512];

data.kol1=kol1;
data.kol2=kol2;
data.naim_kol=naim_kol;
data.naim_menu=naim_menu;
data.snanomer=nom_str;
data.name_window.plus(__FUNCTION__);


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size(GTK_WINDOW(data.window),400,400);


sprintf(bros,"%s %s",iceb_get_namesystem(),naim_menu->ravno());

gtk_window_set_title (GTK_WINDOW (data.window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

if(wpredok != NULL)
 {
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(spisokm_key_press),&data);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new(gettext("Файлы распечаток"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки

sprintf(bros,"F2 %s",gettext("Выбор"));
data.knopka[VIBOR]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[VIBOR], TRUE, TRUE, 0);
g_signal_connect(data.knopka[VIBOR], "clicked",G_CALLBACK(spisokm_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[VIBOR],gettext("Выбор отмеченной записи"));
gtk_widget_set_name(data.knopka[VIBOR],iceb_u_inttochar(VIBOR));
gtk_widget_show(data.knopka[VIBOR]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[VIHOD]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[VIHOD],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[VIHOD],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[VIHOD], "clicked",G_CALLBACK(spisokm_knopka),&data);
gtk_widget_set_name(data.knopka[VIHOD],iceb_u_inttochar(VIHOD));
gtk_widget_show(data.knopka[VIHOD]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[VIBOR]);

spisokm_create_list (&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);


gtk_main();

return(data.voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void spisokm_create_list (class spisokm_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
char bros[512];

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(spisokm_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(spisokm_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

//iceb_u_str rekv[NUM_COLUMNS];

data->kolzap=0;


int kolstrok=data->kol1->kolih();
data->kolzap=0;
for(int i=0; i < kolstrok ; i++)
 {
 
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOL1,data->kol1->ravno(i),
  COL_KOL2,data->kol2->ravno(i),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }


gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

spisokm_add_columns(GTK_TREE_VIEW (data->treeview),data->naim_kol);

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[VIBOR]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[VIBOR]),TRUE);//Доступна
 }

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);


iceb_u_str zagolov;
zagolov.plus(data->naim_menu->ravno());

sprintf(bros," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(bros);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));

}

/*****************/
/*Создаем колонки*/
/*****************/

void spisokm_add_columns(GtkTreeView *treeview,iceb_u_spisok *naim_kol)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;


renderer = gtk_cell_renderer_text_new ();
//  g_object_set(G_OBJECT(renderer),"foreground","red",NULL);

//g_object_set(G_OBJECT(renderer),"background","black",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,naim_kol->ravno(0), renderer,"text", COL_KOL1,NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,naim_kol->ravno(1), renderer,"text", COL_KOL2,NULL);

renderer = gtk_cell_renderer_text_new ();

printf("spisokm_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void spisokm_vibor(GtkTreeSelection *selection,class spisokm_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *imaf;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_KOL1,&imaf,NUM_COLUMNS,&nomer,-1);

data->imafv.new_plus(imaf);
data->snanomer=nomer;

g_free(imaf);

//printf("%s %d\n",data->imafv.ravno(),data->nomervsp);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   spisokm_key_press(GtkWidget *widget,GdkEventKey *event,class spisokm_data *data)
{

switch(event->keyval)
 {
  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[VIBOR],"clicked");
    return(TRUE);
  
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    data->voz=-1;
    g_signal_emit_by_name(data->knopka[VIHOD],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("saldo_sh_key_press-Нажата клавиша Shift\n");

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
void  spisokm_knopka(GtkWidget *widget,class spisokm_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {

  case VIBOR:
    data->voz=data->snanomer;
    gtk_widget_destroy(data->window);
    return;  

  case VIHOD:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=-1;
    gtk_widget_destroy(data->window);
    return;  

    
 }
}

/****************************/
/*Выбор строки*/
/**********************/
void spisokm_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class spisokm_data *data)
{

g_signal_emit_by_name(data->knopka[VIBOR],"clicked");
}

