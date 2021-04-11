/*$Id: l_uplout.c,v 1.18 2013/08/25 08:26:42 sasa Exp $*/
/*08.05.2015	05.03.2008	Белых А.И.	l_uplout.c
Работа со списком объектов учёта топлива
*/
#include        <errno.h>
#include  "buhg_g.h"
#include  "l_uplout.h"

enum
{
  COL_KOD,
  COL_NAIM,
  COL_SKLAD,
  COL_KONTR,
  COL_METKA_KOD,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  SFK3,
  FK4,
  FK5,
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_uplout_data
 {
  public:

  l_uplout_rek poisk;

  iceb_u_str kodv;
  iceb_u_str naimv;
  iceb_u_str kod_gr_tv; //только что введённая единица измерения
  
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
  short     metka_poi;  //0-без поиска 1-с поиском
  int       metka_voz;  //0-выбрали 1-нет  
  short     metka_rr;   //0-работа со списком 1-выбор
  int metka_pok; /*0-все записи 1-только водителей 2-только заправки*/
  //Конструктор
  l_uplout_data()
   {
    snanomer=0;
    metka_rr=metka_voz=kl_shift=metka_poi=0;
    window=treeview=NULL;
    kod_gr_tv.plus("");
    metka_pok=0;
   }      
 };

gboolean   l_uplout_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplout_data *data);
void l_uplout_vibor(GtkTreeSelection *selection,class l_uplout_data *data);
void l_uplout_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uplout_data *data);
void  l_uplout_knopka(GtkWidget *widget,class l_uplout_data *data);
void l_uplout_add_columns (GtkTreeView *treeview);
void l_uplout_udzap(class l_uplout_data *data);
int  l_uplout_prov_row(SQL_str row,class l_uplout_data *data);
void l_uplout_rasp(class l_uplout_data *data);
int  l_uplout_pvu(int,const char *kod,GtkWidget *wpredok);
void l_uplout_create_list (class l_uplout_data *data);

int l_uplout_v(class iceb_u_str *kod_gr,GtkWidget *wpredok);
int l_uplout_p(class l_uplout_rek *rek_poi,GtkWidget *wpredok);
void l_gruppdok_unk(class l_uplout_data *data);


extern SQL_baza  bd;

