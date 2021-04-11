/*$Id: l_uslugi.c,v 1.43 2013/11/24 08:25:06 sasa Exp $*/
/*08.05.2015	17.07.2005	Белых А.И.	l_uslugi.c
Ввод и корректировка списка услуг
*/
#include <errno.h>
#include "buhg_g.h"
#include "l_uslugi.h"
enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK6,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KOD,
 COL_NAIM,
 COL_KODGR,
 COL_EI,
 COL_SHET,
 COL_CENA,
 COL_NDS,
 COL_ARTIKUL,
 COL_KU,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  uslugi_data
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

  class uslugi_rek poi;
  iceb_u_str zapros;

  iceb_u_str kodtv; //код только что введённый
  iceb_u_str kodv; //Выбранный код 
  iceb_u_str naimv; //Наименование выбранной записи
  int  metka_rr;
  //Конструктор
  uslugi_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
   }      
 };

gboolean   uslugi_key_press(GtkWidget *widget,GdkEventKey *event,class uslugi_data *data);
void uslugi_vibor(GtkTreeSelection *selection,class uslugi_data *data);
void uslugi_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class uslugi_data *data);
void  uslugi_knopka(GtkWidget *widget,class uslugi_data *data);
void uslugi_add_columns (GtkTreeView *treeview);
void uslugi_create_list(class uslugi_data *data);
int  uslugi_prov_row(SQL_str row,class uslugi_data *data);

int l_uslugi_v(iceb_u_str *kodzap,GtkWidget *wpredok);
int l_uslugi_p(class uslugi_rek *datap,GtkWidget *wpredok);
int  uslugi_pvu(int met,const char *kod,GtkWidget *wpredok);
void uslugi_udzap(class uslugi_data *data);
void uslugi_unk(GtkWidget *wpredok);
void uslugi_rasp(class uslugi_data *data);
void l_uslugi_zsu(class  uslugi_data *data);
void impuslsu(GtkWidget *wpredok);

extern SQL_baza	bd;
 
int l_uslugi(int metka, //0-воод и корректировка 1-выбор
iceb_u_str *kod,iceb_u_str *naimk,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class uslugi_data data;
char bros[512];
data.metka_rr=metka;
data.name_window.plus(__FUNCTION__);

if(naimk->getdlinna() > 1)
 {
  data.poi.naim.new_plus(naimk->ravno());
  data.poi.metka_poi=1;
 }
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_rr == 1)
 {
  if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
 }


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список услуг"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uslugi_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Список услуг"));


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


sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(uslugi_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"SF2 %s",gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(uslugi_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(uslugi_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить выбранную запись"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"SF3 %s",gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(uslugi_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удалить неиспользуемые коды услуг"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(uslugi_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(uslugi_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Замена"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(uslugi_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Замена выбранного кода услуги на другой с его удалением после замены"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F9 %s",gettext("Импорт"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(uslugi_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Импорт справочника услуг из файла"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show(data.knopka[FK9]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(uslugi_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

uslugi_create_list(&data);
gtk_widget_show(data.window);

if(metka == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
if(data.metka_voz == 0)
 if(metka == 1)
  {
   kod->new_plus(data.kodv.ravno());
   naimk->new_plus(data.naimv.ravno());
  }
return(data.metka_voz);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uslugi_knopka(GtkWidget *widget,class uslugi_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("uslugi_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    data->kodtv.new_plus("");
    if(l_uslugi_v(&data->kodtv,data->window) == 0)
      uslugi_create_list(data);
    return;  

  case SFK2:
    if(l_uslugi_v(&data->kodv,data->window) == 0)
     {
      data->kodtv.new_plus(data->kodv.ravno());
      uslugi_create_list(data);
     }
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    uslugi_udzap(data);
    uslugi_create_list(data);
    return;  

  case SFK3:
    uslugi_unk(data->window);
    uslugi_create_list(data);
    return;  

  case FK4:
    l_uslugi_p(&data->poi,data->window);
    uslugi_create_list(data);
    return;  

  case FK5:
   uslugi_rasp(data);
   return;  

  case FK6:
   l_uslugi_zsu(data);
   uslugi_create_list(data);
   return;  

  case FK9:
   impuslsu(data->window);
   uslugi_create_list(data);
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

gboolean   uslugi_key_press(GtkWidget *widget,GdkEventKey *event,class uslugi_data *data)
{

switch(event->keyval)
 {
  case GDK_KEY_F2:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK3],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK3],"clicked");
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

  case GDK_KEY_F9:
    g_signal_emit_by_name(data->knopka[FK9],"clicked");
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
/*    printf("uslugi_key_press-Нажата клавиша Shift\n");*/

    data->kl_shift=1;

    return(TRUE);

  default:
/*    printf("Не выбрана клавиша !\n");*/
    break;
 }

return(TRUE);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void uslugi_create_list (class uslugi_data *data)
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


//printf("uslugi_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(uslugi_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(uslugi_vibor),data);

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
G_TYPE_INT);

sprintf(strsql,"select * from Uslugi order by kodus asc");

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str ss("");

data->kolzap=0;

float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
   if(uslugi_prov_row(row,data) != 0)
    continue;

  if(iceb_u_SRAV(data->kodtv.ravno(),row[0],0) == 0)
   data->snanomer=data->kolzap;

  //Метка цены
  if(row[6] != NULL)
   {
    if(atoi(row[6]) == 0)
      ss.new_plus(gettext("Без НДС"));
    else
      ss.new_plus(gettext("С НДС"));
   }
  else
   ss.new_plus("");     
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,row[0],
  COL_KODGR,row[1],
  COL_NAIM,row[2],
  COL_EI,row[3],
  COL_CENA,row[5],
  COL_SHET,row[4],
  COL_NDS,ss.ravno(),
  COL_ARTIKUL,row[9],
  COL_KU,row[10],
  COL_DATA_VREM,iceb_u_vremzap(row[8]),
  COL_KTO,iceb_kszap(row[7],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
printf("%s\n",__FUNCTION__);

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

uslugi_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

if(data->poi.metka_poi == 1 )
 {

// printf("Формирование заголовка с реквизитами поиска.\n");
  iceb_u_str strpoi;  

  strpoi.new_plus(gettext("Поиск"));
  strpoi.plus(" !!!");

  iceb_str_poisk(&strpoi,data->poi.kod.ravno(),gettext("Код услуги"));
  iceb_str_poisk(&strpoi,data->poi.naim.ravno(),gettext("Наименование услуги"));
  iceb_str_poisk(&strpoi,data->poi.grupa.ravno(),gettext("Код группы"));

  iceb_label_set_text_color(data->label_poisk,strpoi.ravno(),"red");
  gtk_widget_show(data->label_poisk);
 }
else
 {
  gtk_widget_hide(data->label_poisk); 
 }

zagolov.new_plus(gettext("Список групп услуг"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void uslugi_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код"),renderer,"text",COL_KOD,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_KOD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Группа"),renderer,"text",COL_KODGR,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_KODGR);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Наименование услуги"),renderer,"text",COL_NAIM,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NAIM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Е.и."), renderer,"text", COL_EI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Цена"), renderer,"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт"), renderer,"text", COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("НДС"), renderer,"text", COL_NDS,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Артикул"), renderer,"text", COL_ARTIKUL,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KU,NULL);

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
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void uslugi_vibor(GtkTreeSelection *selection,class uslugi_data *data)
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
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void uslugi_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class uslugi_data *data)
{

if(data->metka_rr == 0)
 {
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");
  return;
 }
iceb_sizww(data->name_window.ravno(),data->window);

gtk_widget_destroy(data->window);

data->metka_voz=0;

}
/****************************/
/*Проверка записей          */
/*****************************/

int  uslugi_prov_row(SQL_str row,class uslugi_data *data)
{
if(data->poi.metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poi.kod.ravno(),row[0],0,0) != 0)
 return(1);

//Полное сравнение
if(iceb_u_proverka(data->poi.grupa.ravno(),row[1],0,0) != 0)
 return(1);


//Поиск образца в строке
if(data->poi.naim.ravno()[0] != '\0' && iceb_u_strstrm(row[2],data->poi.naim.ravno()) == 0)
 return(1);


   
return(0);
}
/*****************************/
/*Удаление записи            */
/*****************************/

void uslugi_udzap(class uslugi_data *data)
{

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
 return;

if(uslugi_pvu(1,data->kodv.ravno(),data->window) != 0)
 return;

char strsql[512];

sprintf(strsql,"delete from Uslugi where kodus=%s",
data->kodv.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;

}
/*****************************/
/*Проверить возможность удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int  uslugi_pvu(int met,const char *kod,GtkWidget *wpredok)
{
char strsql[1024];
int kolstr;
SQLCURSOR cur;

sprintf(strsql,"select metka,kodzap from Usldokum1 where metka=1 and kodzap=%s limit 1",kod);

if((kolstr=iceb_sql_readkey(strsql,wpredok)) < 0)
 return(1);
  
if(kolstr > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Usldokum1",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    


sprintf(strsql,"select metka,kodzap from Usldokum2 where metka=1 \
and kodzap=%s limit 1",kod);

kolstr=iceb_sql_readkey(strsql,wpredok);
  
if(kolstr > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Usldokum2",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kodi from Specif where kodi=%s or kodd=%s and kz=1 limit 1",kod,kod);

kolstr=iceb_sql_readkey(strsql,wpredok);
  
if(kolstr > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Specif",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select mz from Restdok1 where mz=1 and kodz=%s limit 1",kod);

if((kolstr=sql_readkey(&bd,strsql)) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE) /*Такой таблицы нет*/
   iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);

if(kolstr > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Restdok1",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

return(0);
}
/**************************/
/*Удаление неиспользуемых кодов групп*/
/***********************************/

void uslugi_unk(GtkWidget *wpredok)
{

if(iceb_menu_danet(gettext("Удалить неиспользуемые коды ? Вы уверены ?"),2,wpredok) == 2)
 return;

gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));



char strsql[512];
SQL_str row;
SQLCURSOR cur;
int kolstr;
sprintf(strsql,"select kodus from Uslugi");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  if(uslugi_pvu(0,row[0],wpredok) != 0)
    continue;
  sprintf(strsql,"delete from Uslugi where kodus=%s",row[0]);
  iceb_sql_zapis(strsql,1,1,wpredok);
 }
}
/*********************************************/
void l_uslugi_rasp_hap(int nom_str,int *kolstr,FILE *ff,GtkWidget *wpredok)
{
if(kolstr != NULL)
 *kolstr+=4;
fprintf(ff,"%80s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 Код |          Наименование        |Г-па|Е.и.|  Цена  | Счёт  |  НДС  |Дата и время запис.| Кто записал\n"));
  
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");
}



