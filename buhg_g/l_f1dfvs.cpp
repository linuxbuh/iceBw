/*$Id: l_f1dfvs.c,v 1.30 2013/08/25 08:26:45 sasa Exp $*/
/*08.05.2015	22.04.2015	Белых А.И.	l_f1dfvs.c
Работа с военным сбором
*/
#include        <errno.h>
#include  "buhg_g.h"
#include  "l_f1dfvs.h"

enum
{
  COL_INN,
  COL_FIO,
  COL_NAH_DOHOD,
  COL_NAH_VS,
  COL_VIP_DOHOD,
  COL_VIP_VS,
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
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_f1dfvs_data
 {
  public:

  l_f1dfvs_rek poisk;

  class iceb_u_str inn;
  class iceb_u_str naimv;
  class iceb_u_str inn_tv; //только что введённая
  
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

  class iceb_u_str nomd;
  short god;
  int kvrt;
  class iceb_u_str zapros;
  //Конструктор
  l_f1dfvs_data()
   {
    snanomer=0;
    metka_voz=kl_shift=metka_poi=0;
    window=treeview=NULL;
    inn_tv.plus("");
    inn.new_plus("");
   }      
 };

gboolean   l_f1dfvs_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfvs_data *data);
void l_f1dfvs_vibor(GtkTreeSelection *selection,class l_f1dfvs_data *data);
void l_f1dfvs_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_f1dfvs_data *data);
void  l_f1dfvs_knopka(GtkWidget *widget,class l_f1dfvs_data *data);
void l_f1dfvs_add_columns (GtkTreeView *treeview);
void l_f1dfvs_udzap(class l_f1dfvs_data *data);
int  l_f1dfvs_prov_row(SQL_str row,class iceb_u_str *fio,class l_f1dfvs_data *data);
void l_f1dfvs_rasp(class l_f1dfvs_data *data);
int l_f1dfvs_create_list (class l_f1dfvs_data *data);

int l_f1dfvs_v(const char *nomd,class iceb_u_str *inn,GtkWidget *wpredok);
int l_f1dfvs_p(class l_f1dfvs_rek *rek_poi,GtkWidget *wpredok);


extern SQL_baza  bd;

int   l_f1dfvs(const char *nomd,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class l_f1dfvs_data data;
char bros[1024];
SQL_str row;
class SQLCURSOR cur;
/*Читаем шапку документа*/
sprintf(bros,"select * from F8dr where nomd='%s'",nomd);
if(sql_readkey(&bd,bros,&row,&cur) < 1)
 {
  iceb_menu_soob(gettext("Не найден документ !"),wpredok);
  return(1);
 } 

data.god=atoi(row[0]);
data.kvrt=atoi(row[1]);

data.nomd.new_plus(nomd);
data.poisk.clear_data();
data.name_window.plus(__FUNCTION__);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Военный сбор"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_f1dfvs_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Военный сбор"));


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
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_f1dfvs_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_f1dfvs_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_f1dfvs_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_f1dfvs_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_f1dfvs_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_f1dfvs_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

if(l_f1dfvs_create_list(&data) != 0)
 return(1);

gtk_widget_show(data.window);
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
int l_f1dfvs_create_list (class l_f1dfvs_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[1024];
int  kolstr=0;
SQL_str row;
class iceb_u_str fio("");

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_f1dfvs_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_f1dfvs_vibor),data);

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
G_TYPE_INT);


sprintf(strsql,"select * from F1dfvs where nomd='%s' order by inn asc",data->nomd.ravno());
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
double itogo[4];
memset(itogo,'\0',sizeof(itogo));

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  fio.new_plus("");
  
  if(l_f1dfvs_prov_row(row,&fio,data) != 0)
    continue;


  if(iceb_u_SRAV(data->inn_tv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;
  

  itogo[0]+=atof(row[3]);
  itogo[1]+=atof(row[4]);
  itogo[2]+=atof(row[5]);
  itogo[3]+=atof(row[6]);
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_INN,row[1],
  COL_FIO,fio.ravno(),
  COL_NAH_DOHOD,row[3],
  COL_NAH_VS,row[4],
  COL_VIP_DOHOD,row[5],
  COL_VIP_VS,row[6],
  COL_DATA_VREM,iceb_u_vremzap(row[8]),
  COL_KTO,iceb_kszap(row[7],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->inn_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_f1dfvs_add_columns (GTK_TREE_VIEW (data->treeview));


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


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Военный сбор"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql,"%s:%s %s:%d %s:%d",gettext("Документ"),data->nomd.ravno(),gettext("Год"),data->god,gettext("Квартал"),data->kvrt);
zagolov.ps_plus(strsql);

sprintf(strsql,"%s:%.2f %.2f",gettext("Начислено"),itogo[0],itogo[1]);
zagolov.ps_plus(strsql);

sprintf(strsql,"%s:%.2f %.2f",gettext("Выплачено"),itogo[2],itogo[3]);
zagolov.ps_plus(strsql);


gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.inn.ravno(),gettext("Идентификационный номер"));
  iceb_str_poisk(&zagolov,data->poisk.fio.ravno(),gettext("Фамилия"));
  

  iceb_label_set_text_color(data->label_poisk,zagolov.ravno(),"red");
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

void l_f1dfvs_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Иден.код"),renderer,"text",COL_INN,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_INN);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Фамилия"),renderer,"text",COL_FIO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_FIO);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Начис.доход"),renderer,"text",COL_NAH_DOHOD,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAH_DOHOD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Начис.сбор"),renderer,"text",COL_NAH_VS,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAH_VS);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Выпл.доход"),renderer,"text",COL_VIP_DOHOD,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_VIP_DOHOD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Выпл.сбор"),renderer,"text",COL_VIP_VS,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_VIP_VS);
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

