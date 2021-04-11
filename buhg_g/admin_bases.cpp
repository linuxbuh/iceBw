/*$Id: admin_bases.c,v 1.22 2013/12/31 11:49:11 sasa Exp $*/
/*09.10.2020	05.02.2009	Белых А.И.	admin_bases.c
Просмотр списка баз
*/
#include "buhg_g.h"
#include "admin_bases.h"
enum
 {
  FK1,
  FK3,
  FK4,
  FK5,
  FK6,
  FK7,
  FK8,
  FK9,
  FK10,
  KOL_F_KL  
 };

enum
 {
  COL_BAZA,
  COL_KODIROVKA,
  COL_NAIM_ORG,
  NUM_COLUMNS
 };

class admin_bases_data
 {
  public:
  
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *sw;
  GtkWidget *label;
  GtkWidget *label_poisk;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *label_link;
  class iceb_u_str name_window;
    
  int        kl_shift;
  class iceb_u_str imabaz;
  class admin_bases_poi rek_poi;
      
  int snanomer;  
  int kolzap;
  int voz;
  int metka_r;
  admin_bases_data() //Конструктор
   {
    kl_shift=0;
    treeview=NULL;
    snanomer=kolzap=0;
    metka_r=0;
    voz=0;
   }
   
 };

gboolean   admin_bases_key_press(GtkWidget *widget,GdkEventKey *event,class admin_bases_data *data);
static void admin_bases_add_columns (GtkTreeView *treeview);
void admin_bases_add_f10(GtkTreeView *treeview);
void admin_bases_create_list(class admin_bases_data *data);
void  admin_bases_knopka(GtkWidget *widget,class admin_bases_data *data);
void admin_bases_vibor(GtkTreeSelection *selection,class admin_bases_data *data);
int   admin_alx(const char *imabazv,GtkWidget *wpredok);
void admin_bases_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class admin_bases_data *data);

int admin_del_alx(const char *put_na_kat,GtkWidget *wpredok);
int admin_icebuser(const char *imabazv,GtkWidget *wpredok);
int admin_dumpbazin(const char *login,const char *parol,const char *hostname,GtkWidget *wpredok);
void admin_bases_dumpin(const char *namebases,GtkWidget *wpredok);
int admin_bases_p(class admin_bases_poi *rek_poi,GtkWidget *wpredok);
void admin_bases_check_repair(const char *namebase,GtkWidget *wpredok);
int admin_bases_provb(const char *namebase,GtkWidget *wpredok);

extern SQL_baza bd;
extern class iceb_u_str kodirovka_iceb;
extern char *imabaz;
extern class iceb_u_str admin_parol;
extern class iceb_u_str admin_login;
extern class iceb_u_str admin_host;

int admin_bases(int metka_r, /*0-просмотр 1-просмотр с выбором*/
class iceb_u_str *vimabaz,
GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class admin_bases_data data;
data.metka_r=metka_r;
data.name_window.plus(__FUNCTION__);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
  gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),600,-1);

char bros[2048];
sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список баз данных"));

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(admin_bases_key_press),&data);
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
gtk_widget_show(vbox1);
gtk_widget_show(vbox2);
gtk_widget_show(hbox);

GtkWidget *label=gtk_label_new(gettext("Список баз данных"));
gtk_box_pack_start((GtkBox*)vbox2,label,FALSE,FALSE,0);
gtk_widget_show(label);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

sprintf(bros,"<a href=\"%s/i_admin4.html\" title=\"%s\">F1 %s</a>",ICEB_PUT_NA_HTML,gettext("Просмотр документации"),gettext("Помощь"));
data.label_link=gtk_label_new(bros);
gtk_label_set_use_markup (GTK_LABEL (data.label_link), TRUE);
g_signal_connect(data.label_link, "activate-current-link", G_CALLBACK (iceb_activate_link), NULL);

