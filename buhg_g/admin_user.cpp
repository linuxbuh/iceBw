/*$Id: admin_user.c,v 1.18 2013/12/31 11:49:12 sasa Exp $*/
/*27.02.2017	05.02.2009	Белых А.И.	admin_user.c
Просмотр списка привелегий пользователей
*/

#include "buhg_g.h"
#include "admin_user.h"
enum
 {
  FK1,
  FK3,
  FK4,
  FK6,
  FK10,
  KOL_F_KL  
 };

enum
 {
  COL_HOST,
  COL_BAZA,
  COL_LOGIN,
  COL_KODIROVKA,
  COL_NAIM_ORG,
  COL_SELECT,
  COL_INSERT,
  COL_UPDATE,
  COL_DELETE,
  COL_CREATE,
  COL_DROP,
  COL_GRANT,
  NUM_COLUMNS
 };

class admin_user_data
 {
  public:
  
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *sw;
  GtkWidget *label;
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *label_link;
  class iceb_u_str name_window;
  class admin_user_poi rp;
      
  int        kl_shift;
  class iceb_u_str host;
  class iceb_u_str imabaz;
  class iceb_u_str login;
  int snanomer;  
  int kolzap;
  short metka_sort; //0-по логинам 1-по базам
  
  admin_user_data() //Конструктор
   {
    metka_sort=0;
    kl_shift=0;
    treeview=NULL;
    kolzap=snanomer=0;
   }
   
 };

gboolean   admin_user_key_press(GtkWidget *widget,GdkEventKey *event,class admin_user_data *data);
static void admin_user_add_columns (GtkTreeView *treeview);
void admin_user_add_f10(GtkTreeView *treeview);
void admin_user_create_list(class admin_user_data *data);
void  admin_user_knopka(GtkWidget *widget,class admin_user_data *data);
void admin_user_vibor(GtkTreeSelection *selection,class admin_user_data *data);
int admin_user_p(class admin_user_poi *rek_poi,GtkWidget *wpredok);
int admin_user_prov(SQL_str row,class admin_user_poi *rp); 
int admin_user_p(class admin_user_poi *rek_poi,GtkWidget *wpredok);

extern SQL_baza bd;
extern class iceb_u_str kodirovka_iceb;

void admin_user(GtkWidget *wpredok)
{
int gor=0;
int ver=0;
char strsql[512];
class admin_user_data data;

/*устанавливаем базу данных по умолчанию*/
iceb_sql_zapis("USE mysql",1,1,wpredok);
sprintf(strsql,"SET NAMES %s",kodirovka_iceb.ravno());
iceb_sql_zapis(strsql,1,1,wpredok);

data.name_window.plus(__FUNCTION__);
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

char bros[512];
sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список разрешённых доступов к базам данных"));

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(admin_user_key_press),&data);
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

GtkWidget *label=gtk_label_new(gettext("Список разрешённых доступов к базам данных"));
gtk_box_pack_start((GtkBox*)vbox2,label,FALSE,FALSE,0);
gtk_widget_show(label);

data.label_kolstr=gtk_label_new ("");
gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

sprintf(strsql,"<a href=\"%s/i_admin3.html\" title=\"%s\">F1 %s</a>",ICEB_PUT_NA_HTML,gettext("Просмотр документации"),gettext("Помощь"));
data.label_link=gtk_label_new(strsql);
gtk_label_set_use_markup (GTK_LABEL (data.label_link), TRUE);
g_signal_connect(data.label_link, "activate-current-link", G_CALLBACK (iceb_activate_link), NULL);

data.knopka[FK1]=gtk_button_new();
gtk_container_add (GTK_CONTAINER (data.knopka[FK1]), data.label_link);
g_signal_connect(data.knopka[FK1],"clicked",G_CALLBACK(admin_user_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK1], TRUE, TRUE, 0);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(admin_user_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(admin_user_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));

