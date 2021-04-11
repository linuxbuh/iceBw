/*$Id:$*/
/*26.02.2017	25.02.2017	Белых А.И.	l_dokummat4_sm.c
создание списка материалов для выбора
*/
#include  <errno.h>
#include  "buhg_g.h"

enum
{
  COL_KOD_M,
  COL_NOM_KART,
  COL_NAIM_M,
  NUM_COLUMNS
};

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

class  l_dokummat4_sm_data
 {
  public:

  short dd,md,gd;
  int skl;
  class iceb_u_str nomdok;

  class iceb_u_str kod_mat;
  class iceb_u_str nom_kart;

  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  class iceb_u_str name_window;
  
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  short     metka_poi;  //0-без поиска 1-с поиском
  int       metka_voz;  //0-выбрали 1-нет  
  //Конструктор
  l_dokummat4_sm_data()
   {
    snanomer=0;
    metka_voz=kl_shift=metka_poi=0;
    window=treeview=NULL;
   }      
 };

gboolean   l_dokummat4_sm_key_press(GtkWidget *widget,GdkEventKey *event,class l_dokummat4_sm_data *data);
void l_dokummat4_sm_vibor(GtkTreeSelection *selection,class l_dokummat4_sm_data *data);
void l_dokummat4_sm_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_dokummat4_sm_data *data);
void  l_dokummat4_sm_knopka(GtkWidget *widget,class l_dokummat4_sm_data *data);
void l_dokummat4_sm_add_columns (GtkTreeView *treeview);
int l_dokummat4_sm_create_list (class l_dokummat4_sm_data *data);

int l_dokummat4_sm_v(class iceb_u_str *kod_gr,GtkWidget *wpredok);
int l_dokummat4_sm_p(class l_dokummat4_sm_rek *rek_poi,GtkWidget *wpredok);


extern SQL_baza  bd;

int l_dokummat4_sm(short dd,short md,short gd,int skl,const char *nomdok,
class iceb_u_str *kod_mat,class iceb_u_str *nom_kart,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class l_dokummat4_sm_data data;
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

data.name_window.plus(__FUNCTION__);
data.dd=dd;
data.md=md;
data.gd=gd;
data.skl=skl;
data.nomdok.plus(nomdok);
int kolstr=0;
/*если только одна запись то она нам и нужна*/

sprintf(strsql,"select nomkar,kodm from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",data.gd,data.md,data.dd,data.skl,data.nomdok.ravno());
if((kolstr=iceb_sql_readkey(strsql,&row,&cur,wpredok)) == 1)
 {
  kod_mat->new_plus(row[1]);
  nom_kart->new_plus(row[0]);
  return(0);
  
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("В документе нет ни одной записи!"),wpredok);
  return(1);
 }

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);


sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Выберите нужный материал"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_dokummat4_sm_key_press),&data);
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
data.label_kolstr=gtk_label_new (gettext("Выберите нужный материал"));


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
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_dokummat4_sm_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Подтвердить выбор записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_dokummat4_sm_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

if(l_dokummat4_sm_create_list(&data) != 0)
 return(1);

gtk_widget_show(data.window);


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.metka_voz == 0)
 {
  kod_mat->new_plus(data.kod_mat.ravno());
  nom_kart->new_plus(data.nom_kart.ravno());
 }

return(data.metka_voz);
}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
int l_dokummat4_sm_create_list (class l_dokummat4_sm_data *data)
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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_dokummat4_sm_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_dokummat4_sm_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select nomkar,kodm from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",data->gd,data->md,data->dd,data->skl,data->nomdok.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  gdite.close();
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return(1);
 }
//gtk_list_store_clear(model);
class iceb_u_str naim_km("");

data->kolzap=0;
float kolstr1=0. ;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  


  /*узнаём наименование материала*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[1]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_km.new_plus(row1[0]);
  else
   naim_km.new_plus("");
  

  gtk_list_store_append (model, &iter);
/************
  COL_KOD_M,
  COL_NOM_KART,
  COL_NAIM_M,
*************/
  gtk_list_store_set (model, &iter,
  COL_KOD_M,row[1],
  COL_NOM_KART,row[0],
  COL_NAIM_M,naim_km.ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_dokummat4_sm_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


class iceb_u_str stroka;
class iceb_u_str zagolov;
zagolov.plus(gettext("Ведомость списания материала на детали"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());
gtk_label_set_use_markup (GTK_LABEL (data->label_kolstr), TRUE);


gtk_widget_show(data->label_kolstr);
return(0);
}

/*****************/
/*Создаем колонки*/
/*****************/

void l_dokummat4_sm_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код м."),renderer,"text",COL_KOD_M,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOD_M);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Ном.карт."),renderer,"text",COL_NOM_KART,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NOM_KART);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Наименование материала"),renderer,"text",COL_NAIM_M,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAIM_M);
gtk_tree_view_append_column (treeview, column);

}

/****************************/
/*Выбор строки*/
/**********************/

void l_dokummat4_sm_vibor(GtkTreeSelection *selection,class l_dokummat4_sm_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod_mat;
gchar *nom_kart;

gtk_tree_model_get(model,&iter,COL_KOD_M,&kod_mat,COL_NOM_KART,&nom_kart,-1);


data->nom_kart.new_plus(nom_kart);
data->kod_mat.new_plus(kod_mat);
g_free(kod_mat);
g_free(nom_kart);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_dokummat4_sm_knopka(GtkWidget *widget,class l_dokummat4_sm_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_dokummat4_sm_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->metka_voz=0;

    gtk_widget_destroy(data->window);
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

gboolean   l_dokummat4_sm_key_press(GtkWidget *widget,GdkEventKey *event,class l_dokummat4_sm_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:

    g_signal_emit_by_name(data->knopka[FK2],"clicked");
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
void l_dokummat4_sm_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_dokummat4_sm_data *data)
{

 g_signal_emit_by_name(data->knopka[FK2],"clicked");


}

