/*$Id:$*/
/*10.03.2017	10.03.2017	Белых А.И.	l_musnrm.cpp
Работа с справочником норм расхода материалов
*/
#include        <errno.h>
#include  "buhg_g.h"
#include  "l_musnrm.h"

enum
{
  COL_KODD,
  COL_NAIMD,
  COL_EID,
  COL_KODM,
  COL_NAIMM,  
  COL_EIM,
  COL_KOLIH,
  COL_VS,
  COL_VZ,
  COL_KOMENT,  
  COL_DATA_VREM,
  COL_KTO,  
  COL_NZ,
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_musnrm_data
 {
  public:

  class l_musnrm_rek poisk;

  int nz; /*номер выбранной записи*/
  class iceb_u_str kodv;
  class iceb_u_str naimv;
  class iceb_u_str kod_det_tv; //только что введённый код
  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  class iceb_u_str name_window;
  
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  
  class iceb_u_str zapros;

  //Конструктор
  l_musnrm_data()
   {
    nz=0;
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    kod_det_tv.plus("");
   }      
 };

gboolean   l_musnrm_key_press(GtkWidget *widget,GdkEventKey *event,class l_musnrm_data *data);
void l_musnrm_vibor(GtkTreeSelection *selection,class l_musnrm_data *data);
void l_musnrm_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_musnrm_data *data);
void  l_musnrm_knopka(GtkWidget *widget,class l_musnrm_data *data);
void l_musnrm_add_columns (GtkTreeView *treeview);
void l_musnrm_udzap(class l_musnrm_data *data);
int  l_musnrm_prov_row(SQL_str row,class l_musnrm_data *data);
void l_musnrm_rasp(class l_musnrm_data *data);
int l_musnrm_create_list (class l_musnrm_data *data);

int l_musnrm_v(int nom_zap,class iceb_u_str *kod_gr,GtkWidget *wpredok);
int l_musnrm_p(class l_musnrm_rek *rek_poi,GtkWidget *wpredok);


extern SQL_baza  bd;

int l_musnrm(GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class l_musnrm_data data;
char bros[1024];

data.poisk.clear_data();
data.name_window.plus(__FUNCTION__);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Справочник норм списания материалов"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_musnrm_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

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
data.label_kolstr=gtk_label_new (gettext("Справочник норм списания материалов"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_musnrm_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_musnrm_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_musnrm_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);


sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_musnrm_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_musnrm_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_musnrm_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

if(l_musnrm_create_list(&data) != 0)
 return(1);

gtk_widget_show(data.window);
//  gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
int l_musnrm_create_list (class l_musnrm_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur,cur1;
char strsql[1024];
int  kolstr=0;
SQL_str row,row1;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_musnrm_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_musnrm_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT,
G_TYPE_INT);


sprintf(strsql,"select * from Musnrm order by kd asc,km asc");
data->zapros.new_plus(strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  gdite.close();
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return(1);
 }
//gtk_list_store_clear(model);
class iceb_u_str naim_det("");
class iceb_u_str naim_mat("");
class iceb_u_str vs("");
class iceb_u_str vz("");
char kolih[64];
data->kolzap=0;
float kolstr1=0. ;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(l_musnrm_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->kod_det_tv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;
  
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_det.new_plus(row1[0]);
  else
   naim_det.new_plus("");

  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[3]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_mat.new_plus(row1[0]);
  else
   naim_mat.new_plus("");
  
  sprintf(kolih,"%.10g",atof(row[4]));
  
  if(atoi(row[6]) == 0)
   vs.new_plus(gettext("Основной"));
  else
   vs.new_plus(gettext("Альтернативный"));

  if(atoi(row[7]) == 0)
   vz.new_plus(gettext("Материал"));
  else
   vz.new_plus(gettext("Покупная заготовка"));
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KODD,row[1],
  COL_NAIMD,naim_det.ravno(),
  COL_EID,row[2],
  COL_KODM,row[3],
  COL_NAIMM,naim_mat.ravno(),
  COL_EIM,row[5],
  COL_KOLIH,kolih,
  COL_VS,vs.ravno(),
  COL_VZ,vz.ravno(),
  COL_KOMENT,row[8],
  COL_DATA_VREM,iceb_u_vremzap(row[10]),
  COL_KTO,iceb_kszap(row[9],data->window),
  COL_NZ,atoi(row[0]),  
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kod_det_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_musnrm_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


class iceb_u_str stroka;
class iceb_u_str zagolov;
zagolov.plus(gettext("Справочник норм списания материалов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());
gtk_label_set_use_markup (GTK_LABEL (data->label_kolstr), TRUE);

if(data->poisk.metka_poi == 1)
 {
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kodd.ravno(),gettext("Код детали"));
  iceb_str_poisk(&zagolov,data->poisk.naimd.ravno(),gettext("Наименование детали"));
  iceb_str_poisk(&zagolov,data->poisk.kodm.ravno(),gettext("Код материала"));
  iceb_str_poisk(&zagolov,data->poisk.naimm.ravno(),gettext("Наименование материала"));

  iceb_label_set_text_color(data->label_poisk,zagolov.ravno(),"#F90101");

  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);
return(0);
}

/*****************/
/*Создаем колонки*/
/*****************/

void l_musnrm_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код д."),renderer,"text",COL_KODD,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KODD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Наименование детали"),renderer,"text",COL_NAIMD,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAIMD);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Ед.изм."),renderer,"text",COL_EID,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_EID);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код м."),renderer,"text",COL_KODM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KODM);
gtk_tree_view_append_column (treeview, column);



renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Наименование материала"),renderer,"text",COL_NAIMM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAIMM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Ед.изм."),renderer,"text",COL_EIM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_EIM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Количество"),renderer,"text",COL_KOLIH,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOLIH);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Вид списания"),renderer,"text",COL_VS,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_VS);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Вид заготовки"),renderer,"text",COL_VZ,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_VZ);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Коментарий"),renderer,"text",COL_KOMENT,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOMENT);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата и время записи"),renderer,"text",COL_DATA_VREM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_VREM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кто записал"),renderer,"text",COL_KTO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KTO);
gtk_tree_view_append_column (treeview, column);

}

/****************************/
/*Выбор строки*/
/**********************/

void l_musnrm_vibor(GtkTreeSelection *selection,class l_musnrm_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;
gint nz;

gtk_tree_model_get(model,&iter,COL_KODD,&kod,COL_NZ,&nz,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(kod);
data->snanomer=nomer;
data->nz=nz;

g_free(kod);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_musnrm_knopka(GtkWidget *widget,class l_musnrm_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    if(l_musnrm_v(0,&data->kod_det_tv,data->window) == 0)
      l_musnrm_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(l_musnrm_v(data->nz,&data->kodv,data->window) == 0)
      l_musnrm_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_musnrm_udzap(data);
    l_musnrm_create_list(data);
    return;  
  

  case FK4:
    l_musnrm_p(&data->poisk,data->window);
    l_musnrm_create_list(data);
    return;  

  case FK5:
    l_musnrm_rasp(data);
    return;  

    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_musnrm_key_press(GtkWidget *widget,GdkEventKey *event,class l_musnrm_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:

    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    else
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    data->kl_shift=0; /*обязательно сбрасываем*/
    return(TRUE);
   
  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    data->kl_shift=0; /*обязательно сбрасываем*/
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);

  
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:

    data->kl_shift=1;

    return(TRUE);

  default:
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void l_musnrm_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_musnrm_data *data)
{

g_signal_emit_by_name(data->knopka[SFK2],"clicked");
return;

}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_musnrm_udzap(class l_musnrm_data *data)
{

char strsql[512];

sprintf(strsql,"delete from Musnrm where nz=%d",data->nz);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_musnrm_prov_row(SQL_str row,class l_musnrm_data *data)
{
SQL_str row1;
class SQLCURSOR cur1;
char strsql[1024];

if(data->poisk.metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.kodd.ravno(),row[1],0,0) != 0)
 return(1);


if(iceb_u_proverka(data->poisk.kodm.ravno(),row[3],0,0) != 0)
 return(1);

if(data->poisk.naimd.getdlinna() > 1)
 {
  class iceb_u_str naimd("");
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naimd.new_plus(row1[0]);

  if(iceb_u_proverka(data->poisk.naimd.ravno(),naimd.ravno(),4,0) != 0)
   return(1);
 }  
  
if(data->poisk.naimm.getdlinna() > 1)
 {
  class iceb_u_str naimm("");
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[3]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naimm.new_plus(row1[0]);

  if(iceb_u_proverka(data->poisk.naimm.ravno(),naimm.ravno(),4,0) != 0)
   return(1);

 }
   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_musnrm_rasp(class l_musnrm_data *data)
{
char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur;

class SQLCURSOR cur1;
int vs=0,vz=0;
class iceb_u_str naim_det("");
class iceb_u_str naim_mat("");
class iceb_u_str naim_vidsp("");
class iceb_u_str naim_vidzag("");

int kolstr=0;


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return;
 }

char imaf[64];
sprintf(imaf,"kateg%d.lst",getpid());
class iceb_fopen fil;
if(fil.start(imaf,"w",data->window) != 0)
 return;



iceb_zagolov(gettext("Справочник норм списания материалов"),fil.ff,data->window);

if(data->poisk.metka_poi == 1)
 {
  if(data->poisk.kodd.getdlinna() > 1)
   fprintf(fil.ff,"%s:%s\n",gettext("Код детали"),data->poisk.kodd.ravno());
  if(data->poisk.naimd.getdlinna() > 1)
   fprintf(fil.ff,"%s:%s\n",gettext("Наименование детали"),data->poisk.naimd.ravno());

  if(data->poisk.kodm.getdlinna() > 1)
   fprintf(fil.ff,"%s:%s\n",gettext("Код материала"),data->poisk.kodm.ravno());
  if(data->poisk.naimm.getdlinna() > 1)
   fprintf(fil.ff,"%s:%s\n",gettext("Наименование материала"),data->poisk.naimm.ravno());
 }

fprintf(fil.ff,"\
--------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(fil.ff,gettext("\
Код.дт|    Наименование детали       |Ед.изм|Код ма|  Наименование материала      |Ед.изм|Количество|Вид списания|Вид заготовки|\n"));
/*
123456|123456789012345678901234567890|123456|123456|123456789012345678901234567890|123456 1234567890 123456789012 1234567890123
*/
fprintf(fil.ff,"\
--------------------------------------------------------------------------------------------------------------------------------\n");

while(cur.read_cursor(&row) != 0)
 {
  if(l_musnrm_prov_row(row,data) != 0)
    continue;


  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_det.new_plus(row1[0]);
  else
   naim_det.new_plus("");

  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[3]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_mat.new_plus(row1[0]);
  else
   naim_mat.new_plus("");
    
  vs=atoi(row[6]);
  vz=atoi(row[7]);

  if(vs == 0)
   naim_vidsp.new_plus(gettext("Основной"));
  else
   naim_vidsp.new_plus(gettext("Альтернативный"));

  if(vz == 0)
   naim_vidzag.new_plus(gettext("Материал"));
  else
   naim_vidzag.new_plus(gettext("Покупная"));
   
  fprintf(fil.ff,"%6s %-*.*s %-*s %6s %-*.*s %-*s %10.10g %-*.*s %-*.*s\n",
  row[1],
  iceb_u_kolbait(30,naim_det.ravno()),
  iceb_u_kolbait(30,naim_det.ravno()),
  naim_det.ravno(),
  iceb_u_kolbait(6,row[2]),
  row[2],
  row[3],
  iceb_u_kolbait(30,naim_mat.ravno()),
  iceb_u_kolbait(30,naim_mat.ravno()),
  naim_mat.ravno(),
  iceb_u_kolbait(6,row[5]),
  row[5],
  atof(row[4]),
  iceb_u_kolbait(12,naim_vidsp.ravno()),
  iceb_u_kolbait(12,naim_vidsp.ravno()),
  naim_vidsp.ravno(),
  iceb_u_kolbait(13,naim_vidzag.ravno()),
  iceb_u_kolbait(13,naim_vidzag.ravno()),
  naim_vidzag.ravno());
  

 }
fprintf(fil.ff,"\
--------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(fil.ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(fil.ff,data->window);

fil.end();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Справочник норм списания материалов"));

iceb_ustpeh(oth.spis_imaf.ravno(0),3,data->window);


iceb_rabfil(&oth,data->window);

}
