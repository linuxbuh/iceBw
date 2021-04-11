/*$Id: l_upldok3.c,v 1.5 2013/10/04 07:27:16 sasa Exp $*/
/*08.05.2015	23.04.2012	Белых А.И.	l_upldok3.c
Работа со списком видов норм списания топлива для конкретного путевого листа
*/
#include        <errno.h>
#include  "buhg_g.h"
#include  "l_upldok3.h"

enum
{
  COL_KODNST,
  COL_NAIM,
  COL_NST,
  COL_EI,
  COL_KOLIH,
  COL_ZTPN,
  COL_ZTPF,
  COL_KOMENT,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  SFK1,
  FK2,
  FK3,
  SFK3,
  FK4,
  FK5,
  FK6,
  FK10,
  SFK2,
  KOL_F_KL

};

class  l_upldok3_data
 {
  public:

  l_upldok3_rek poisk;

  iceb_u_str kod_nst;
  iceb_u_str naimv;
  iceb_u_str kod_nst_tv; //только что введённый код
  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
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
  double probeg;
  
  //Конструктор
  l_upldok3_data()
   {
    snanomer=0;
    metka_voz=kl_shift=metka_poi=0;
    window=treeview=NULL;
    kod_nst_tv.plus("");
    kod_avt=0;
    nom_znak.plus("");
    naim_avt.plus("");
    probeg=0.; 
   }      
 };

gboolean   l_upldok3_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok3_data *data);
void l_upldok3_vibor(GtkTreeSelection *selection,class l_upldok3_data *data);
void l_upldok3_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_upldok3_data *data);
void  l_upldok3_knopka(GtkWidget *widget,class l_upldok3_data *data);
void l_upldok3_add_columns (GtkTreeView *treeview);
void l_upldok3_udzap(class l_upldok3_data *data);
int  l_upldok3_prov_row(SQL_str row,class l_upldok3_data *data);
void l_upldok3_rasp(class l_upldok3_data *data);
int l_upldok3_create_list (class l_upldok3_data *data);

int l_upldok_v1(class iceb_u_str *data_dok,class iceb_u_str *nom_dok,class iceb_u_str *kod_podr,GtkWidget *wpredok);
int l_upldok3_v(const char *data_dok,const char *nom_dok,int kod_podr,class iceb_u_str *kod_nst,GtkWidget *wpredok);
int l_upldok3_p(class l_upldok3_rek *rek_poi,GtkWidget *wpredok);
int l_upldok_ud(class  l_upldok3_data *data);
int l_upldok_1(const char *data_dok,const char *nomd,const char *kod_pod,GtkWidget *wpredok);
void l_upldok3_rhd(class  l_upldok3_data *data);


extern SQL_baza  bd;

int l_upldok3(const char *data_dok,
const char *nom_dok,
int kod_podr,
GtkWidget *wpredok)
{
l_upldok3_data data;
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

data.poisk.clear_data();
data.data_dok.new_plus(data_dok);
data.nom_dok.new_plus(nom_dok);
data.kod_podr.new_plus(kod_podr);


l_upldok3_rhd(&data);



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


sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт списания топлива к путевому листу"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_upldok3_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Расчёт списания топлива к путевому листу"));


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

sprintf(strsql,"%sF1 %s",RFK,gettext("Заголовок"));
data.knopka[SFK1]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK1], TRUE, TRUE, 0);
g_signal_connect(data.knopka[SFK1], "clicked",G_CALLBACK(l_upldok3_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK1],gettext("Переход в режим работы с заголовком документа"));
gtk_widget_set_name(data.knopka[SFK1],iceb_u_inttochar(SFK1));
gtk_widget_show(data.knopka[SFK1]);

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_upldok3_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_upldok3_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_upldok3_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(l_upldok3_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удаление документа"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);


sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_upldok3_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_upldok3_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F6 %s",gettext("Топливо"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(l_upldok3_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Списание топлива"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_upldok3_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

if(l_upldok3_create_list(&data) != 0)
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
int l_upldok3_create_list (class l_upldok3_data *data)
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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_upldok3_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_upldok3_vibor),data);

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
G_TYPE_INT);


sprintf(strsql,"select * from Upldok3 where datd='%s' and kp=%d and nomd='%s'",data->data_dok.ravno_sqldata(),data->kod_podr.ravno_atoi(),data->nom_dok.ravno());
data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  gdite.close();
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
//gtk_list_store_clear(model);

