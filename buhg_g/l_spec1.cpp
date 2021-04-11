/*$Id: l_spec1.c,v 1.36 2013/12/31 11:49:14 sasa Exp $*/
/*08.05.2015	15.05.2005	Белых А.И.	l_spec1.c
Ввод и корректировка спецификаций на изделия
*/
#include <errno.h>
#include "buhg_g.h"

enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK7,
 FK10,
 KOL_F_KL
};

enum
{
 COL_METKA,
 COL_KOD,
 COL_NAIM,
 COL_EI,
 COL_KOLIH,
 COL_KOMENT,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  l_spec1_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *treeview;
  GtkWidget *window;
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       voz;
  
  //Реквизиты выбранной записи
  iceb_u_str kod;
      
  iceb_u_spisok imaf;

  int       kod_v; //Код только что введенного материалла и для поиска материалла по коду материалла
  iceb_u_str naim_poi; //Поиск по наименованию

  iceb_u_str kod_izdel; //Код изделия которое просмотриваем    
  iceb_u_str naim_izdel; //Наименование изделия, которое просмотриваем
  int uroven; //Уровень глубины просмотра
  
  l_spec1_data()
   {
    clear();
    kod_v=0;
    naim_izdel.plus("");
   }
  void clear()
   {
    voz=0;
    kl_shift=0;
    snanomer=0;
    treeview=NULL;
    naim_poi.new_plus("");
    kod_v=0;
   }
 };

gboolean   l_spec1_key_press(GtkWidget *widget,GdkEventKey *event,class l_spec1_data *data);
void l_spec1_vibor(GtkTreeSelection *selection,class l_spec1_data *data);
void l_spec1_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_spec1_data *data);
void  l_spec1_knopka(GtkWidget *widget,class l_spec1_data *data);
void l_spec1_add_columns (GtkTreeView *treeview);
void l_spec1_create_list (class l_spec1_data *data);


int specprov(char *kodiz,iceb_u_str *naim,iceb_u_str *poisk,int metka_naim,GtkWidget *wpredok);
void spec1_rasp(int kod_izdel,GtkWidget *wpredok);


extern SQL_baza  bd;

void l_spec1(const char *kod_izdel, //Код изделия список которого просмотриваем
int uroven, //Уровень глубины просмотра
GtkWidget *wpredok)
{
class  l_spec1_data data;
char strsql[512];
SQL_str row;
SQLCURSOR cur;

data.kod_izdel.new_plus(kod_izdel);
data.uroven=uroven+1;

//Узнаём наименование изделия
sprintf(strsql,"select naimat from Material where kodm=%d",data.kod_izdel.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.naim_izdel.new_plus(row[0]);
else
 {
  sprintf(strsql,"select naius from Uslugi where kodus=%d",data.kod_izdel.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   data.naim_izdel.new_plus(row[0]);
 }
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),
gettext("Работа со спецификацией на изделие"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_spec1_key_press),&data);
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

data.label_kolstr=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);



//Кнопки
sprintf(strsql,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_spec1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод нового материалла"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"SF2 %s",gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_spec1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_spec1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить выбранную запись"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"SF3 %s",gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(l_spec1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удалить все записи"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);

sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_spec1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_spec1_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка спецификации изделия"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F7 %s",gettext("Ввести"));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(l_spec1_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Ввод новой услуги"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show(data.knopka[FK7]);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_spec1_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);

gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

l_spec1_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_spec1_create_list (class l_spec1_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR curr;
char strsql[512];
int  kolstr=0;
SQL_str row;

//printf("l_spec1_create_list %d\n",data->snanomer);



data->kl_shift=0; //0-отжата 1-нажата  

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_spec1_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_spec1_vibor),data);

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

sprintf(strsql,"select kodd,kz,kolih,koment,ei,ktoz,vrem from Specif where kodi=%d",data->kod_izdel.ravno_atoi());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
iceb_u_str naim;
int kolstr1;
SQL_str row1;
SQLCURSOR cur1;
float kolstr2=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr2);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  if(specprov(row[0],&naim,&data->naim_poi,1,data->window) != 0)
   continue;
  


