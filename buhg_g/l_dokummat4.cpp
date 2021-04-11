/*$Id:$*/
/*26.02.2017	25.02.2017	Белых А.И.	l_dokummat4.c
Работа с ведомостью списания материалов на детали
*/
#include  <errno.h>
#include  "buhg_g.h"
#include  "l_dokummat4.h"

enum
{
  COL_KOD_M,
  COL_NOM_KART,
  COL_NAIM_M,
  COL_KOD_D,
  COL_NAIM_D,
  COL_KOLIH,
  COL_KOMENT,
  COL_DATA_VREM,
  COL_KTO,  
  COL_NOM_ZAP,
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

class  l_dokummat4_data
 {
  public:

  class l_dokummat4_rek poisk;

  int nom_zap_vib; /*уникальный номер выбранной записи*/
  class iceb_u_str kod_mat_tv; /*код материала только что введённый*/
  
  short dd,md,gd;
  int skl;
  class iceb_u_str nomdok;
  class iceb_u_str naim_skl;
  
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
  l_dokummat4_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    kod_mat_tv.plus("");
    naim_skl.plus("");
   }      
 };

gboolean   l_dokummat4_key_press(GtkWidget *widget,GdkEventKey *event,class l_dokummat4_data *data);
void l_dokummat4_vibor(GtkTreeSelection *selection,class l_dokummat4_data *data);
void l_dokummat4_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_dokummat4_data *data);
void  l_dokummat4_knopka(GtkWidget *widget,class l_dokummat4_data *data);
void l_dokummat4_add_columns (GtkTreeView *treeview);
void l_dokummat4_udzap(class l_dokummat4_data *data);
int  l_dokummat4_prov_row(SQL_str row,class l_dokummat4_data *data);
void l_dokummat4_rasp(class l_dokummat4_data *data);
int l_dokummat4_create_list (class l_dokummat4_data *data);

int l_dokummat4_v(short dd,short md,short gd,int skl,const char *nomdok,int nomer_zap,GtkWidget *wpredok);
int l_dokummat4_p(class l_dokummat4_rek *rek_poi,GtkWidget *wpredok);
int l_dokummat4_pps(class  l_dokummat4_data *data);


extern SQL_baza  bd;

void l_dokummat4(short dd,short md,short gd,int skl,const char *nomdok,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class l_dokummat4_data data;
char bros[1024];
SQL_str row;
class SQLCURSOR cur;

data.poisk.clear_data();
data.name_window.plus(__FUNCTION__);
data.dd=dd;
data.md=md;
data.gd=gd;
data.skl=skl;
data.nomdok.plus(nomdok);

sprintf(bros,"select naik from Sklad where kod=%d",skl);
if(iceb_sql_readkey(bros,&row,&cur,wpredok) == 1)
 data.naim_skl.new_plus(row[0]);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Ведомость списания материала на детали"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_dokummat4_key_press),&data);
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
data.label_kolstr=gtk_label_new (gettext("Ведомость списания материала на детали"));


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
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_dokummat4_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_dokummat4_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_dokummat4_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_dokummat4_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_dokummat4_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_dokummat4_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

if(l_dokummat4_create_list(&data) != 0)
 return;

gtk_widget_show(data.window);
//gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
int l_dokummat4_create_list (class l_dokummat4_data *data)
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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_dokummat4_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_dokummat4_vibor),data);

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
G_TYPE_INT,
G_TYPE_INT);

sprintf(strsql,"select km,nk,kd,kol,nz,kom,ktoz,vrem from Dokummat4 where datd='%04d-%02d-%02d' and \
skl=%d and nomd='%s'",
data->gd,data->md,data->dd,data->skl,data->nomdok.ravno());

