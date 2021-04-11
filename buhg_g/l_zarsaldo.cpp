/*$Id: l_zarsaldo.c,v 1.22 2013/09/26 09:46:51 sasa Exp $*/
/*08.05.2015	21.06.2006	Белых А.И.	l_zarsaldo.c
Работа со списком 
*/
#include        <errno.h>
#include  "buhg_g.h"
#include "l_zarsaldo.h"
 
enum
{
  COL_GOD,
  COL_TAB_NOM,
  COL_SALDO,
  COL_SALDO_B,
  COL_FIO,
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

class  l_zarsaldo_data
 {
  public:

  short god_v;
  int tabn_v;
  short god_tv; //только что введённый год
  int tabn_tv; //только что введённый табельный номер
  
  short god; //Год просмотра сальдо
  
  class iceb_u_str zapros_bd; //Запрос к базе данных
  class l_zarsaldo_rek poi;
  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  
  short     metka_rr;   //0-работа со списком 1-выбор
  //Конструктор
  l_zarsaldo_data()
   {
    snanomer=0;
    metka_rr=metka_voz=kl_shift=0;
    window=treeview=NULL;
    god_tv=tabn_tv=0;
   }      
 };

gboolean   l_zarsaldo_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsaldo_data *data);
void l_zarsaldo_vibor(GtkTreeSelection *selection,class l_zarsaldo_data *data);
void l_zarsaldo_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zarsaldo_data *data);
void  l_zarsaldo_knopka(GtkWidget *widget,class l_zarsaldo_data *data);
void l_zarsaldo_add_columns (GtkTreeView *treeview);
void l_zarsaldo_udzap(class l_zarsaldo_data *data);
void l_zarsaldo_rasp(class l_zarsaldo_data *data);
void l_zarsaldo_create_list (class l_zarsaldo_data *data);

int l_zarsaldo_v(short *godk,int *tabnk,GtkWidget *wpredok);
int l_zarsaldo_p(class l_zarsaldo_rek *rek_poi,GtkWidget *wpredok);


extern SQL_baza  bd;
extern short		startgodz; /*Стаpтовый год для расчёта сальдо*/

void   l_zarsaldo(GtkWidget *wpredok)
{
l_zarsaldo_data data;
char bros[512];
data.god=startgodz;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Сальдо по табельным номерам"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zarsaldo_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Сальдо по табельным номерам"));


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
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_zarsaldo_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_zarsaldo_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_zarsaldo_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);


sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_zarsaldo_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Ввод реквизитов для поиска нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_zarsaldo_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_zarsaldo_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_zarsaldo_create_list(&data);

gtk_widget_show(data.window);
//if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}
/*******************************/
/*Проверка реквизитов поиска*/
/*****************************/
int l_zarsaldo_prov(SQL_str row,class l_zarsaldo_rek *poi,GtkWidget *wpredok)
{
if(poi->metka_poi == 0)
 return(0);
 
if(iceb_u_proverka(poi->tabnom.ravno(),row[1],0,0) != 0)
 return(1);

if(poi->fio.getdlinna() > 1)
 {
  char strsql[512];
  SQL_str row1;
  class SQLCURSOR cur;
  sprintf(strsql,"select fio from Kartb where tabn=%d",atoi(row[1]));
  if(iceb_sql_readkey(strsql,&row1,&cur,wpredok) == 1)
   {
    if(iceb_u_proverka(poi->fio.ravno(),row1[0],4,0) != 0)
     return(1);
   }
 }
 
return(0);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_zarsaldo_create_list (class l_zarsaldo_data *data)
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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_zarsaldo_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_zarsaldo_vibor),data);

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

if(data->poi.god.getdlinna() > 1)
 data->god=data->poi.god.ravno_atoi();
else
 data->god=startgodz;
  
if(data->god == 0)
 {
  short dt,mt,gt;
  iceb_u_poltekdat(&dt,&mt,&gt);
  data->god=gt;
 }

sprintf(strsql,"select * from Zsal where god=%d order by god,tabn asc",data->god);
data->zapros_bd.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str ss;

data->kolzap=0;
SQL_str row1;
SQLCURSOR cur1;
double isaldo=0.;
double isaldob=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  

  if(l_zarsaldo_prov(row,&data->poi,data->window) != 0)
   continue;

  if(data->god_tv == atoi(row[0]) && data->tabn_tv == atoi(row[1]))
    data->snanomer=data->kolzap;


  //Сальдо общее
  isaldo+=atof(row[2]);
  
  //Сальдо бюджетное
  isaldob+=atof(row[5]);

  //Узнаём фамилию
  ss.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    ss.new_plus(row1[0]);
     
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_GOD,row[0],
  COL_TAB_NOM,row[1],
  COL_SALDO,row[2],
  COL_SALDO_B,row[5],
  COL_FIO,ss.ravno(),
  COL_DATA_VREM,iceb_u_vremzap(row[4]),
  COL_KTO,iceb_kszap(row[3],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->god_tv=0;
data->tabn_tv=0;

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_zarsaldo_add_columns (GTK_TREE_VIEW (data->treeview));


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
zagolov.plus(gettext("Сальдо по табельным номерам"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql,"\n%s:%.2f %s:%.2f\n%s:%d",gettext("Общее сальдо"),isaldo,
gettext("Cальдо бюджетное"),isaldob,
gettext("Год просмотра"),data->god);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->poi.metka_poi == 1)
 {
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poi.god.ravno(),gettext("Год"));
  iceb_str_poisk(&zagolov,data->poi.tabnom.ravno(),gettext("Табельный номер"));
  iceb_str_poisk(&zagolov,data->poi.fio.ravno(),gettext("Фамилия"));
  

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

void l_zarsaldo_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Год"), renderer,"text", COL_GOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Табельный номер"), renderer,"text", COL_TAB_NOM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сальдо"), renderer,"text", COL_SALDO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сальдо бюджетное"), renderer,"text", COL_SALDO_B,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Фамилия"), renderer,"text", COL_FIO,NULL);

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

