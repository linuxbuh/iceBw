/*$Id: l_zarsdo.c,v 1.30 2013/08/25 08:26:45 sasa Exp $*/
/*01.05.2020	30.05.2016	Белых А.И.	l_zarsdo.c
Работа со списком должностных окладов
*/
#include        <errno.h>
#include  "buhg_g.h"
#include  "l_zarsdo.h"

enum
{
  COL_TABNOM,
  COL_FIO,
  COL_DATAZ,
  COL_KOD_NAH,
  COL_SUMA,
  COL_METKA_SP,
  COL_KOL_HAS,
  COL_KOEF,
  COL_SHET,
  COL_KOMENT,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK6,
  FK7,
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_zarsdo_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  class iceb_u_str name_window;
  
  class l_zarsdo_rek poisk;

  class iceb_u_str tabnom;
  class iceb_u_str dataz;
  class iceb_u_str kod_nah;
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  class iceb_u_str zapros;
  //Конструктор
  l_zarsdo_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;

   }      
 };

gboolean   l_zarsdo_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsdo_data *data);
void l_zarsdo_vibor(GtkTreeSelection *selection,class l_zarsdo_data *data);
void l_zarsdo_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zarsdo_data *data);
void  l_zarsdo_knopka(GtkWidget *widget,class l_zarsdo_data *data);
void l_zarsdo_add_columns (GtkTreeView *treeview);
void l_zarsdo_udzap(class l_zarsdo_data *data);
int  l_zarsdo_prov_row(SQL_str row,class l_zarsdo_data *data);
void l_zarsdo_rasp(class l_zarsdo_data *data);
int l_zarsdo_create_list (class l_zarsdo_data *data);

int l_zarsdo_v(const char *tabnom,const char *dataz,const char *kod_nah,GtkWidget *wpredok);
int l_zarsdo_p(class l_zarsdo_rek *rek_poi,GtkWidget *wpredok);


extern SQL_baza  bd;

void l_zarsdo(int tabnom,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class l_zarsdo_data data;
char bros[1024];
data.poisk.clear_data();

if(tabnom != 0)
 {
  data.poisk.tabnom.plus(tabnom);
  data.poisk.metka_poi=1;
 }
printf("%s-tabnom=%s\n",__FUNCTION__,data.poisk.tabnom.ravno());

data.name_window.plus(__FUNCTION__);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список должностных окладов"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zarsdo_key_press),&data);
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
data.label_kolstr=gtk_label_new (gettext("Список должностных окладов"));


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
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_zarsdo_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_zarsdo_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_zarsdo_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);


sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_zarsdo_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_zarsdo_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Сортировка"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(l_zarsdo_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Переключение режима просмотра - отсортированные по табельным номерам/отсортированные по датам/только последние записи изменения окладов"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"76 %s",gettext("Работающие/уволенные"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(l_zarsdo_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Переключение режима просмотра-все/только работающие/только уволенные"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_zarsdo_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

if(l_zarsdo_create_list(&data) != 0)
 return;

gtk_widget_show(data.window);
gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return;

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
int l_zarsdo_create_list (class l_zarsdo_data *data)
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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_zarsdo_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_zarsdo_vibor),data);

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
G_TYPE_INT);


if(data->poisk.metka_sort == 0)
 sprintf(strsql,"select * from Zarsdo order by tn asc,dt desc,kn asc");
if(data->poisk.metka_sort == 1)
 sprintf(strsql,"select * from Zarsdo order by dt desc,tn asc,kn asc");
if(data->poisk.metka_sort == 2)
 sprintf(strsql,"select distinct tn from Zarsdo order by tn asc");

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  gdite.close();
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
//gtk_list_store_clear(model);

