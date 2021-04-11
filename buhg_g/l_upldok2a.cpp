/*$Id: l_upldok2a.c,v 1.4 2013/08/25 08:26:41 sasa Exp $*/
/*08.05.2015	23.04.2012	Белых А.И.	l_upldok2a.c
Работа со списком списания топлива по счетам для конкретного путевого листа
*/
#include <math.h>
#include        <errno.h>
#include  "buhg_g.h"

enum
{
  COL_SHET,
  COL_KODTOP,
  COL_ZATR,
  COL_ZATRPN,
  COL_KOMENT,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  FK5,
  FK10,
  SFK2,
  KOL_F_KL

};

class l_upldok2a_data
 {
  public:


  class iceb_u_str shet;
  class iceb_u_str kod_top;
  class iceb_u_str shet_tv; //только что введённый код
  class iceb_u_str kod_top_tv;
    
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  short     metka_poi;  //0-без поиска 1-с поиском
  int       metka_voz;  //0-выбрали 1-нет  

  int kod_avt;
  class iceb_u_str nom_znak;
  class iceb_u_str naim_avt;

  class iceb_u_str data_dok;
  class iceb_u_str nom_dok;
  class iceb_u_str kod_podr;
  class iceb_u_str zapros;

  class iceb_u_spisok KST; //Коды списанного в путевом листе топлива class
  iceb_u_double KOL; //Количество списанного топлива
  
  //Конструктор
  l_upldok2a_data()
   {
    snanomer=0;
    metka_voz=kl_shift=metka_poi=0;
    window=treeview=NULL;
    shet_tv.plus("");
    kod_avt=0;
    nom_znak.plus("");
    naim_avt.plus("");
   }      
 };

gboolean   l_upldok2a_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok2a_data *data);
void l_upldok2a_vibor(GtkTreeSelection *selection,class l_upldok2a_data *data);
void l_upldok2a_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_upldok2a_data *data);
void  l_upldok2a_knopka(GtkWidget *widget,class l_upldok2a_data *data);
void l_upldok2a_add_columns (GtkTreeView *treeview);
void l_upldok2a_udzap(class l_upldok2a_data *data);
void l_upldok2a_rasp(class l_upldok2a_data *data);
int l_upldok2a_create_list (class l_upldok2a_data *data);

int l_upldok_v1(class iceb_u_str *data_dok,class iceb_u_str *nom_dok,class iceb_u_str *kod_podr,GtkWidget *wpredok);
int l_upldok2a_v(const char *data_dok,const char *nom_dok,int kod_podr,class iceb_u_str *shet,class iceb_u_str *kodtop,GtkWidget *wpredok);
int l_upldok2a_p(class l_upldok2a_rek *rek_poi,GtkWidget *wpredok);
int l_upldok_1(const char *data_dok,const char *nomd,const char *kod_pod,GtkWidget *wpredok);
void l_upldok2a_spst(const char *datd,int kp,const char *nomd,class iceb_u_spisok *KST,class iceb_u_double *KOL,GtkWidget *wpredok);


extern SQL_baza  bd;

int l_upldok2a(const char *data_dok,
const char *nom_dok,
int kod_podr,
GtkWidget *wpredok)
{
l_upldok2a_data data;
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

data.data_dok.new_plus(data_dok);
data.nom_dok.new_plus(nom_dok);
data.kod_podr.new_plus(kod_podr);

l_upldok2a_spst(data.data_dok.ravno_sqldata(),data.kod_podr.ravno_atoi(),data.nom_dok.ravno(),&data.KST,&data.KOL,wpredok);

sprintf(strsql,"select ka from Upldok where god=%d and kp=%d and nomd='%s'",data.data_dok.ravno_god(),data.kod_podr.ravno_atoi(),data.nom_dok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  data.kod_avt=atoi(row[0]);
 } 



sprintf(strsql,"select nomz,naik from Uplavt where kod=%d",data.kod_avt);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  data.nom_znak.new_plus(row[0]);
  data.naim_avt.new_plus(row[1]);
 }


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Списание топлива по счетам"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_upldok2a_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Списание топлива по счетам"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_upldok2a_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_upldok2a_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_upldok2a_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);



sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_upldok2a_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_upldok2a_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