sprintf(bros,"F6 %s",gettext("Сортировка"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(admin_user_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Переключение сортировки логины/базы"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK10], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(admin_user_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы"));
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_widget_show_all(data.window);

admin_user_create_list(&data);

gtk_widget_show(data.window);

gtk_widget_hide (data.label_poisk);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));



}
/****************************/
/*Выбор строки  по двойному клику мышкой*/
/**********************/
void admin_user_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class kontr_data *data)
{
//printf("rabfiln_v_row \n");

}

/*****************/
/*Создание списка*/
/*****************/
void admin_user_create_list(class admin_user_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
//printf("admin_user_create_list\n");


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(admin_user_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(admin_user_vibor),data);

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

char strsql[512];
if(data->metka_sort == 0)
  sprintf(strsql,"select * from db order by User asc");
else
  sprintf(strsql,"select * from db order by Db asc");

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
data->kolzap=0;

while(cur.read_cursor(&row) != 0)
 {
//  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(admin_user_prov(row,&data->rp) != 0)
   continue;

  /*Устанавливаем базу данных для того чтобы прочесть какая у неё кодировка по умолчанию*/
  sprintf(strsql,"USE %s",row[1]);
  sql_zap(&bd,strsql);

  /*читаем кодировку базы данных*/  
  kodirovka.new_plus("");
  if(sql_readkey(&bd,"SELECT @@character_set_database",&row1,&cur1) > 0)
   kodirovka.new_plus(row1[0]);
  /*Узнаём наименование организации если это iceB база*/

  naim_org.new_plus("");

  sprintf(strsql,"select naikon from %s.Kontragent where kodkon='00'",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
   naim_org.new_plus(row1[0]);

  naim_org_v.new_plus("");
  if(iceb_u_SRAV(kodirovka.ravno(),"koi8",1) == 0)
   naim_org_v.new_plus(naim_org.ravno());
  else    
   naim_org_v.new_plus(naim_org.ravno());

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_HOST,row[0],
  COL_BAZA,row[1],
  COL_LOGIN,row[2],
  COL_KODIROVKA,kodirovka.ravno(),
  COL_NAIM_ORG,naim_org_v.ravno(),
  COL_SELECT,row[3],
  COL_INSERT,row[4],
  COL_UPDATE,row[5],
  COL_DELETE,row[6],
  COL_CREATE,row[7],
  COL_DROP,row[8],
  COL_GRANT,row[9],
  NUM_COLUMNS,data->kolzap,
  -1);
  data->kolzap++;
 }
/*возвращаем базу данных по умолчанию*/
iceb_sql_zapis("USE mysql",1,1,data->window);

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

admin_user_add_columns (GTK_TREE_VIEW (data->treeview));


if(kolstr == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
 }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str zagolov;

sprintf(strsql,"%s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());
if(data->rp.metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->rp.login.ravno(),gettext("Логин"));
  iceb_str_poisk(&zagolov,data->rp.imabaz.ravno(),gettext("Имя базы данных"));
  iceb_str_poisk(&zagolov,data->rp.host.ravno(),gettext("Хост на базу"));
  

  iceb_label_set_text_color(data->label_poisk,zagolov.ravno(),"red");
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

}

/**********************************/
/*Новая программа создания колонок*/
/**********************************/

static void admin_user_add_columns (GtkTreeView *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
//  GtkTreeModel *model = gtk_tree_view_get_model (treeview);

  /***************/
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (gettext("Хост"),
						     renderer,
						     "text",
						     COL_HOST,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_HOST);
  gtk_tree_view_append_column (treeview, column);

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
  column = gtk_tree_view_column_new_with_attributes (gettext("Логин"),
						     renderer,
						     "text",
						     COL_LOGIN,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_LOGIN);
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

  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Select",
						     renderer,
						     "text",
						     COL_SELECT,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_SELECT);
  gtk_tree_view_append_column (treeview, column);

  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Insert",
						     renderer,
						     "text",
						     COL_INSERT,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_INSERT);
  gtk_tree_view_append_column (treeview, column);

  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Update",
						     renderer,
						     "text",
						     COL_UPDATE,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_UPDATE);
  gtk_tree_view_append_column (treeview, column);

  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Delete",
						     renderer,
						     "text",
						     COL_DELETE,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_DELETE);
  gtk_tree_view_append_column (treeview, column);

  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Create",
						     renderer,
						     "text",
						     COL_CREATE,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_CREATE);
  gtk_tree_view_append_column (treeview, column);

  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Drop",
						     renderer,
						     "text",
						     COL_DROP,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_DROP);
  gtk_tree_view_append_column (treeview, column);

  /* ********** */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Grant",
						     renderer,
						     "text",
						     COL_GRANT,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_GRANT);
  gtk_tree_view_append_column (treeview, column);

}



