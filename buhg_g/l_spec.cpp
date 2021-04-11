/*$Id: l_spec.c,v 1.38 2013/12/31 11:49:14 sasa Exp $*/
/*19.02.2017	10.05.2005	Белых А.И.	l_spec.c
Ввод и корректировка спецификаций на изделия
*/
#include <errno.h>
#include <ctype.h>
#include "buhg_g.h"

enum
{
 FK2,
 SFK2,
 FK3,
 FK4,
 FK5,
 FK8,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KOD,
 COL_NAIM,
 NUM_COLUMNS
};

class  l_spec_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *treeview;
  GtkWidget *window;
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       voz;
  
  //Реквизиты выбранной записи
  iceb_u_str kod;
      
  iceb_u_spisok imaf;

  int       kod_v; //Код только что введенного материалла и для поиска материалла по коду материалла
  iceb_u_str naim_poi; //Поиск по наименованию
    
  l_spec_data()
   {
    clear();
    
   }

  void clear()
   {
    voz=0;
    kl_shift=0;
    snanomer=0;
    treeview=NULL;
    naim_poi.new_plus("");
    kod_v=0;
   }
 };

gboolean   l_spec_key_press(GtkWidget *widget,GdkEventKey *event,class l_spec_data *data);
void l_spec_vibor(GtkTreeSelection *selection,class l_spec_data *data);
void l_spec_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_spec_data *data);
void  l_spec_knopka(GtkWidget *widget,class l_spec_data *data);
void l_spec_add_columns (GtkTreeView *treeview);
void l_spec_create_list (class l_spec_data *data);


int	vniz(class l_spec_data *data);
void l_spec1(const char *kod_izdel,int uroven,GtkWidget *wpredok);

void rasizdel(GtkWidget *wpredok);
void spizw(const char*,GtkWidget *wpredok);
void specrasw(const char*,GtkWidget *wpredok);

extern SQL_baza  bd;

void l_spec(GtkWidget *wpredok)
{
class  l_spec_data data;
char bros[512];
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(bros,"%s %s",iceb_get_namesystem(),
gettext("Список всех введённых изделий"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_spec_key_press),&data);
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

data.label_kolstr=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);



//Кнопки
sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_spec_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод нового изделия"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"SF2 %s",gettext("Просмотр"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_spec_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Просмотр записей изделия"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_spec_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить выбранную запись"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_spec_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_spec_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка списка изделий"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F8 %s",gettext("Списание"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(l_spec_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Списание материалов в \"Материальном учёте\""));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));
gtk_widget_show(data.knopka[FK8]);

