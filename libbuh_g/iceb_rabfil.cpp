/*$Id: iceb_rabfil.c,v 1.84 2013/12/31 11:49:19 sasa Exp $*/
/*09.05.2015	20.01.2004	Белых А.И.	iceb_rabfil.c
Программа работы с файлами распечаток
*/
#include <sys/stat.h>
#include   "iceb_libbuh.h"

enum
 {
  FK2,
  SFK2,
  FK3,
  SFK3,
  FK4,
  FK5,
  FK6,
  FK7,
  FK8,
  FK10,
  KOL_F_KL  
 };

enum
 {
  COL_FIXED,
  COL_KODIROVKA,
  COL_NAIM,
  COL_FIL,
  COL_RAZMER,
  NUM_COLUMNS
 };



class rabfil_data
 {
  public:
  
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *sw;
  GtkWidget *label;
  GtkWidget *label2;
  GtkWidget *knopka[KOL_F_KL];
  class iceb_u_str name_window;

  class iceb_u_str redaktor;  //Редактор файлов
  class iceb_u_str prosmotr; //Просмотр файлов
  class iceb_u_str prog_lpr; //Программа печати
  class iceb_u_str deletop;  //Удаление с очереди на печать
  class iceb_u_str prosop;  //Просмотр очереди на печать
  class iceb_u_str nameprinter;  //Имя принтера
  class iceb_u_str iceb_ps_filtr; //фильтр Epson->PostScript
  class iceb_u_str prosmotr_ps; //Просмотрщик PostScript файлов

  class iceb_u_str kluh_s_fil; //Ключ -L/etc/iceB/logotip.ps
    
  int        kl_shift;
  int metka_pros_nast; /*0-прсмотр настроек выключен 1- включён*/
  
  class iceb_u_int kodirovka; /*0-текущая кодировка 1 - WINDOWS-1251*/
  class iceb_u_str ivfil; //Имя выбранного файла
  int snanomer;
  gchar *current_dir; /*Текущий каталог*/
  class spis_oth *oth;

  rabfil_data() //Конструктор
   {
    snanomer=0;
    metka_pros_nast=0;
    kl_shift=0;
    treeview=NULL;
    kluh_s_fil.plus("");
    current_dir=g_get_current_dir();
   }
  ~rabfil_data()
   {
    g_free(current_dir);
   }   
 };

static void rabfil_add_columns (GtkTreeView *treeview);
void rabfiln_add_f10(GtkTreeView *treeview);

void rabfiln_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class kontr_data *data);
void rabfiln_vibor(GtkTreeSelection *selection,class rabfil_data *data);
void  rabfiln_knopka(GtkWidget *widget,class rabfil_data *data);
gboolean   rabfiln_key_press(GtkWidget *widget,GdkEventKey *event,class rabfil_data *data);
void rabfiln_str_nast(iceb_u_str *soob,class rabfil_data *data);
void rabfil_create_list(class rabfil_data *data);
void rabfiln_read_nast(class rabfil_data *data);
void iceb_rabfil_mk(class rabfil_data *data);
void iceb_rabfil_op(class rabfil_data *data);
int iceb_progforlst(const char *imaf,GtkWidget *wpredok);
void iceb_copfil(const char *imafil,GtkWidget *wpredok);

extern SQL_baza bd;

/******************************************************/
void iceb_rabfil(class spis_oth *oth,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
char bros[1024];

class rabfil_data data;

data.name_window.plus(__FUNCTION__);
data.oth=oth;
//Чтение настроек
rabfiln_read_nast(&data);
//data.kodirovka.make_class(data.imaf->kolih());
data.kodirovka.make_class(data.oth->spis_imaf.kolih());

for(int nomer=0; nomer < data.oth->spis_mk.kolih(); nomer++)
 {
  int kkk=0;
  if((kkk=data.oth->spis_imaf.find(data.oth->spis_mk.ravno(nomer))) >= 0)
   data.kodirovka.plus(1,kkk);
 }

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),600,-1);

sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Работа с файлами документов"));

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rabfiln_key_press),&data);
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
gtk_widget_show(vbox1);
gtk_widget_show(vbox2);
gtk_widget_show(hbox);


