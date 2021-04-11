/*$Id: l_sptbn1.c,v 1.17 2013/12/31 11:49:14 sasa Exp $*/
/*08.05.2015	16.06.2008	Белых А.И.	l_sptbn1.c
Просмотр списка табельных номеров без возможности заходить в катрточки
*/
#include <errno.h>
#include "buhg_g.h"
#include "l_sptbn.h"

enum
{
 FK4,
 FK5,
 FK6,
 FK7,
 SFK7,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_METKA,
 COL_TABNOM,
 COL_FIO,
 NUM_COLUMNS
};

class  sptbn1_data
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
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  

  class l_sptbn_p  poi;
  iceb_u_str zapros;

  iceb_u_str tabnom_tv;//Код толькочто введённой записи
  iceb_u_str tabnom_v; //Выбранный код 
  iceb_u_str fio_v; //Наименование выбранной записи

  short msor;   
  short ds,ms,gs; //Дата на которую будет вычисляться сальдо
  short dt,mt,gt; //текущая дата
  int podr;
  short metka_pros; //0-всех 1-работающих 2-уволенных*/
  short metka_nast; //0-всех 1-имеющих настр. запись 2-не имеющих*/
  short metka_prov; //0-всех 1-без проводок
  class iceb_u_str metka_zap;  
  class iceb_u_str naim_podr;
  
  //Конструктор
  sptbn1_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    
    iceb_u_poltekdat(&dt,&mt,&gt);
    iceb_u_poltekdat(&ds,&ms,&gs);
    ds=1;
    iceb_u_dpm(&ds,&ms,&gs,3);
    msor=0;
  
    metka_pros=1;
    metka_nast=0;
    metka_prov=0;
    metka_zap.new_plus("");
    naim_podr.new_plus("");
   }      
 };

gboolean   sptbn1_key_press(GtkWidget *widget,GdkEventKey *event,class sptbn1_data *data);
void sptbn1_vibor(GtkTreeSelection *selection,class sptbn1_data *data);
void sptbn1_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class sptbn1_data *data);
void  sptbn1_knopka(GtkWidget *widget,class sptbn1_data *data);
void sptbn1_add_columns (GtkTreeView *treeview);
void sptbn1_create_list(class sptbn1_data *data);
int  sptbn1_prov_row(SQL_str row,class sptbn1_data *data);

int l_sptbn1_p(class l_sptbn_p *datap,GtkWidget *wpredok);
void l_sptbn1_ras(class sptbn1_data *data);

extern SQL_baza	bd;

int l_sptbn1(class iceb_u_str *tabnom,
class iceb_u_str *fio,
int podr,
GtkWidget *wpredok)
{
int gor=0;
int ver=0;

class sptbn1_data data;
char strsql[512];

data.podr=podr;
data.name_window.plus(__FUNCTION__);

if(data.podr != 0)
 {
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select naik from Podr where kod=%d",data.podr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   data.naim_podr.new_plus(row[0]);
 }



data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);



sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Список работников"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(sptbn1_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Просмотр списка табельных номеров"));


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


sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(sptbn1_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(sptbn1_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F6 %s",gettext("Уволенные"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(sptbn1_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Переключение режима просмотра списка (работающие, уволенные, все)"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(strsql,"F7 %s","*");
data.knopka[FK7]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(sptbn1_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Переключение режима просмотра списка (все, только без проводок)"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show(data.knopka[FK7]);

sprintf(strsql,"%sF7 %s",RFK,"V");
data.knopka[SFK7]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK7], "clicked",G_CALLBACK(sptbn1_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK7],gettext("Переключение режима просмотра списка (все, только помеченные символом \"V\", только без символа \"V\")"));
gtk_widget_set_name(data.knopka[SFK7],iceb_u_inttochar(SFK7));
gtk_widget_show(data.knopka[SFK7]);

sprintf(strsql,"F9 %s",gettext("Сортировка"));
data.knopka[FK9]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(sptbn1_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Переключение сортировки (по фамилиям, по табельным номерам)"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show(data.knopka[FK9]);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(sptbn1_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

sptbn1_create_list(&data);
gtk_widget_show(data.window);

//if(metka == 0)
//  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.metka_voz == 0)
  {
   tabnom->new_plus(data.tabnom_v.ravno());
   fio->new_plus(data.fio_v.ravno());
  }

return(data.metka_voz);


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  sptbn1_knopka(GtkWidget *widget,class sptbn1_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("sptbn1_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {

  case FK4:
    l_sptbn1_p(&data->poi,data->window);
    sptbn1_create_list(data);
    return;  

  case FK5:
   l_sptbn1_ras(data);
   return;  

  case FK6:
    data->metka_pros++;
    if(data->metka_pros == 3)
     data->metka_pros=0;
    sptbn1_create_list(data);
    return;  

  case FK7:
    data->metka_prov++;
    if(data->metka_prov == 2)
     data->metka_prov=0;
    sptbn1_create_list(data);
    return;  

  case SFK7:
    data->metka_nast++;
    if(data->metka_nast == 3)
     data->metka_nast=0;
    sptbn1_create_list(data);
    return;  

  case FK9:
    data->msor++;
    if(data->msor == 2)
     data->msor=0;
    sptbn1_create_list(data);
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

gboolean   sptbn1_key_press(GtkWidget *widget,GdkEventKey *event,class sptbn1_data *data)
{

switch(event->keyval)
 {

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
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK7],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK7],"clicked");
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
    printf("sptbn1_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void sptbn1_create_list (class sptbn1_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row;
iceb_u_str zagolov;

//printf("sptbn1_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(sptbn1_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(sptbn1_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

if(data->podr == 0)
 {
  if(data->msor == 0)
    sprintf(strsql,"select tabn,fio,datk,inn,sovm,bankshet from Kartb order by fio asc");
  else
    sprintf(strsql,"select tabn,fio,datk,inn,sovm,bankshet from Kartb order by tabn asc");
 }
else
 {
  if(data->msor == 0)
   sprintf(strsql,"select tabn,fio,datk,inn,sovm,bankshet from Kartb where podr=%d \
order by fio asc",data->podr);
  else
   sprintf(strsql,"select tabn,fio,datk,inn,sovm,bankshet from Kartb where podr=%d \
order by tabn asc",data->podr);
 } 
//printf("strsql=%s\n",strsql);

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;

float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(sptbn1_prov_row(row,data) != 0)
    continue;

  if(iceb_u_SRAV(data->tabnom_tv.ravno(),row[0],0) == 0)
   data->snanomer=data->kolzap;

  if(row[4][0] == '1')
   data->metka_zap.plus("S");      

  //Метка записи
  ss[COL_METKA].new_plus(data->metka_zap.ravno());

  //Табельный номер
  ss[COL_TABNOM].new_plus(row[0]);

  //Фамилия
  ss[COL_FIO].new_plus(row[1]);
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKA,ss[COL_METKA].ravno(),
  COL_TABNOM,ss[COL_TABNOM].ravno(),
  COL_FIO,ss[COL_FIO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->tabnom_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

sptbn1_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

//printf("metka_poi=%d\n",data->poi.metka_poi);
if(data->poi.metka_poi == 1 )
 {

// printf("Формирование заголовка с реквизитами поиска.\n");
  iceb_u_str strpoi;  

  strpoi.new_plus(gettext("Поиск"));
  strpoi.plus(" !!!");

  iceb_str_poisk(&strpoi,data->poi.fio.ravno(),gettext("Фамилия"));
  iceb_str_poisk(&strpoi,data->poi.inn.ravno(),gettext("Идентификационный номер"));
  iceb_str_poisk(&strpoi,data->poi.metka_nks.ravno(),gettext("Карт-счёт"));

  iceb_label_set_text_color(data->label_poisk,strpoi.ravno(),"red");
  gtk_widget_show(data->label_poisk);
 }
else
 {
  gtk_widget_hide(data->label_poisk); 
 }

zagolov.new_plus(gettext("Просмотр списка табельных номеров"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
if(data->podr != 0)
 {
  zagolov.ps_plus(gettext("Подразделение"));
  zagolov.plus(":");
  zagolov.plus(data->podr);
  zagolov.plus(" ");
  zagolov.plus(data->naim_podr.ravno());
 }

if(data->metka_pros == 0)
 zagolov.ps_plus(gettext("Просмотр всех работников"));
if(data->metka_pros == 1)
 zagolov.ps_plus(gettext("Просмотр только работающих работников"));
if(data->metka_pros == 2)
 zagolov.ps_plus(gettext("Просмотр только уволенных работников"));
 
if(data->metka_prov == 1)
 zagolov.ps_plus(gettext("Просмотр только записей без проводок"));

if(data->metka_nast == 1)
 zagolov.ps_plus(gettext("Просмотр только записей с меткой \"V\""));
if(data->metka_nast == 2)
 zagolov.ps_plus(gettext("Просмотр только записей без метки \"V\""));
 
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));
//printf("l_sptbn1-end\n");
}
/*****************/
/*Создаем колонки*/
/*****************/

void sptbn1_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("sptbn1_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Метка"), renderer,"text", COL_METKA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Табельный номер"), renderer,"text", COL_TABNOM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Фамилия Имя Отчество"), renderer,"text", COL_FIO,NULL);


//printf("sptbn1_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void sptbn1_vibor(GtkTreeSelection *selection,class sptbn1_data *data)
{
//printf("sptbn1_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;

gtk_tree_model_get(model,&iter,COL_TABNOM,&kod,COL_FIO,&naim,NUM_COLUMNS,&nomer,-1);

data->tabnom_v.new_plus(kod);
data->fio_v.new_plus(naim);
data->snanomer=nomer;

g_free(kod);
g_free(naim);

//printf("sptbn1_vibor-%s %d\n",data->tabnom_v.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void sptbn1_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class sptbn1_data *data)
{
iceb_sizww(data->name_window.ravno(),data->window);
data->metka_voz=0;

gtk_widget_destroy(data->window);


}
/****************************/
/*Проверка записей          */
/*****************************/

int  sptbn1_prov_row(SQL_str rows,class sptbn1_data *data)
{

if(data->metka_pros == 2 && rows[2][0] == '0')
  return(1);
  
if(data->metka_pros == 1 && rows[2][0] != '0')
  return(2);

if(data->poi.metka_poi == 1)
 {
  if(data->poi.fio.getdlinna() > 1 )
   if(iceb_u_strstrm(rows[1],data->poi.fio.ravno()) == 0)
    return(3);

  if(data->poi.inn.getdlinna() > 1)
   if(iceb_u_strstrm(rows[3],data->poi.inn.ravno()) == 0)
    return(4);

  if(data->poi.metka_nks.ravno_pr() == 1)
   if(rows[5][0] == '\0')
    return(5);

  if(data->poi.metka_nks.ravno_pr() == 2)
   if(rows[5][0] != '\0')
    return(6);
 }


short mprov1=0;
data->metka_zap.new_plus("");

/*Проверяем наличие промежуточной записи*/
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select prov,blok from Zarn where god=%d and mes=%d \
and tabn=%s",data->gt,data->mt,rows[0]);

if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  data->metka_zap.new_plus("V");    
  if(row[0][0] == '0')
   data->metka_zap.plus("*");
  else
   mprov1=1;
  if(row[1][0] != '0')
   data->metka_zap.plus("#");
 }
else 
  data->metka_zap.new_plus("*");    

if(data->metka_nast == 2 && data->metka_zap.ravno()[0] == 'V')
  return(4);

if(data->metka_nast == 1 && data->metka_zap.ravno()[0] == '*')
  return(5);


if(data->metka_prov == 1 && mprov1 == 1)
  return(6);

   
return(0);
}
/***************************/
/*Распечатать*/
/****************/

void l_sptbn1_ras(class sptbn1_data *data)
{

char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"sptbn1%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список работников"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Список работников"),0,0,0,0,0,0,ff,data->window);

fprintf(ff,"\
----------------------------------------------------------\n");

fprintf(ff,gettext("\
 Т/н |Фамилия Имя Отчество\n"));
  
fprintf(ff,"\
-----------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;

int kolzap=0;
while(cur.read_cursor(&row) != 0)
 {
  if(sptbn1_prov_row(row,data) != 0)
    continue;

  kolzap++;
  fprintf(ff,"%-5s %s\n",row[0],row[1]);

 }
fprintf(ff,"\
------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolzap);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);



}