//  if(iceb_u_SRAV(data->vvod.god.ravno(),row[0],0) == 0 && iceb_u_SRAV(data->vvod.shet.ravno(),row[1],0) == 0)
//    data->snanomer=data->kolzap;
  if(data->kod_v == atoi(row[0]))
   data->snanomer=data->kolzap;

  //Метка записи   
  ss[COL_METKA].new_plus("");
  if(row[1][0] == '0')
    ss[COL_METKA].new_plus("M");
  if(row[1][0] == '1')
    ss[COL_METKA].new_plus("U");
   
  /*Проверяем не является ли входящий материал изделием*/
  sprintf(strsql,"select kodi from Specif where kodi=%s limit 1",row[0]);
  kolstr1=sql_readkey(&bd,strsql,&row1,&cur1);

  if(kolstr1 != 0)
   ss[COL_METKA].new_plus("*");


  //Количество 
  sprintf(strsql,"%.10g",atof(row[2]));
  ss[COL_KOLIH].new_plus(strsql);


  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKA,ss[COL_METKA].ravno(),
  COL_KOD,row[0],
  COL_NAIM,naim.ravno(),
  COL_EI,row[4],
  COL_KOLIH,ss[COL_KOLIH].ravno(),
  COL_KOMENT,row[3],
  COL_DATA_VREM,iceb_u_vremzap(row[6]),
  COL_KTO,iceb_kszap(row[5],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

data->kod_v=0;

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_spec1_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

ss[0].new_plus(gettext("Работа со спецификацией на изделие"));
ss[0].ps_plus(gettext("Изделие"));
ss[0].plus(":");
ss[0].plus(data->kod_izdel.ravno());
ss[0].plus(" ");
ss[0].plus(data->naim_izdel.ravno());

sprintf(strsql,"%s:%d %s:%d",
gettext("Количество записей"),data->kolzap,
gettext("Уровень"),data->uroven);

ss[0].ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),ss[0].ravno());

gtk_widget_show(data->label_kolstr);

if(data->naim_poi.getdlinna() > 1)
 {
  sprintf(strsql,"%s:%s",gettext("Поиск"),data->naim_poi.ravno());
  iceb_label_set_text_color(data->label_poisk,strsql,"red");
  gtk_widget_show(data->label_poisk);
  
 }

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_spec1_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("l_spec1_add_columns\n");


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
"M", renderer,"text", COL_METKA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Код"), renderer,"text", COL_KOD,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Единица измерения"), renderer,"text", COL_EI,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Количество"), renderer,"text", COL_KOLIH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

