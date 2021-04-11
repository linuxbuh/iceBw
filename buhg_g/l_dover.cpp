/*$Id: l_dover.c,v 1.15 2013/12/31 11:49:12 sasa Exp $*/
/*11.07.2015	21.04.2009	Белых А.И.	l_dover.c
Работа с записями в доверенности
*/
#include  "buhg_g.h"

enum
{
  COL_NZ, /*НЕВИДИМЫЕ ДАННЫЕ*/
  COL_ZAPIS,
  COL_EI,
  COL_KOLIH,
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
  FK5,
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_dover_data
 {
  public:


  
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  

  class iceb_u_str datdov;
  class iceb_u_str nomdov;
  int nz_vibr;
  
  //Конструктор
  l_dover_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
   }      
 };

gboolean   l_dover_key_press(GtkWidget *widget,GdkEventKey *event,class l_dover_data *data);
void l_dover_vibor(GtkTreeSelection *selection,class l_dover_data *data);
void l_dover_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_dover_data *data);
void  l_dover_knopka(GtkWidget *widget,class l_dover_data *data);
void l_dover_add_columns (GtkTreeView *treeview);
void l_dover_udzap(class l_dover_data *data);
void l_dover_create_list (class l_dover_data *data);

int l_dover_v(const char *datadov,const char *nomdov,int nomerz,GtkWidget *wpredok);
int xudrasdokw(class iceb_u_str *datdov,class iceb_u_str *nomdov,class iceb_u_spisok *imafo,class iceb_u_spisok *naimf,GtkWidget *wpredok);


extern SQL_baza  bd;

int l_dover(const char *datadov,const char *nomerdov,GtkWidget *wpredok)
{
l_dover_data data;
char bros[512];

data.datdov.new_plus(datadov);
data.nomdov.new_plus(nomerdov);
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Доверенность"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_dover_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Доверенность"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки

sprintf(bros,"%sF1 %s",RFK,gettext("Шапка"));
data.knopka[SFK1]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK1], TRUE, TRUE, 0);
g_signal_connect(data.knopka[SFK1], "clicked",G_CALLBACK(l_dover_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK1],gettext("Просмотр шапки документа"));
gtk_widget_set_name(data.knopka[SFK1],iceb_u_inttochar(SFK1));
gtk_widget_show(data.knopka[SFK1]);

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_dover_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_dover_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_dover_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить документ"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(l_dover_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удалить документ"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);


sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_dover_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_dover_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

l_dover_create_list(&data);

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
void l_dover_create_list (class l_dover_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
//GdkColor color;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_dover_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_dover_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_INT,
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select * from Uddok1 where datd='%s' and nomd='%s'",data->datdov.ravno_sqldata(),data->nomdov.ravno());

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
  

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_NZ,row[2],
  COL_ZAPIS,row[3],
  COL_EI,row[4],
  COL_KOLIH,row[5],
  COL_DATA_VREM,iceb_u_vremzap(row[7]),
  COL_KTO,iceb_kszap(row[6],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_dover_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Доверенность"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
zagolov.ps_plus(gettext("Дата"));
zagolov.plus(":");
zagolov.plus(data->datdov.ravno());
zagolov.plus(" ");

zagolov.ps_plus(gettext("Номер"));
zagolov.plus(":");
zagolov.plus(data->nomdov.ravno());

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());


gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_dover_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Наименование"),renderer,"text",COL_ZAPIS,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ZAPIS);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Ед.изм."),renderer,"text",COL_EI,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_EI);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Количество"),renderer,"text",COL_KOLIH,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOLIH);
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

void l_dover_vibor(GtkTreeSelection *selection,class l_dover_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gint nz_vibr;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_NZ,&nz_vibr,NUM_COLUMNS,&nomer,-1);

data->snanomer=nomer;
data->nz_vibr=nz_vibr;


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_dover_knopka(GtkWidget *widget,class l_dover_data *data)
{
char strsql[512];
class iceb_u_spisok imafo;
class iceb_u_spisok naimf;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_dover_knopka knop=%d\n",knop);
data->kl_shift=0; //0-отжата 1-нажата  

switch (knop)
 {
  case SFK1:
    if(udvhdw(&data->datdov,&data->nomdov,data->window) == 0)
        l_dover_create_list(data);
    return;  

  case FK2:
    if(l_dover_v(data->datdov.ravno(),data->nomdov.ravno(),0,data->window) == 0)
      l_dover_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(l_dover_v(data->datdov.ravno(),data->nomdov.ravno(),data->nz_vibr,data->window) == 0)
      l_dover_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_pbpds(data->datdov.ravno(),data->window) != 0)
      return;     
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_dover_udzap(data);
    l_dover_create_list(data);
    return;  
  
  case SFK3:
    if(iceb_pbpds(data->datdov.ravno(),data->window) != 0)
      return;     

    if(iceb_menu_danet(gettext("Удалить документ? Вы уверены ?"),2,data->window) == 2)
     return;

    sprintf(strsql,"delete from Uddok1 where datd='%s' and nomd='%s'",
    data->datdov.ravno_sqldata(),data->nomdov.ravno());

    if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
     return;

    sprintf(strsql,"delete from Uddok where datd='%s' and nomd='%s'",
    data->datdov.ravno_sqldata(),data->nomdov.ravno());

    iceb_sql_zapis(strsql,0,0,data->window);

    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;  

  case FK5:
    if(xudrasdokw(&data->datdov,&data->nomdov,&imafo,&naimf,data->window) == 0)
       iceb_rabfil(&imafo,&naimf,data->window);
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

gboolean   l_dover_key_press(GtkWidget *widget,GdkEventKey *event,class l_dover_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {
  case GDK_KEY_F1:
    if(data->kl_shift == 1) //Нажата
      g_signal_emit_by_name(data->knopka[SFK1],"clicked");
//    else
//      g_signal_emit_by_name(data->knopka[FK1],"clicked");
    return(TRUE);

  case GDK_KEY_F2:

    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    else
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");

    return(TRUE);
   
  case GDK_KEY_F3:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK3],"clicked");
    else
      g_signal_emit_by_name(data->knopka[SFK3],"clicked");

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
void l_dover_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_dover_data *data)
{
//printf("l_dover_v_row\n");
//printf("l_dover_v_row корректировка\n");
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");


}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_dover_udzap(class l_dover_data *data)
{


char strsql[512];

sprintf(strsql,"delete from Uddok1 where datd='%s' and nomd='%s' and nz=%d",
data->datdov.ravno_sqldata(),data->nomdov.ravno(),data->nz_vibr);

iceb_sql_zapis(strsql,0,0,data->window);



}