/**********************************/
/*Распечатать список*/
/****************************/

void uslugi_rasp(class uslugi_data *data)
{
char strsql[1024];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
int kolstr=0;

sprintf(strsql,"select * from Uslugi order by kodus asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
char imaf[64];
sprintf(imaf,"usl%d.lst",getpid());


if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

char imafcsv[64];
sprintf(imafcsv,"usl%d.csv",getpid());

class iceb_fopen filcsv;
if(filcsv.start(imafcsv,"w",data->window) != 0)
 return;

fprintf(filcsv.ff,"#%s\n#%s %s %s\n",
gettext("Экспорт списка услуг"),
gettext("Выгружено из базы"),
iceb_get_namebase(),
iceb_get_pnk("00",1,data->window));

struct  tm      *bf;
time_t          tmm;
time(&tmm);
bf=localtime(&tmm);

fprintf(filcsv.ff,"\
#%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(filcsv.ff,"#Расшифровка полей:\n\
#1 код услуги\n\
#2 код группы\n\
#3 наименование услуги\n\
#4 единица измерения\n\
#5 счёт учета\n\
#6 цена услуги\n\
#7 0-цена без НДС 1-цена с НДС\n\
#8 артикул\n\
#9 код УКТ ВЭД Украинская класификация товаров внешне экономической деятельности\n");

iceb_u_zagolov(gettext("Список услуг"),0,0,0,0,0,0,iceb_get_pnk("00",0,data->window),ff);

l_uslugi_rasp_hap(1,NULL,ff,data->window);

class iceb_u_str nds("");

cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(uslugi_prov_row(row,data) != 0)
    continue;
  if(row[6] != NULL)
   {
    if(atoi(row[6]) == 1)
     nds.new_plus(gettext("С НДС"));
    else
     nds.new_plus(gettext("Без НДС"));
   }
  else
   nds.new_plus("");
   
  fprintf(ff,"%-5s %-*.*s %-4s %-*s %8.8g %-*s %-*s %s %s\n",
  row[0],
  iceb_u_kolbait(30,row[2]),iceb_u_kolbait(30,row[2]),row[2],
  row[1],
  iceb_u_kolbait(4,row[3]),row[3],
  atof(row[6]),
  iceb_u_kolbait(7,row[4]),row[4],
  iceb_u_kolbait(7,nds.ravno()),nds.ravno(),
  iceb_u_vremzap(row[8]),iceb_kszap(row[7],data->window));

  for(int ii=30; ii < iceb_u_strlen(row[2]); ii+=30)
   fprintf(ff,"%5s %-*.*s\n",
   "",
   iceb_u_kolbait(30,iceb_u_adrsimv(ii,row[2])),
   iceb_u_kolbait(30,iceb_u_adrsimv(ii,row[2])),
   iceb_u_adrsimv(ii,row[2]));

  fprintf(filcsv.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10]);

 }
fprintf(ff,"ICEB_LST_END\n\
---------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
filcsv.end();


class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка справочника услуг"));

int orient=iceb_ustpeh(oth.spis_imaf.ravno(0),3,data->window);
iceb_rnl(oth.spis_imaf.ravno(0),orient,NULL,&l_uslugi_rasp_hap,data->window);

oth.spis_imaf.plus(imafcsv);
oth.spis_naim.plus(gettext("Распечатка справочника услуг для загрузки в другую базу"));

iceb_rabfil(&oth,data->window);

}
/*************************************************/
/*Замена одного кода услуги на другой с удалением первого*/
/*********************************************************/
void l_uslugi_zsu(class  uslugi_data *data)
{
class iceb_u_str kodusln("");
class iceb_u_str repl(gettext("Код услуги"));
repl.plus(":",data->kodv.ravno());

SQL_str row;
class SQLCURSOR cur;

char strsql[512];
sprintf(strsql,"select naius from Uslugi where kodus=%d",data->kodv.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 repl.plus(" ",row[0]);

repl.ps_plus(gettext("Введите код услуги на который нужно заменить"));
naz:;

if(iceb_menu_vvod1(repl.ravno(),&kodusln,20,"",data->window) != 0)
 return;

if(kodusln.ravno_atoi() == data->kodv.ravno_atoi())
 {
  iceb_menu_soob(gettext("Коды услуг одинаковы!"),data->window);
  goto naz;
 }

/*проверяем код услуги*/
sprintf(strsql,"select naius from Uslugi where kodus=%d",kodusln.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d %s",gettext("Нет кода услуги"),kodusln.ravno_atoi(),gettext("в справочнике услуг"));
  iceb_menu_soob(strsql,data->window);
  goto naz;
 }

if(iceb_parol(0,data->window) != 0)
 return;


class iceb_gdite_data gdite;
iceb_gdite(&gdite,1,data->window);


int nz=0;
int nomer_er=0;
nazad:;

sprintf(strsql,"update Usldokum1 set kodzap=%d,nz=%d where kodzap=%d and metka=1",kodusln.ravno_atoi(),nz++,data->kodv.ravno_atoi());
if(sql_zap(&bd,strsql) != 0)
 {
  if((nomer_er=sql_nerror(&bd)) == ER_DBACCESS_DENIED_ERROR) //Только чтение
   {
    iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),data->window);
    return;
   }
  else
   {
    if(nomer_er == ER_DUP_ENTRY) /*Запись уже есть*/     
     goto nazad;
    else
      iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);
   }
 }