sprintf(bros,"F9 %s",gettext("Расчёт"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(l_spec_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Расчёт входимости материалов в изделие"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show(data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_spec_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);

gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

l_spec_create_list(&data);

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
void l_spec_create_list (class l_spec_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR curr;
char strsql[512];
int  kolstr=0;
SQL_str row;




data->kl_shift=0; //0-отжата 1-нажата  

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_spec_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_spec_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

sprintf(strsql,"select distinct kodi from Specif order by kodi asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
class iceb_u_str naim("");
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  if(specprov(row[0],&naim,&data->naim_poi,1,data->window) != 0)
   continue;
  


//  if(iceb_u_SRAV(data->vvod.god.ravno(),row[0],0) == 0 && iceb_u_SRAV(data->vvod.shet.ravno(),row[1],0) == 0)
//    data->snanomer=data->kolzap;
  if(data->kod_v == atoi(row[0]))
   data->snanomer=data->kolzap;
   

  
  //Код изделия
  ss[COL_KOD].new_plus(row[0]);

  //Наименование изделия
  ss[COL_NAIM].new_plus(naim.ravno());
  
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,ss[COL_KOD].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

data->kod_v=0;

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_spec_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str repl;
repl.plus(gettext("Список всех введённых изделий"));

sprintf(strsql,"%s:%d",gettext("Количество записей"),data->kolzap);
repl.ps_plus(strsql);
gtk_label_set_text(GTK_LABEL(data->label_kolstr),repl.ravno());

gtk_widget_show(data->label_kolstr);
if(data->naim_poi.getdlinna() > 1)
 {
  sprintf(strsql,"%s:%s",gettext("Поиск"),data->naim_poi.ravno());
  iceb_label_set_text_color(data->label_poisk,strsql,"red");
  gtk_widget_show(data->label_poisk);
  
 }

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_spec_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;



renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Код"), renderer,"text", COL_KOD,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);


}

/****************************/
/*Выбор строки*/
/**********************/

void l_spec_vibor(GtkTreeSelection *selection,class l_spec_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_KOD,&kod,NUM_COLUMNS,&nomer,-1);

data->kod.new_plus(kod);

data->snanomer=nomer;

g_free(kod);

//printf("%s %s %d\n",data->kodmat.ravno(),data->nomkar.ravno(),data->snanomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_spec_knopka(GtkWidget *widget,class l_spec_data *data)
{
char strsql[512];
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_spec_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {


  case FK2:
    if(vniz(data) == 0)
      l_spec_create_list(data);

    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;    
    l_spec1(data->kod.ravno(),1,data->window);
    l_spec_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
     return;    
    //Проверяем входимость в другие изделия      
    sprintf(strsql,"select kodd from Specif where kodd=%d",data->kod.ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) > 0)
     {
      iceb_u_str repl;
      repl.plus(gettext("Удаление невозможно !"));
      repl.ps_plus(gettext("Изделие входит в другие изделия как узел"));
      iceb_menu_soob(&repl,data->window);
      return;
     }

    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
     return;

    sprintf(strsql,"delete from Specif where kodi=%d",data->kod.ravno_atoi());

    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);

    l_spec_create_list(data);
    
    return;  

    
  
  case FK4:
    data->naim_poi.new_plus("");
    
    if(iceb_menu_vvod1(gettext("Введите наименование"),&naim,512,"",data->window) == 0)
      data->naim_poi.new_plus(naim.ravno());
    l_spec_create_list(data);
    return;  
  
  case FK5:
    if(data->kolzap == 0)
     return;    

    rasizdel(data->window);

    return;  

  
  case FK8:
    spizw(data->kod.ravno(),data->window);
    return;  

  case FK9:
    specrasw(data->kod.ravno(),data->window);
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_spec_key_press(GtkWidget *widget,GdkEventKey *event,class l_spec_data *data)
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

  case GDK_KEY_F8:
    g_signal_emit_by_name(data->knopka[FK8],"clicked");
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
    printf("l_spec_key_press-Нажата клавиша Shift\n");

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
void l_spec_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_spec_data *data)
{
g_signal_emit_by_name(data->knopka[SFK2],"clicked");
}
/************************************************/
/*Получение наименования и проверка на совпадение*/
/************************************************/
/*Если вернули 0 подходит
	       1 не подходит
*/
int	specprov(char *kodiz,iceb_u_str *naim,iceb_u_str *poisk,
int metka_naim, //0-если поиск не заказан выход 1-если поиск не заказан читаем наименование и выходим
GtkWidget *wpredok)
{
SQL_str         row1;
SQLCURSOR       cur1;
char		strsql[512];

if(poisk->getdlinna() <= 1 && metka_naim == 0)
 return(0);
naim->new_plus(""); 
for(int metka_zap=0 ; metka_zap < 2; metka_zap++)
 {
  if(metka_zap == 0)
    sprintf(strsql,"select naimat from Material where kodm=%s",kodiz);
  if(metka_zap == 1)
    sprintf(strsql,"select naius from Uslugi where kodus=%s",kodiz);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    if(metka_zap == 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код записи"),kodiz);
      iceb_menu_soob(strsql,wpredok);
     }
   }  
  else
   {
    naim->new_plus(row1[0]);
    break;
   }
 }
if(poisk->getdlinna() <= 1)
 return(0);

if(iceb_u_strstrm(naim->ravno(),poisk->ravno()) == 0)
  return(1);

return(0);

}
/*********************/
/*Ввод нового изделия*/
/*********************/
/*Если вернули 0 ввели новое изделие
               1 не ввели новое изделие
*/

int	vniz(class l_spec_data *data)
{
iceb_u_str	kodiz;
int		i;
iceb_u_str	naim;
int		kodm;
class iceb_u_str bros("");
char		strsql[512];
SQL_str         row;
SQLCURSOR       cur1;




kodiz.plus("");

iceb_u_spisok spis;
spis.plus(gettext("Введите код или наименование изделия"));
spis.plus(gettext(" + перед числом - поиск по числу в наименовании"));


if((i=iceb_menu_vvod1(&spis,&kodiz,20,"",data->window)) != 0)
   return(1);

naim.plus("");

kodm=kodiz.ravno_atoi();

if(kodiz.getdlinna() <= 1 || isdigit(kodiz.ravno()[0]) == 0)
 {

  if(iceb_u_polen(kodiz.ravno(),&bros,2,'+') != 0)
    bros.new_plus(kodiz.ravno());

  kodm=0;
  if(isdigit(bros.ravno()[0]) != 0)
     kodm=bros.ravno_atoi();
  naim.new_plus(kodiz.ravno());
  if(kodm != 0)
   naim.new_plus(kodm);
  
  if(l_mater(1,&kodiz,&naim,0,0,data->window) != 0)
   return(1);
  
 }

//Проверяем может изделие уже введено
sprintf(strsql,"select kodi from Specif where kodi=%d limit 1",kodiz.ravno_atoi());
if(sql_readkey(&bd,strsql,&row,&cur1) >= 1)
 {
  iceb_menu_soob(gettext("Это изделие уже введено !"),data->window);
  return(1);
 }


sprintf(strsql,"select * from Material \
where kodm=%d",kodiz.ravno_atoi());

if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код изделия"),kodiz.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
iceb_u_str kod_zap;
kod_zap.plus("");

int voz=l_spec_v(kodiz.ravno(),&kod_zap,0,data->window);
data->kod_v=0;
if(voz == 0)
 data->kod_v=kodiz.ravno_atoi();


return(voz);
}
/********************************/
/*Распечатка списка изделий*/
/*******************************/
void rasizdel(GtkWidget *wpredok)
{
FILE		*ff;
char		imaf[64];
SQL_str         row,row1;
char		strsql[512];
int		kolstr;
class iceb_u_str naim("");


sprintf(strsql,"select distinct kodi from Specif");
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0 )
  return;

sprintf(imaf,"izdel%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
iceb_u_startfil(ff);


fprintf(ff,"%s\n",gettext("Список всех введенных изделий"));
fprintf(ff,"-----------------------------------------------------\n");

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naimat from Material where kodm=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s-%s %s !",__FUNCTION__,gettext("Не найден код материалла"),row[0]);
    iceb_menu_soob(strsql,wpredok);
    naim.new_plus("");
   }  
  else
   naim.new_plus(row1[0]);
  fprintf(ff,"%-4s %s\n",row[0],naim.ravno());
 }
iceb_podpis(ff,wpredok);
fclose(ff);

iceb_u_spisok imaff;
iceb_u_spisok naimf;
imaff.plus(imaf);
naimf.plus(gettext("Список всех введенных изделий"));

iceb_rabfil(&imaff,&naimf,wpredok);

}
