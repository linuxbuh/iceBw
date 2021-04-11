/*$Id: l_saldo_sh.c,v 1.42 2014/08/31 06:19:20 sasa Exp $*/
/*07.02.2016	30.12.2003	Белых А.И.	l_saldo_sh.c
Работа со списком сальдо по счетам
*/
#include        <sys/stat.h>
#include        <errno.h>
#include  "buhg_g.h"
#include  "l_saldo_sh.h"

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK9,
  FK10,
  SFK2,
  KOL_F_KL
};

enum
{
  COL_GOD,
  COL_SHET,
  COL_DEBET,
  COL_KREDIT,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

class  saldo_sh_data
 {
  public:

  class saldo_sh_poi poisk;

  class iceb_u_str god_tv;
  class iceb_u_str shet_tv;
  
  iceb_u_str godv;
  iceb_u_str shetv;

  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *vwindow;
  GtkWidget *pwindow;
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  
  //Конструктор
  saldo_sh_data()
   {
    snanomer=0;
    kl_shift=0;
    vwindow=pwindow=window=treeview=NULL;
    god_tv.plus("");
    shet_tv.plus("");
    
   }      
 };


void saldo_sh_create_list (class saldo_sh_data *data);
gboolean   saldo_sh_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_sh_data *data);
void saldo_sh_vibor(GtkTreeSelection *selection,class saldo_sh_data *data);
void saldo_sh_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class saldo_sh_data *data);
void  saldo_sh_knopka(GtkWidget *widget,class saldo_sh_data *data);
void saldo_sh_add_columns (GtkTreeView *treeview);
int l_saldo_sh_v(class iceb_u_str *god_k,class iceb_u_str *shet_k,GtkWidget *wpredok);
int l_saldo_sh_p(class saldo_sh_poi *rkp,GtkWidget *wpredok);
int saldo_sh_udzap(class saldo_sh_data *data);
int  saldo_sh_prov_row(SQL_str row,class saldo_sh_data *data);
void saldo_sh_rasp(class saldo_sh_data *data);
int l_saldo_sh_is(class saldo_sh_data *data);

extern SQL_baza	bd;

void l_saldo_sh(GtkWidget *wpredok)
{
saldo_sh_data data;
char bros[512];

data.poisk.clear_zero();

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Сальдо"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(saldo_sh_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Стартовые сальдо по счетам"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(saldo_sh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(saldo_sh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(saldo_sh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(saldo_sh_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(saldo_sh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F9 %s",gettext("Импорт"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK9],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Импорт стартовых сальдо из файла"));
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(saldo_sh_knopka),&data);
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show(data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(saldo_sh_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));


saldo_sh_create_list (&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));



}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void saldo_sh_create_list (class saldo_sh_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;

//printf("saldo_sh_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(saldo_sh_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(saldo_sh_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select gs,ns,deb,kre,ktoi,vrem from Saldo where kkk=0 order by gs desc,ns asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);


data->kolzap=0;
double debet=0.;
double kredit=0.;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(saldo_sh_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->god_tv.ravno(),row[0],0) == 0 && iceb_u_SRAV(data->shet_tv.ravno(),row[1],0) == 0)
    data->snanomer=data->kolzap;

  //Дебет
  debet+=atof(row[2]);

  //Кредит
  kredit+=atof(row[3]);


  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_GOD,row[0],
  COL_SHET,row[1],
  COL_DEBET,row[2],
  COL_KREDIT,row[3],
  COL_DATA_VREM,iceb_u_vremzap(row[5]),
  COL_KTO,iceb_kszap(row[4],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

data->god_tv.new_plus("");
data->shet_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

saldo_sh_add_columns (GTK_TREE_VIEW (data->treeview));


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
zagolov.plus(gettext("Стартовые сальдо по счетам"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql,"%s: %.2f %s: %.2f",
gettext("Дебет"),debet,
gettext("Кредит"),kredit);

zagolov.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->poisk.metka_poi == 1)
 {
//  printf("Формирование заголовка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.god.ravno(),gettext("Год"));
  iceb_str_poisk(&zagolov,data->poisk.shet.ravno(),gettext("Счёт"));
  iceb_str_poisk(&zagolov,data->poisk.debet.ravno(),gettext("Дебет"));
  iceb_str_poisk(&zagolov,data->poisk.kredit.ravno(),gettext("Кредит"));

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

void saldo_sh_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
					       -1,gettext("Год"), renderer,
					       "text", COL_GOD,
					       NULL);

renderer = gtk_cell_renderer_text_new ();


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
					       -1,gettext("Счёт"), renderer,
					       "text", COL_SHET,
					       NULL);
renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дебет"), renderer,
"text", COL_DEBET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кредит"), renderer,
"text", COL_KREDIT,NULL);

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