nz=0;

nazad1:;

sprintf(strsql,"update Usldokum2 set kodzap=%d,nz=%d where kodzap=%d and metka=1",kodusln.ravno_atoi(),nz++,data->kodv.ravno_atoi());
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY) /*Запись уже есть*/
   goto nazad1; 
  else
   iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
 }
 
sprintf(strsql,"update Roznica set kodm=%d where kodm=%d and metka=2",kodusln.ravno_atoi(),data->kodv.ravno_atoi());
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"update Specif set kodd=%d where kodd=%d and kz=1",kodusln.ravno_atoi(),data->kodv.ravno_atoi());
iceb_sql_zapis(strsql,1,0,data->window);



nz=0;

nazad2:;

//sprintf(strsql,"select mz from Restdok1 where mz=1 and kodz=%s limit 1",kod);

sprintf(strsql,"update Restdok1 set kodz=%d,nz=%d where kodz=%d and mz=1",kodusln.ravno_atoi(),nz++,data->kodv.ravno_atoi());
if(sql_zap(&bd,strsql) != 0)
 {

  if((nomer_er=sql_nerror(&bd)) != ER_NO_SUCH_TABLE) /*Такой таблицы нет*/
   {  
    if(nomer_er == ER_DUP_ENTRY) /*Запись уже есть*/
     goto nazad2; 
    else
     iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
   }
 }


if(uslugi_pvu(1,data->kodv.ravno(),data->window) != 0)
 return;
  
sprintf(strsql,"delete from Uslugi where kodus=%d",data->kodv.ravno_atoi());
iceb_sql_zapis(strsql,1,0,data->window);

}
