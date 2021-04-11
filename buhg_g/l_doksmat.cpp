/*$Id: l_doksmat.c,v 1.51 2014/03/31 06:05:33 sasa Exp $*/
/*09.01.2017	22.06.2004	Белых А.И.	l_doksmat.c
Просмотр списка документов в материальном учёте
*/
#include        <errno.h>
#include        "buhg_g.h"
#include        "l_doksmat.h"
#include "zar_eks_ko.h"
enum
{
 FK2,
 FK3,
 FK4,
 FK5,
 FK6,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_METKAZ,
 COL_DATAD,
 COL_NOMDOK,
 COL_KONTR,
 COL_KODOP,
 COL_SKLAD,
 COL_NOMNALNAK,
 COL_NOMPD,  
 COL_NAIMKONTR,
 COL_NAIMKODOP,
 COL_NAIMSKLAD,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  doksmat_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_metka;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *treeview;
  GtkWidget *window;
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  GtkWidget *radiobutton[3];
  GtkWidget *radiobutton1[2];

  //Реквизиты выбранной записи
  iceb_u_str datav;
  iceb_u_str nomdokv;
  iceb_u_str skladv;
  int        tipzv;
     
  short dp,mp,gp;
  short metka_r;   //0-все документы 1-не подтвержденные 2-без проводок
  short metka_sort_dat; /*0-в порядке возростания дат 1- в порядку убывания*/
  //реквизиты поискп
  static class doksmat_poi rk;
  class iceb_u_str zapros; //Запрос к базе донных
   
  doksmat_data()
   {
    metka_r=0;
    kl_shift=0;
    snanomer=0;
    treeview=NULL;
    metka_sort_dat=0;
   }

 };

class doksmat_poi doksmat_data::rk;


gboolean   doksmat_key_press(GtkWidget *widget,GdkEventKey *event,class doksmat_data *data);
void doksmat_vibor(GtkTreeSelection *selection,class doksmat_data *data);
void doksmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class doksmat_data *data);
void  doksmat_knopka(GtkWidget *widget,class doksmat_data *data);
void doksmat_add_columns (GtkTreeView *treeview);
void doksmat_create_list (class doksmat_data *data);
void l_doksmat_mv(class  doksmat_data *data);

void doksmat_radio0(GtkWidget *,class doksmat_data *);
void doksmat_radio1(GtkWidget *widget,class doksmat_data *data);

int l_doksmat_p(class doksmat_poi *,GtkWidget*);
int doksmat_prov_row(SQL_str row,class doksmat_poi *data,GtkWidget *wpredok);
void doksmat_ras(class doksmat_data *data);
int eks_dok_muw(short dd,short md,short gd,const char *nomdok,int sklad,class iceb_u_str *imaf,FILE *ff,GtkWidget *wpredok);

extern SQL_baza	bd;
extern short startgod; /*Стартовый год просмотров*/

void  l_doksmat(short dp,short mp,short gp,GtkWidget *wpredok)
{


class doksmat_data data;
data.rk.metka_poi=0; //0-без поиска 1- с поиском
data.rk.metka_opl=0; /*0-все 1-только оплаченные 2-только не оплаченные*/

char bros[1024];

data.dp=dp;
data.mp=mp;
data.gp=gp;
sprintf(bros,"01.01.%d",startgod);
data.rk.datan.new_plus(bros);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//gtk_window_set_default_size(GTK_WINDOW(data.window),400,300);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(bros,"%s %s (%s)",iceb_get_namesystem(),
gettext("Список документов"),
gettext("Материальный учёт"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(doksmat_key_press),&data);
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

sprintf(bros,"%s (%s)",
gettext("Список документов"),
gettext("Материальный учёт"));
data.label_kolstr=gtk_label_new (bros);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_metka=gtk_label_new ("");
data.label_poisk=gtk_label_new ("");


gtk_box_pack_start (GTK_BOX (vbox2),data.label_metka,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_size_request(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

GtkWidget *hboxradio = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous(GTK_BOX(hboxradio),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

//Вставляем радиокнопки

data.radiobutton[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Все"));
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[0], TRUE, TRUE, 0);

g_signal_connect(data.radiobutton[0], "clicked",G_CALLBACK(doksmat_radio0),&data);

sprintf(bros,"\"?\"");
data.radiobutton[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton[0]),bros);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[1], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton[1], "clicked",G_CALLBACK(doksmat_radio0),&data);

sprintf(bros,"\"*\"");
data.radiobutton[2]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton[1]),bros);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[2], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton[2], "clicked",G_CALLBACK(doksmat_radio0),&data);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_VERTICAL);
gtk_box_pack_start (GTK_BOX (hboxradio),separator1, TRUE, TRUE, 0);