class iceb_u_str naim_nst("");
data->kolzap=0;
float kolstr1=0. ;
double i_ztpf=0.;
double i_ztpn=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_upldok3_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->kod_nst_tv.ravno(),row[3],0) == 0)
    data->snanomer=data->kolzap;

  naim_nst.new_plus("");
  sprintf(strsql,"select naik from Uplnst where kod='%s'",row[3]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_nst.new_plus(row1[0]);

  i_ztpf+=atof(row[7]);  
  i_ztpn+=atof(row[6]);
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KODNST,row[3],
  COL_NAIM,naim_nst.ravno(),
  COL_NST,row[5],
  COL_EI,row[8],
  COL_KOLIH,row[4],
  COL_ZTPN,row[6],
  COL_ZTPF,row[7],
  COL_KOMENT,row[9],
  COL_DATA_VREM,iceb_u_vremzap(row[11]),
  COL_KTO,iceb_kszap(row[10],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kod_nst_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_upldok3_add_columns (GTK_TREE_VIEW (data->treeview));


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
zagolov.plus(gettext("Расчёт списания топлива к путевому листу"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql,"%s:%s %s:%s",gettext("Дата"),data->data_dok.ravno(),gettext("Номер документа"),data->nom_dok.ravno());
zagolov.ps_plus(strsql);

sprintf(strsql,"%s:%d %s %s",gettext("Автомобиль"),data->kod_avt,data->nom_znak.ravno(),data->naim_avt.ravno());
zagolov.ps_plus(strsql);
sprintf(strsql,"%s:%.6g %s:%.3f %s:%.3f %s:%.3f",gettext("Пробег"),data->probeg,gettext("Затраты по норме"),i_ztpn,gettext("Затраты по факту"),i_ztpf,gettext("Разница"),i_ztpf-i_ztpn);
zagolov.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kod.ravno(),gettext("Код"));
  iceb_str_poisk(&zagolov,data->poisk.naim.ravno(),gettext("Наименование"));
  

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

void l_upldok3_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код"),renderer,"text",COL_KODNST,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KODNST);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Наименование"),renderer,"text",COL_NAIM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAIM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Норма списания"),renderer,"text",COL_NST,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NST);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Един.измер."),renderer,"text",COL_EI,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_EI);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Количество"),renderer,"text",COL_KOLIH,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOLIH);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Затраты по норме"),renderer,"text",COL_ZTPN,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ZTPN);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Затраты по факту"),renderer,"text",COL_ZTPF,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ZTPF);
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

void l_upldok3_vibor(GtkTreeSelection *selection,class l_upldok3_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_KODNST,&kod,COL_NAIM,&naim,NUM_COLUMNS,&nomer,-1);

data->kod_nst.new_plus(kod);
data->naimv.new_plus(naim);
data->snanomer=nomer;

