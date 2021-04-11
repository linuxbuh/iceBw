/*$Id: iceb_l_opmup.c,v 1.46 2013/12/31 11:49:18 sasa Exp $*/
/*30.11.2016	05.05.2004	Белых А.И.	iceb_l_opmup.c
Работа со списком операций прихода для материольного учёта
*/
#include        <errno.h>
#include  "iceb_libbuh.h"
#include  "iceb_l_opmup.h"
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

enum
{
  COL_KOD,
  COL_NAIM,
  COL_VIDOP,
  COL_PROV,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

class  opmup_data
 {
  public:

  class opmup_rek poisk;

  class iceb_u_str kodv;
  class iceb_u_str naimv;
  class iceb_u_str kod_tv; //код только что введённый  

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
  short     metka_rr;   //0-работа со списком 1-выбор
  //Конструктор
  opmup_data()
   {
    snanomer=0;
    metka_rr=metka_voz=kl_shift=metka_poi=0;
    window=treeview=NULL;
   }      
 };

gboolean   opmup_key_press(GtkWidget *widget,GdkEventKey *event,class opmup_data *data);
void opmup_vibor(GtkTreeSelection *selection,class opmup_data *data);
void opmup_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class opmup_data *data);
void  opmup_knopka(GtkWidget *widget,class opmup_data *data);
void opmup_add_columns (GtkTreeView *treeview);
int iceb_l_opmup_v(class iceb_u_str *data,GtkWidget*);
int iceb_l_opmup_p(class opmup_rek *data,GtkWidget*);
void opmup_udzap(class opmup_data *data);
int  opmup_prov_row(SQL_str row,class opmup_data *data);
void opmup_rasp(class opmup_data *data);
int  opmup_pvu(int,const char *kod,GtkWidget *wpredok);
void opmup_create_list (class opmup_data *data);

extern SQL_baza bd;

int   iceb_l_opmup(int metka_rr, //0-ввод и корек. 1-выбор
iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok)
{
opmup_data data;
char bros[512];
int gor=0;
int ver=0;

data.poisk.clear_zero();
data.metka_rr=metka_rr;
data.name_window.plus(__FUNCTION__);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_rr == 1)
 {
  if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
 }


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список операций приходов"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(opmup_key_press),&data);
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

GtkWidget *label;
if(metka_rr == 1)
 {
  label=gtk_label_new (gettext("Выберите нужную запись"));
  iceb_label_set_text_color(label,gettext("Выберите нужную запись"),"#F90101");
  gtk_box_pack_start (GTK_BOX (vbox2),label,FALSE, FALSE, 0);
  gtk_widget_show(label);
 }