/*вставляем вторую группу радиокнопок*/
data.radiobutton1[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("По возростанию дат"));
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton1[0], TRUE, TRUE, 0);

g_signal_connect(data.radiobutton1[0], "clicked",G_CALLBACK(doksmat_radio1),&data);

data.radiobutton1[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton1[0]),gettext("По убыванию дат"));
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton1[1], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton1[1], "clicked",G_CALLBACK(doksmat_radio1),&data);


gtk_box_pack_start(GTK_BOX (vbox2), hboxradio, FALSE,FALSE, 0);

gtk_widget_show_all(hboxradio);

//Кнопки

sprintf(bros,"F2 %s",gettext("Просмотр"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(doksmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Просмотр выбранного документа"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"F3 %s",gettext("Снять метку"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(doksmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Снять метку с документа"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(doksmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(doksmat_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Экспорт"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(doksmat_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Экспорт кассовых ордеров в подсистему \"Учёт кассовых оредров\""));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F9 %s",gettext("Оплата"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(doksmat_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Переключение режимов просмотра (только оплаченыые, только не оплаченные, все)"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show(data.knopka[FK9]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(doksmat_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);



gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

doksmat_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  doksmat_radio0(GtkWidget *widget,class doksmat_data *data)
{
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton[0])) == TRUE)
 {
  if(data->metka_r == 0)
   return;
  data->metka_r=0;
 }
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton[1])) == TRUE)
 {
  if(data->metka_r == 1)
   return;
  data->metka_r=1;
 }
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton[2])) == TRUE)
 {
  if(data->metka_r == 2)
   return;
  data->metka_r=2;
 }

doksmat_create_list(data);

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  doksmat_radio1(GtkWidget *widget,class doksmat_data *data)
{
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton1[0])) == TRUE)
 {
  if(data->metka_sort_dat == 0)
   return;
  data->metka_sort_dat=0;
 }
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton1[1])) == TRUE)
 {
  if(data->metka_sort_dat == 1)
   return;
  data->metka_sort_dat=1;
 }

doksmat_create_list(data);

}

/***********************************/
/*Создаем список для просмотра */
/***********************************/
void doksmat_create_list (class doksmat_data *data)
{

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

class iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR curr;
char strsql[1024];
int  kolstr=0;
SQL_str row;
SQL_str row1;
short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);
//GdkColor color;

//printf("doksmat_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(doksmat_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(doksmat_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


short dn=data->dp,mn=data->mp,gn=data->gp;
short dk=0,mk=0,gk=0;

if(data->rk.metka_poi == 1 && data->rk.datan.getdlinna() > 1)
 {
  iceb_rsdatp(&dn,&mn,&gn,data->rk.datan.ravno(),&dk,&mk,&gk,data->rk.datak.ravno(),data->window);
 }

if(gn == 0)
 gn=startgod;

if(gn == 0)
 gn=ggg;

sprintf(strsql,"select * from Dokummat where datd >= '%04d-01-01'",startgod);

if(dn != 0)
 sprintf(strsql,"select * from Dokummat where datd >= '%04d-%02d-%02d'",gn,mn,dn);

if(dn != 0 && dk != 0)
  sprintf(strsql,"select * from Dokummat where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",gn,mn,dn,gk,mk,dk);

data->zapros.new_plus(strsql);

if(data->metka_r == 1)
  data->zapros.plus(" and pod=0");

if(data->metka_r == 2)
  data->zapros.plus(" and pro=0");

if(data->metka_sort_dat == 0)
 data->zapros.plus(" order by datd asc,tip asc,nomd asc");