iceb_u_str soob;
rabfiln_str_nast(&soob,&data);

data.label=gtk_label_new(soob.ravno());
gtk_box_pack_start(GTK_BOX(vbox2),data.label,FALSE,FALSE,0);

sprintf(bros,"%s\n%s:%s",gettext("Список распечаток"),gettext("Текущий каталог"),data.current_dir);
data.label2=gtk_label_new(bros);
gtk_box_pack_start(GTK_BOX(vbox2),data.label2,FALSE,FALSE,0);
gtk_widget_show(data.label2);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки

sprintf(bros,"F2 %s",gettext("Просмотр"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(rabfiln_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Просмотр выбранного документа"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Просмотр"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(rabfiln_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Просмотр перед печатью"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Редактирование"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(rabfiln_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Редактирование выбранного документа"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Смена редактора"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3], TRUE, TRUE, 0);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(rabfiln_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Ввод имени новой программы для редактирования файлов"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Копировать"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(rabfiln_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Копировать файл на нужный носитель"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(rabfiln_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка выбранного документа"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);


sprintf(bros,"F6 %s",gettext("Очередь"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(rabfiln_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Просмотр очереди на печать"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Налаштовка"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(rabfiln_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Включить/выключить просмотр настроек"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F8 %s",gettext("Перекодировать"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(rabfiln_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Перекодировать в кодировку WINDOWS-1251. Повторное нажатие кнопки возвращает предыдущюю кодировку."));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));
gtk_widget_show(data.knopka[FK8]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK10], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(rabfiln_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в окне с удалением всех файлов"));
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);

//gtk_widget_grab_focus(data.knopka[FK10]);

rabfil_create_list(&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}
/*****************/
/*Создание списка*/
/*****************/
void rabfil_create_list(class rabfil_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
//printf("rabfil_create_list\n");


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(rabfiln_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(rabfiln_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_BOOLEAN,
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING,
G_TYPE_INT);


//Определяем количество файлов
//int kolfil=data->imaf->kolih();
int kolfil=data->oth->spis_imaf.kolih();
char razmerfil[56];
struct stat bufstat;
char kodir[8];
gboolean metka_s=FALSE;
for(int i=0 ; i < kolfil; i++)
 {
  memset(kodir,'\0',sizeof(kodir));
  if(data->kodirovka.ravno(i) == 1)
   kodir[0]='W';
   
  memset(razmerfil,'\0',sizeof(razmerfil));
//  if(stat(data->imaf->ravno(i),&bufstat) == 0)
  if(stat(data->oth->spis_imaf.ravno(i),&bufstat) == 0)
   {
    if(bufstat.st_size > 1000000)
     {
      double razm=bufstat.st_size/1000000.;
      razm=iceb_u_okrug(razm,0.01);
      sprintf(razmerfil,"%.2fM",razm);
     }
    else
     sprintf(razmerfil,"%ld",bufstat.st_size);
   }    

  if(data->oth->spis_ms.find(data->oth->spis_imaf.ravno(i)) >= 0)
   metka_s=TRUE;
  else
   metka_s=FALSE;


  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_FIXED,metka_s,
  COL_KODIROVKA,kodir,
//  COL_NAIM,data->naimdok->ravno(i),
//  COL_FIL,data->imaf->ravno(i),
  COL_NAIM,data->oth->spis_naim.ravno(i),
  COL_FIL,data->oth->spis_imaf.ravno(i),
  COL_RAZMER,razmerfil,
  NUM_COLUMNS,i,
  -1);

 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

rabfil_add_columns (GTK_TREE_VIEW (data->treeview));


if(kolfil == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK4]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK4]),TRUE);//Доступна
 }
/****************
if(data->metka_knop == 1)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
****************/
if(kolfil > 0)
 {
  GtkTreePath *path=gtk_tree_path_new_from_string("0");

  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
//  gtk_tree_view_row_activated(GTK_TREE_VIEW (data->treeview),path,NULL);

  gtk_tree_path_free(path);
  gtk_widget_grab_focus(data->treeview); /*установить фокус ввода на записи в списке*/
 }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

}

/**********************************/
/*Новая программа создания колонок*/
/**********************************/
static void fixed_toggled (GtkCellRendererToggle *cell,
	       gchar                 *path_str,
	       gpointer               data)
{
  GtkTreeModel *model = (GtkTreeModel *)data;
  GtkTreeIter  iter;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
  gboolean fixed;

  /* get toggled iter */
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get (model, &iter, COL_FIXED, &fixed, -1);

  /* do something with the value */
  fixed ^= 1;

  /* set new value */
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, COL_FIXED, fixed, -1);

  /* clean up */
  gtk_tree_path_free (path);
}
/******************************/
static void rabfil_add_columns (GtkTreeView *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeModel *model = gtk_tree_view_get_model (treeview);

  /* column for fixed toggles */
  renderer = gtk_cell_renderer_toggle_new ();
  g_signal_connect (renderer, "toggled", G_CALLBACK (fixed_toggled), model);

  column = gtk_tree_view_column_new_with_attributes (gettext("Сохранить"),
  renderer,"active", COL_FIXED,NULL);

  gtk_tree_view_append_column (treeview, column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("K",renderer,"text",COL_KODIROVKA,NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_KODIROVKA);
  gtk_tree_view_append_column (treeview, column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (gettext("Наименование документа"),
  renderer,"text",COL_NAIM,NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_NAIM);
  gtk_tree_view_append_column (treeview, column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (gettext("Имя файла"),
  renderer,"text",COL_FIL,NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_FIL);
  gtk_tree_view_append_column (treeview, column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (gettext("Размер"),
  renderer,"text",COL_RAZMER,NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_RAZMER);
  gtk_tree_view_append_column (treeview, column);

}



/****************************/
/*Выбор строки  по двойному клику мышкой*/
/**********************/
void rabfiln_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class kontr_data *data)
{

}


/****************************/
/*Выбор строки*/
/**********************/

void rabfiln_vibor(GtkTreeSelection *selection,class rabfil_data *data)
{
//printf("rabfiln_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *imaf;
gint nomer_str=0;
gtk_tree_model_get(model,&iter,COL_FIL,&imaf,NUM_COLUMNS,&nomer_str,-1);

data->ivfil.new_plus(imaf);
data->snanomer=nomer_str;
g_free(imaf);

//printf("%s\n",data->ivfil.ravno());

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rabfiln_knopka(GtkWidget *widget,class rabfil_data *data)
{
class iceb_u_str repl("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("rabfiln_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
    iceb_refresh();
    iceb_prosf(data->ivfil.ravno(),data->window);
    
    gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));
    return;  

  case SFK2:
    gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
    iceb_refresh();
    iceb_prospp(data->ivfil.ravno(),data->window);
    
    gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));
    return;  

  case FK3:
    gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
    iceb_refresh();

    iceb_f_redfil(data->ivfil.ravno(),1,data->redaktor.ravno(),data->window);

    gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));

    return;  

  case SFK3:
    iceb_menu_vvod1(gettext("Введите имя программы для редактирования файлов"),&data->redaktor,40,"",data->window);

    rabfiln_str_nast(&repl,data);

    gtk_label_set_text(GTK_LABEL(data->label),repl.ravno());    
    return;  

  case FK4:
   iceb_rabfil_mk(data);
   return;

  case FK5:
    iceb_print_operation( GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,data->ivfil.ravno(),data->window);
    return;  


  case FK6:
    iceb_rabfil_op(data);
    return;  

  case FK7:
    data->metka_pros_nast++;
    if(data->metka_pros_nast > 1)
     data->metka_pros_nast=0;

    if(data->metka_pros_nast == 0)
     gtk_widget_hide(data->label);
    else
     gtk_widget_show(data->label);
     
    return;   

  case FK8: /*Перекодировать файл в WINDOWS-1251 и обратно*/
    if(data->kodirovka.ravno(data->snanomer) == 0)
     {
      if(iceb_perecod(2,data->ivfil.ravno(),data->window) != 0)
       return;
      data->kodirovka.plus(1,data->snanomer);
      rabfil_create_list(data);
      return;
     }
    if(data->kodirovka.ravno(data->snanomer) == 1)
     {
      if(iceb_perecod(3,data->ivfil.ravno(),data->window) != 0)
       return;
      data->kodirovka.new_plus(0,data->snanomer);
     }
    rabfil_create_list(data);
    return;   
   
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);

    rabfiln_add_f10(GTK_TREE_VIEW (data->treeview));

    gtk_widget_destroy(data->window);
    return;
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rabfiln_key_press(GtkWidget *widget,GdkEventKey *event,class rabfil_data *data)
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
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK3],"clicked");
    else
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

  case GDK_KEY_F7:
    g_signal_emit_by_name(data->knopka[FK7],"clicked");
    return(TRUE);

  case GDK_KEY_F8:
    g_signal_emit_by_name(data->knopka[FK8],"clicked");
    return(TRUE);



  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("kontr_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/********************************/
/*Формирование строки настроек  */
/********************************/

void rabfiln_str_nast(iceb_u_str *soob,class rabfil_data *data)
{


soob->new_plus(gettext("Программа печати"));
soob->plus(": ");
if(data->prog_lpr.getdlinna() > 1)
 soob->plus(data->prog_lpr.ravno());

data->nameprinter.new_plus(getenv("PRINTER"));
soob->plus("  ");
soob->plus(gettext("Принтер"));
soob->plus(": ");
if(data->nameprinter.getdlinna() > 1)
 soob->plus_ps(data->nameprinter.ravno());
else
 soob->plus_ps(gettext("По умолчанию"));

soob->plus(gettext("Удаление из очереди на печать"));
soob->plus(": ");
if(data->deletop.getdlinna() > 1)
 soob->plus(data->deletop.ravno());
else
 soob->plus("");


soob->plus("  ");
soob->plus(gettext("Просмотр"));
soob->plus(": ");
if(data->prosmotr.getdlinna() > 1)
 soob->plus_ps(data->prosmotr.ravno());
else
 soob->plus_ps("");

soob->plus(gettext("Редактирование"));
soob->plus(": ");
if(data->redaktor.getdlinna() > 1)
 soob->plus(data->redaktor.ravno());
else
 soob->plus("");

soob->plus("  ");
soob->plus(gettext("Просмотр очереди на печать"));
soob->plus(": ");
if(data->prosop.getdlinna() > 1)
 soob->plus(data->prosop.ravno());
else
 soob->plus("");

soob->ps_plus(gettext("Просмотрщик PostScript файлов"));
soob->plus(": ");
if(data->prosmotr_ps.getdlinna() > 1)
 soob->plus(data->prosmotr_ps.ravno());
else
 soob->plus("");

soob->ps_plus(gettext("Фильтр Epson->PostScript"));
soob->plus(": ");
if(data->iceb_ps_filtr.getdlinna() > 1)
 soob->plus(data->iceb_ps_filtr.ravno());
else
 soob->plus("");

soob->ps_plus(gettext("Ключ для печати логотипа"));
soob->plus(": ");
if(data->iceb_ps_filtr.getdlinna() > 1)
 soob->plus(data->kluh_s_fil.ravno());
else
 soob->plus("");

}

/****************************************/
/*Чтение настроек                       */
/****************************************/

void rabfiln_read_nast(class rabfil_data *data)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_u_str nazvanie("");


//Читаем необходимые настройки
sprintf(strsql,"select str from Alx where fil='nastsys.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
    if(iceb_u_polen(row_alx[0],&nazvanie,1,'|') != 0)
      continue;
      
    if(iceb_u_SRAV(nazvanie.ravno(),"Системный принтер",0) == 0)
     {
      iceb_u_polen(row_alx[0],&data->prog_lpr,2,'|');
      continue;
     }

    if(iceb_u_SRAV(nazvanie.ravno(),"Снять с очереди на печать",0) == 0)
     {
      iceb_u_polen(row_alx[0],&data->deletop,2,'|');
      continue;
     }

    if(iceb_u_SRAV(nazvanie.ravno(),"Редактор для графического интерфейса",0) == 0)
     {
      iceb_u_polen(row_alx[0],&data->redaktor,2,'|');
      continue;
     }

    if(iceb_u_SRAV(nazvanie.ravno(),"Просмотрщик файлов для графического интерфейса",0) == 0)
     {
      iceb_u_polen(row_alx[0],&data->prosmotr,2,'|');
      continue;
     }

    if(iceb_u_SRAV(nazvanie.ravno(),"Просмотр очереди на печать",0) == 0)
     {
      iceb_u_polen(row_alx[0],&data->prosop,2,'|');
      continue;
     }

    if(iceb_u_SRAV(nazvanie.ravno(),"Просмотрщик PostScript файлов",0) == 0)
     {
      iceb_u_polen(row_alx[0],&data->prosmotr_ps,2,'|');
      continue;
     }

    if(iceb_u_SRAV(nazvanie.ravno(),"Фильт Epson->PostScript",0) == 0)
     {
      iceb_u_polen(row_alx[0],&data->iceb_ps_filtr,2,'|');
      continue;
     }

    if(iceb_u_SRAV(nazvanie.ravno(),"Путь на файл с логотипом",0) == 0)
     {
      iceb_u_polen(row_alx[0],&data->kluh_s_fil,2,'|');
      continue;
     }



 }


}


/******************************/
/*Меню для копирования файлов*/
/*****************************/
void iceb_rabfil_mk(class rabfil_data *data)
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Копировать файл на WINDOWS машину"));//0
punkt_m.plus(gettext("Копировать файл в каталог"));//1
punkt_m.plus(gettext("Выполнить программу"));//2


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);

static iceb_u_str katalog("");
class iceb_u_str fulput("");
switch(nomer)
 {
  case 0:
   iceb_spis_komp(data->ivfil.ravno(),data->window);
   break;

  case  1:
    iceb_copfil(data->ivfil.ravno(),data->window);
    break;

  case  2:
    iceb_progforlst(data->ivfil.ravno(),data->window);
    break;    
 }

}
/**************************/
/*Очередь на печать*/
/**********************/
void iceb_rabfil_op(class rabfil_data *data)
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Просмотр очереди на печать"));//0
punkt_m.plus(gettext("Удаление из очереди на печать"));//1


int nomer=0;

while(nomer < 2 && nomer >= 0)
 {
  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);

  switch(nomer)
   {
    case 0:
     iceb_ponp(data->prosop.ravno(),data->deletop.ravno(),data->window);
     break;

    case 1:
      iceb_uionp(data->window);
      break;
     
   }
 }
}