data.label_kolstr=gtk_label_new (gettext("Список операций приходов"));

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
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(opmup_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[SFK2]),FALSE);//Недоступна
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(opmup_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной запси"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK3]),FALSE);//Недоступна
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(opmup_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной запси"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(opmup_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(opmup_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(opmup_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

opmup_create_list(&data);

gtk_widget_show(data.window);
if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();

if(data.metka_voz == 0)
 {
  kod->new_plus(data.kodv.ravno());
  naim->new_plus(data.naimv.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void opmup_create_list (class opmup_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
//GdkColor color;

//printf("opmup_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(opmup_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(opmup_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select * from Prihod order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(opmup_prov_row(row,data) != 0)
    continue;

  if(iceb_u_SRAV(data->kod_tv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Вид операции
  ss[COL_VIDOP].new_plus(gettext("Внешняя"));
  if(row[4][0] == '1')
    ss[COL_VIDOP].new_plus(gettext("Внутренняя"));
  if(row[4][0] == '2')
    ss[COL_VIDOP].new_plus(gettext("Изменение учётных данных"));
    
  //Проводки
  ss[COL_PROV].new_plus(gettext("Нужно делать"));
  if(atoi(row[5]) == 1)
    ss[COL_PROV].new_plus(gettext("Не нужно делать"));


  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,row[0],
  COL_NAIM,row[1],
  COL_VIDOP,ss[COL_VIDOP].ravno(),
  COL_PROV,ss[COL_PROV].ravno(),
  COL_DATA_VREM,iceb_u_vremzap(row[3]),
  COL_KTO,iceb_kszap(row[2],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kod_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

opmup_add_columns (GTK_TREE_VIEW (data->treeview));


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

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список операций приходов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

printf("data->metka_poi=%d\n",data->metka_poi);
if(data->metka_poi == 1)
 {
  //printf("Формирование заголовка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");
  
  if(data->poisk.kod.getdlinna() > 1)  
   {
    stroka.new_plus(gettext("Код"));
    stroka.plus(": ");
    stroka.plus(data->poisk.kod.ravno());
    zagolov.ps_plus(stroka.ravno());

   }

  if(data->poisk.naim.getdlinna() > 1)  
   {
    stroka.new_plus(gettext("Наименование"));
    stroka.plus(": ");
    stroka.plus(data->poisk.naim.ravno());
    zagolov.ps_plus(stroka.ravno());
   }

  iceb_label_set_text_color(data->label_poisk,zagolov.ravno(),"#F90101");
//  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void opmup_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Вид операции"), renderer,"text", COL_VIDOP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Проводки"), renderer,"text", COL_PROV,NULL);

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

void opmup_vibor(GtkTreeSelection *selection,class opmup_data *data)
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

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  opmup_knopka(GtkWidget *widget,class opmup_data *data)
{
//iceb_u_str repl;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_l_opmup_v(&data->kod_tv,data->window);
    opmup_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    iceb_l_opmup_v(&data->kodv,data->window);
    opmup_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    opmup_udzap(data);
    opmup_create_list(data);
    return;  


  case FK4:
    if(iceb_l_opmup_p(&data->poisk,data->window) == 0)
     data->metka_poi=1;
    else 
     data->metka_poi=0;
    opmup_create_list(data);
     
    return;  

  case FK5:
    opmup_rasp(data);
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

gboolean   opmup_key_press(GtkWidget *widget,GdkEventKey *event,class opmup_data *data)
{

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
    printf("opmup_key_press-Нажата клавиша Shift\n");

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
void opmup_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class opmup_data *data)
{

if(data->metka_rr == 0)
 {
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");
  return;
 }

if(data->metka_rr == 1)
 iceb_sizww(data->name_window.ravno(),data->window);

gtk_widget_destroy(data->window);

data->metka_voz=0;

}

/*****************************/
/*Удаление записи            */
/*****************************/

void opmup_udzap(class opmup_data *data)
{

iceb_u_str sp;
sp.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&sp,2,data->window) == 2)
 return;

if(opmup_pvu(1,data->kodv.ravno(),data->window) != 0)
 return;

char strsql[512];

sprintf(strsql,"delete from Prihod where kod='%s'",
data->kodv.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/****************************/
/*Проверка записей          */
/*****************************/

int  opmup_prov_row(SQL_str row,class opmup_data *data)
{
if(data->metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.kod.ravno(),row[0],0,0) != 0)
 return(1);


//Поиск образца в строке
if(data->poisk.naim.ravno()[0] != '\0' && iceb_u_strstrm(row[1],data->poisk.naim.ravno()) == 0)
 return(1);


   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void opmup_rasp(class opmup_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Prihod order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"moppt%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список операций приходов"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Список операций приходов"),0,0,0,0,0,0,ff,data->window);
fprintf(ff,"%s:%s\n",gettext("Подсистема"),gettext("Материальный учёт"));
fprintf(ff,"\
----------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Код |          Наименование        |Вид операции|Проводки|Дата и время запис.| Кто записал\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------\n");

iceb_u_str s1;
iceb_u_str s2;


cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(opmup_prov_row(row,data) != 0)
    continue;

  s1.new_plus(gettext("Внешняя"));
  if(row[4][0] == '1')
    s1.new_plus(gettext("Внутренняя"));
  if(row[4][0] == '2')
    s1.new_plus(gettext("Изменение учётных данных"));
    
  //Проводки
  s2.new_plus(gettext("Нужно делать"));
  if(row[5][0] == '1')
    s2.new_plus(gettext("Неужно делать"));

  fprintf(ff,"%-*s %-*.*s %-*.*s %-*.*s %s %s\n",
  iceb_u_kolbait(4,row[0]),row[0],
  iceb_u_kolbait(30,row[1]),iceb_u_kolbait(30,row[1]),row[1],
  iceb_u_kolbait(12,s1.ravno()),iceb_u_kolbait(12,s1.ravno()),s1.ravno(),
  iceb_u_kolbait(8,s2.ravno()),iceb_u_kolbait(8,s2.ravno()),s2.ravno(),
  iceb_u_vremzap(row[3]),iceb_kszap(row[2],data->window));

  if(iceb_u_strlen(row[1]) > 30)
   fprintf(ff,"%4s %s\n","",iceb_u_adrsimv(30,row[1]));
 }
fprintf(ff,"\
----------------------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
/*****************************/
/*Проверить возможность удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int  opmup_pvu(int met,const char *kod,GtkWidget *wpredok)
{
char strsql[1024];
SQLCURSOR cur;

sprintf(strsql,"select kodop from Dokummat where kodop='%s' and tip=1 limit 1",kod);

if(iceb_sql_readkey(strsql,wpredok) != 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Dokummat",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select ko from Reenn1 where ko='%s' and mi=1 limit 1",kod);

if(iceb_sql_readkey(strsql,wpredok) != 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Reenn",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    


return(0);

}