else
 data->zapros.plus(" order by datd desc,tip asc,nomd asc");

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
class iceb_u_str kodkon("");
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(doksmat_prov_row(row,&data->rk,data->window) != 0)
    continue;

  //Метка записи
  if(row[0][0] == '1')
   ss[COL_METKAZ].new_plus("+");
  else
   ss[COL_METKAZ].new_plus("-");

  if(row[7][0] == '0')
   ss[COL_METKAZ].plus("?");
  if(row[8][0] == '0')
   ss[COL_METKAZ].plus("*");
  if(row[14][0] == '0')
   ss[COL_METKAZ].plus("$");

  //Узнаём наименование контрагента
  
  iceb_kodkon(&kodkon,row[3]);  
  
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodkon.ravno());
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    ss[COL_NAIMKONTR].new_plus(row1[0]);
  else    
    ss[COL_NAIMKONTR].new_plus("");

  //Узнаём наименование операции
  if(row[0][0] == '1')
    sprintf(strsql,"select naik from Prihod where kod='%s'",row[6]);
  else  
    sprintf(strsql,"select naik from Rashod where kod='%s'",row[6]);
  
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    ss[COL_NAIMKODOP].new_plus(row1[0]);
  else    
    ss[COL_NAIMKODOP].new_plus("");

  //Узнаём наименование склада
   
  sprintf(strsql,"select naik from Sklad where kod=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    ss[COL_NAIMSKLAD].new_plus(row1[0]);
  else    
    ss[COL_NAIMSKLAD].new_plus("");
/*****************
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_vremzap(row[10]));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[9],data->window));
**********************/  

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKAZ,ss[COL_METKAZ].ravno(),
  COL_DATAD,iceb_u_datzap(row[1]),
  COL_NOMDOK,row[4],
  COL_KONTR,row[3],
  COL_KODOP,row[6],
  COL_SKLAD,row[2],
  COL_NOMNALNAK,row[5],
  COL_NOMPD,row[11],
  COL_NAIMKONTR,ss[COL_NAIMKONTR].ravno(),
  COL_NAIMKODOP,ss[COL_NAIMKODOP].ravno(),
  COL_NAIMSKLAD,ss[COL_NAIMSKLAD].ravno(),
  COL_DATA_VREM,iceb_u_vremzap(row[10]),
  COL_KTO,iceb_kszap(row[9],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

doksmat_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

class iceb_u_str stroka;
if(data->metka_r != 0)
 {
  if(data->metka_r == 1)
   stroka.new_plus(gettext("Просмотр только не подтверждённых документов"));
  if(data->metka_r == 2)
   stroka.new_plus(gettext("Просмотр только документов без проводок"));
  gtk_label_set_text(GTK_LABEL(data->label_metka),stroka.ravno());
  gtk_widget_show(data->label_metka);
 }
else 
 gtk_widget_hide(data->label_metka); 

iceb_u_str zagolov;
zagolov.plus(gettext("Список документов"));
zagolov.plus(" (");
zagolov.plus(gettext("Материальный учёт"));
zagolov.plus(")");

sprintf(strsql," %s:%d",gettext("Стартовый год"),startgod);
zagolov.plus(strsql);

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
sprintf(strsql,"\n%s: \"+\" %s \"-\" %s \"?\" %s \"*\" %s \"$\" %s",gettext("Метки"),gettext("приход"),gettext("расход"),gettext("не подтверждён"),gettext("без проводок"),gettext("не оплаченные"));
zagolov.plus(strsql);

if(data->dp != 0)
 {
  sprintf(strsql,"\n%s:%d.%d.%d",gettext("Дата начала просмотра"),data->dp,data->mp,data->gp);
  zagolov.plus(strsql);
  
 }

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->rk.metka_poi == 1 || data->rk.metka_opl != 0)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->rk.nomdok.ravno(),gettext("Номер документа"));
  iceb_str_poisk(&zagolov,data->rk.datan.ravno(),gettext("Дата ночала"));
  iceb_str_poisk(&zagolov,data->rk.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&zagolov,data->rk.kontr.ravno(),gettext("Контрагент"));
  iceb_str_poisk(&zagolov,data->rk.kodop.ravno(),gettext("Код операции"));
  iceb_str_poisk(&zagolov,data->rk.skl.ravno(),gettext("Склад"));
  iceb_str_poisk(&zagolov,data->rk.dover.ravno(),gettext("Доручення"));
  iceb_str_poisk(&zagolov,data->rk.nomnalnak.ravno(),gettext("Номер нал.нак."));
  iceb_str_poisk(&zagolov,data->rk.naim_kontr.ravno(),gettext("Наименование контрагента"));

  if(data->rk.pr == 1)
   {
    zagolov.ps_plus(gettext("Приход/расход"));
    zagolov.plus(": +");
   }
  if(data->rk.pr == 2)
   {
    zagolov.ps_plus(gettext("Приход/расход"));
    zagolov.plus(": -");
   }
  if(data->rk.metka_opl == 1)
   zagolov.ps_plus(gettext("Только оплаченные"));
  if(data->rk.metka_opl == 2)
   zagolov.ps_plus(gettext("Только неоплаченные"));

  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void doksmat_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Метка"),renderer,"text",COL_METKAZ,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_METKAZ);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Дата док-та"),renderer,"text",COL_DATAD,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_DATAD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Номер документа"),renderer,"text",COL_NOMDOK,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NOMDOK);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Контрагент"),renderer,"text",COL_KONTR,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_KONTR);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Операция"),renderer,"text",COL_KODOP,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_KODOP);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Склад"),renderer,"text",COL_SKLAD,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_SKLAD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Номер нал.нак."),renderer,"text",COL_NOMNALNAK,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NOMNALNAK);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Номер пар.док."),renderer,"text",COL_NOMPD,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NOMPD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Наименование контрагента"),renderer,"text",COL_NAIMKONTR,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NAIMKONTR);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Наименование операции"),renderer,"text",COL_NAIMKODOP,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NAIMKODOP);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Наименование склада"),renderer,"text",COL_NAIMSKLAD,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NAIMSKLAD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Дата и время записи"),renderer,"text",COL_DATA_VREM,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_VREM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Кто записал"),renderer,"text",COL_KTO,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_KTO);
