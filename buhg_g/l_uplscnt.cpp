/*$Id: l_uplscnt.c,v 1.17 2013/08/25 08:26:42 sasa Exp $*/
/*08.05.2015	10.03.2008	Белых А.И.	l_uplscnt.c
Работа со списком средневзвешенных цен на топливо
*/
#include        <errno.h>
#include  "buhg_g.h"
#include  "l_uplscnt.h"

enum
{
  COL_DATA,
  COL_KOD_TOP,
  COL_CENA,
  COL_KOEF,
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

class  l_uplscnt_data
 {
  public:

  l_uplscnt_rek poisk;

  iceb_u_str datav;
  iceb_u_str kod_topv;
  iceb_u_str data_tv; //только что введённая 
  class iceb_u_str kod_top_tv;
  
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
  //Конструктор
  l_uplscnt_data()
   {
    snanomer=0;
    metka_voz=kl_shift=metka_poi=0;
    window=treeview=NULL;
    data_tv.plus("");
    kod_top_tv.plus("");
   }      
 };

gboolean   l_uplscnt_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplscnt_data *data);
void l_uplscnt_vibor(GtkTreeSelection *selection,class l_uplscnt_data *data);
void l_uplscnt_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uplscnt_data *data);
void  l_uplscnt_knopka(GtkWidget *widget,class l_uplscnt_data *data);
void l_uplscnt_add_columns (GtkTreeView *treeview);
void l_uplscnt_udzap(class l_uplscnt_data *data);
int  l_uplscnt_prov_row(SQL_str row,class l_uplscnt_data *data);
void l_uplscnt_rasp(class l_uplscnt_data *data);
void l_uplscnt_create_list (class l_uplscnt_data *data);

int l_uplscnt_v(class iceb_u_str *data_k,class iceb_u_str *kod_top_k,GtkWidget *wpredok);
int l_uplscnt_p(class l_uplscnt_rek *rek_poi,GtkWidget *wpredok);


extern SQL_baza  bd;

void l_uplscnt(GtkWidget *wpredok)
{
l_uplscnt_data data;
char bros[512];

data.poisk.clear_data();

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Справочник средних цен на топливо"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uplscnt_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE,1);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Справочник средних цен на топливо"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE,1);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE,1);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);

//Кнопки

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE,1);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_uplscnt_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE,1);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_uplscnt_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE,1);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_uplscnt_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);


sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_uplscnt_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE,1);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE,1);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_uplscnt_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE,1);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_uplscnt_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_uplscnt_create_list(&data);

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
void l_uplscnt_create_list (class l_uplscnt_data *data)
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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_uplscnt_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_uplscnt_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select * from Uplmtsc order by datsc asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);


data->kolzap=0;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(l_uplscnt_prov_row(row,data) != 0)
    continue;


  if(iceb_u_sravmydat(data->data_tv.ravno(),row[0]) == 0 && iceb_u_SRAV(data->kod_topv.ravno(),row[1],0) == 0)
    data->snanomer=data->kolzap;

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATA,iceb_u_datzap(row[0]),
  COL_KOD_TOP,row[1],
  COL_CENA,row[2],
  COL_KOEF,row[5],
  COL_DATA_VREM,iceb_u_vremzap(row[4]),
  COL_KTO,iceb_kszap(row[3],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->data_tv.new_plus("");
data->kod_top_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_uplscnt_add_columns (GTK_TREE_VIEW (data->treeview));


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
zagolov.plus(gettext("Справочник средних цен на топливо"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.data.ravno(),gettext("Дата"));
  iceb_str_poisk(&zagolov,data->poisk.kod_top.ravno(),gettext("Код топлива"));
  

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

void l_uplscnt_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата"), renderer,"text", COL_DATA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код топлива"), renderer,"text", COL_KOD_TOP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Цена"), renderer,"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Коэффициент"), renderer,"text", COL_KOEF,NULL);

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

void l_uplscnt_vibor(GtkTreeSelection *selection,class l_uplscnt_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATA,&kod,COL_KOD_TOP,&naim,NUM_COLUMNS,&nomer,-1);

data->datav.new_plus(kod);
data->kod_topv.new_plus(naim);
data->snanomer=nomer;

g_free(kod);
g_free(naim);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_uplscnt_knopka(GtkWidget *widget,class l_uplscnt_data *data)
{
//char    bros[512];
iceb_u_str repl;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_uplscnt_knopka knop=%d\n",knop);
data->kl_shift=0;
switch (knop)
 {
  case FK2:
    if(l_uplscnt_v(&data->data_tv,&data->kod_top_tv,data->window) == 0)
      l_uplscnt_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;

    if(l_uplscnt_v(&data->datav,&data->kod_topv,data->window) == 0)
      l_uplscnt_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_uplscnt_udzap(data);
    l_uplscnt_create_list(data);
    return;  
  


  case FK4:
    if(l_uplscnt_p(&data->poisk,data->window) == 0)
     data->metka_poi=1;
    else
     data->metka_poi=0;
    l_uplscnt_create_list(data);
    return;  

  case FK5:
    l_uplscnt_rasp(data);
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

gboolean   l_uplscnt_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplscnt_data *data)
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
void l_uplscnt_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uplscnt_data *data)
{
g_signal_emit_by_name(data->knopka[SFK2],"clicked");


}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_uplscnt_udzap(class l_uplscnt_data *data)
{


char strsql[512];

sprintf(strsql,"delete from Uplmtsc where datsc='%s' and kodt='%s'",
data->datav.ravno_sqldata(),data->kod_topv.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_uplscnt_prov_row(SQL_str row,class l_uplscnt_data *data)
{
if(data->metka_poi == 0)
 return(0);

if(data->poisk.data.getdlinna() > 1)
 {
  short d1,m1,g1;
  short d2,m2,g2;

  iceb_u_rsdat(&d1,&m1,&g1,row[0],2);
  iceb_u_rsdat(&d2,&m2,&g2,data->poisk.data.ravno(),1);
  if(iceb_u_sravmydat(d1,m1,g1,d2,m2,g2) != 0)
   return(1);
 }

//Полное сравнение
if(iceb_u_proverka(data->poisk.kod_top.ravno(),row[1],0,0) != 0)
 return(1);


   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_uplscnt_rasp(class l_uplscnt_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Uplmtsc order by datsc asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"scnt%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Справочник средних цен на топливо"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Справочник средних цен на топливо"),0,0,0,0,0,0,ff,data->window);

if(data->metka_poi == 1)
 {
  if(data->poisk.data.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Дата"),data->poisk.data.ravno());
  if(data->poisk.kod_top.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код топлива"),data->poisk.kod_top.ravno());
 }

fprintf(ff,"\
-------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
   Дата   |Код топлива|  Цена    |Коэффициент|Дата и время запис.| Кто записал\n"));
/*
1234567890 12345678901 1234567890 12345678901
*/
fprintf(ff,"\
-------------------------------------------------------------------------------------\n");

short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
  if(l_uplscnt_prov_row(row,data) != 0)
    continue;
  
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  
  fprintf(ff,"%02d.%02d.%04d %-*s %-11s %10s %s %s\n",
  d,m,g,
  iceb_u_kolbait(11,row[1]),row[1],
  row[2],row[5],iceb_u_vremzap(row[4]),iceb_kszap(row[3],data->window));

 }
fprintf(ff,"\
-------------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
