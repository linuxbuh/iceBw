/*$Id: l_vmat.c,v 1.26 2013/08/13 06:10:25 sasa Exp $*/
/*12.05.2015	26.02.2004	Белых А.И.	l_vmat.c
Выбор материалла или услуг из списка 
Если вернули 0-выбрали
             1-нет
*/

#include <stdlib.h>
#include <unistd.h>
//#include <gtimer.h>
#include <glib.h>
#include "i_rest.h"

enum
{
  FK2,
  FK_VVERH,
  FK_VNIZ,
  FK10,
  KOL_F_KL
};

enum
{
 COL_KOD,
 COL_NAIM,
 COL_CENA,
 COL_EI,
 NUM_COLUMNS
};
 
class  vmat_data
 {
  public:
  int        kodgr;  //Код группы для которой ищем записи
  iceb_u_str naigr;  //наименование группы
  
  iceb_u_str kod; //выбранный код записи

  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  gint       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       voz;     //0-код выбран 1-нет  
  int       metkarr; //0-работа со списком товаров 1-списком услуг
  
  short metka_released;
  
  //Конструктор
  vmat_data()
   {
    voz=0;
    kod.new_plus("");
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    kodgr=0;
    naigr.plus("");
    metka_released=0;
   }      
 };

void vmat_create_list (class vmat_data *data);
void vmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class vmat_data *data);
gboolean   vmat_key_press(GtkWidget *widget,GdkEventKey *event,class vmat_data *data);
void  vmat_knopka(GtkWidget *widget,class vmat_data *data);
void vmat_add_columns(GtkTreeView *treeview);
void vmat_vibor(GtkTreeSelection *selection,class vmat_data *data);

void l_vmat_setstr(class vmat_data *data);
void  l_vmat_knopka_released(GtkWidget *widget,class vmat_data *data);
void  l_vmat_knopka_vniz_press(GtkWidget *widget,class vmat_data *data);
void  l_vmat_knopka_vverh_press(GtkWidget *widget,class vmat_data *data);

extern SQL_baza	bd;
extern char *name_system;

int l_vmat(int *kodv,//выбранный код
int kodgr, //Код группы для которой ищем записи
int metkarr, //0-работа со списком товаров 1-списком услуг
GtkWidget *wpredok)
{
vmat_data data;
char strsql[300];
SQLCURSOR cur;
SQL_str   row;

data.kodgr=kodgr;
data.metkarr=metkarr;
if(metkarr == 0)
  sprintf(strsql,"select naik from Grup where kod=%d",kodgr);
if(metkarr == 1)
  sprintf(strsql,"select naik from Uslgrup where kod=%d",kodgr);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 data.naigr.new_plus(row[0]);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

gtk_window_set_default_size (GTK_WINDOW  (data.window),600,600);

if(metkarr == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Список товаров"));
if(metkarr == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Список услуг"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vmat_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Выберите нужную запись"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки

sprintf(strsql,"F2 %s",gettext("Выбор"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(vmat_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Выбор отмеченной записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

GtkWidget *label=gtk_label_new("+");

data.knopka[FK_VVERH]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data.knopka[FK_VVERH]),label);

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK_VVERH], TRUE, TRUE, 0);
//g_signal_connect(data.knopka[FK_VVERH], "clicked",G_CALLBACK(vmat_knopka),&data);
g_signal_connect(data.knopka[FK_VVERH], "pressed",G_CALLBACK(l_vmat_knopka_vverh_press),&data);
g_signal_connect(data.knopka[FK_VVERH], "released",G_CALLBACK(l_vmat_knopka_released),&data);
//gtk_widget_set_tooltip_text(data.knopka[FK_VVERH],gettext("Выбор отмеченной записи"));
gtk_widget_set_name(data.knopka[FK_VVERH],iceb_u_inttochar(FK_VVERH));
gtk_widget_show(data.knopka[FK_VVERH]);
gtk_widget_show(label);

label=gtk_label_new("-");

data.knopka[FK_VNIZ]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data.knopka[FK_VNIZ]),label);

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK_VNIZ], TRUE, TRUE, 0);
//g_signal_connect(data.knopka[FK_VNIZ], "clicked",G_CALLBACK(vmat_knopka),&data);
g_signal_connect(data.knopka[FK_VNIZ], "pressed",G_CALLBACK(l_vmat_knopka_vniz_press),&data);
g_signal_connect(data.knopka[FK_VNIZ], "released",G_CALLBACK(l_vmat_knopka_released),&data);
//gtk_widget_set_tooltip_text(data.knopka[FK_VNIZ],gettext("Выбор отмеченной записи"));
gtk_widget_set_name(data.knopka[FK_VNIZ],iceb_u_inttochar(FK_VNIZ));
gtk_widget_show(data.knopka[FK_VNIZ]);
gtk_widget_show(label);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(vmat_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
//gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

vmat_create_list (&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);

gtk_main();

*kodv=atoi(data.kod.ravno());
return(data.voz);
}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void vmat_create_list (class vmat_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
char strsql[400];
int  kolstr=0;
SQLCURSOR cur;
SQL_str   row;

data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(vmat_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(vmat_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);



model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


//gtk_list_store_clear(model);
if(data->metkarr == 0)
  sprintf(strsql,"select kodm,naimat,ei,cenapr from Material where kodgr=%d \
  and cenapr != 0. and ei != '' order by naimat asc",data->kodgr);
if(data->metkarr == 1)
  sprintf(strsql,"select kodus,naius,ei,cena from Uslugi where kodgr=%d \
  and cena != 0. and ei != '' order by naius asc",data->kodgr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str s1;
iceb_u_str s2;
iceb_u_str s3;
iceb_u_str s4;

float kolstr1=0.;
data->kolzap=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  

  //код
  s1.new_plus(row[0]);
  
  //наименование
  s2.new_plus(row[1]);

  //единица измерения
  s3.new_plus(row[2]);


  //цена
  sprintf(strsql,"%.2f",atof(row[3]));  
  s4.new_plus(strsql);

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,s1.ravno(),
  COL_NAIM,s2.ravno(),
  COL_EI,s3.ravno(),
  COL_CENA,s4.ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

vmat_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK_VVERH]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK_VNIZ]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK_VVERH]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK_VNIZ]),TRUE);//Доступна
 }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