gtk_tree_view_append_column (treeview, column);


}

/****************************/
/*Выбор строки*/
/**********************/

void doksmat_vibor(GtkTreeSelection *selection,class doksmat_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datad;
gchar *nomdok;
gchar *sklad;
gchar *tipz;

gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATAD,&datad,COL_NOMDOK,&nomdok,COL_SKLAD,&sklad,
COL_METKAZ,&tipz,NUM_COLUMNS,&nomer,-1);

data->datav.new_plus(datad);
data->nomdokv.new_plus(nomdok);
data->skladv.new_plus(sklad);

if(tipz[0] == '+')
 data->tipzv=1;
else
 data->tipzv=2;
 
data->snanomer=nomer;

g_free(datad);
g_free(nomdok);
g_free(sklad);
g_free(tipz);


}
/****************************************/
/*меню выбора снятия метки*/
/************************/
int l_doksmat_sm(class  doksmat_data *data)
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;
int nomer=0;
char strsql[1024];
titl.plus(gettext("Снять метку"));

zagolovok.plus(gettext("Снять метку"));

punkt_m.plus(gettext("Снять метку не подтверждённого документа"));//0
punkt_m.plus(gettext("Снять метку документа без проводок"));//1


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return(1);

    case 0:

    if(iceb_menu_danet("Снять метку неподтверждённого документа? Вы уверены?",2,data->window) == 2)
      return(1);    

     sprintf(strsql,"update Dokummat set pod=1 where datd='%s' and sklad=%d and nomd='%s'",data->datav.ravno_sqldata(),data->skladv.ravno_atoi(),data->nomdokv.ravno());
     iceb_sql_zapis(strsql,1,0,data->window);
     return(0);


    case 1:
    if(iceb_menu_danet("Снять метку документа без проводок? Вы уверены?",2,data->window) == 2)
      return(1);    

     sprintf(strsql,"update Dokummat set pro=1 where datd='%s' and sklad=%d and nomd='%s'",data->datav.ravno_sqldata(),data->skladv.ravno_atoi(),data->nomdokv.ravno());
     iceb_sql_zapis(strsql,1,0,data->window);
     return(0);
 
   }
 }