if(l_upldok2a_create_list(&data) != 0)
 return(1);

gtk_widget_show(data.window);
//if(metka_rr == 0)
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
int l_upldok2a_create_list (class l_upldok2a_data *data)
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

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_upldok2a_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_upldok2a_vibor),data);

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


sprintf(strsql,"select kt,shet,zt,zpn,koment,ktoz,vrem from Upldok2a where datd='%s' and kp=%d and nomd='%s'",data->data_dok.ravno_sqldata(),data->kod_podr.ravno_atoi(),data->nom_dok.ravno());
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
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);


  if(iceb_u_SRAV(data->shet_tv.ravno(),row[1],0) == 0 && iceb_u_SRAV(data->kod_top_tv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;


  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_SHET,row[1],
  COL_KODTOP,row[0],
  COL_ZATR,row[2],
  COL_ZATRPN,row[3],

  COL_KOMENT,row[4],
  COL_DATA_VREM,iceb_u_vremzap(row[6]),
  COL_KTO,iceb_kszap(row[5],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->shet_tv.new_plus("");
data->kod_top.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_upldok2a_add_columns (GTK_TREE_VIEW (data->treeview));


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
zagolov.plus(gettext("Списание топлива по счетам"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql,"%s:%s %s:%s",gettext("Дата"),data->data_dok.ravno(),gettext("Номер документа"),data->nom_dok.ravno());
zagolov.ps_plus(strsql);

sprintf(strsql,"%s:%d %s %s",gettext("Автомобиль"),data->kod_avt,data->nom_znak.ravno(),data->naim_avt.ravno());
zagolov.ps_plus(strsql);
for(int nom=0; nom < data->KST.kolih(); nom++)
 {
  sprintf(strsql,"%6s %10.3f",data->KST.ravno(nom),data->KOL.ravno(nom));
  zagolov.ps_plus(strsql);
 }
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

gtk_widget_show(data->label_kolstr);

return(0);

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_upldok2a_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Счёт"),renderer,"text",COL_SHET,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_SHET);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код топлива"),renderer,"text",COL_KODTOP,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KODTOP);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Затраты по норме"),renderer,"text",COL_ZATRPN,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ZATRPN);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Затраты по факту"),renderer,"text",COL_ZATR,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ZATR);
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

void l_upldok2a_vibor(GtkTreeSelection *selection,class l_upldok2a_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *shet;
gchar *kodtop;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_SHET,&shet,COL_KODTOP,&kodtop,NUM_COLUMNS,&nomer,-1);

data->shet.new_plus(shet);
data->kod_top.new_plus(kodtop);
data->snanomer=nomer;