void saldo_sh_vibor(GtkTreeSelection *selection,class saldo_sh_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *god;
gchar *shet;
gint  nomer;


gtk_tree_model_get(model,&iter,0,&god,1,&shet,NUM_COLUMNS,&nomer,-1);

data->godv.new_plus(god);
data->shetv.new_plus(shet);
data->snanomer=nomer;

g_free(god);
g_free(shet);

//printf("%s %s %d\n",data->godv.ravno(),data->shetv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldo_sh_knopka(GtkWidget *widget,class saldo_sh_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:

    if(l_saldo_sh_v(&data->god_tv,&data->shet_tv,data->window) == 0)
      saldo_sh_create_list(data);
    return;  

  case SFK2:
    if(l_saldo_sh_v(&data->godv,&data->shetv,data->window) == 0)
      saldo_sh_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(saldo_sh_udzap(data) == 0)
      saldo_sh_create_list(data);
    return;  


  case FK4:
    l_saldo_sh_p(&data->poisk,data->window);
    saldo_sh_create_list(data);
    return;  

  case FK5:
//    gtk_widget_hide(data->window);
//    saldo_sh_rasspis(data);
//    gtk_widget_show(data->window);
    saldo_sh_rasp(data);
    return;  

    
  case FK9:
    if(l_saldo_sh_is(data) == 0)
      saldo_sh_create_list(data);
    return;

  case FK10:
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   saldo_sh_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_sh_data *data)
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

  case GDK_KEY_F9:
    g_signal_emit_by_name(data->knopka[FK9],"clicked");
    return(FALSE);
  
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("saldo_sh_key_press-Нажата клавиша Shift\n");

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
void saldo_sh_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class saldo_sh_data *data)
{
g_signal_emit_by_name(data->knopka[SFK2],"clicked");
}

/*****************************/
/*Удаление записи            */
/*****************************/

int saldo_sh_udzap(class saldo_sh_data *data)
{
if(iceb_pbpds(1,data->godv.ravno_atoi(),data->window) != 0)
 return(1);

if(iceb_pbsh(1,data->godv.ravno_atoi(),data->shetv.ravno(),"","",data->window) != 0)
 return(1);
 
iceb_u_str sp;
sp.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&sp,2,data->window) == 2)
 return(1);

char strsql[512];

sprintf(strsql,"delete from Saldo where kkk=0 and gs=%d and ns='%s'",
data->godv.ravno_atoi(),data->shetv.ravno());

iceb_sql_zapis(strsql,0,0,data->window);

return(0);


}
/****************************/
/*Проверка записей          */
/*****************************/