/****************************/
/*Выбор строки*/
/**********************/

void admin_user_vibor(GtkTreeSelection *selection,class admin_user_data *data)
{
//printf("admin_usern_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *host;
gchar *imabaz;
gchar *login;
int nomer=0;

gtk_tree_model_get(model,&iter,COL_HOST,&host,COL_BAZA,&imabaz,COL_LOGIN,&login,NUM_COLUMNS,&nomer,-1);

data->host.new_plus(host);
data->imabaz.new_plus(imabaz);
data->login.new_plus(login);
data->snanomer=nomer;

g_free(host);
g_free(login);
g_free(imabaz);

printf("%s %s %s %d\n",data->host.ravno(),data->imabaz.ravno(),data->login.ravno(),data->snanomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_user_knopka(GtkWidget *widget,class admin_user_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается
char strsql[512];

switch (knop)
 {

  case FK1:
    g_signal_emit_by_name(data->label_link,"activate-current-link");
    return;

  case FK3: /*Удалить запись в таблице*/
    if(iceb_menu_danet("Удалить запись? Вы уверены?",2,data->window) == 2)
     return;

    sprintf(strsql,"delete from db where Host='%s' and Db='%s' and User='%s'",
    data->host.ravno(),data->imabaz.ravno(),data->login.ravno());

    iceb_sql_zapis(strsql,1,1,data->window);    

    iceb_sql_zapis("FLUSH PRIVILEGES",1,1,data->window);    

    /*если нет больше записей с этим логином то проверям возможность удаления в icebuser*/
    sprintf(strsql,"select User from db where Db='%s' and User='%s'",data->imabaz.ravno(),data->login.ravno());
    if(sql_readkey(&bd,strsql) > 0) /*Запись есть*/
     {
      admin_user_create_list(data);
      return;
     }
    if(prov_uduserw(data->login.ravno(),data->imabaz.ravno(),0,data->window) != 0)
     {
      admin_user_create_list(data);
      return;
     }
    sprintf(strsql,"delete from %s.icebuser where login='%s'",data->imabaz.ravno(),data->login.ravno());
    iceb_sql_zapis(strsql,1,0,data->window);

    admin_user_create_list(data);
    return;  

  case FK4: /*Поиск записей*/
    admin_user_p(&data->rp,data->window);

    admin_user_create_list(data);
    return;  

  case FK6: /*Сортировка*/
    data->metka_sort++;
    if(data->metka_sort > 1)
     data->metka_sort=0;
    admin_user_create_list(data);
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

gboolean   admin_user_key_press(GtkWidget *widget,GdkEventKey *event,class admin_user_data *data)
{

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
/******************************/
/*проверка реквизитов поиска*/
/******************************/
int admin_user_prov(SQL_str row,class admin_user_poi *rp)
{

if(rp->metka_poi == 0)
 return(0);
if(iceb_u_proverka(rp->login.ravno(),row[2],0,0) != 0)
 return(1);
if(iceb_u_proverka(rp->imabaz.ravno(),row[1],0,0) != 0)
 return(1);
if(iceb_u_proverka(rp->host.ravno(),row[0],0,0) != 0)
 return(1);
 
return(0);
 

}