if(data->kolzap > 0)
  l_vmat_setstr(data); //стать на нужную строку

iceb_u_str stroka;
iceb_u_str zagolov;

sprintf(strsql,"%s:%d %s",gettext("Группа"),data->kodgr,data->naigr.ravno());
zagolov.plus_ps(strsql);
zagolov.plus(gettext("Выберите нужную запись"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());


gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void vmat_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
					       -1,gettext("Код"), renderer,
					       "text", COL_KOD,
					       NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Цена"), renderer,"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Е/и"), renderer,"text", COL_EI,NULL);



}

/****************************/
/*Выбор строки*/
/**********************/

void vmat_vibor(GtkTreeSelection *selection,class vmat_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;


gtk_tree_model_get(model,&iter,0,&kod,NUM_COLUMNS,&nomer,-1);


data->kod.new_plus(kod);
data->snanomer=nomer;

g_free(kod);

//printf(" %s %d\n",data->kod.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  vmat_knopka(GtkWidget *widget,class vmat_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("vmat_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {
  case FK2:
    if(data->kolzap == 0)
      return;
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK_VVERH:
    if(data->kolzap == 0)
      return;

    data->snanomer--;
    l_vmat_setstr(data); //стать на нужную строку

    return;  

  case FK_VNIZ:
    if(data->kolzap == 0)
      return;
    
    data->snanomer++;
    l_vmat_setstr(data); //стать на нужную строку
    return;  

  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vmat_key_press(GtkWidget *widget,GdkEventKey *event,class vmat_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);
   
  
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("vmat_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void vmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class vmat_data *data)
{
g_signal_emit_by_name(data->knopka[FK2],"clicked");
}
/***********************************/
/*Стат на нужную строку*/
/************************************/
void l_vmat_setstr(class vmat_data *data)
{
  if(data->snanomer  > data->kolzap-1)
     data->snanomer= data->kolzap-1;
  if(data->snanomer < 0)
     data->snanomer=0;
      
char strsql[50];
  sprintf(strsql,"%d",data->snanomer);
  GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

//  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  gtk_tree_path_free(path);
}

/*************************/
/*Кнопка вниз нажата*/
/**********************************/
void  l_vmat_knopka_vniz_press(GtkWidget *widget,class vmat_data *data)
{
data->metka_released=1;
while(data->metka_released == 1)
 {
  data->snanomer++;
  printf("вниз-%d\n",data->snanomer);

  l_vmat_setstr(data); //стать на нужную строку

  g_usleep(SLEEP_STR);

  iceb_refresh();  //обязательно после sleep
 }


}
/*************************/
/*Кнопка вверх нажата*/
/**********************************/

void  l_vmat_knopka_vverh_press(GtkWidget *widget,class vmat_data *data)
{
data->metka_released=1;
while(data->metka_released == 1)
 {
  data->snanomer--;
  printf("вверх-%d\n",data->snanomer);

  l_vmat_setstr(data); //стать на нужную строку


  g_usleep(SLEEP_STR);

  iceb_refresh();  //обязательно после sleep
 }


}
/*************************/
/*Кнопка вниз/вверх отжата*/
/**********************************/
void  l_vmat_knopka_released(GtkWidget *widget,class vmat_data *data)
{
data->metka_released=0;
}