printf("l_spec1_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void l_spec1_vibor(GtkTreeSelection *selection,class l_spec1_data *data)
{
printf("l_spec1_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_KOD,&kod,NUM_COLUMNS,&nomer,-1);

data->kod.new_plus(kod);

data->snanomer=nomer;

g_free(kod);

//printf("%s %s %d\n",data->kodmat.ravno(),data->nomkar.ravno(),data->snanomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_spec1_knopka(GtkWidget *widget,class l_spec1_data *data)
{
char strsql[512];
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_spec1_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {


  case FK2:
    if(l_spec_v(data->kod_izdel.ravno(),&kod,0,data->window) == 0)
     {
      data->kod_v=kod.ravno_atoi();
      l_spec1_create_list(data);
     }     
    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;    

    if(l_spec_v(data->kod_izdel.ravno(),&data->kod,0,data->window) == 0)
     {
      data->kod_v=kod.ravno_atoi();
      l_spec1_create_list(data);
     }     

    return;
    
  case FK3:
    if(data->kolzap == 0)
     return;    

    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
     return;

    sprintf(strsql,"delete from Specif where kodi=%d and kodd=%d",
    data->kod_izdel.ravno_atoi(),data->kod.ravno_atoi());
    
    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
    l_spec1_create_list(data);
    
    return;  

  case SFK3:
    if(data->kolzap == 0)
     return;    
    if(iceb_menu_danet(gettext("Удалить все записи ? Вы уверены ?"),2,data->window) == 2)
     return;

    sprintf(strsql,"delete from Specif where kodi=%d",data->kod_izdel.ravno_atoi());
    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);

    l_spec1_create_list(data);
     
    return;  

    
  
  case FK4:
    data->naim_poi.new_plus("");
    
    if(iceb_menu_vvod1(gettext("Введите наименование"),&naim,512,"",data->window) == 0)
      data->naim_poi.new_plus(naim.ravno());
    l_spec1_create_list(data);
    return;  
  
  case FK5:
    if(data->kolzap == 0)
     return;    
    spec1_rasp(data->kod_izdel.ravno_atoi(),data->window);

    return;  

  
  case FK7:
    if(l_spec_v(data->kod_izdel.ravno(),&kod,1,data->window) == 0)
     {
      data->kod_v=kod.ravno_atoi();
      l_spec1_create_list(data);
     }     
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_spec1_key_press(GtkWidget *widget,GdkEventKey *event,class l_spec1_data *data)
{
iceb_u_str repl;
printf("l_spec1_key_press keyval=%d state=%d\n",
event->keyval,event->state);

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

  case GDK_KEY_F7:
    g_signal_emit_by_name(data->knopka[FK7],"clicked");
    return(TRUE);


  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("l_spec1_key_press-Нажата клавиша Shift\n");

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
void l_spec1_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_spec1_data *data)
{
printf("l_spec1_v_row\n");
//g_signal_emit_by_name(data->knopka[SFK2],"clicked");
l_spec1(data->kod.ravno(),data->uroven,data->window);
l_spec1_create_list(data);

}
/*******************************/
/*Распечатка спецификации на изделие*/
/**************************************/

void spec1_rasp(int kod_izdel,GtkWidget *wpredok)
{
SQL_str         row,row1;
char		strsql[512];
int		kolnuz;
class iceb_u_str naim("");
char		imaf[64];
char		bros[512];
FILE            *ff;
int		kodizr;
int		suz,zsuz;
int		kolstr;
int		kodd;

zsuz=0;
kodizr=kod_izdel;

sprintf(imaf,"spec%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
 
/*Определяем входящие узлы*/

class iceb_razuz_data data;
data.kod_izd=kod_izdel;
kolnuz=iceb_razuz(&data,wpredok);

/*
printw("Разузловка завершена.\n");
refresh();
*/
SQLCURSOR cur;

SQLCURSOR cur1;

naz:;

naim.new_plus("");
for(int metka_zap=0; metka_zap < 2; metka_zap++)
 {
  if(metka_zap == 0)
    sprintf(strsql,"select naimat from Material where kodm=%d",kodizr);
  if(metka_zap == 1)
    sprintf(strsql,"select naius from Uslugi where kodus=%d",kodizr);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    if(metka_zap == 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код записи"),kodizr);
      iceb_menu_soob(strsql,wpredok);
     }
   }  
  else
   { 
    naim.new_plus(row1[0]);
    break;
   }
 }
 
if(kod_izdel == kodizr)
 {
  fprintf(ff,"%s: %d %s\n\
--------------------------------------------------------------------\n",
  gettext("Спецификация на изделие"),
  kodizr,naim.ravno());
 }
else
 {
  fprintf(ff,"\n%s: %d %s\n\
--------------------------------------------------------------------\n",
  gettext("Спецификация на узел"), 
  kodizr,naim.ravno());
 }

sprintf(strsql,"select kodd,kolih,ei,kz from Specif where kodi=%d",kodizr);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0 )
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи в изделии !"),wpredok);
  fclose(ff);
  unlink(imaf);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  kodd=atoi(row[0]);
  if(row[3][0] == '0')
   sprintf(strsql,"select naimat from Material where kodm=%s",row[0]);
  if(row[3][0] == '1')
   sprintf(strsql,"select naius from Uslugi where kodus=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    if(row[3][0] == '0')
      sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodd);
    if(row[3][0] == '1')
      sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),kodd);
    iceb_menu_soob(strsql,wpredok);
    naim.new_plus("");
   }  
  else
   {
    naim.new_plus(row1[0]);
   }
  memset(bros,'\0',sizeof(bros));
  if(data.kod_uz.find(kodd) != -1)
     strcpy(bros,gettext("Узел"));

  fprintf(ff,"%-4s %-*s %*s %*s %10.10g\n",
  row[0],
  iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(6,row[2]),row[2],
  iceb_u_kolbait(5,bros),bros,
  atof(row[1]));

 }

for(suz=zsuz ; suz < kolnuz ; suz++)
 {
  kodizr=data.kod_uz.ravno(suz);
  zsuz++;
  goto naz;
 }

iceb_podpis(ff,wpredok);

fclose(ff);

iceb_u_spisok imaff;
iceb_u_spisok naimf;

imaff.plus(imaf);
naimf.plus(gettext("Спецификация на изделие"));

iceb_ustpeh(imaff.ravno(0),3,wpredok);

iceb_rabfil(&imaff,&naimf,wpredok);

}