return(1);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  doksmat_knopka(GtkWidget *widget,class doksmat_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {

  case FK2:
    if(data->kolzap == 0)
     return;
    l_dokmat(&data->datav,data->tipzv,&data->skladv,&data->nomdokv,data->window);
    /*При выходе из документа устанвливаются метки на документ поэтому обязательно нужно перечитать список*/
    doksmat_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
     return;

    if(l_doksmat_sm(data) != 0)
      return;     
    doksmat_create_list(data);
    return;  

  case FK4:
    l_doksmat_p(&data->rk,data->window);
    doksmat_create_list(data);
    return;  

  case FK5:
    if(data->kolzap == 0)
     return;
    doksmat_ras(data);
    return;  

  case FK6:
    l_doksmat_mv(data);
    return;  

  case FK9:
    data->rk.metka_opl++;
    if(data->rk.metka_opl > 2)
     data->rk.metka_opl=0;    
    doksmat_create_list(data);
    return;  
    
  case FK10:
//    printf("doksmat_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   doksmat_key_press(GtkWidget *widget,GdkEventKey *event,class doksmat_data *data)
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

  case GDK_KEY_F6:
    g_signal_emit_by_name(data->knopka[FK6],"clicked");
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
    printf("doksmat_key_press-Нажата клавиша Shift\n");

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
void doksmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class doksmat_data *data)
{
//data->metkazapisi=1;
//g_signal_emit_by_name(data->knopka[SFK2],"clicked");
g_signal_emit_by_name(data->knopka[FK2],"clicked");

}

/******************************/
/*Проверка строки на условия поиска*/
/***********************************/

int doksmat_prov_row(SQL_str row,class doksmat_poi *data,GtkWidget *wpredok)
{
int metka_opl=atoi(row[14]);

if(data->metka_opl == 1 && metka_opl != 1)
  return(1);
if(data->metka_opl == 2 && metka_opl != 0)
  return(1);

if(data->metka_poi == 0)
 return(0);


//Поиск образца в строке
if(iceb_u_proverka(data->nomdok.ravno(),row[4],0,0) != 0)
  return(1);

if(data->pr == 1)
 if(row[0][0] != '1')
  return(1);

if(data->pr == 2)
 if(row[0][0] != '2')
  return(1);
 
if(data->kontr.getdlinna() > 1 || data->kodop.getdlinna() > 1 || data->skl.getdlinna() > 1)
 {
  SQL_str row1;
  SQLCURSOR cur;
  char strsql[512];
  sprintf(strsql,"select sklad,kontr,nomnn,kodop from Dokummat where datd='%s' and \
sklad=%s and nomd='%s' and tip=%s",row[1],row[2],row[4],row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена шапка документа !"));
    repl.ps_plus(row[4]);
    repl.plus(" ");
    repl.plus(row[0]);
    repl.plus(" ");
    repl.plus(row[2]);
    
    iceb_menu_soob(&repl,wpredok);

   }
  else
   {
    //Поиск образца в строке
    if(iceb_u_proverka(data->skl.ravno(),row1[0],0,0) != 0)
      return(1);

    //Поиск образца в строке
    if(iceb_u_proverka(data->kontr.ravno(),row1[1],0,0) != 0)
      return(1);

    //Поиск образца в строке
    if(iceb_u_proverka(data->nomnalnak.ravno(),row1[2],0,0) != 0)
      return(1);

    //Поиск образца в строке
    if(iceb_u_proverka(data->kodop.ravno(),row1[3],0,0) != 0)
      return(1);
   }

 }

if(data->dover.getdlinna() > 1 )
 {
  SQL_str row1;
  SQLCURSOR cur;
  char strsql[512];
  short d,m,god;
  iceb_u_rsdat(&d,&m,&god,row[1],2);
  
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and \
sklad=%s and nomd='%s' and nomerz=1",god,row[2],row[4]);
  if(iceb_sql_readkey(strsql,&row1,&cur,wpredok) == 1)
   {
    class iceb_u_str dover("");
    iceb_u_polen(row1[0],&dover,1,'#');
    //Поиск образца в строке
    if(iceb_u_proverka(data->dover.ravno(),dover.ravno(),4,0) != 0)
      return(1);
   }
  else
   return(1);
 }

if(data->naim_kontr.getdlinna() > 1)
 {
  SQL_str row1;
  SQLCURSOR cur1;
  char strsql[512];
  class iceb_u_str kodkontr("");

  if(iceb_u_polen(row[3],&kodkontr,1,'-') != 0)
   kodkontr.new_plus(row[3]);

  sprintf(strsql,"select naikon,pnaim from Kontragent where kodkon='%s'",kodkontr.ravno_filtr());
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   {
    if(iceb_u_proverka(data->naim_kontr.ravno(),row1[0],4,0) != 0)
     if(iceb_u_proverka(data->naim_kontr.ravno(),row1[1],4,0) != 0)
      return(1);     
   }
 }
 
return(0);
}
/****************************/
/*Распечатка*/
/***********************/
void doksmat_ras(class doksmat_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
iceb_clock sss(data->window);

SQLCURSOR cur;
SQLCURSOR curr;
int  kolstr=0;
SQL_str row;
SQL_str row1;
char strsql[512];

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
if(kolstr == 0)
 return;

char imaf[64];
FILE *ff;

sprintf(imaf,"doksm%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

iceb_u_zagolov(gettext("Список документов"),0,0,0,0,0,0,iceb_get_pnk("00",0,data->window),ff);
fprintf(ff,"\"+\"-%s\n",gettext("отмечены приходные документы"));
fprintf(ff,"\"-\"-%s\n",gettext("отмечены расходные документы"));
fprintf(ff,"\"?\"-%s\n",gettext("отмечены не подтверждённые документы"));
fprintf(ff,"\"*\"-%s\n",gettext("отмечены документы без проводок"));

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
Метка|  Дата    |Номер документа|      Контрагент    |    Операция        |        Склад       |Номер н.н.|Номер р.д.|Дата и время записи|Кто записал\n");
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------\n");
class iceb_u_str kodkon("");
iceb_u_str metka;
iceb_u_str datad;
iceb_u_str kontr;
iceb_u_str kodop;
iceb_u_str sklad;
iceb_u_str dataz;
iceb_u_str ktoz;
double kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(doksmat_prov_row(row,&data->rk,data->window) != 0)
    continue;

  //Метка записи
  if(row[0][0] == '1')
   metka.new_plus("+");
  else
   metka.new_plus("-");

  if(row[7][0] == '0')
   metka.plus("?");
  if(row[8][0] == '0')
   metka.plus("*");

  //Дата документа
  datad.new_plus(iceb_u_datzap(row[1]));
  

  //Контрагент
  kontr.new_plus(row[3]);
  kontr.plus(" ");

  //Узнаём наименование контрагента
  iceb_kodkon(&kodkon,row[3]);  
  
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodkon.ravno());
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    kontr.plus(row1[0]);


  //Код операции
  kodop.new_plus(row[6]);
  kodop.plus(" ");

  //Узнаём наименование операции
  if(row[0][0] == '1')
    sprintf(strsql,"select naik from Prihod where kod='%s'",row[6]);
  else  
    sprintf(strsql,"select naik from Rashod where kod='%s'",row[6]);
  
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    kodop.plus(row1[0]);

  //Склад
  sklad.new_plus(row[2]);
  sklad.plus(" ");

  //Узнаём наименование склада
   
  sprintf(strsql,"select naik from Sklad where kod=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    sklad.plus(row1[0]);

  //Дата и время записи
  dataz.new_plus(iceb_u_vremzap(row[10]));

  //Кто записал
  ktoz.new_plus(iceb_kszap(row[9],data->window));
  
  fprintf(ff,"%-5s %-10s %-*s %-*.*s %-*.*s %-*.*s %-*s %-10s %s %s\n",
  metka.ravno(),
  datad.ravno(),
  iceb_u_kolbait(15,row[4]),
  row[4],
  iceb_u_kolbait(20,kontr.ravno()),
  iceb_u_kolbait(20,kontr.ravno()),
  kontr.ravno(),
  iceb_u_kolbait(20,kodop.ravno()),
  iceb_u_kolbait(20,kodop.ravno()),
  kodop.ravno(),
  iceb_u_kolbait(20,sklad.ravno()),
  iceb_u_kolbait(20,sklad.ravno()),
  sklad.ravno(),
  iceb_u_kolbait(10,row[5]),
  row[5],
  row[11],
  dataz.ravno(),
  ktoz.ravno());
  
 }

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------\n");

iceb_podpis(ff,data->window);

fclose(ff);
gdite.close();

class iceb_u_spisok imafs;
class iceb_u_spisok naim;

imafs.plus(imaf);
naim.plus(gettext("Список документов"));

iceb_ustpeh(imaf,3,data->window);
iceb_rabfil(&imafs,&naim,data->window);

}



/*************************************/
/*Экспорт кассовых ордеров*/
/***************************************/
void l_doksmat_eko_r(class doksmat_data *data,class zar_eks_ko_rek *rek)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[1024];
short vt=0;
class iceb_u_str shetk("");
class iceb_u_str naimop("");

//Проверяем код операции
if(rek->prn == 1)
 sprintf(strsql,"select naik,shetkor from Kasop1 where kod='%s'",rek->kodop.ravno());
if(rek->prn == 2)
 sprintf(strsql,"select naik,shetkor from Kasop2 where kod='%s'",rek->kodop.ravno());

if(iceb_sql_readkey(strsql,&row,&cur1,data->window) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),rek->kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  return;
 }