data->zapros.new_plus(strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  gdite.close();
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
//gtk_list_store_clear(model);
class iceb_u_str naim_km("");
class iceb_u_str naim_kd("");

data->kolzap=0;
float kolstr1=0. ;
char kolih[64];
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(l_dokummat4_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->kod_mat_tv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;
  


  
  /*узнаём наименование материала*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[0]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_km.new_plus(row1[0]);
  else
   naim_km.new_plus("");
  
  /*узнаём наименование детали*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[2]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_kd.new_plus(row1[0]);
  else
   naim_kd.new_plus("");

  sprintf(kolih,"%.10g",atof(row[3]));
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD_M,row[0],
  COL_NOM_KART,row[1],
  COL_NAIM_M,naim_km.ravno(),
  COL_KOD_D,row[2],
  COL_NAIM_D,naim_kd.ravno(),
  COL_KOLIH,kolih,
  COL_KOMENT,row[5],
  COL_DATA_VREM,iceb_u_vremzap(row[7]),
  COL_KTO,iceb_kszap(row[6],data->window),
  COL_NOM_ZAP,atoi(row[4]),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kod_mat_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_dokummat4_add_columns (GTK_TREE_VIEW (data->treeview));


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
zagolov.plus(gettext("Ведомость списания материала на детали"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql,"%s:%02d.%02d.%d %s:%s %s:%d %s",gettext("Дата"),data->dd,data->md,data->gd,gettext("Номер документа"),data->nomdok.ravno(),gettext("Склад"),data->skl,data->naim_skl.ravno());
zagolov.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());
gtk_label_set_use_markup (GTK_LABEL (data->label_kolstr), TRUE);

if(data->poisk.metka_poi == 1)
 {
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kod_mat.ravno(),gettext("Код материала"));
  iceb_str_poisk(&zagolov,data->poisk.naim_km.ravno(),gettext("Наименование материала"));

  iceb_str_poisk(&zagolov,data->poisk.kod_det.ravno(),gettext("Код детали"));
  iceb_str_poisk(&zagolov,data->poisk.naim_kd.ravno(),gettext("Наименование детали"));

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

void l_dokummat4_add_columns(GtkTreeView *treeview)
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

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код д."),renderer,"text",COL_KOD_D,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOD_D);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Наименование детали"),renderer,"text",COL_NAIM_D,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAIM_D);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Количество"),renderer,"text",COL_KOLIH,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOLIH);
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

void l_dokummat4_vibor(GtkTreeSelection *selection,class l_dokummat4_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gint  nomer;
gint nomer_zap;

gtk_tree_model_get(model,&iter,COL_NOM_ZAP,&nomer_zap,NUM_COLUMNS,&nomer,-1);

data->snanomer=nomer;
data->nom_zap_vib=nomer_zap;


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_dokummat4_knopka(GtkWidget *widget,class l_dokummat4_data *data)
{
class iceb_u_str kod_mat("");
class iceb_u_str nom_kart("");
int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    if(iceb_pbpds(data->md,data->gd,data->window) != 0)
     return;
    if(l_dokummat4_v(data->dd,data->md,data->gd,data->skl,data->nomdok.ravno(),0,data->window) == 0)
        l_dokummat4_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(l_dokummat4_v(data->dd,data->md,data->gd,data->skl,data->nomdok.ravno(),data->nom_zap_vib,data->window) == 0)
      l_dokummat4_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;

    if(iceb_pbpds(data->md,data->gd,data->window) != 0)
     return;

    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_dokummat4_udzap(data);
    l_dokummat4_create_list(data);
    return;  
  

  case FK4:
    l_dokummat4_p(&data->poisk,data->window);
    l_dokummat4_create_list(data);
    return;  

  case FK5:
    l_dokummat4_rasp(data);
    return;  

    
  case FK10:
    l_dokummat4_pps(data);
    iceb_sizww(data->name_window.ravno(),data->window);
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_dokummat4_key_press(GtkWidget *widget,GdkEventKey *event,class l_dokummat4_data *data)
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
void l_dokummat4_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_dokummat4_data *data)
{
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");


}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_dokummat4_udzap(class l_dokummat4_data *data)
{


char strsql[512];

sprintf(strsql,"delete from Dokummat4 where nz=%d",data->nom_zap_vib);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_dokummat4_prov_row(SQL_str row,class l_dokummat4_data *data)
{
SQL_str row1;
class SQLCURSOR cur1;
char strsql[1024];

if(data->poisk.metka_poi == 0)
 return(0);

if(iceb_u_proverka(data->poisk.kod_mat.ravno(),row[0],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poisk.kod_det.ravno(),row[2],0,0) != 0)
 return(1);

if(data->poisk.naim_km.getdlinna() > 1)
 {
  /*узнаём наименование материала*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[0]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   if(iceb_u_proverka(data->poisk.naim_km.ravno(),row1[0],4,0) != 0)
     return(1);

 }  
if(data->poisk.naim_kd.getdlinna() > 1)
 {
  /*узнаём наименование детали*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[2]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   if(iceb_u_proverka(data->poisk.naim_kd.ravno(),row1[0],4,0) != 0)
     return(1);
 }

   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_dokummat4_rasp(class l_dokummat4_data *data)
{
char strsql[512];
SQL_str row,row1;
FILE *ff;
SQLCURSOR cur,cur1;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;
class iceb_u_str naim_km("");
class iceb_u_str naim_kd("");
class iceb_u_str ei("");

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"vsmi%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Ведомость списания материалов на детали"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Ведомость списания материалов на детали"),ff,data->window);

fprintf(ff,"%s:%02d.%02d.%04d %s:%s %s:%d %s\n",
gettext("Дата документа"),data->dd,data->md,data->gd,
gettext("Номер"),data->nomdok.ravno(),
gettext("Склад"),data->skl,data->naim_skl.ravno());
if(data->poisk.metka_poi == 1)
 {
  if(data->poisk.kod_mat.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код материала"),data->poisk.kod_mat.ravno());

  if(data->poisk.naim_km.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Наименование материала"),data->poisk.naim_km.ravno());

  if(data->poisk.kod_det.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код детали"),data->poisk.kod_det.ravno());

  if(data->poisk.naim_kd.getdlinna() > 1)  
   fprintf(ff,"%s:%s\n",gettext("Наименование детали"),data->poisk.naim_kd.ravno());
 }
 
fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код м.|Ном.к.|   Наименование материала     |Код д.|   Наименование детали        |Ед.изм.|Количество|\n"));

fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");


while(cur.read_cursor(&row) != 0)
 {
  if(l_dokummat4_prov_row(row,data) != 0)
    continue;

  /*узнаём наименование материала*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[0]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_km.new_plus(row1[0]);
  else
   naim_km.new_plus("");
  
  /*узнаём наименование детали*/
  sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[2]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_kd.new_plus(row1[0]);
  else
   naim_kd.new_plus("");
   
  /*узнаём единицу измерения*/
  sprintf(strsql,"select ei from Kart where sklad=%d and nomk=%s",data->skl,row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   ei.new_plus(row1[0]);
  else
   ei.new_plus("");

  fprintf(ff,"%-6s|%-6s|%-*.*s|%-6s|%-*.*s|%*s|%10.6g|\n",
  row[0],
  row[1],
  iceb_u_kolbait(30,naim_km.ravno()),
  iceb_u_kolbait(30,naim_km.ravno()),
  naim_km.ravno(),
  row[2],
  iceb_u_kolbait(30,naim_kd.ravno()),
  iceb_u_kolbait(30,naim_kd.ravno()),
  naim_kd.ravno(),  
  iceb_u_kolbait(7,ei.ravno()),
  ei.ravno(),  
  atof(row[3]));

   
 }

fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");

iceb_podpis(ff,data->window);

fclose(ff);


iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
/***********************************/
/*проверка полноты списания*/
/**********************************/
int l_dokummat4_pps(class  l_dokummat4_data *data)
{
SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[1024];
double kolih_pod=0.;
double kolih_dok=0.;
int voz=0;

sprintf(strsql,"select distinct nk from Dokummat4 where datd='%04d-%02d-%02d' and skl=%d and nomd='%s'",
data->gd,data->md,data->dd,data->skl,data->nomdok.ravno());


if(cur.make_cursor(&bd,strsql) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {

  sprintf(strsql,"select km,kol from Dokummat4 where datd='%04d-%02d-%02d' and skl=%d and nomd='%s' and nk=%s",
  data->gd,data->md,data->dd,data->skl,data->nomdok.ravno(),row[0]);

  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
    return(1);
   }
  int kod_mat=0;
  kolih_dok=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    kod_mat=atoi(row1[0]);
    kolih_dok+=atof(row1[1]);
   }

  /*узнаём количество списанного материала в карточке*/
  kolih_pod=readkolkw(data->skl,atoi(row[0]),data->dd,data->md,data->gd,data->nomdok.ravno(),data->window);

  if(kolih_dok < kolih_pod)
   {
    sprintf(strsql,"%s %d %s! %f < %f",gettext("Материал"),kod_mat,gettext("списан не полностью"),kolih_dok,kolih_pod);
    iceb_menu_soob(strsql,data->window);
    voz=1;
   }
 }


return(voz);
}