data->kolzap=0;
float kolstr1=0. ;
class iceb_u_str fio("");
int metka_sp=0;
class iceb_u_str naim_sp("");
class l_zarsdo_menu_mr metka_str;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(l_zarsdo_prov_row(row,data) != 0)
    continue;


  
  /*читаем фамилию*/
  sprintf(strsql,"select fio from Kartb where tabn=%d",atoi(row[0]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   fio.new_plus(row1[0]);
  else
   fio.new_plus("");

  if(data->poisk.metka_sort != 2)
   {

    metka_sp=atoi(row[4]);
    naim_sp.new_plus(metka_str.strsr.ravno(metka_sp));


    gtk_list_store_append (model, &iter);

    gtk_list_store_set (model, &iter,
    COL_TABNOM,row[0],
    COL_FIO,fio.ravno(),
    COL_DATAZ,iceb_u_datzap_mg(row[1]),
    COL_KOD_NAH,row[2],
    COL_SUMA,row[3],
    COL_METKA_SP,naim_sp.ravno(),
    COL_KOL_HAS,row[5],
    COL_KOEF,row[6],
    COL_SHET,row[7],
    COL_KOMENT,row[8],    
    COL_DATA_VREM,iceb_u_vremzap(row[10]),
    COL_KTO,iceb_kszap(row[9],data->window),
    NUM_COLUMNS,data->kolzap,
    -1);
   }
  else
   {
    sprintf(strsql,"select * from Zarsdo where tn=%d order by dt desc,tn asc,kn asc limit 1",atoi(row[0]));
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) <= 0)
     continue;
    metka_sp=atoi(row1[4]);

    naim_sp.new_plus(metka_str.strsr.ravno(metka_sp));
//    fio.new_plus(iceb_u_getfiosin(fio.ravno(),0));
          
    gtk_list_store_append (model, &iter);

    gtk_list_store_set (model, &iter,
    COL_TABNOM,row1[0],
    COL_FIO,fio.ravno(),
    COL_DATAZ,iceb_u_datzap_mg(row1[1]),
    COL_KOD_NAH,row1[2],
    COL_SUMA,row1[3],
    COL_METKA_SP,naim_sp.ravno(),
    COL_KOL_HAS,row1[5],
    COL_KOEF,row1[6],
    COL_SHET,row1[7],
    COL_KOMENT,row1[8],    
    COL_DATA_VREM,iceb_u_vremzap(row1[10]),
    COL_KTO,iceb_kszap(row1[9],data->window),
    NUM_COLUMNS,data->kolzap,
    -1);

   }

  data->kolzap++;
 }
//data->kod_kat_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_zarsdo_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


class iceb_u_str stroka;
class iceb_u_str zagolov;
zagolov.plus(gettext("Список должностных окладов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

if(data->poisk.metka_sort == 0)
   zagolov.ps_plus(gettext("Сортировка по табельным номерам"));
if(data->poisk.metka_sort == 1)
   zagolov.ps_plus(gettext("Сортировка по датам"));
if(data->poisk.metka_sort == 2)
   zagolov.ps_plus(gettext("Только последние записи"));


if(data->poisk.metka_uv == 1)
   zagolov.ps_plus(gettext("Только работающие"));
if(data->poisk.metka_uv == 2)
   zagolov.ps_plus(gettext("Только уволенные"));

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());
gtk_label_set_use_markup (GTK_LABEL (data->label_kolstr), TRUE);

if(data->poisk.metka_poi == 1)
 {
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");


  iceb_str_poisk(&zagolov,data->poisk.tabnom.ravno(),gettext("Табельный номер"));
  iceb_str_poisk(&zagolov,data->poisk.dataz.ravno(),gettext("Дата"));
  iceb_str_poisk(&zagolov,data->poisk.kod_nah.ravno(),gettext("Код начисления"));
  iceb_str_poisk(&zagolov,data->poisk.fio.ravno(),gettext("Фамилия"));
  iceb_str_poisk(&zagolov,data->poisk.shet.ravno(),gettext("Счёт"));
  iceb_str_poisk(&zagolov,data->poisk.koment.ravno(),gettext("Коментарий"));
  class l_zarsdo_menu_mr sr;
  sr.strsr.plus(gettext("Все способы расчёта"));
  iceb_str_poisk(&zagolov,sr.strsr.ravno(data->poisk.metka_sr),gettext("Способ расчёта"));
  
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

void l_zarsdo_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Т.н."),renderer,"text",COL_TABNOM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_TABNOM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Фамилия"),renderer,"text",COL_FIO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_FIO);
gtk_tree_view_append_column (treeview, column);




renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата"),renderer,"text",COL_DATAZ,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATAZ);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код нач."),renderer,"text",COL_KOD_NAH,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOD_NAH);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Сумма"),renderer,"text",COL_SUMA,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_SUMA);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Способ расчёта"),renderer,"text",COL_METKA_SP,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_METKA_SP);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Количество часов"),renderer,"text",COL_KOL_HAS,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOL_HAS);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Коэффициент"),renderer,"text",COL_KOEF,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOEF);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Счёт"),renderer,"text",COL_SHET,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_SHET);
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

void l_zarsdo_vibor(GtkTreeSelection *selection,class l_zarsdo_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *tabnom;
gchar *dataz;
gchar *kod_nah;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_TABNOM,&tabnom,COL_DATAZ,&dataz,COL_KOD_NAH,&kod_nah,NUM_COLUMNS,&nomer,-1);

data->tabnom.new_plus(tabnom);
data->dataz.new_plus(dataz);
data->kod_nah.new_plus(kod_nah);
data->snanomer=nomer;