int  saldo_sh_prov_row(SQL_str row,class saldo_sh_data *data)
{
if(data->poisk.metka_poi == 0)
 return(0);

if(data->poisk.god.getdlinna() > 1)
 if(atoi(data->poisk.god.ravno()) != atoi(row[0]))
   return(1);

if(iceb_u_proverka(data->poisk.shet.ravno(),row[1],1,0) != 0)
 return(1);
 
if(data->poisk.debet.getdlinna() > 1)
 if(iceb_u_atof(data->poisk.debet.ravno()) != iceb_u_atof(row[2]))
   return(1);

if(data->poisk.kredit.getdlinna() > 1)
 if(iceb_u_atof(data->poisk.kredit.ravno()) != iceb_u_atof(row[3]))
   return(1);
   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void saldo_sh_rasp(class saldo_sh_data *data)
{
char strsql[512];
SQL_str row;
class iceb_fopen fil;
class iceb_fopen fil_csv;
SQLCURSOR cur;
char imaf[64];
char imaf_csv[64];
int kolstr=0;

sprintf(strsql,"select gs,ns,deb,kre,ktoi,vrem from Saldo where kkk=0 order by gs asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(imaf,"saldo%d.lst",getpid());

if(fil.start(imaf,"w",data->window) != 0)
 return;

sprintf(imaf_csv,"saldocsv%d.lst",getpid());

if(fil_csv.start(imaf_csv,"w",data->window) != 0)
 return;

iceb_zagolov(gettext("Список стартовых сальдо по счетам"),0,0,0,0,0,0,fil.ff,data->window);

if(data->poisk.shet.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Счёт"),data->poisk.shet.ravno());
if(data->poisk.god.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Год"),data->poisk.god.ravno());
if(data->poisk.debet.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Дебет"),data->poisk.debet.ravno());
if(data->poisk.kredit.getdlinna() > 1)
 fprintf(fil.ff,"%s:%s\n",gettext("Кредит"),data->poisk.kredit.ravno());

fprintf(fil.ff,"\
------------------------------------------------------------------------\n");

fprintf(fil.ff,gettext("\
Год |Счёт  |   Дебет    |   Кредит   |Дата и время запис.| Кто записал\n"));


fprintf(fil.ff,"\
------------------------------------------------------------------------\n");
fprintf(fil_csv.ff,"#%s\n",gettext("Стартовое сальдо по счетам"));
double deb=0.,kre=0.;


cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(saldo_sh_prov_row(row,data) != 0)
    continue;


  fprintf(fil.ff,"%s %-*s %12.2f %12.2f %s %s\n",
  row[0],
  iceb_u_kolbait(6,row[1]),row[1],
  atof(row[2]),atof(row[3]),
  iceb_u_vremzap(row[5]),
  iceb_kszap(row[4],data->window));

  fprintf(fil_csv.ff,"%s|%s|%.2f|%.2f\n",
  row[0],
  row[1],
  atof(row[2]),atof(row[3]));

  deb+=atof(row[2]);
  kre+=atof(row[3]);

 }
fprintf(fil.ff,"\
------------------------------------------------------------------------\n");

fprintf(fil.ff,"%*s: %12.2f %12.2f\n",
iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),deb,kre);
fprintf(fil.ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(fil.ff,data->window);

fil.end();
fil_csv.end();

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Стартовые сальдо по счетам"));

iceb_ustpeh(imaf,3,data->window);

oth.spis_imaf.plus(imaf_csv);
oth.spis_naim.plus(gettext("Стартовые сальдо по счетам в формате csv"));



iceb_rabfil(&oth,data->window);


}
/*******************************************/
/*Импорт сальдо из файла*/
/********************************/
int l_saldo_sh_is(class saldo_sh_data *data)
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

class iceb_u_str naim;
naim.plus(gettext("Импорт стартовых сальдо из файла"));
naz:; 
if(iceb_menu_import(&imaf,naim.ravno(),"impsaldo.alx",__FUNCTION__,data->window) != 0)
 return(1);

char strsql[1024];
struct stat work;
if(stat(imaf.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_menu_soob(strsql,data->window);
  goto naz;
 }
class iceb_fopen fil;
if(fil.start(imaf.ravno(),"r",data->window) != 0)
 goto naz;

class iceb_fopen filprom;
char imafprom[64];
sprintf(imafprom,"prom%d.tmp",getpid());
if(filprom.start(imafprom,"w",data->window) != 0)
 goto naz;

int razmer=0;
class iceb_u_str strok("");
int  i=0;
class iceb_u_str god("");
class iceb_u_str shet("");
class iceb_u_str deb("");
class iceb_u_str kre("");
OPSHET ops;
time_t vrem;
time(&vrem);
SQLCURSOR cur1;
/*проверяем блокировку дат*/
while(iceb_u_fgets(&strok,fil.ff) == 0)
 {
  if(strok.ravno()[0] == '#')
   continue;

  if(iceb_u_polen(strok.ravno(),&god,1,'|') != 0)
    continue;

  if(iceb_pbpds(1,god.ravno_atoi(),data->window) != 0)
    return(1);
 }
rewind(fil.ff);

while(iceb_u_fgets(&strok,fil.ff) == 0)
 {
  if(strok.ravno()[0] == '#')
   continue;
  i=strlen(strok.ravno());
  razmer+=i;
  if(strok.ravno()[i-2] == '\r') //Возврат каретки меняем на пробел
   iceb_u_vstav(&strok," ",i-2,i-1,1);

  printf("%s",strok.ravno());
//  strzag(LINES-1,0,work.st_size,razmer);
  
  
  if(iceb_u_polen(strok.ravno(),&god,1,'|') != 0)
   {
    fprintf(filprom.ff,"%s",strok.ravno());
    continue;
   }

  if(god.ravno_atoi() < 1900)
   {
    sprintf(strsql,"%s !",gettext("Неправильно введён год"));
    iceb_menu_soob(strsql,data->window);

    fprintf(filprom.ff,"%s",strok.ravno());
    fprintf(filprom.ff,"#%s !\n",gettext("Неправильно введён год"));
    continue;
   }

  iceb_u_polen(strok.ravno(),&shet,2,'|');
  iceb_u_polen(strok.ravno(),&deb,3,'|');
  iceb_u_polen(strok.ravno(),&kre,4,'|');

  //проверяем счёт 
  if(iceb_prsh1(shet.ravno(),&ops,data->window) != 0)
   {
    fprintf(filprom.ff,"%s",strok.ravno());
    fprintf(filprom.ff,"#%s\n",gettext("Не правильно введён счёт !"));
    continue;
   }    

  if(ops.saldo == 3) //Счёт с развёрнутым сальдо
   {
    fprintf(filprom.ff,"%s",strok.ravno());
    fprintf(filprom.ff,"#%s\n",gettext("Счёт с развёрнутым сальдо!"));
    continue;    
   }


  sprintf(strsql,"select kkk from Saldo where kkk='%s' and gs=%d and ns='%s'",
  "0",god.ravno_atoi(),shet.ravno());

  if(iceb_sql_readkey(strsql,data->window) > 0)
   {
      /*записи могут повторятся если делается повторная загрузка сальдо без удаления сальдо в базе
        либо в базе нет сальдо а в файле записи повторяются
        этот вывод позволит увидеть какие записи повторяются если это была первая загрузка*/
    fprintf(filprom.ff,"%s",strok.ravno());
    fprintf(filprom.ff,"#%s\n",gettext("Этот счёт уже был!"));
   }


  //Удаляем старую запись если она есть (нет уникального ключа)
  sprintf(strsql,"delete from Saldo where kkk='%s' and gs=%d and ns='%s'",
  "0",god.ravno_atoi(),shet.ravno());

  iceb_sql_zapis(strsql,1,0,data->window);
     
  //записываем стартовое сальдо
  sprintf(strsql,"replace into Saldo (kkk,gs,ns,kodkon,deb,kre,ktoi,vrem) values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
  "0",god.ravno_atoi(),shet.ravno(),"",deb.ravno_atof(),kre.ravno_atof(),iceb_getuid(data->window),vrem);

//  printf("%s\n",strsql);
  
  iceb_sql_zapis(strsql,1,0,data->window);
  

 }

fil.end();
filprom.end();

unlink(imaf.ravno());

rename(imafprom,imaf.ravno());
return(0);
}
