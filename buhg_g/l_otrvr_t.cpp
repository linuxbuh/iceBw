/*$Id: l_otrvr_t.c,v 1.21 2013/12/31 11:49:13 sasa Exp $*/
/*11.07.2015	07.09.2006	Белых А.И.	l_otrvr_t.c
Ввод и корректировка отработанного времени по табельному номеру
*/

#include <errno.h>
#include "buhg_g.h"

enum
{
 FK2,
 SFK2,
 FK3,
 FK5,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KOD,
 COL_NAIM,
 COL_R_DNI,
 COL_HASI,
 COL_K_DNI,
 COL_KOLIH_R_DNEI,
 COL_HAS_V_DEN,
 COL_DATAN,
 COL_DATAK,
 COL_KOMENT,
 COL_DATA_VREM,
 COL_KTO,  
//Не выводится на экран
 COL_NOM_ZAP,
 NUM_COLUMNS
};

class  otrvr_t_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей

  iceb_u_str zapros;

  iceb_u_str kodtv;//Код толькочто введённой записи
  
  class iceb_u_str kodv; //Выбранный код 
  int nom_zap;           //Номер только что выбранной записи
    
  int tabnom;
  short mp,gp;
  class iceb_u_str fio;
  
  //Конструктор
  otrvr_t_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    nom_zap=0;
    fio.plus("");
   }      
 };

gboolean   otrvr_t_key_press(GtkWidget *widget,GdkEventKey *event,class otrvr_t_data *data);
void otrvr_t_vibor(GtkTreeSelection *selection,class otrvr_t_data *data);
void otrvr_t_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class otrvr_t_data *data);
void  otrvr_t_knopka(GtkWidget *widget,class otrvr_t_data *data);
void otrvr_t_add_columns (GtkTreeView *treeview);
void otrvr_t_create_list(class otrvr_t_data *data);

void otrvr_t_rasp(class otrvr_t_data *data);
void otrvr_t_udzap(class otrvr_t_data *data);
int l_otrvr_t_v(int metka_rr,int tabnom,short mp,short gp,int vid_tab,int nom_zap,GtkWidget *wpredok);

extern SQL_baza	bd;
 