int   l_uplout(int metka_rr, //0-ввод и корек. 1-выбор
iceb_u_str *kod,iceb_u_str *naim,
int metka_pok, /*0-все записи 1-только водителей 2-только заправки*/
GtkWidget *wpredok)
{
int gor=0;
int ver=0;

class l_uplout_data data;
char bros[512];

data.poisk.clear_data();
data.metka_rr=metka_rr;
data.metka_pok=metka_pok;
data.name_window.plus(__FUNCTION__);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_rr == 1)
 {
  if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
 }


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список объектов учёта топлива"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uplout_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Список объектов учёта топлива"));


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
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_uplout_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_uplout_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_uplout_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(l_uplout_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удалить неиспользуемые коды групп"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_uplout_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_uplout_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_uplout_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_uplout_create_list(&data);

gtk_widget_show(data.window);
if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();

if(data.metka_voz == 0)
 {
  kod->new_plus(data.kodv.ravno());
  naim->new_plus(data.naimv.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_uplout_create_list (class l_uplout_data *data)
{
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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_uplout_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_uplout_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

if(data->metka_pok == 0)
 sprintf(strsql,"select * from Uplouot order by kod asc");
if(data->metka_pok == 1)
 sprintf(strsql,"select * from Uplouot where mt=1 order by kod asc");
if(data->metka_pok == 2)
 sprintf(strsql,"select * from Uplouot where mt=0 order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
class iceb_u_str naim("");
SQL_str row1;
class SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(l_uplout_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->kod_gr_tv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Код
  ss[COL_KOD].new_plus(row[0]);
  
  ss[COL_METKA_KOD].new_plus("");

  /*Метка кода*/     
  if(row[1][0] == '1')
    ss[COL_METKA_KOD].new_plus(gettext("Водитель"));
  if(row[1][0] == '0')
    ss[COL_METKA_KOD].new_plus(gettext("Заправка"));
   
  /*Склад*/
  naim.new_plus("");
  sprintf(strsql,"select naik from Sklad where kod=%d",atoi(row[2]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim.new_plus(row1[0]);
  sprintf(strsql,"%s %s",row[2],naim.ravno());
  ss[COL_SKLAD].new_plus(strsql);

  /*контрагент*/
  naim.new_plus("");
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[4]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim.new_plus(row1[0]);
  sprintf(strsql,"%s %s",row[4],naim.ravno());
  ss[COL_KONTR].new_plus(strsql);
  

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,row[0],
  COL_NAIM,row[3],
  COL_KONTR,ss[COL_KONTR].ravno(),
  COL_SKLAD,ss[COL_SKLAD].ravno(),
  COL_METKA_KOD,ss[COL_METKA_KOD].ravno(),
  COL_DATA_VREM,iceb_u_vremzap(row[6]),
  COL_KTO,iceb_kszap(row[5],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kod_gr_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_uplout_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список объектов учёта топлива"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kod.ravno(),gettext("Код"));
  iceb_str_poisk(&zagolov,data->poisk.naim.ravno(),gettext("Наименование"));
  

  iceb_label_set_text_color(data->label_poisk,zagolov.ravno(),"red");
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_uplout_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Вид объекта"), renderer,"text", COL_METKA_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Склад"), renderer,"text", COL_SKLAD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Контрагент"), renderer,"text", COL_KONTR,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

}

/****************************/
/*Выбор строки*/
/**********************/

void l_uplout_vibor(GtkTreeSelection *selection,class l_uplout_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_KOD,&kod,COL_NAIM,&naim,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(kod);
data->naimv.new_plus(naim);
data->snanomer=nomer;

g_free(kod);
g_free(naim);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_uplout_knopka(GtkWidget *widget,class l_uplout_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_uplout_knopka knop=%d\n",knop);
data->kl_shift=0;
switch (knop)
 {
  case FK2:
    l_uplout_v(&data->kod_gr_tv,data->window);

    l_uplout_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    l_uplout_v(&data->kodv,data->window);

    l_uplout_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_uplout_udzap(data);
    l_uplout_create_list(data);
    return;  
  
  case SFK3:
    if(data->kolzap == 0)
      return;

    if(iceb_menu_danet(gettext("Удалить неиспользуемые коды ? Вы уверены ?"),2,data->window) != 1)
     return;

    l_gruppdok_unk(data);
    l_uplout_create_list(data);
    return;  


  case FK4:
    if(l_uplout_p(&data->poisk,data->window) == 0)
     data->metka_poi=1;
    else
     data->metka_poi=0;
    l_uplout_create_list(data);
    return;  

  case FK5:
    l_uplout_rasp(data);
    return;  

    
  case FK10:
    if(data->metka_rr == 1)
      iceb_sizww(data->name_window.ravno(),data->window);
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplout_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplout_data *data)
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
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK3],"clicked");
    else
      g_signal_emit_by_name(data->knopka[SFK3],"clicked");
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
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void l_uplout_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uplout_data *data)
{
if(data->metka_rr == 0)
 {
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");
  return;
 }

iceb_sizww(data->name_window.ravno(),data->window);

data->metka_voz=0;

gtk_widget_destroy(data->window);


}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_uplout_udzap(class l_uplout_data *data)
{

if(l_uplout_pvu(1,data->kodv.ravno(),data->window) != 0)
 return;

char strsql[512];

sprintf(strsql,"delete from Uplouot where kod=%d",data->kodv.ravno_atoi());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_uplout_prov_row(SQL_str row,class l_uplout_data *data)
{
if(data->metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.kod.ravno(),row[0],0,0) != 0)
 return(1);


//Поиск образца в строке
if(data->poisk.naim.ravno()[0] != '\0')
 if(iceb_u_strstrm(row[3],data->poisk.naim.ravno()) == 0)
  return(1);


   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_uplout_rasp(class l_uplout_data *data)
{
char strsql[1024];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Uplouot order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"ouot%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список объектов учёта топлива"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Список объектов учёта топлива"),0,0,0,0,0,0,ff,data->window);

if(data->metka_poi == 1)
 {
  if(data->poisk.kod.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код"),data->poisk.kod.ravno());
  if(data->poisk.naim.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Наименование"),data->poisk.naim.ravno());
 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Код |Наименование        |Вид объекта|       Склад        |   Контрагент       |Дата и время запис.| Кто записал\n"));
//   12345678901234567890 12345678901 12345678901234567890 12345678901234567890
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------\n");

class iceb_u_str naim("");
class iceb_u_str naim_sklad("");
class iceb_u_str naim_kontr("");
class iceb_u_str vidob("");
SQL_str row1;
class SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
  if(l_uplout_prov_row(row,data) != 0)
    continue;

  /*Метка кода*/     
  if(row[1][0] == '1')
    vidob.new_plus(gettext("Водитель"));
  else
    vidob.new_plus(gettext("Заправка"));
   
  /*Склад*/
  sprintf(strsql,"select naik from Sklad where kod=%d",atoi(row[2]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim.new_plus(row1[0]);
  else
   naim.new_plus("");

  sprintf(strsql,"%s %s",row[2],naim.ravno());
  naim_sklad.new_plus(strsql);
  
  /*контрагент*/
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[4]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim.new_plus(row1[0]);
  else
   naim.new_plus("");
  sprintf(strsql,"%s %s",row[4],naim.ravno());
  naim_kontr.new_plus(strsql);
  


  fprintf(ff,"%-4s %-*.*s %-*s %-*.*s %-*.*s %s %s\n",
  row[0],
  iceb_u_kolbait(20,row[3]),iceb_u_kolbait(20,row[3]),row[3],
  iceb_u_kolbait(11,vidob.ravno()),vidob.ravno(),
  iceb_u_kolbait(20,naim_sklad.ravno()),iceb_u_kolbait(20,naim_sklad.ravno()),naim_sklad.ravno(),
  iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
  iceb_u_vremzap(row[6]),iceb_kszap(row[5],data->window));
  
  if(iceb_u_strlen(row[3]) > 20)
   fprintf(ff,"%4s %s\n","",iceb_u_adrsimv(20,row[3]));

 }
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);

iceb_rabfil(&imaf,&naimot,data->window);

}
/*****************************/
/*Проверить возможность удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int  l_uplout_pvu(int met,const char *kod,GtkWidget *wpredok)
{
char strsql[512];


sprintf(strsql,"select kv from Upldok where kv=%s limit 1",kod);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Upldok",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kv from Upldok1 where kodsp=%s limit 1",kod);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Upldok1",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kv from Uplsal where kv=%s limit 1",kod);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Uplsal",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kzv from Uplavt where kzv=%s limit 1",kod);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Uplavt",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    



return(0);

}
/**********************************/
/*Удаление неиспользуемых кодов групп*/
/**************************************/
void l_gruppdok_unk(class l_uplout_data *data)
{
SQL_str row;
SQLCURSOR cur;

char strsql[512];
sprintf(strsql,"select kod from Uplouot");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
int koludzap=0;

while(cur.read_cursor(&row) != 0)
 {
  if(l_uplout_pvu(0,row[0],data->window) != 0)
   continue;
  sprintf(strsql,"delete from Uplouot where kod=%d",atoi(row[0]));
  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   break;
  koludzap++;
 }

sprintf(strsql,"%s:%d",gettext("Количество удалённых записей"),koludzap);
iceb_menu_soob(strsql,data->window);


}