/*************************************/
/*Удаление файлов при выходе по F10*/
/***********************************/

void rabfiln_add_f10(GtkTreeView *treeview)
{
GtkTreeIter iter;
GtkTreeModel *list_store = gtk_tree_view_get_model (treeview);
/*Получаем первый итератор в списке*/
gboolean valid=gtk_tree_model_get_iter_first(list_store,&iter);
while(valid)
 {
  gchar *imaf;
  gboolean  fixed=FALSE;
  gtk_tree_model_get(list_store,&iter,COL_FIXED,&fixed,COL_FIL,&imaf,-1);

  if(fixed == TRUE) 
   {
    /*printf("Сохранить %s\n",imaf);*/
   }
  else
   {
    
    /*printf("Не сохранять %s\n",imaf);*/
    unlink(imaf);
   }
  g_free(imaf);

  valid=gtk_tree_model_iter_next(list_store,&iter);

 }

}

/***************************************/

void	iceb_rabfil(class iceb_u_spisok *spfil,class iceb_u_spisok *spnaimdok,
GtkWidget *wpredok)
{
class spis_oth oth;

for(int nom=0; nom < spfil->kolih(); nom++)
 oth.spis_imaf.plus(spfil->ravno(nom));

for(int nom=0; nom < spnaimdok->kolih(); nom++)
 oth.spis_naim.plus(spnaimdok->ravno(nom));

iceb_rabfil(&oth,wpredok);

}