void l_zarsaldo_vibor(GtkTreeSelection *selection,class l_zarsaldo_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *god;
gchar *tabn;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_GOD,&god,COL_TAB_NOM,&tabn,NUM_COLUMNS,&nomer,-1);

data->god_v=atoi(god);
data->tabn_v=atoi(tabn);
data->snanomer=nomer;

g_free(god);
g_free(tabn);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_zarsaldo_knopka(GtkWidget *widget,class l_zarsaldo_data *data)
{
int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    l_zarsaldo_v(&data->god_tv,&data->tabn_tv,data->window);

    l_zarsaldo_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    l_zarsaldo_v(&data->god_v,&data->tabn_v,data->window);

    l_zarsaldo_create_list(data);
    return;  

  case FK3:

    if(data->kolzap == 0)
      return;

    if(iceb_pbpds(1,data->god,data->window) != 0)
     return;

    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;

    l_zarsaldo_udzap(data);
    l_zarsaldo_create_list(data);
    return;  
  

  case FK4:
    l_zarsaldo_p(&data->poi,data->window);
    l_zarsaldo_create_list(data);
    return;  

  case FK5:
    l_zarsaldo_rasp(data);
    return;  

    
  case FK10:
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarsaldo_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsaldo_data *data)
{
iceb_u_str repl;

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
void l_zarsaldo_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zarsaldo_data *data)
{
//printf("l_zarsaldo_v_row\n");
//printf("l_zarsaldo_v_row корректировка\n");
if(data->metka_rr == 0)
 {
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");
  return;
 }

data->metka_voz=0;

gtk_widget_destroy(data->window);


}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_zarsaldo_udzap(class l_zarsaldo_data *data)
{
if(iceb_pbpds(1,data->god,data->window) != 0)
 return;
 
if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
   return;

char strsql[512];

sprintf(strsql,"delete from Zsal where god=%d and tabn=%d",data->god,data->tabn_v);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_zarsaldo_rasp(class l_zarsaldo_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;


if((kolstr=cur.make_cursor(&bd,data->zapros_bd.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"kateg%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Сальдо по табельным номерам"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Сальдо по табельным номерам"),0,0,0,0,0,0,ff,data->window);
fprintf(ff,"%s:%d\n",gettext("Год просмотра"),data->god);

if(data->poi.metka_poi == 1)
 {
  if(data->poi.god.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Год"),data->poi.god.ravno());
  if(data->poi.tabnom.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Табельный номер"),data->poi.tabnom.ravno());
  if(data->poi.fio.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Фамилия"),data->poi.fio.ravno());
 }

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Год |Таб.номер |Общ.сальдо|Б.сальдо  |Фамилия , Имя, Отчестьво      |Дата и время запис.| Кто записал\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------\n");

class iceb_u_str fio("");
SQL_str row1;
SQLCURSOR cur1;
double it1=0.;
double it2=0.;
double it_p_o=0.; //положительный итог
double it_o_o=0.; //Отрицательный итог
double it_p_b=0.; //положительный итог
double it_o_b=0.; //Отрицательный итог
double suma_o;
double suma_b;
cur.poz_cursor(0);

while(cur.read_cursor(&row) != 0)
 {
  if(l_zarsaldo_prov(row,&data->poi,data->window) != 0)
   continue;

  sprintf(strsql,"select fio from Kartb where tabn=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    fio.new_plus(row1[0]);
  else
    fio.new_plus("");
        
  fprintf(ff,"%-4s %-10s %10.2f %10.2f %-*.*s %s %s\n",
  row[0],row[1],atof(row[2]),atof(row[5]),
  iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno(),
  iceb_u_vremzap(row[4]),iceb_kszap(row[3],data->window));
  
  suma_o=atof(row[2]);
  suma_b=atof(row[5]);
  it1+=suma_o;
  it2+=suma_b;

  if(suma_o > 0.)
    it_p_o+=suma_o;
  else
    it_o_o+=suma_o;

  if(suma_b > 0.)
    it_p_b+=suma_b;
  else
    it_o_b+=suma_b;

 }
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f\n",iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),it1,it2);

fprintf(ff,"\n%s\n\
%s:%.2f\n\
%s:%.2f\n",
gettext("Общее сальдо"),
gettext("Положительное"),it_p_o,
gettext("Отрицательное"),it_o_o);

fprintf(ff,"\n%s\n\
%s:%.2f\n\
%s:%.2f\n\n",
gettext("Бюджет"),
gettext("Положительное"),it_p_b,
gettext("Отрицательное"),it_o_b);


fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