g_free(tabnom);
g_free(dataz);
g_free(kod_nah);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_zarsdo_knopka(GtkWidget *widget,class l_zarsdo_data *data)
{
iceb_u_str repl;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_zarsdo_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
     if(l_zarsdo_v("","","",data->window) == 0)
      l_zarsdo_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
     if(l_zarsdo_v(data->tabnom.ravno(),data->dataz.ravno(),data->kod_nah.ravno(),data->window) == 0)
      l_zarsdo_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_zarsdo_udzap(data);
    l_zarsdo_create_list(data);
    return;  
  


  case FK4:
    l_zarsdo_p(&data->poisk,data->window);
    l_zarsdo_create_list(data);
    return;  

  case FK5:
    l_zarsdo_rasp(data);
    return;  

  case FK6:
    data->poisk.metka_sort++;
    if(data->poisk.metka_sort >= 3)
     data->poisk.metka_sort=0;
    l_zarsdo_create_list(data);
    return;  

  case FK7:
    data->poisk.metka_uv++;
    if(data->poisk.metka_uv >= 3)
     data->poisk.metka_uv=0;
    l_zarsdo_create_list(data);
    return;  

    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarsdo_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsdo_data *data)
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

  case GDK_KEY_F6:
    g_signal_emit_by_name(data->knopka[FK6],"clicked");
    return(TRUE);

  case GDK_KEY_F7:
    g_signal_emit_by_name(data->knopka[FK7],"clicked");
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
void l_zarsdo_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zarsdo_data *data)
{

g_signal_emit_by_name(data->knopka[SFK2],"clicked");
return;

}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_zarsdo_udzap(class l_zarsdo_data *data)
{


char strsql[512];

sprintf(strsql,"delete from Zarsdo where dt='%s' and tn=%d and kn=%d",data->dataz.ravno_sqldata(),data->tabnom.ravno_atoi(),data->kod_nah.ravno_atoi());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_zarsdo_prov_row(SQL_str row,class l_zarsdo_data *data)
{


if(data->poisk.metka_uv == 1) /*только работающие*/
 {
  SQL_str row1;
  class SQLCURSOR cur1;
  char strsql[512];
  sprintf(strsql,"select datk from Zarn where tabn=%d order by god desc,mes desc limit 1",atoi(row[0]));

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   if(row1[0][0] != '0')
    return(1);
 }

if(data->poisk.metka_uv == 2) /*только уволенные*/
 {
  SQL_str row1;
  class SQLCURSOR cur1;
  char strsql[512];
  sprintf(strsql,"select datk from Zarn where tabn=%d order by god desc,mes desc limit 1",atoi(row[0]));

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   if(row1[0][0] == '0')
    return(1);
 }




if(data->poisk.metka_poi == 0)
 return(0);

 
if(iceb_u_proverka(data->poisk.tabnom.ravno(),row[0],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poisk.shet.ravno(),row[7],1,0) != 0)
 return(1);

if(iceb_u_proverka(data->poisk.koment.ravno(),row[8],4,0) != 0)
 return(1);

if(iceb_u_proverka(data->poisk.kod_nah.ravno(),row[2],0,0) != 0)
 return(1);

if(data->poisk.dataz.getdlinna() > 1)
 {
  short d=0,m=0,g=0;
  short mp=0,gp=0;
  
  iceb_u_rsdat(&d,&m,&g,row[1],2);

  iceb_u_rsdat1(&mp,&gp,data->poisk.dataz.ravno());
  if(m != mp || g != gp)
   return(1);  
 }


if(data->poisk.fio.getdlinna() > 1)
 {
  SQL_str row1;
  class SQLCURSOR cur1;
  char strsql[512];
  class iceb_u_str fio("");


  sprintf(strsql,"select fio from Kartb where tabn=%d",atoi(row[0]));

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   fio.new_plus(row1[0]);

  if(iceb_u_proverka(data->poisk.fio.ravno(),fio.ravno(),4,0) != 0)
   return(1);
   
 } 


if(data->poisk.podr.getdlinna() > 1)
 {
  SQL_str row1;
  class SQLCURSOR cur1;
  char strsql[512];
  class iceb_u_str podr("");


  sprintf(strsql,"select podr from Kartb where tabn=%d",atoi(row[0]));

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   podr.new_plus(row1[0]);

  if(iceb_u_proverka(data->poisk.podr.ravno(),podr.ravno(),0,0) != 0)
   return(1);

 }

class l_zarsdo_menu_mr sr;

if(data->poisk.metka_sr < sr.strsr.kolih())
 {
  if(data->poisk.metka_sr != atof(row[4]))
   return(1);
  
 }

return(0);
}
/************************************************/
/*шапка распечатки*/
/**********************************/
void l_zarsdo_rh(class iceb_rnl_c *rh,int *nom_str,FILE *ff)
{
if(nom_str != NULL)
  *nom_str+=4;

fprintf(ff,"%95s%s:%d\n","",gettext("Лист"),rh->nom_list);
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Т.н. |          Фамилия             |Дата   |К.н.|  Сумма   |Способ расчёта|Кол.ч.|Коэф.| Счёт |Коментарий\n"));

/*
123456 123456789012345678901234567890 00.0000 1234 1234567890 12345678901234 123456 12345 123456
*/
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------\n");

}

/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_zarsdo_rasp(class l_zarsdo_data *data)
{
char strsql[1024];
SQL_str row,row1;
SQLCURSOR cur,cur1;
class iceb_u_spisok imaf;
class iceb_u_spisok naimot;
int kolstr=0;


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"sdow%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список должностных окладов"));

