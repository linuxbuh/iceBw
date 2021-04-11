/*$Id: admin_alx.c,v 1.14 2014/04/30 06:14:58 sasa Exp $*/
/*10.07.2015	31.10.2010	Белых А.И.	admin_alx.c
Работа со списком файлов настройки
*/
#include        <errno.h>
#include  "buhg_g.h"

enum
{
  COL_NAIMFIL,
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
  FK6,
  FK10,
  KOL_F_KL
};

class  admin_alx_data
 {
  public:

  class iceb_u_str naimfil;
  class iceb_u_str naimfilpoi;  
  class iceb_u_str imabazv;
  class iceb_u_str editor;
  
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
  admin_alx_data()
   {
    snanomer=0;
    metka_voz=kl_shift=metka_poi=0;
    window=treeview=NULL;
    naimfil.plus("");
    naimfilpoi.plus("");
    editor.new_plus("");
   }      
 };

gboolean   admin_alx_key_press(GtkWidget *widget,GdkEventKey *event,class admin_alx_data *data);
void admin_alx_vibor(GtkTreeSelection *selection,class admin_alx_data *data);
void admin_alx_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class admin_alx_data *data);
void  admin_alx_knopka(GtkWidget *widget,class admin_alx_data *data);
void admin_alx_add_columns (GtkTreeView *treeview);
void admin_alx_udzap(class admin_alx_data *data);
int  admin_alx_prov_row(SQL_str row,class admin_alx_data *data);
void admin_alx_create_list (class admin_alx_data *data);

int admin_alx_poi(class iceb_u_str *naimfilpoi,GtkWidget *wpredok);
void admin_alx_rs(class admin_alx_data *data);



extern SQL_baza  bd;

int   admin_alx(const char *imabazv,GtkWidget *wpredok)
{
admin_alx_data data;
char bros[512];

sprintf(bros,"USE %s",imabazv);
iceb_sql_zapis(bros,1,0,wpredok);

/*проверяем есть ли в базе нужная талица*/
sprintf(bros,"select un from icebuser limit 1");
if(sql_readkey(&bd,"select un from icebuser limit 1") < 0)
 {
  iceb_menu_soob(gettext("Это не iceB база данных!"),wpredok);
  return(1);
 }


data.imabazv.plus(imabazv);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список файлов настройки"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(admin_alx_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Список файлов настройки"));


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