data.knopka[FK1]=gtk_button_new();
gtk_container_add (GTK_CONTAINER (data.knopka[FK1]), data.label_link);
g_signal_connect(data.knopka[FK1],"clicked",G_CALLBACK(admin_bases_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK1], TRUE, TRUE, 0);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(admin_bases_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(admin_bases_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск баз данных"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));

sprintf(bros,"F5 %s",gettext("Настройка"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(admin_bases_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Просмотр списка файлов настройки"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));

sprintf(bros,"F6 %s",gettext("Операторы"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(admin_bases_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Просмотр списка операторов"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));

sprintf(bros,"F7 %s",gettext("Выгрузка"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(admin_bases_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Выгрузка дампа базы на которой установлена подсветка строки в файл"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));

sprintf(bros,"F8 %s",gettext("Загрузка"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(admin_bases_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Загрузка базы из файла с дампом базы"));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));

gtk_widget_show_all(data.window);

sprintf(bros,"F9 %s",gettext("Проверка"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(admin_bases_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Проверка и ремонт таблиц базы данных"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show(data.knopka[FK9]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK10], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(admin_bases_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы"));
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);

//gtk_widget_grab_focus(data.knopka[FK10]);

admin_bases_create_list(&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
if(data.voz == 0)
 vimabaz->new_plus(data.imabaz.ravno());
 
return(data.voz);


}

/*****************/
/*Создание списка*/
/*****************/
void admin_bases_create_list(class admin_bases_data *data)
{
char strsql[512];
GtkListStore *model=NULL;
GtkTreeIter iter;
//printf("admin_bases_create_list\n");


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);


g_signal_connect(data->treeview,"row_activated",G_CALLBACK(admin_bases_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(admin_bases_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


sprintf(strsql,"SET NAMES %s",kodirovka_iceb.ravno());
iceb_sql_zapis(strsql,1,1,data->window);


model = gtk_list_store_new (NUM_COLUMNS+1,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_INT);

sprintf(strsql,"SHOW DATABASES");
class SQLCURSOR cur;
SQL_str row;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

//Определяем количество файлов
//int kolfil=data->imaf->kolih();
class iceb_u_str naim_org("");
class iceb_u_str naim_org_v;
class SQLCURSOR cur1;
SQL_str row1;
class iceb_u_str kodirovka;
int voz=0;
data->kolzap=0;

while(cur.read_cursor(&row) != 0)
 {
//  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(iceb_u_SRAV("mysql",row[0],0) == 0)
   continue;

  if(data->rek_poi.metka_poi == 1)
   if(iceb_u_proverka(data->rek_poi.imabaz.ravno(),row[0],4,0) != 0)
    continue;   

  /*Устанавливаем базу данных для того чтобы прочесть какая у неё кодировка по умолчанию*/
  sprintf(strsql,"USE %s",row[0]);
  sql_zap(&bd,strsql);

  /*читаем кодировку базы данных*/  
  kodirovka.new_plus("");
  if(sql_readkey(&bd,"SELECT @@character_set_database",&row1,&cur1) > 0)
   kodirovka.new_plus(row1[0]);
  /*Узнаём наименование организации если это iceB база*/

  naim_org.new_plus("");

  sprintf(strsql,"select naikon from %s.Kontragent where kodkon='00'",row[0]);
  if((voz=sql_readkey(&bd,strsql,&row1,&cur1)) > 0)
   naim_org.new_plus(row1[0]);

  if(data->rek_poi.metka_poi == 1)
   if(iceb_u_proverka(data->rek_poi.naim00.ravno(),naim_org.ravno(),4,0) != 0)
    continue;

  if(voz < 0)
   {
    naim_org_v.new_plus("Это не iceB база данных");
   }
  
  if(voz == 0)
   naim_org_v.new_plus("Не введён контрагент с кодом 00");

  if(voz > 0)
   {   
    naim_org_v.new_plus("");
    if(g_utf8_validate(naim_org.ravno(),-1,NULL) != TRUE)
     naim_org_v.new_plus(naim_org.ravno());
    else    
     naim_org_v.new_plus(naim_org.ravno());
   }
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_BAZA,row[0],
  COL_KODIROVKA,kodirovka.ravno(),
  COL_NAIM_ORG,naim_org_v.ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);
  data->kolzap++;
 }
/*возвращаем базу данных по умолчанию*/
iceb_sql_zapis("USE mysql",1,1,data->window);

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

admin_bases_add_columns (GTK_TREE_VIEW (data->treeview));


if(kolstr == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),TRUE);//Доступна
 }

if(data->rek_poi.metka_poi == 1)
 {
  class iceb_u_str zagolov("");  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->rek_poi.imabaz.ravno(),gettext("Имя базы"));
  iceb_str_poisk(&zagolov,data->rek_poi.naim00.ravno(),gettext("Наименование организации"));
  

  iceb_label_set_text_color(data->label_poisk,zagolov.ravno(),"red");
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

}

/**********************************/
/*Новая программа создания колонок*/
/**********************************/

static void admin_bases_add_columns (GtkTreeView *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (gettext("База"),
						     renderer,
						     "text",
						     COL_BAZA,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_BAZA);
  gtk_tree_view_append_column (treeview, column);


  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (gettext("Кодировка"),
						     renderer,
						     "text",
						     COL_KODIROVKA,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_KODIROVKA);
  gtk_tree_view_append_column (treeview, column);

  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (gettext("Организация"),
						     renderer,
						     "text",
						     COL_NAIM_ORG,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_NAIM_ORG);
  gtk_tree_view_append_column (treeview, column);

}



/****************************/
/*Выбор строки*/
/**********************/

void admin_bases_vibor(GtkTreeSelection *selection,class admin_bases_data *data)
{
//printf("admin_basesn_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *imabaz;
gint nomer;

gtk_tree_model_get(model,&iter,COL_BAZA,&imabaz,NUM_COLUMNS,&nomer,-1);

data->imabaz.new_plus(imabaz);
data->snanomer=nomer;
g_free(imabaz);

}
/****************************/
/*Выбор строки*/
/**********************/
void admin_bases_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class admin_bases_data *data)
{
if(data->metka_r == 1)
 {
  iceb_sizww(data->name_window.ravno(),data->window);
  data->voz=0;
  gtk_widget_destroy(data->window);
 }

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_bases_knopka(GtkWidget *widget,class admin_bases_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("admin_basesn_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается
char strsql[512];
class iceb_u_str put_na_alx("");

switch (knop)
 {
  case FK1:
    g_signal_emit_by_name(data->label_link,"activate-current-link");
    return;

  case FK3: /*Удалить запись в таблице*/
    if(iceb_u_SRAV("mysql",data->imabaz.ravno(),0) == 0  || iceb_u_SRAV("information_schema",data->imabaz.ravno(),0) == 0) 
     {
      iceb_menu_soob(gettext("Эту базу удалять нельзя!"),data->window);
      return;
     }
    if(iceb_menu_danet(gettext("Удалить базу? Вы уверены?"),2,data->window) == 2)
     return;
    if(iceb_parol(1,data->window) != 0)
     return;
    sprintf(strsql,"drop database %s",data->imabaz.ravno());
    iceb_sql_zapis(strsql,1,1,data->window);    
    admin_bases_create_list(data);

    return;  

  case FK4: /*ПОИСК*/
   admin_bases_p(&data->rek_poi,data->window);
   admin_bases_create_list(data);
   return;
   
  case FK5: /*Работа с файлом настройки*/
   
    if(admin_bases_provb(data->imabaz.ravno(),data->window) != 0) /*проверяем это iceB база или нет*/
     return;
   
    if(imabaz != NULL)
     delete [] imabaz;   
    imabaz=new char[data->imabaz.getdlinna()];
    strcpy(imabaz,data->imabaz.ravno());

    admin_alx(imabaz,data->window);

    iceb_sql_zapis("USE mysql",1,1,data->window);

    return;  

  case FK6: /*Работа со списком операторов*/

    if(admin_bases_provb(data->imabaz.ravno(),data->window) != 0) /*проверяем это iceB база или нет*/
     return;

    if(imabaz != NULL)
     delete [] imabaz;   

    imabaz=new char[data->imabaz.getdlinna()];
    strcpy(imabaz,data->imabaz.ravno());

    admin_icebuser(data->imabaz.ravno(),data->window);

    return;

  case FK7: /*Выгрузка дама базы*/
    admin_bases_dumpin(data->imabaz.ravno(),data->window);
    return;

  case FK8: /*загрузка базы из файла с дампом базы*/
    admin_dumpbazin(admin_login.ravno(),admin_parol.ravno(),admin_host.ravno(),data->window);
    admin_bases_create_list(data);
    return;

  case FK9: /*проверка и ремонт таблиц базы*/

    sprintf(strsql,"USE %s",data->imabaz.ravno());
    iceb_sql_zapis(strsql,1,0,data->window);

    admin_bases_check_repair(data->imabaz.ravno(),data->window);

    iceb_sql_zapis("USE mysql",1,1,data->window);

    break;
               
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_bases_key_press(GtkWidget *widget,GdkEventKey *event,class admin_bases_data *data)
{
//printf("admin_basesn_key_press keyval=%d state=%d\n",
//event->keyval,event->state);

switch(event->keyval)
 {
  case GDK_KEY_F1:
    g_signal_emit_by_name(data->label_link,"activate-current-link");
    return(FALSE);

   
  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
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

  case GDK_KEY_F8:
    g_signal_emit_by_name(data->knopka[FK8],"clicked");
    return(TRUE);

  case GDK_KEY_F9:
    g_signal_emit_by_name(data->knopka[FK9],"clicked");
    return(TRUE);


  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("kontr_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/*******************/
/*выбрать вид выгрузки */
/*********************/

void admin_bases_dumpin(const char *namebases,GtkWidget *wpredok)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выгрузка"));
char strsql[512];

zagolovok.plus(gettext("Выгрузка дампа баз"));

sprintf(strsql,"%s %s",gettext("Выгрузка дампа базы"),namebases);
punkt_m.plus(strsql);//0
punkt_m.plus(gettext("Выгрузка всех баз данных"));//1


int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,wpredok);
switch(nomer)
 {
  case -1:
   return;

  case 0:
    iceb_sql_zapis("FLUSH TABLES",1,0,wpredok);
    iceb_dumpbazout(namebases,admin_login.ravno(),admin_parol.ravno(),admin_host.ravno(),wpredok);
    break;

  case 1:
    iceb_sql_zapis("FLUSH TABLES",1,0,wpredok);
    iceb_dumpbazout("",admin_login.ravno(),admin_parol.ravno(),admin_host.ravno(),wpredok);
    break;
 };
 
}
/*******************/
/*проверка физицеской целосности*/
/*********************/

void admin_bases_check_repair(const char *namebase,GtkWidget *wpredok)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;


titl.plus(gettext("Проверка"));

zagolovok.plus(gettext("Проверка и ремонт таблиц"));
zagolovok.plus("\n",gettext("База"));
zagolovok.plus(":",namebase);

punkt_m.plus(gettext("Проверка физической целосности таблиц базы данных"));//0
punkt_m.plus(gettext("Восстановление таблиц базы данных"));//1

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,NULL);

switch(nomer)
 {
  case 0:
    check_tablew(namebase,wpredok);
    break;

  case 1:
    repair_tablew(namebase,wpredok);
    break;
   
  default:
   break;
 }
}
/*************************************/
/*проверка базы*/
/************************************/
int admin_bases_provb(const char *namebase,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select naikon from %s.Kontragent where kodkon='00'",namebase);
if(sql_readkey(&bd,strsql) < 0)
 {
  iceb_menu_soob(gettext("Это не iceB база данных!"),wpredok);
  return(1);  
 }
return(0);
}