void l_f1dfvs_vibor(GtkTreeSelection *selection,class l_f1dfvs_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *inn;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_INN,&inn,NUM_COLUMNS,&nomer,-1);

data->inn.new_plus(inn);
data->snanomer=nomer;

g_free(inn);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_f1dfvs_knopka(GtkWidget *widget,class l_f1dfvs_data *data)
{
iceb_u_str repl;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_f1dfvs_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    if(l_f1dfvs_v(data->nomd.ravno(),&data->inn_tv,data->window) == 0)
      l_f1dfvs_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;

    if(l_f1dfvs_v(data->nomd.ravno(),&data->inn,data->window) == 0)
      l_f1dfvs_create_list(data);

    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_f1dfvs_udzap(data);
    l_f1dfvs_create_list(data);
    return;  
  

  case FK4:
    if(l_f1dfvs_p(&data->poisk,data->window) == 0)
     data->metka_poi=1;
    else
     data->metka_poi=0;
    l_f1dfvs_create_list(data);
    return;  

  case FK5:
    l_f1dfvs_rasp(data);
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

gboolean   l_f1dfvs_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfvs_data *data)
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
void l_f1dfvs_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_f1dfvs_data *data)
{
 g_signal_emit_by_name(data->knopka[SFK2],"clicked");

}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_f1dfvs_udzap(class l_f1dfvs_data *data)
{


char strsql[512];

sprintf(strsql,"delete from F1dfvs where nomd='%s' and inn='%s'",data->nomd.ravno(),data->inn.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_f1dfvs_prov_row(SQL_str row,class iceb_u_str *fio,class l_f1dfvs_data *data)
{

fio->new_plus("");
if(row[2][0] == '\0')
 {
  SQL_str row1;
  class SQLCURSOR cur1;
  char strsql[1024];
  /*Читаем фамилию*/

  sprintf(strsql,"select fio from Kartb where inn='%s'",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) < 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден идентификационный номер"),row[1]);
    iceb_menu_soob(strsql,data->window);
   }
  else
    fio->new_plus(row1[0]);
 }
else
   fio->new_plus(row[2]);


if(data->metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.inn.ravno(),row[1],4,0) != 0)
 return(1);


if(iceb_u_proverka(data->poisk.fio.ravno(),fio->ravno(),4,0) != 0)
 return(1);

   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_f1dfvs_rasp(class l_f1dfvs_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;
class iceb_u_str fio("");


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"kateg%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Военный сбор"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Военный сбор"),ff,data->window);

fprintf(ff,"%s:%s %s:%d %s:%d\n",
gettext("Документ"),
data->nomd.ravno(),
gettext("Год"),
data->god,
gettext("Квартал"),
data->kvrt);

if(data->metka_poi == 1)
 {
  if(data->poisk.inn.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Идентификационный номер"),data->poisk.inn.ravno());
  if(data->poisk.fio.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Фамилия"),data->poisk.fio.ravno());
 }




fprintf(ff,"----------------------------------------------------------------------------------\n");

fprintf(ff,"%s\n",gettext("Идент.код |       Фамилия                |Нач.доход |Нач.сбор|Вып.доход |Вып.сбор|"));

fprintf(ff,"----------------------------------------------------------------------------------\n");

double itogo[4];
memset(itogo,'\0',sizeof(itogo));

cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(l_f1dfvs_prov_row(row,&fio,data) != 0)
    continue;


  fprintf(ff,"%-10s|%-*.*s|%10.2f|%8.2f|%10.2f|%8.2f|\n",
  row[1],
  iceb_u_kolbait(30,fio.ravno()),
  iceb_u_kolbait(30,fio.ravno()),
  fio.ravno(),
  atof(row[3]),
  atof(row[4]),
  atof(row[5]),
  atof(row[6]));
  
  itogo[0]+=atof(row[3]);
  itogo[1]+=atof(row[4]);
  itogo[2]+=atof(row[5]);
  itogo[3]+=atof(row[6]);
  

 }
fprintf(ff,"----------------------------------------------------------------------------------\n");
fprintf(ff,"%46s:%10.2f %8.2f %10.2f %8.2f\n",gettext("Итого"),itogo[0],itogo[1],itogo[2],itogo[3]);

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