class iceb_fopen fil;
if(fil.start(imaf.ravno(0),"w",data->window) != 0)
 return;

iceb_zagolov(gettext("Список должностных окладов"),fil.ff,data->window);

if(data->poisk.metka_sort == 2)
 fprintf(fil.ff,"%s\n",gettext("Только последние записи"));
if(data->poisk.metka_uv == 1)
 fprintf(fil.ff,"%s\n",gettext("Только работающие"));
if(data->poisk.metka_uv == 2)
 fprintf(fil.ff,"%s\n",gettext("Только уволенные"));

if(data->poisk.tabnom.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Табельный номер"),data->poisk.tabnom.ravno());
if(data->poisk.dataz.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Дата"),data->poisk.dataz.ravno());
if(data->poisk.kod_nah.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Код начисления"),data->poisk.kod_nah.ravno());
if(data->poisk.fio.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Фамилия"),data->poisk.fio.ravno());
if(data->poisk.koment.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Коментарий"),data->poisk.koment.ravno());
if(data->poisk.shet.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Счёт"),data->poisk.shet.ravno());

class l_zarsdo_menu_mr sr;
if(data->poisk.metka_sr < sr.strsr.kolih())
 fprintf(fil.ff,"%s:%s\n",gettext("Способ расчёта"),sr.strsr.ravno(data->poisk.metka_sr));

class iceb_rnl_c rh;
l_zarsdo_rh(&rh,NULL,fil.ff);
 

short d=0,m=0,g=0;
int metka_sp=0;
class iceb_u_str fio("");
class iceb_u_str naim_sp("");
class l_zarsdo_menu_mr metka_str;
int kolzap=0;
while(cur.read_cursor(&row) != 0)
 {
  if(l_zarsdo_prov_row(row,data) != 0)
    continue;

  kolzap++;
  sprintf(strsql,"select fio from Kartb where tabn=%d",atoi(row[0]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   fio.new_plus(row1[0]);

  if(data->poisk.metka_sort != 2)
   {
    iceb_u_rsdat(&d,&m,&g,row[1],2);
    
    metka_sp=atoi(row[4]);

    naim_sp.new_plus(metka_str.strsr.ravno(metka_sp));


    fprintf(fil.ff,"%6s %-*.*s %02d.%04d %4s %10s %-*.*s %6s %5s %6s %s\n",
    row[0],
    iceb_u_kolbait(30,fio.ravno()),  
    iceb_u_kolbait(30,fio.ravno()),  
    fio.ravno(),
    m,g,
    row[2],
    row[3],
    iceb_u_kolbait(14,naim_sp.ravno()),
    iceb_u_kolbait(14,naim_sp.ravno()),
    naim_sp.ravno(),
    row[5],
    row[6],
    row[7],
    row[8]);
   }
  else
   {

    sprintf(strsql,"select * from Zarsdo where tn=%d order by dt desc,tn asc,kn asc limit 1",atoi(row[0]));
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) <= 0)
     continue;


    iceb_u_rsdat(&d,&m,&g,row1[1],2);
    
    metka_sp=atoi(row1[4]);

    naim_sp.new_plus(metka_str.strsr.ravno(metka_sp));


    fprintf(fil.ff,"%6s %-*.*s %02d.%04d %4s %10s %-*.*s %6s %5s %6s %s\n",
    row1[0],
    iceb_u_kolbait(30,fio.ravno()),  
    iceb_u_kolbait(30,fio.ravno()),  
    fio.ravno(),
    m,g,
    row1[2],
    row1[3],
    iceb_u_kolbait(14,naim_sp.ravno()),
    iceb_u_kolbait(14,naim_sp.ravno()),
    naim_sp.ravno(),
    row1[5],
    row1[6],
    row1[7],
    row1[8]);
   } 
  

 }

fprintf(fil.ff,"\
------------------------------------------------------------------------------------------------------------\n");
fprintf(fil.ff,"%s:%d\n",gettext("Количество записей"),kolzap);
iceb_podpis(fil.ff,data->window);

fil.end();

//iceb_ustpeh(imaf.ravno(0),3,data->window);

iceb_ustpeh(imaf.ravno(0),3,&rh.orient,data->window);
iceb_rnl(imaf.ravno(0),&rh,&l_zarsdo_rh,data->window);

iceb_rabfil(&imaf,&naimot,data->window);

}
