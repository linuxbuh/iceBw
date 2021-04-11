/*$Id: l_sptbn.c,v 1.29 2013/12/31 11:49:14 sasa Exp $*/
/*08.05.2019	14.09.2006	Белых А.И.	l_sptbn.c
Просмотр списка табельных номеров
*/
#include <errno.h>
#include "buhg_g.h"
#include "l_sptbn.h"

enum
{
 FK2,
 SFK2,
 FK3,
 FK4,
 FK5,
 FK6,
 FK7,
 SFK7,
 FK8,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_METKA,
 COL_TABNOM,
 COL_FIO,
 COL_BANK,
 COL_SALDO,
 NUM_COLUMNS
};

class  sptbn_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  class iceb_u_str name_window;
  
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  

  class l_sptbn_p  poi;
  iceb_u_str zapros;

  iceb_u_str tabnom_tv;//Код толькочто введённой записи
  iceb_u_str tabnom_v; //Выбранный код 
  iceb_u_str fio_v; //Наименование выбранной записи

  int  metka_rr;
  short msor;   
  short ds,ms,gs; //Дата на которую будет вычисляться сальдо
  short dt,mt,gt; //текущая дата
  int podr;
  short metka_pros; //0-всех 1-работающих 2-уволенных*/
  short metka_nast; //0-всех 1-имеющих настр. запись 2-не имеющих*/
  short metka_prov; //0-всех 1-без проводок
  char metka_zap[32];  
  short metka_saldo; //0-без сальд 1- с сальдо
  class iceb_u_str naim_podr;
  
  //Конструктор
  sptbn_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    
    iceb_u_poltekdat(&dt,&mt,&gt);
    iceb_u_poltekdat(&ds,&ms,&gs);
    ds=1;
    iceb_u_dpm(&ds,&ms,&gs,3);
    msor=0;
  
    metka_pros=1;
    metka_nast=0;
    metka_prov=0;
    metka_saldo=0;
    memset(metka_zap,'\0',sizeof(metka_zap));
    naim_podr.plus("");
   }      
 };

gboolean   sptbn_key_press(GtkWidget *widget,GdkEventKey *event,class sptbn_data *data);
void sptbn_vibor(GtkTreeSelection *selection,class sptbn_data *data);
void sptbn_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class sptbn_data *data);
void  sptbn_knopka(GtkWidget *widget,class sptbn_data *data);
void sptbn_add_columns (GtkTreeView *treeview,short);
void sptbn_create_list(class sptbn_data *data);
int  sptbn_prov_row(SQL_str row,class sptbn_data *data);

int l_sptbn_p(class l_sptbn_p *datap,GtkWidget *wpredok);
void l_sptbn_ras(class sptbn_data *data);

extern SQL_baza	bd;

int l_sptbn(int metka, //0-ввод/корректировка 1-выбор
class iceb_u_str *tabnom,
class iceb_u_str *fio,
int podr,
GtkWidget *wpredok)
{
int gor=0;
int ver=0;

class sptbn_data data;
char strsql[512];

data.metka_rr=metka;
data.name_window.plus(__FUNCTION__);

data.podr=podr;

if(data.podr != 0)
 {
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select naik from Podr where kod=%d",data.podr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   data.naim_podr.new_plus(row[0]);
 }



data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_rr == 1)
 {
  if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
 }


sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Список работников"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(sptbn_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Просмотр списка табельных номеров"));


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