sprintf(bros,"F2 %s",gettext("Редактирование"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(admin_alx_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Редактирование файла настройки"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(admin_alx_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранного файла"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(admin_alx_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(admin_alx_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Редактор"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(admin_alx_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Ввод нового редактора"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(admin_alx_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

admin_alx_create_list(&data);

gtk_widget_show(data.window);
  gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void admin_alx_create_list (class admin_alx_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur,cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;
//GdkColor color;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(admin_alx_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(admin_alx_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select distinct fil from Alx order by fil asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);


data->kolzap=0;
float kolstr1=0. ;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s\n",row[0],row[1],row[2]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(admin_alx_prov_row(row,data) != 0)
    continue;

  /*Читаем время и кто записал*/
  sprintf(strsql,"select ktoz,vrem from Alx where fil='%s' order by ns asc limit 1",row[0]);
  iceb_sql_readkey(strsql,&row1,&cur1,data->window);    
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_NAIMFIL,row[0],
  COL_DATA_VREM,iceb_u_vremzap(row1[1]),
  COL_KTO,iceb_kszap(row1[0],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

admin_alx_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


class iceb_u_str stroka;
class iceb_u_str zagolov;
zagolov.plus(gettext("Список файлов настройки"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql,"%s:%s/%s",gettext("База"),data->imabazv.ravno(),iceb_get_pnk("00",0,data->window));
zagolov.ps_plus(strsql);


gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->naimfilpoi.ravno(),gettext("Наименование"));
  

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

void admin_alx_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Наименование"),renderer,"text",COL_NAIMFIL,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAIMFIL);
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

void admin_alx_vibor(GtkTreeSelection *selection,class admin_alx_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *naimfil;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_NAIMFIL,&naimfil,NUM_COLUMNS,&nomer,-1);

data->naimfil.new_plus(naimfil);
data->snanomer=nomer;

g_free(naimfil);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_alx_knopka(GtkWidget *widget,class admin_alx_data *data)
{
iceb_u_str repl;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("admin_alx_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    iceb_f_redfil(data->naimfil.ravno(),2,data->editor.ravno(),data->window);
    return;  


  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    admin_alx_udzap(data);
    admin_alx_create_list(data);
    return;  
  


  case FK4:
    data->metka_poi=admin_alx_poi(&data->naimfilpoi,data->window);
    admin_alx_create_list(data);
    return;  

  case FK5:
    admin_alx_rs(data);
//    admin_alx_rasp(data);
    return;  

  case FK6:
    iceb_menu_vvod1(gettext("Введите редактор"),&data->editor,40,"",data->window);
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

gboolean   admin_alx_key_press(GtkWidget *widget,GdkEventKey *event,class admin_alx_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {

  case GDK_KEY_F2:

    g_signal_emit_by_name(data->knopka[FK2],"clicked");

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
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void admin_alx_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class admin_alx_data *data)
{
//printf("admin_alx_v_row\n");
//printf("admin_alx_v_row корректировка\n");
 g_signal_emit_by_name(data->knopka[FK2],"clicked");

}

/*****************************/
/*Удаление записи            */
/*****************************/

void admin_alx_udzap(class admin_alx_data *data)
{
if(iceb_parol(0,data->window) != 0)
 return;

char strsql[512];

sprintf(strsql,"delete from Alx where fil='%s'",data->naimfil.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/****************************/
/*Проверка записей          */
/*****************************/

int  admin_alx_prov_row(SQL_str row,class admin_alx_data *data)
{
if(data->metka_poi == 0)
 return(0);



if(iceb_u_proverka(data->naimfilpoi.ravno(),row[0],4,0) != 0)
 return(1);
   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void admin_alx_rasp(class admin_alx_data *data)
{
char strsql[512];
SQL_str row,row1;
FILE *ff;
class SQLCURSOR cur,cur1;
class iceb_u_spisok imaf;
class iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select distinct fil from Alx order by fil asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"alx%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список файлов настройки"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Список файлов настройки"),ff,data->window);

if(data->metka_poi == 1)
 {
  if(data->naimfilpoi.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Наименование"),data->naimfilpoi.ravno());
 }

fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Наименование файла  |Дата и время запис.| Оператор\n"));
/*********
12345678901234567890 1234567890123456789
**********/
fprintf(ff,"\
------------------------------------------------------------------------\n");

while(cur.read_cursor(&row) != 0)
 {
  if(admin_alx_prov_row(row,data) != 0)
    continue;

  /*Читаем время и кто записал*/
  sprintf(strsql,"select ktoz,vrem from Alx where fil='%s' order by ns asc limit 1",row[0]);
  iceb_sql_readkey(strsql,&row1,&cur1,data->window);    

  fprintf(ff,"%-*s %s %s\n",
  iceb_u_kolbait(20,row[0]),row[0],
  iceb_u_vremzap(row1[1]),iceb_kszap(row1[0],data->window));


 }
fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
/********************/
/*Распечатать файл*/
/********************/
void admin_alx_rf(class admin_alx_data *data)
{
FILE	*ff;
SQL_str row;
class SQLCURSOR cur;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",data->naimfil.ravno());
if(cur.make_cursor(&bd,strsql) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }


if((ff = fopen(data->naimfil.ravno(),"w")) == NULL)
 {
  iceb_er_op_fil(data->naimfil.ravno(),"",errno,data->window);
  return;
 }



while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff,"%s",row[0]);  
 }

fclose(ff);

class iceb_u_spisok imaf;

class iceb_u_spisok naimot;

imaf.plus(data->naimfil.ravno());
sprintf(strsql,"%s %s",gettext("Распечатка файла"),data->naimfil.ravno());
naimot.plus(strsql);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);
}
/*********************/
/*ввод образца для поиска*/
/**************************/

int admin_alx_poi(class iceb_u_str *naimfilpoi,GtkWidget *wpredok)
{


if(iceb_menu_vvod1(gettext("Введите образец для поиска"),naimfilpoi,20,"",wpredok) != 0)
 return(0);

return(1);

}
/**********************************/
/*Распечатка*/
/***********************************/
void admin_alx_rs(class admin_alx_data *data)
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Распечатать"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Распечатать список файлов"));//0
punkt_m.plus(gettext("Распечатать выбранный файл"));//1


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      admin_alx_rasp(data);
      return;       

    case 1:
      admin_alx_rf(data);
      return;
   }
   
 }
}
