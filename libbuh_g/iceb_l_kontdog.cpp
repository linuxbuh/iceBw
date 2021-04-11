/*$Id: iceb_l_kontdog.c,v 1.8 2013/12/31 11:49:18 sasa Exp $*/
/*27.04.2018	20.02.2011	Белых А.И.	iceb_l_kontdog.c
Работа со списком договоров
Возвращает уникальный номер записи еcли выбрали
-1 если запись не выбрали
*/
#include        <errno.h>
#include  "iceb_libbuh.h"

enum
{
  COL_DATAD,
  COL_NOMDOG,
  COL_VID_NN,
  COL_METKA_PU,
  COL_VIDDOG,
  COL_DATA_VREM,
  COL_KTO,  
  UN_NOM_ZAP,
  METKA_PU,
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  FK5,
  FK6,
  FK10,
  SFK2,
  KOL_F_KL
};

class  iceb_l_kontdog_data
 {
  public:
  
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
  int       metka_voz;  //-1 есил не выбрали или уникальный номер записи
  short     metka_rr;   //0-работа со списком 1-выбор
  int un_nom_zap;
  int metka_pu;
  class iceb_u_str kodkon;
  class iceb_u_str naim_kont;     
  //Конструктор
  iceb_l_kontdog_data()
   {
    snanomer=0;
    metka_rr=metka_voz=kl_shift=0;
    window=treeview=NULL;
    metka_pu=un_nom_zap=0;
    naim_kont.plus("");
   }      
 };

gboolean   iceb_l_kontdog_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_kontdog_data *data);
void iceb_l_kontdog_vibor(GtkTreeSelection *selection,class iceb_l_kontdog_data *data);
void iceb_l_kontdog_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class iceb_l_kontdog_data *data);
void  iceb_l_kontdog_knopka(GtkWidget *widget,class iceb_l_kontdog_data *data);
void iceb_l_kontdog_add_columns (GtkTreeView *treeview);
void iceb_l_kontdog_udzap(class iceb_l_kontdog_data *data);
void iceb_l_kontdog_rasp(class iceb_l_kontdog_data *data);
void iceb_l_kontdog_create_list (class iceb_l_kontdog_data *data);

int iceb_l_kontdog_v(int un_nom_zap,const char *kodkon,GtkWidget *wpredok);


extern SQL_baza  bd;

int   iceb_l_kontdog(int metka_rr, //0-ввод и корек. 1-выбор
const char *kodkon,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
SQL_str row;
SQLCURSOR cur;
class iceb_l_kontdog_data data;
char strsql[1024];
data.kodkon.new_plus(kodkon);
data.metka_rr=metka_rr;
data.name_window.plus(__FUNCTION__);

sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodkon);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.naim_kont.new_plus(row[0]);




data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_rr == 1)
 {
  if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
 }


sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Список договоров"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_l_kontdog_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Список договоров"));


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

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(iceb_l_kontdog_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(iceb_l_kontdog_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(iceb_l_kontdog_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(iceb_l_kontdog_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F6 %s",gettext("По умолчанию"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(iceb_l_kontdog_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Установить/снять метку договора по умолчанию"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(iceb_l_kontdog_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

iceb_l_kontdog_create_list(&data);

gtk_widget_show(data.window);
if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void iceb_l_kontdog_create_list (class iceb_l_kontdog_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(iceb_l_kontdog_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(iceb_l_kontdog_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT,
G_TYPE_INT,
G_TYPE_INT);


sprintf(strsql,"select * from Kontragent2 where kodkon='%s' order by datd desc",data->kodkon.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str metka_pu("");

data->kolzap=0;
float kolstr1=0. ;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s-%s %s %s %s\n",__FUNCTION__,row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  gtk_list_store_append (model, &iter);

  if(atoi(row[8]) == 1)
   metka_pu.new_plus("****");
  else
   metka_pu.new_plus("");
   
  gtk_list_store_set (model, &iter,
  COL_DATAD,iceb_u_datzap(row[2]),
  COL_NOMDOG,row[3],
  COL_VID_NN,row[7],
  COL_METKA_PU,metka_pu.ravno(),
  COL_VIDDOG,row[4],
  COL_DATA_VREM,iceb_u_vremzap(row[6]),
  COL_KTO,iceb_kszap(row[5],data->window),
  UN_NOM_ZAP,atoi(row[0]),
  METKA_PU,atoi(row[8]),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

iceb_l_kontdog_add_columns (GTK_TREE_VIEW (data->treeview));

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список договоров"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql,"%s:%s %s",gettext("Контрагент"),data->kodkon.ravno(),data->naim_kont.ravno());
zagolov.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void iceb_l_kontdog_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата"),renderer,"text",COL_DATAD,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATAD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Номер договора"),renderer,"text",COL_NOMDOG,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NOMDOG);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Вид нал.нак."),renderer,"text",COL_VID_NN,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_VID_NN);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("По умолчанию"),renderer,"text",COL_METKA_PU,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_METKA_PU);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Вид договора"),renderer,"text",COL_VIDDOG,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_VIDDOG);
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

void iceb_l_kontdog_vibor(GtkTreeSelection *selection,class iceb_l_kontdog_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gint  nomer=0;
gint un_nom_zap=0;
gint metka_pu=0;
gtk_tree_model_get(model,&iter,UN_NOM_ZAP,&un_nom_zap,METKA_PU,&metka_pu,NUM_COLUMNS,&nomer,-1);

data->snanomer=nomer;
data->un_nom_zap=un_nom_zap;
data->metka_pu=metka_pu;

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_l_kontdog_knopka(GtkWidget *widget,class iceb_l_kontdog_data *data)
{
char strsql[1024];

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    if(iceb_l_kontdog_v(-1,data->kodkon.ravno(),data->window) == 0)
      iceb_l_kontdog_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(iceb_l_kontdog_v(data->un_nom_zap,data->kodkon.ravno(),data->window) == 0)
      iceb_l_kontdog_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    iceb_l_kontdog_udzap(data);
    iceb_l_kontdog_create_list(data);
    return;  
  

  case FK5:
    iceb_l_kontdog_rasp(data);
    return;  

  case FK6: /*Установка/снятие метки договора по умолчанию*/

    sprintf(strsql,"update Kontragent2 set \
pu=0 \
where kodkon='%s'",
    data->kodkon.ravno());

    iceb_sql_zapis(strsql,0,0,data->window);

    if(data->metka_pu == 0)
     {
      sprintf(strsql,"update Kontragent2 set \
pu=1,\
ktoz=%d,\
vrem=%ld \
where nz=%d",
      iceb_getuid(data->window),
      time(NULL),
      data->un_nom_zap);

      iceb_sql_zapis(strsql,0,0,data->window);
     }

    iceb_l_kontdog_create_list(data);
    return;  

    
  case FK10:
    if(data->metka_rr == 1)
      iceb_sizww(data->name_window.ravno(),data->window);
    data->metka_voz=-1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_l_kontdog_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_kontdog_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:

    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    else
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");

    return(TRUE);
   
  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
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
void iceb_l_kontdog_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class iceb_l_kontdog_data *data)
{
if(data->metka_rr == 0)
 {
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");
  return;
 }

iceb_sizww(data->name_window.ravno(),data->window);

data->metka_voz=data->un_nom_zap;

gtk_widget_destroy(data->window);


}

/*****************************/
/*Удаление записи            */
/*****************************/

void iceb_l_kontdog_udzap(class iceb_l_kontdog_data *data)
{


char strsql[512];

sprintf(strsql,"delete from Kontragent2 where nz=%d",data->un_nom_zap);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void iceb_l_kontdog_rasp(class iceb_l_kontdog_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Kontragent2 where kodkon='%s' order by datd asc",data->kodkon.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"kdog%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список договоров"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Список договоров"),0,0,0,0,0,0,ff,data->window);
/*************
if(data->metka_poi == 1)
 {
  if(data->poisk.kod.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код"),data->poisk.kod.ravno());
  if(data->poisk.naim.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Наименование"),data->poisk.naim.ravno());
 }
**************/
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
   Дата   |Номер договора|         Вид договора         |Вид нал.нак.|Дата и время запис.| Кто записал\n"));
/*
1234567890 12345678901234 123456789012345678901234567890 123456789012
*/
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");

cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {

  fprintf(ff,"%-10s %-*.*s %-*.*s %-*s %s %s\n",
  iceb_u_datzap(row[2]),
  iceb_u_kolbait(14,row[3]),iceb_u_kolbait(14,row[3]),row[3],
  iceb_u_kolbait(30,row[4]),iceb_u_kolbait(30,row[4]),row[4],
  iceb_u_kolbait(12,row[7]),row[7],
  iceb_u_vremzap(row[6]),iceb_kszap(row[5],data->window));

  

 }
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