void l_otrvr_t(int tabnom,short mp,short gp,GtkWidget *wpredok)
{
otrvr_t_data data;
char bros[512];
SQL_str row;
class SQLCURSOR cur;
char strsql[512];


data.tabnom=tabnom;
data.mp=mp;
data.gp=gp;


sprintf(strsql,"select fio from Kartb where tabn=%d",data.tabnom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.fio.new_plus(row[0]);


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Ввод и корректировка отработанного времени"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(otrvr_t_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Ввод и корректировка отработанного времени"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_widget_show (data.sw);

//Кнопки


sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(otrvr_t_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(otrvr_t_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(otrvr_t_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить выбранную запись"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(otrvr_t_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(otrvr_t_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

otrvr_t_create_list(&data);
gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


//printf("l_otrvr_t end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  otrvr_t_knopka(GtkWidget *widget,class otrvr_t_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("otrvr_t_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    if(iceb_pbpds(data->mp,data->gp,data->window) != 0)
     return;  
    if(l_zarvidtab(1,&kod,&naim,data->window) != 0)
     return;
    l_otrvr_t_v(0,data->tabnom,data->mp,data->gp,kod.ravno_atoi(),0,data->window);
    otrvr_t_create_list(data);
    return;  

  case SFK2:

    l_otrvr_t_v(1,data->tabnom,data->mp,data->gp,data->kodv.ravno_atoi(),data->nom_zap,data->window);
    otrvr_t_create_list(data);
    return;  

  case FK3:
    if(iceb_pbpds(data->mp,data->gp,data->window) != 0)
     return;  
    if(data->kolzap == 0)
      return;
    otrvr_t_udzap(data);
    otrvr_t_create_list(data);
    return;  


  case FK5:
   otrvr_t_rasp(data);
    return;  

    
  case FK10:
//    printf("otrvr_t_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   otrvr_t_key_press(GtkWidget *widget,GdkEventKey *event,class otrvr_t_data *data)
{
iceb_u_str repl;
//printf("otrvr_t_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {
  case GDK_KEY_plus: //это почемуто не работает Нажата клавиша плюс на основной клавиатуре
  case GDK_KEY_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
  case GDK_KEY_F2:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
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
    printf("otrvr_t_key_press-Нажата клавиша Shift\n");

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
void otrvr_t_create_list (class otrvr_t_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;
iceb_u_str zagolov;

//printf("otrvr_t_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(otrvr_t_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(otrvr_t_vibor),data);

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
G_TYPE_INT,
G_TYPE_INT);

sprintf(strsql,"select kodt,dnei,has,kdnei,datn,datk,ktoz,vrem,kolrd,kolrh,kom,nomz from Ztab \
where tabn=%d and god=%d and mes=%d order by kodt,nomz asc",data->tabnom,data->gp,data->mp);

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
     
  if(iceb_u_SRAV(data->kodtv.ravno(),row[0],0) == 0)
   data->snanomer=data->kolzap;

  //Код 
//  ss[COL_KOD].new_plus(row[0]);

  //Наименование
  ss[COL_NAIM].new_plus("");
  sprintf(strsql,"select naik from Tabel where kod=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    ss[COL_NAIM].new_plus(row1[0]);

  //Количество отработанных рабочих дней
  sprintf(strsql,"%.10g",atof(row[1]));
  ss[COL_R_DNI].new_plus(strsql);

  //Количество отработанных часов
  sprintf(strsql,"%.10g",atof(row[2]));
  ss[COL_HASI].new_plus(strsql);

  //Количество отработанных календарных дней
  sprintf(strsql,"%.10g",atof(row[3]));
  ss[COL_K_DNI].new_plus(strsql);

  //Количество рабочих дней в месяце
  sprintf(strsql,"%.10g",atof(row[8]));
  ss[COL_KOLIH_R_DNEI].new_plus(strsql);

  //Количество рабочих часов в день
  sprintf(strsql,"%.10g",atof(row[9]));
  ss[COL_HAS_V_DEN].new_plus(strsql);

  //Дата начала
  ss[COL_DATAN].new_plus(iceb_u_datzap(row[4]));

  //Дата конца
  ss[COL_DATAK].new_plus(iceb_u_datzap(row[5]));


  gtk_list_store_append (model, &iter);


  gtk_list_store_set (model, &iter,
  COL_KOD,row[0],
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_R_DNI,ss[COL_R_DNI].ravno(),
  COL_HASI,ss[COL_HASI].ravno(),
  COL_K_DNI,ss[COL_K_DNI].ravno(),
  COL_KOLIH_R_DNEI,ss[COL_KOLIH_R_DNEI].ravno(),
  COL_HAS_V_DEN,ss[COL_HAS_V_DEN].ravno(),
  COL_DATAN,ss[COL_DATAN].ravno(),
  COL_DATAK,ss[COL_DATAK].ravno(),
  COL_KOMENT,row[10],
  COL_DATA_VREM,iceb_u_vremzap(row[7]),
  COL_KTO,iceb_kszap(row[6],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kodtv.new_plus("");
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

otrvr_t_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


zagolov.new_plus(gettext("Ввод и корректировка отработанного времени"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

zagolov.ps_plus(data->tabnom);
zagolov.plus(" ");
zagolov.plus(data->fio.ravno());

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void otrvr_t_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

printf("otrvr_t_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование вида табеля"), renderer,"text", COL_NAIM,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дни"), renderer,"text", COL_R_DNI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Часы"), renderer,"text", COL_HASI,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Календарные\nдни"), renderer,"text", COL_K_DNI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Кол.раб.\nдней в месяце"), renderer,"text", COL_KOLIH_R_DNEI,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Кол.раб.\nчасов в дне"), renderer,"text", COL_HAS_V_DEN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата\nначала"), renderer,"text", COL_DATAN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата\nконца"), renderer,"text", COL_DATAK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

printf("otrvr_t_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void otrvr_t_vibor(GtkTreeSelection *selection,class otrvr_t_data *data)
{
printf("otrvr_t_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;
gint  nom_zap;

gtk_tree_model_get(model,&iter,COL_KOD,&kod,COL_NOM_ZAP,&nom_zap,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(kod);
data->snanomer=nomer;
data->nom_zap=nom_zap;

g_free(kod);

//printf("otrvr_t_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void otrvr_t_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class otrvr_t_data *data)
{
//printf("otrvr_t_v_row\n");
//printf("otrvr_t_v_row корректировка\n");

g_signal_emit_by_name(data->knopka[SFK2],"clicked");


}
/*****************************/
/*Удаление записи            */
/*****************************/

void otrvr_t_udzap(class otrvr_t_data *data)
{

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
 return;

char strsql[512];

sprintf(strsql,"delete from Ztab where tabn=%d and god=%d and mes=%d and kodt=%d and nomz=%d",
data->tabnom,data->gp,data->mp,
data->kodv.ravno_atoi(),data->nom_zap);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;

}
/**********************************/
/*Распечатать список*/
/****************************/

void otrvr_t_rasp(class otrvr_t_data *data)
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

sprintf(strsql,"otrvr_t%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Отработанное время"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Отработанное время"),0,0,0,0,0,0,ff,data->window);

fprintf(ff,"%d %s\n",data->tabnom,data->fio.ravno());
fprintf(ff,"%s:%02d.%d\n",gettext("Дата"),data->mp,data->gp);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Код |                    |Колич.|Колич.|Колич.|Колич.|Колич.|  Дата    |   Дата   |                   |\n\
табе|  Наименование      |отраб.|отраб.|кален.|рабоч.|рабоч.| начала   |  конца   |Дата и время запис.| Кто записал\n\
 ля |    табеля          |дней  |часов |дней  |дней в|часов |          |          |                   |\n\
    |                    |      |      |      |месяце|в дне |          |          |                   |\n"));
//                        123456 123456 123456 123456 123456 1234567890 1234567890
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

SQL_str row1;
class SQLCURSOR cur1;

class iceb_u_str naim("");
class iceb_u_str datan("");
class iceb_u_str datak("");
float kol_otr_dnei;
float kol_otr_has;
float kol_kal_dnei;
float i_kol_otr_dnei=0.;
float i_kol_otr_has=0.;
float i_kol_kal_dnei=0.;
float kol_rab_dnei;
float kol_rab_has;

while(cur.read_cursor(&row) != 0)
 {

  //Наименование
  
  sprintf(strsql,"select naik from Tabel where kod=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim.new_plus(row1[0]);
  else
   naim.new_plus("");  
  kol_otr_dnei=atof(row[1]);
  kol_otr_has=atof(row[2]);
  kol_kal_dnei=atof(row[3]);
  
  i_kol_otr_dnei+=kol_otr_dnei;
  i_kol_otr_has+=kol_otr_has;
  i_kol_kal_dnei+=kol_kal_dnei;

  kol_rab_dnei=atof(row[8]);
  kol_rab_has=atof(row[9]);
  
  datan.new_plus(iceb_u_datzap(row[4]));
  datak.new_plus(iceb_u_datzap(row[5]));
  

  fprintf(ff,"%-4s %-*.*s %6.6g %6.6g %6.6g %6.6g %6.6g %10s %10s %s %s\n",
  row[0],
  iceb_u_kolbait(20,naim.ravno()),iceb_u_kolbait(20,naim.ravno()),naim.ravno(),
  kol_otr_dnei,
  kol_otr_has,
  kol_kal_dnei,
  kol_rab_dnei,
  kol_rab_has,
  datan.ravno(),
  datak.ravno(),
  iceb_u_vremzap(row[7]),
  iceb_kszap(row[6],data->window));

  if(iceb_u_strlen(naim.ravno()) > 20)
   fprintf(ff,"%4s %s\n","",iceb_u_adrsimv(20,naim.ravno()));

 }
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %6.6g %6.6g %6.6g\n",
iceb_u_kolbait(25,gettext("Итого")),gettext("Итого"),i_kol_otr_dnei,i_kol_otr_has,i_kol_kal_dnei);

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