naimop.new_plus(row[0]);
shetk.new_plus(row[1]);



iceb_poldan("Код группы возвратная тара",strsql,"matnast.alx",data->window);
vt=atoi(strsql);

class iceb_u_spisok kod_kontr;
class iceb_u_spisok koment_kontr;
class iceb_u_double suma_kontr;


short		dko,mko,gko;
iceb_u_rsdat(&dko,&mko,&gko,rek->data.ravno(),1);
int kolih_kasord=0;
class iceb_u_str nomd("");
class iceb_u_str koment("");
double suma=0.;
double sumabn=0.;
double sumkor=0.;
short d,m,g;
int lnds=0;
double kolih=0.;
double suma_nds=0.;
double suma_dok=0.;
float pnds=0.;
int tipz_mat=0;
if(rek->prn == 1)
 tipz_mat=2;
else
 tipz_mat=1;
int nomer_kontr=0;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(rek->prn == atoi(row[0])) /*Если приходный кассовый ордер то нам нужны расходные документы и наооборот*/
   continue;
  
 if(doksmat_prov_row(row,&data->rk,data->window) != 0)
    continue;
 //if(sravdok(row,poisk) != 0)
//    continue;
  
  iceb_u_rsdat(&d,&m,&g,row[1],2);

  pnds=atof(row[13]);

  /*Узнаем НДС документа*/
  lnds=0;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' and sklad=%s and nomerz=11",g,row[4],row[3]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    lnds=atoi(row1[0]);
  suma_nds=0.;
  if(lnds == 0)
   {
    /*Узнаем сумму НДС документа*/
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=6",g,row[4],row[3]);

    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      suma_nds=atof(row1[0]);
   }

  sumzap1w(d,m,g,row[4],atoi(row[2]),&suma,&sumabn,lnds,vt,&sumkor,tipz_mat,&kolih,data->window);

  if(lnds == 0 && suma_nds == 0.)
    suma_nds=(suma+sumkor)*pnds/100.;
  