g_free(kod);
g_free(naim);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_upldok3_knopka(GtkWidget *widget,class l_upldok3_data *data)
{
class iceb_u_str kod("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_upldok3_knopka knop=%d\n",knop);

switch (knop)
 {
  case SFK1:

    if(l_upldok_v1(&data->data_dok,&data->kod_podr,&data->nom_dok,data->window) == 0)
     {
      /*Читаем реквизиты документа*/
      l_upldok3_rhd(data);
      l_upldok3_create_list(data);
     }
    return;  
  case FK2:
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;
  
    if(l_upldok3_v(data->data_dok.ravno(),data->nom_dok.ravno(),data->kod_podr.ravno_atoi(),&data->kod_nst_tv,data->window) == 0)
      l_upldok3_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(l_upldok3_v(data->data_dok.ravno(),data->nom_dok.ravno(),data->kod_podr.ravno_atoi(),&data->kod_nst,data->window) == 0)
      l_upldok3_create_list(data);
    return;  

  case FK3:
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_upldok3_udzap(data);
    l_upldok3_create_list(data);
    return;  

  case SFK3:
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;
    if(l_upldok_ud(data) != 0)
     return;
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;  
  

  case FK4:
    if(l_upldok3_p(&data->poisk,data->window) == 0)
     data->metka_poi=1;
    else
     data->metka_poi=0;
    l_upldok3_create_list(data);
    return;  

  case FK5:
    l_upldok3_rasp(data);
    return;  

  case FK6:
    kod.new_plus(data->kod_podr.ravno_atoi());
    l_upldok_1(data->data_dok.ravno(),data->nom_dok.ravno(),kod.ravno(),data->window);
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

gboolean   l_upldok3_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok3_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {

  case GDK_KEY_F1:
/********
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK1],"clicked");
    else
*************/
      g_signal_emit_by_name(data->knopka[SFK1],"clicked");
    data->kl_shift=0; /*обязательно сбрасываем*/
    return(TRUE);

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

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);

  case GDK_KEY_F6:
    g_signal_emit_by_name(data->knopka[FK6],"clicked");
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
void l_upldok3_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_upldok3_data *data)
{

  g_signal_emit_by_name(data->knopka[SFK2],"clicked");


}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_upldok3_udzap(class l_upldok3_data *data)
{


char strsql[512];

sprintf(strsql,"delete from Upldok3 where datd='%s' and kp=%d and nomd='%s' and knst='%s'",data->data_dok.ravno_sqldata(),data->kod_podr.ravno_atoi(),data->nom_dok.ravno(),data->kod_nst.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_upldok3_prov_row(SQL_str row,class l_upldok3_data *data)
{
if(data->metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.kod.ravno(),row[3],0,0) != 0)
 return(1);

if(data->poisk.naim.getdlinna() > 1)
 {
  SQL_str row1;
  class SQLCURSOR cur;
  class iceb_u_str naim_nst("");
  char strsql[1024];
  sprintf(strsql,"select naik from Uplnst where kod='%s'",row[3]);
  if(iceb_sql_readkey(strsql,&row1,&cur,data->window) == 1)
   naim_nst.new_plus(row1[0]);

  if(iceb_u_proverka(data->poisk.naim.ravno(),naim_nst.ravno(),4,0) != 0)
   return(1);
 }
   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_upldok3_rasp(class l_upldok3_data *data)
{
char strsql[512];
SQL_str row,row1;
class SQLCURSOR cur1;

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

sprintf(strsql,"upldok3%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Расчёт списания топлива к путевому листу"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Расчёт списания топлива к путевому листу"),ff,data->window);
fprintf(ff,"%s:%s %s:%s %s:%d\n",gettext("Номер документа"),data->nom_dok.ravno(),gettext("Дата документа"),data->data_dok.ravno(),gettext("Код подразделения"),data->kod_podr.ravno_atoi());
fprintf(ff,"%s:%d %s %s\n",gettext("Автомобиль"),data->kod_avt,data->nom_znak.ravno(),data->naim_avt.ravno());

if(data->metka_poi == 1)
 {
  if(data->poisk.kod.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код"),data->poisk.kod.ravno());
  if(data->poisk.naim.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Наименование"),data->poisk.naim.ravno());
 }

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 Код  |          Наименование        |Един.измер.|Норма списания|Затр.по норме|Затр.по факту|Дата и время запис.| Кто записал\n"));
/*
123456 123456789012345678901234567890 12345678901 12345678901234 1234567890123 1234567890123
*/

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
class iceb_u_str naim_nst("");

cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(l_upldok3_prov_row(row,data) != 0)
    continue;

  naim_nst.new_plus("");
  sprintf(strsql,"select naik from Uplnst where kod='%s'",row[3]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_nst.new_plus(row1[0]);


  fprintf(ff,"%-*s %-*.*s %-*s %-14.14g %-13.13g %-13.13g %s %s\n",
  iceb_u_kolbait(6,row[3]),
  row[3],
  iceb_u_kolbait(30,naim_nst.ravno()),iceb_u_kolbait(30,naim_nst.ravno()),naim_nst.ravno(),
  iceb_u_kolbait(11,row[8]),
  row[8],
  atof(row[5]),
  atof(row[6]),
  atof(row[7]),
  iceb_u_vremzap(row[11]),
  iceb_kszap(row[10],data->window));

  for(int ii=30; ii < iceb_u_strlen(naim_nst.ravno()); ii+=30)
   fprintf(ff,"%6s %-*.*s\n",
   "",
   iceb_u_kolbait(30,iceb_u_adrsimv(ii,naim_nst.ravno())),
   iceb_u_kolbait(30,iceb_u_adrsimv(ii,naim_nst.ravno())),
   iceb_u_adrsimv(ii,naim_nst.ravno()));
  

 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
/***************************/
/*Удаление всего документа*/
/**************************/
int l_upldok_ud(class  l_upldok3_data *data)
{


if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) == 2)
  return(1);    

char strsql[512];

sprintf(strsql,"delete from Upldok2a where datd='%s' and kp=%d and nomd='%s'",
data->data_dok.ravno_sqldata(),data->kod_podr.ravno_atoi(),data->nom_dok.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return(1);

sprintf(strsql,"delete from Upldok1 where datd='%s' and kp=%d and nomd='%s'",
data->data_dok.ravno_sqldata(),data->kod_podr.ravno_atoi(),data->nom_dok.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return(1);

sprintf(strsql,"delete from Upldok3 where datd='%s' and kp=%d and nomd='%s'",
data->data_dok.ravno_sqldata(),data->kod_podr.ravno_atoi(),data->nom_dok.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return(1);

sprintf(strsql,"delete from Upldok where god=%d and kp=%d and nomd='%s'",
data->data_dok.ravno_god(),data->kod_podr.ravno_atoi(),data->nom_dok.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return(1);

 

return(0);
}
/**************************/
/*Чтение шапки документа*/
/************************/
void l_upldok3_rhd(class  l_upldok3_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select ka,psv,psz from Upldok where god=%d and kp=%d and nomd='%s'",data->data_dok.ravno_god(),data->kod_podr.ravno_atoi(),data->nom_dok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  data->kod_avt=atoi(row[0]);
  data->probeg=atof(row[2]) - atof(row[1]);
 } 
}