g_free(shet);
g_free(kodtop);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_upldok2a_knopka(GtkWidget *widget,class l_upldok2a_data *data)
{
class iceb_u_str kod("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_upldok2a_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;
  
    if(l_upldok2a_v(data->data_dok.ravno(),data->nom_dok.ravno(),data->kod_podr.ravno_atoi(),&data->shet_tv,&data->kod_top_tv,data->window) == 0)
      l_upldok2a_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(l_upldok2a_v(data->data_dok.ravno(),data->nom_dok.ravno(),data->kod_podr.ravno_atoi(),&data->shet,&data->kod_top,data->window) == 0)
      l_upldok2a_create_list(data);
    return;  

  case FK3:
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_upldok2a_udzap(data);
    l_upldok2a_create_list(data);
    return;  

  

  case FK5:
    l_upldok2a_rasp(data);
    return;  

    
  case FK10:
    l_upldok2a_provpst(data->data_dok.ravno_sqldata(),data->kod_podr.ravno_atoi(),data->nom_dok.ravno(),data->window);
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_upldok2a_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok2a_data *data)
{
iceb_u_str repl;

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
void l_upldok2a_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_upldok2a_data *data)
{

  g_signal_emit_by_name(data->knopka[SFK2],"clicked");


}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_upldok2a_udzap(class l_upldok2a_data *data)
{


char strsql[512];

sprintf(strsql,"delete from Upldok2a where datd='%s' and kp=%d and nomd='%s' and kt='%s' and shet='%s'",data->data_dok.ravno_sqldata(),data->kod_podr.ravno_atoi(),data->nom_dok.ravno(),data->kod_top.ravno(),data->shet.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_upldok2a_rasp(class l_upldok2a_data *data)
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

sprintf(strsql,"upldok2a%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Списание топлива по счетам"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Списание топлива по счетам"),ff,data->window);
fprintf(ff,"%s:%s %s:%s %s:%d\n",gettext("Номер документа"),data->nom_dok.ravno(),gettext("Дата документа"),data->data_dok.ravno(),gettext("Код подразделения"),data->kod_podr.ravno_atoi());
fprintf(ff,"%s:%d %s %s\n",gettext("Автомобиль"),data->kod_avt,data->nom_znak.ravno(),data->naim_avt.ravno());


fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 Счёт |Код топлива|Затраты|Затраты|           Коментарий         |Дата и время запис.| Кто записал\n\
      |           |по нор.|по фак.|                              |                   |            \n"));
/*
123456 12345678901 1234567 123456789012345678901234567890
*/

fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");

cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {



  fprintf(ff,"%-*s %-*s %7.7g %7.7g %-*.*s %s %s\n",
  iceb_u_kolbait(6,row[1]),
  row[1],
  iceb_u_kolbait(11,row[0]),row[0],
  atof(row[3]),
  atof(row[2]),
  iceb_u_kolbait(30,row[4]),
  iceb_u_kolbait(30,row[4]),
  row[4],
  iceb_u_vremzap(row[6]),
  iceb_kszap(row[5],data->window));


 }
fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
/*****************************************************/
/*Создание списка списанного в путевом листе топлива*/
/*****************************************************/
void l_upldok2a_spst(const char *datd,int kp,const char *nomd,class iceb_u_spisok *KST,class iceb_u_double *KOL,GtkWidget *wpredok)
{
char	strsql[512];
int	kolstr=0;
SQL_str row;
int nomer=0;

sprintf(strsql,"select kodtp,kolih from Upldok1 where datd='%s' and kp=%d and nomd='%s' and tz=2",datd,kp,nomd);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  if((nomer=KST->find(row[0])) < 0)
     KST->plus(row[0]);
  KOL->plus(atof(row[1]),nomer);
  
 }


}
/*************************************/
/*Проверка полноты списания топлива  */
/*************************************/

void l_upldok2a_provpst(const char *datd,int kp,const char *nomd,GtkWidget *wpredok)
{
char	strsql[512];
int	kolstr=0;
SQL_str row;
int nomer=0;
int kolmt=0;
int metkasp=1; //0-не полнстью списано 1- полностью

//printw("\nprovpst-%d.%d.%d %s %s\n",dd,md,gd,nomd,podr);

class iceb_u_spisok KST; //Коды списанного в путевом листе топлива class
iceb_u_double KOL; //Количество списанного топлива

l_upldok2a_spst(datd,kp,nomd,&KST,&KOL,wpredok);

kolmt=KST.kolih();
if(kolmt > 0)
 {
  SQLCURSOR cur;
  for(int ii=0; ii < kolmt; ii++)
   {
    double kolsp=0.;
    
    sprintf(strsql,"select zt from Upldok2a where datd='%s' and kp=%d and nomd='%s' and kt='%s'",
    datd,kp,nomd,KST.ravno(ii));
    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
      continue;
     }

    while(cur.read_cursor(&row) != 0)
      kolsp+=atof(row[0]);
   class iceb_u_str strmenu("");
   int metkanr=0;

   if(fabs(kolsp - KOL.ravno(ii)) > 0.0009)
    {
      metkanr++;
      metkasp=0;
      if(metkanr == 1)
        {
         sprintf(strsql,"%s",gettext("Не полностью списано топливо по объектам списания !"));
         strmenu.new_plus(strsql);
        }             

       sprintf(strsql,"%s %.3f != %.3f",
       KST.ravno(nomer),
       kolsp,
       KOL.ravno(ii));
       
       strmenu.ps_plus(strsql);
              
       iceb_menu_soob(&strmenu,wpredok);
     }
   }
 }
else
  metkasp=0;

sprintf(strsql,"update Upldok set mt=%d where  datd='%s' and kp=%d and nomd='%s'",
metkasp,datd,kp,nomd);
iceb_sql_zapis(strsql,1,1,wpredok);

}