/*  printw("suma=%f sumabn=%f sumkor=%f\n",suma,sumabn,sumkor);*/
  suma_dok=suma+sumkor+suma_nds+sumabn;
    
  sprintf(strsql,"%s:%.10g",gettext("Количество"),kolih);  
  koment.new_plus(strsql);

  if(rek->metka_r == 0)
   {
    kolih_kasord++;
    iceb_lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
    iceb_nomnak(gko,rek->kassa.ravno(),&nomd,rek->prn,2,1,data->window);
    zaphkorw(0,rek->kassa.ravno(),rek->prn,dko,mko,gko,nomd.ravno(),shetk.ravno(),rek->kodop.ravno(),naimop.ravno(),"","","","","",0,0,0,"","",data->window);

    printf("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());

    zapzkorw(rek->kassa.ravno(),rek->prn,dko,mko,gko,nomd.ravno(),row[4],suma_dok,"",koment.ravno(),data->window);
   }
  else
   {
    if((nomer_kontr=kod_kontr.find_r(row[3])) < 0)
     {
      kod_kontr.plus(row[3]);
      koment_kontr.plus(koment.ravno());
     }

    suma_kontr.plus(suma_dok,nomer_kontr);


   }  

 }

if(rek->metka_r == 1)
 {
  kolih_kasord++;
  iceb_lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
  iceb_nomnak(gko,rek->kassa.ravno(),&nomd,rek->prn,2,1,data->window);
  zaphkorw(0,rek->kassa.ravno(),rek->prn,dko,mko,gko,nomd.ravno(),shetk.ravno(),rek->kodop.ravno(),naimop.ravno(),"","","","","",0,0,0,"","",data->window);
  printf("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());

  for(nomer_kontr=0; nomer_kontr < kod_kontr.kolih(); nomer_kontr++)
   {
    
    zapzkorw(rek->kassa.ravno(),rek->prn,dko,mko,gko,nomd.ravno(),kod_kontr.ravno(nomer_kontr),suma_kontr.ravno(nomer_kontr),"",koment_kontr.ravno(nomer_kontr),data->window);
   }
 }

sprintf(strsql,"%s:%d",gettext("Количество экспортированых кассовых ордеров"),kolih_kasord);
iceb_menu_soob(strsql,data->window);

}
/*****************************/
/*Експорт накладных в cvs файл*/
/*******************************/
void l_doksmat_eks_nakl(class doksmat_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

short dd=0,md=0,gd=0;
class iceb_u_str imaf("eksdokmu");
imaf.plus(getpid());
imaf.plus(".csv");

class iceb_fopen fil;
if(fil.start(imaf.ravno(),"w",data->window) != 0)
 return;

iceb_zageks(gettext("Экспорт накладных из подсистемы \"Материальный учёт\""),fil.ff,data->window);

short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);
class iceb_u_str zapros;