sprintf(strsql,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(sptbn_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Карточка"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(sptbn_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Просмотр карточки работника"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(sptbn_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить выбранную запись"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);


sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(sptbn_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(sptbn_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F6 %s",gettext("Уволенные"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(sptbn_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Переключение режима просмотра списка (работающие, уволенные, все)"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(strsql,"F7 %s","*");
data.knopka[FK7]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(sptbn_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Переключение режима просмотра списка (все, только без проводок)"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show(data.knopka[FK7]);

sprintf(strsql,"%sF7 %s",RFK,"V");
data.knopka[SFK7]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK7], "clicked",G_CALLBACK(sptbn_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK7],gettext("Переключение режима просмотра списка (все, только помеченные символом \"V\", только без символа \"V\")"));
gtk_widget_set_name(data.knopka[SFK7],iceb_u_inttochar(SFK7));
gtk_widget_show(data.knopka[SFK7]);

sprintf(strsql,"F8 %s",gettext("Сальдо"));
data.knopka[FK8]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(sptbn_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Включить/выключить расчёт сальдо"));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));
gtk_widget_show(data.knopka[FK8]);

sprintf(strsql,"F9 %s",gettext("Сортировка"));
data.knopka[FK9]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(sptbn_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Переключение сортировки (по фамилиям, по табельным номерам)"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show(data.knopka[FK9]);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(sptbn_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

sptbn_create_list(&data);
gtk_widget_show(data.window);

if(metka == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.metka_voz == 0)
 if(metka == 1)
  {
   tabnom->new_plus(data.tabnom_v.ravno());
   fio->new_plus(data.fio_v.ravno());
  }

return(data.metka_voz);


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  sptbn_knopka(GtkWidget *widget,class sptbn_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("sptbn_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    data->tabnom_tv.new_plus("");
    if(vkartz(&data->tabnom_tv,data->window) != 0)
      sptbn_create_list(data);
    return;  

  case SFK2:

    if(vkartz(&data->tabnom_v,data->window) != 0)
      sptbn_create_list(data);

    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    
    zarudkarw(data->tabnom_v.ravno_atoi(),data->window);

    sptbn_create_list(data);
    return;  


  case FK4:
    l_sptbn_p(&data->poi,data->window);
    sptbn_create_list(data);
    return;  

  case FK5:
   l_sptbn_ras(data);
   return;  

  case FK6:
    data->metka_pros++;
    if(data->metka_pros == 3)
     data->metka_pros=0;
    sptbn_create_list(data);
    return;  

  case FK7:
    data->metka_prov++;
    if(data->metka_prov == 2)
     data->metka_prov=0;
    sptbn_create_list(data);
    return;  

  case SFK7:
    data->metka_nast++;
    if(data->metka_nast == 3)
     data->metka_nast=0;
    sptbn_create_list(data);
    return;  

  case FK8:
    data->metka_saldo++;
    if(data->metka_saldo == 2)
     data->metka_saldo=0;
    sptbn_create_list(data);
    return;  

  case FK9:
    data->msor++;
    if(data->msor == 2)
     data->msor=0;
    sptbn_create_list(data);
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

gboolean   sptbn_key_press(GtkWidget *widget,GdkEventKey *event,class sptbn_data *data)
{

switch(event->keyval)
 {
  case GDK_KEY_F2:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    if(data->kl_shift == 1)
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

  case GDK_KEY_F6:
    g_signal_emit_by_name(data->knopka[FK6],"clicked");
    return(TRUE);

  case GDK_KEY_F7:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK7],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK7],"clicked");
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
//    printf("sptbn_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void sptbn_create_list (class sptbn_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[1024];
int  kolstr=0;
SQL_str row,row1;
iceb_u_str zagolov;

//printf("sptbn_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(sptbn_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(sptbn_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_INT, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_DOUBLE, 
G_TYPE_INT);

if(data->podr == 0)
 {
  if(data->msor == 0)
    sprintf(strsql,"select tabn,fio,datk,inn,sovm,bankshet,kb from Kartb order by fio asc");
  else
    sprintf(strsql,"select tabn,fio,datk,inn,sovm,bankshet,kb from Kartb order by tabn asc");
 }
else
 {
  if(data->msor == 0)
   sprintf(strsql,"select tabn,fio,datk,inn,sovm,bankshet,kb from Kartb where podr=%d \
order by fio asc",data->podr);
  else
   sprintf(strsql,"select tabn,fio,datk,inn,sovm,bankshet,kb from Kartb where podr=%d \
order by tabn asc",data->podr);
 } 

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

data->kolzap=0;
double saldo=0;
class iceb_u_str naimb("");
int kodbanka=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(sptbn_prov_row(row,data) != 0)
    continue;

  if(iceb_u_SRAV(data->tabnom_tv.ravno(),row[0],0) == 0)
   data->snanomer=data->kolzap;

  if(row[4][0] == '1')
   strcat(data->metka_zap,"S");      

  if(data->metka_saldo == 1)
   {
    double saldb=0.;

    saldo=zarsaldw(1,data->ms,data->gs,atoi(row[0]),&saldb,data->window);
   }
  else
    saldo=0.;


  naimb.new_plus("");
  if((kodbanka=atoi(row[6])) != 0)
   {
    sprintf(strsql,"select naik from Zarsb where kod=%d",kodbanka);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     naimb.new_plus(row1[0]);
   }
   


  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKA,data->metka_zap,
  COL_TABNOM,atoi(row[0]),
  COL_FIO,row[1],
  COL_SALDO,saldo,
  COL_BANK,naimb.ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->tabnom_tv.new_plus("");
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

sptbn_add_columns (GTK_TREE_VIEW (data->treeview),data->metka_saldo);


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
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

//printf("metka_poi=%d\n",data->poi.metka_poi);
if(data->poi.metka_poi == 1 )
 {

// printf("Формирование заголовка с реквизитами поиска.\n");
  iceb_u_str strpoi;  

  strpoi.new_plus(gettext("Поиск"));
  strpoi.plus(" !!!");

  iceb_str_poisk(&strpoi,data->poi.fio.ravno(),gettext("Фамилия"));
  iceb_str_poisk(&strpoi,data->poi.inn.ravno(),gettext("Идентификационный номер"));
  iceb_str_poisk(&strpoi,data->poi.metka_nks.ravno(),gettext("Наличие карт-счёта"));
  iceb_str_poisk(&strpoi,data->poi.nomks.ravno(),gettext("Номер карт-счёта"));

  iceb_label_set_text_color(data->label_poisk,strpoi.ravno(),"red");
  gtk_widget_show(data->label_poisk);
 }
else
 {
  gtk_widget_hide(data->label_poisk); 
 }

zagolov.new_plus(gettext("Просмотр списка табельных номеров"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
sprintf(strsql,"%s",gettext("Метка: *-проводки S-совместитель V-был вход в карточку #-карточка заблокирована"));
zagolov.ps_plus(strsql);
if(data->podr != 0)
 {
  zagolov.ps_plus(gettext("Подразделение"));
  zagolov.plus(":");
  zagolov.plus(data->podr);
  zagolov.plus(" ");
  zagolov.plus(data->naim_podr.ravno());
 }

if(data->metka_pros == 0)
 zagolov.ps_plus(gettext("Просмотр всех работников"));
if(data->metka_pros == 1)
 zagolov.ps_plus(gettext("Просмотр только работающих работников"));
if(data->metka_pros == 2)
 zagolov.ps_plus(gettext("Просмотр только уволенных работников"));
 
if(data->metka_prov == 1)
 zagolov.ps_plus(gettext("Просмотр только записей без проводок"));

if(data->metka_nast == 1)
 zagolov.ps_plus(gettext("Просмотр только записей с меткой \"V\""));
if(data->metka_nast == 2)
 zagolov.ps_plus(gettext("Просмотр только записей без метки \"V\""));
 
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));
//printf("l_sptbn-end\n");
}
/*****************/
/*Создаем колонки*/
/*****************/

void sptbn_add_columns(GtkTreeView *treeview,short metka_saldo)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;

//printf("sptbn_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Метка"),renderer,"text",COL_METKA,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_METKA);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Табельный номер"),renderer,"text",COL_TABNOM,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_TABNOM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Фамилия Имя Отчество"),renderer,"text",COL_FIO,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_FIO);
gtk_tree_view_append_column (treeview, column);

column = gtk_tree_view_column_new_with_attributes (gettext("Банк"),renderer,"text",COL_BANK,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_BANK);
gtk_tree_view_append_column (treeview, column);

if(metka_saldo == 1)
 {
  renderer = gtk_cell_renderer_text_new ();

  column = gtk_tree_view_column_new_with_attributes (gettext("Сальдо"),renderer,"text",COL_SALDO,NULL);
  gtk_tree_view_column_set_sort_column_id (column, COL_SALDO);
  gtk_tree_view_append_column (treeview, column);
 }

//printf("sptbn_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void sptbn_vibor(GtkTreeSelection *selection,class sptbn_data *data)
{

GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gint kod;
gchar *naim;
gint  nomer;

gtk_tree_model_get(model,&iter,COL_TABNOM,&kod,COL_FIO,&naim,NUM_COLUMNS,&nomer,-1);

data->tabnom_v.new_plus(kod);
data->fio_v.new_plus(naim);
data->snanomer=nomer;

g_free(naim);


}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void sptbn_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class sptbn_data *data)
{

if(data->metka_rr == 0)
 {
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");
  return;
 }
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);

data->metka_voz=0;

}
/****************************/
/*Проверка записей          */
/*****************************/

int  sptbn_prov_row(SQL_str rows,class sptbn_data *data)
{

if(data->metka_pros == 2 && rows[2][0] == '0')
  return(1);
  
if(data->metka_pros == 1 && rows[2][0] != '0')
  return(2);

if(data->poi.metka_poi == 1)
 {
/**************
  if(data->poi.fio.getdlinna() > 1 )
   if(iceb_u_strstrm(rows[1],data->poi.fio.ravno()) == 0)
    return(3);

  if(data->poi.inn.getdlinna() > 1)
   if(iceb_u_strstrm(rows[3],data->poi.inn.ravno()) == 0)
    return(4);
*******************/
  if(iceb_u_proverka(data->poi.fio.ravno(),rows[1],4,0) != 0)
    return(3);

  if(iceb_u_proverka(data->poi.inn.ravno(),rows[3],4,0) != 0)
    return(4);


  if(data->poi.metka_nks.ravno_pr() == 1)
   if(rows[5][0] == '\0')
    return(5);

  if(data->poi.metka_nks.ravno_pr() == 2)
   if(rows[5][0] != '\0')
    return(6);

  if(iceb_u_proverka(data->poi.nomks.ravno(),rows[5],4,0) != 0)
    return(7);
 }


short mprov1=0;
memset(data->metka_zap,'\0',sizeof(data->metka_prov));

/*Проверяем наличие промежуточной записи*/
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select prov,blok from Zarn where god=%d and mes=%d \
and tabn=%s",data->gt,data->mt,rows[0]);

if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  strcpy(data->metka_zap,"V");    
  if(row[0][0] == '0')
   strcat(data->metka_zap,"*");
  else
   mprov1=1;
//  printw("row[0]=%s\n",row[0]);
  if(row[1][0] != '0')
   strcat(data->metka_zap,"#");
 }
else 
  strcpy(data->metka_zap,"*");    

if(data->metka_nast == 2 && data->metka_zap[0] == 'V')
  return(4);

if(data->metka_nast == 1 && data->metka_zap[0] == '*')
  return(5);

//printw("mprov=%d mprov1=%d \n",mprov,mprov1);
//OSTANOV();

if(data->metka_prov == 1 && mprov1 == 1)
  return(6);

   
return(0);
}
/***************************/
/*Распечатать*/
/****************/

void l_sptbn_ras(class sptbn_data *data)
{

char strsql[1024];
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

sprintf(strsql,"sptbn%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список работников"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Список работников"),0,0,0,0,0,0,ff,data->window);

fprintf(ff,"\
----------------------------------------------------------\n");

fprintf(ff,gettext("\
 Т/н |Фамилия Имя Отчество\n"));
  
fprintf(ff,"\
-----------------------------------------------------------\n");


char imafz[64];
sprintf(imafz,"sptnz%d.csv",getpid());

class iceb_fopen ffz;
if(ffz.start(imafz,"w",data->window) != 0)
 return;

fprintf(ffz.ff,"#%s %s\n",gettext("Список работников для загрузки в базу"),iceb_get_pnk("00",1,data->window));
time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

fprintf(ffz.ff,"\
#%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);



fprintf(ffz.ff,"#Описание полей в записи:\n\
#1  табельный номер\n\
#2  фамилия имя отчество\n\
#3  должность\n\
#4  звание\n\
#5  код подразделения\n\
#6  код категории\n\
#7  дата начала работы\n\
#8  дата конца работы\n\
#9  счёт учета\n\
#10 метка совместителя 0-не совм. 1-совместитель\n\
#11 индивидуальный налоговый номер\n\
#12 адрес\n\
#13 номер паспорта\n\
#14 кем выдан\n\
#15 телефон\n\
#16 код города налоговой инспекции\n\
#17 характер работы\n\
#18 код льгот для формы 8ДР\n\
#19 банковский счёт на который перечисляется зарплата\n\
#20 дата выдачи документа\n\
#21 тип кредитной карты\n\
#22 день рождения\n\
#23 пол 0-мужчина 1-женщина\n\
#24 дата последнего повышения зарплаты\n\
#25 код банка для перечисления зарплаты на карт счёт\n\
#26 код основания для учёта специального стажа\n");


SQL_str row1;
class SQLCURSOR cur1;
int kolzap=0;
while(cur.read_cursor(&row) != 0)
 {
  if(sptbn_prov_row(row,data) != 0)
    continue;

  sprintf(strsql,"select * from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден табельный номер"),row[0]);
    iceb_menu_soob(strsql,data->window);
    continue;
   }

  kolzap++;
  fprintf(ff,"%-5s %s\n",row[0],row[1]);

/*
0  tabn  табельный номер
1  fio   фамилия имя отчество
2  dolg  должность
3  zvan  звание
4  podr  подразделение
5  kateg категория
6  datn  дата начала работы
7  datk  дата конца работы
8  shet  счёт учета
9  sovm  метка совместителя 0-не совм. 1-совместитель
10 inn   индивидуальный налоговый номер
11 adres адрес
12 nomp  номер паспорта
13 vidan кем выдан
14 telef телефон
15 kodg  код города налоговой инспекции
16 harac характер работы
17 lgoti код льгот для формы 8ДР
18 bankshet банковский счёт на который перечисляется зарплата
19 datvd дата выдачи документа
20 tipkk тип кредитной карты
21 denrog день рождения
22 pl     0-мужчина 1-женщина
23 dppz   дата последнего повышения зарплаты
24 kb    код банка для перечисления зарплаты на карт счёт
25 kss код основания для учёта специального стажа
*/
 fprintf(ffz.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
 row1[0],row1[1],row1[2],row1[3],row1[4],row1[5],row1[6],row1[7],row1[8],row1[9],row1[10],row1[11],row1[12],row1[13],
 row1[14],row1[15],row1[16],row1[17],row1[18],row1[19],row1[20],row1[21],row1[22],row1[23],row1[24],row1[25]);

 }
fprintf(ff,"\
------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolzap);
iceb_podpis(ff,data->window);

fclose(ff);
ffz.end();

iceb_ustpeh(imaf.ravno(0),3,data->window);


imaf.plus(imafz);
naimot.plus(gettext("Список работников для загрузки в базу"));

iceb_rabfil(&imaf,&naimot,data->window);



}