short dn=data->dp,mn=data->mp,gn=data->gp;
short dk=0,mk=0,gk=0;

if(data->rk.metka_poi == 1 && data->rk.datan.getdlinna() > 1)
 {
  iceb_rsdatp(&dn,&mn,&gn,data->rk.datan.ravno(),&dk,&mk,&gk,data->rk.datak.ravno(),data->window);
 }

if(gn == 0)
 gn=startgod;

if(gn == 0)
 gn=ggg;

sprintf(strsql,"select * from Dokummat where datd >= '%04d-01-01'",gn);

if(dn != 0)
 sprintf(strsql,"select * from Dokummat where datd >= '%04d-%02d-%02d'",gn,mn,dn);

if(dn != 0 && dk != 0)
  sprintf(strsql,"select * from Dokummat where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",gn,mn,dn,gk,mk,dk);

zapros.new_plus(strsql);

if(data->metka_r == 1)
  zapros.plus(" and pod=0");

if(data->metka_r == 2)
  zapros.plus(" and pro=0");

/*первыми должны выгружаться приходные документы*/
zapros.plus(" order by tip asc,datd asc,nomd asc");

if((kolstr=cur.make_cursor(&bd,zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
printf("%s\n",__FUNCTION__);

float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  if(doksmat_prov_row(row,&data->rk,data->window) != 0)
    continue;

  iceb_u_rsdat(&dd,&md,&gd,row[1],2);
  
  eks_dok_muw(dd,md,gd,row[4],atoi(row[2]),&imaf,fil.ff,data->window);

 }
fil.end();
gdite.close();

class spis_oth oth;
oth.spis_imaf.plus(imaf.ravno());
oth.spis_naim.plus(gettext("Экспорт накладных"));

iceb_rabfil(&oth,data->window);

}

/***********************************/
/*Експорт кассовых ордеров*/
/****************************/

void l_dokmat_eko(class  doksmat_data *data)
{
class zar_eks_ko_rek rek;

  
if(eks_ko_v(&rek,data->window) != 0)
 return;
l_doksmat_eko_r(data,&rek);

}
/****************************************/
/*меню выбора экспорта*/
/************************/
void l_doksmat_mv(class  doksmat_data *data)
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Экспорт"));

zagolovok.plus(gettext("Экспорт документов"));

punkt_m.plus(gettext("Экспорт документов в файл формата csv"));//0
punkt_m.plus(gettext("Экспорт кассовых ордеров в подсистему \"Учёт кассовых ордеров\""));//1


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
     l_doksmat_eks_nakl(data);
     break;


    case 1:
      l_dokmat_eko(data);
      break;
 
   }
 }

}

