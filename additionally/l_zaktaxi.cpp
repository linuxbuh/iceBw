/*$Id: l_zaktaxi.c,v 1.1 2013/08/13 06:26:34 sasa Exp $*/
/*12.05.2015	21.06.2006	Белых А.И.	l_zaktaxi.c
Работа со списком категорий
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include <math.h>
#include  "taxi.h"
#include  "l_zaktaxi.h"

/**************
  "M",
  "Дата и время заказа",
  "Адрес подачи",
  "Адрес поездки",
  "Телефон",
  "Код клиента",
  "Водитель",
  "Гос.номер",
  "Код завершения",
  "К/п",
  "Коментарий",
  "Фамилия",
  "Сумма",
  "Дата и время звонка",
  "Дата и время завершения",
  "Дата и время записи",
  "Оператор"
*************/

enum
{
  COL_M,
  COL_DATA_TIME,
  COL_ADRES_START,
  COL_ADRES_END,
  COL_TELEF,
  COL_KOD_KLIENTA,
  COL_VODITEL,
  COL_GOS_NOMER,
  COL_KOD_ZAVER,
  COL_KOLIH_PAS,
  COL_KOMENT,
  COL_FIO,
  COL_SUMA,
  COL_DATA_TIME_ZAK,
  COL_DATA_TIME_UKZ,
  COL_DATA_TIME_WRITE,
  COL_OPERATOR,  
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

class  l_zaktaxi_data
 {
  public:

  class l_zaktaxi_rek rp;

  gint      timer_read;
  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *labelkz;
  GtkWidget *sw;
  GtkWidget *spin1;
  GtkWidget *spin;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *hboxradio;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *radiobutton0; //Не завершенные заказы
  GtkWidget *radiobutton1; //Все заказы
  GtkWidget *radiobutton2; //Сутки
  GtkWidget *radiobutton3; //Первые 10 заказов
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int unz;  /*уникальный номер выбранной записи*/
  int       kolzap;     //Количество записей
  short     metkapoi;  //0-без поиска 1-с поиском
  int       metka_voz;  //0-выбрали 1-нет  
  short     metkarr;   //только незавершённые
  //Конструктор
  l_zaktaxi_data()
   {
    unz=0;
    snanomer=0;
    metkarr=metka_voz=kl_shift=metkapoi=0;
    window=treeview=NULL;
   }      
 };

gboolean   l_zaktaxi_key_press(GtkWidget *widget,GdkEventKey *event,class l_zaktaxi_data *data);
void l_zaktaxi_vibor(GtkTreeSelection *selection,class l_zaktaxi_data *data);
void l_zaktaxi_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zaktaxi_data *data);
void  l_zaktaxi_knopka(GtkWidget *widget,class l_zaktaxi_data *data);
static void l_zaktaxi_add_columns (GtkTreeView *treeview);
void l_zaktaxi_udzap(class l_zaktaxi_data *data);
int  l_zaktaxi_prov_row(SQL_str row,class l_zaktaxi_data *data);

void  l_zaktaxi_radio0(GtkWidget *widget,class l_zaktaxi_data *data);
void  l_zaktaxi_radio1(GtkWidget *widget,class l_zaktaxi_data *data);
void  l_zaktaxi_radio2(GtkWidget *widget,class l_zaktaxi_data *data);
void  l_zaktaxi_radio3(GtkWidget *widget,class l_zaktaxi_data *data);
static void l_zaktaxi_knopaos(GtkWidget *widget,class l_zaktaxi_data *data);

void l_zaktaxi_zapros(char *strsql,class  l_zaktaxi_data *data);
int lzaktaxiv(int un_nom_zap,GtkWidget *wpredok);
int lzaktaxi_p(class l_zaktaxi_rek *rek_poi,GtkWidget *wpredok);
void l_zaktaxi_ras(class l_zaktaxi_data *data);

class l_zaktaxi_data data;


extern SQL_baza  bd;

int l_zaktaxi(GtkWidget *wpredok)
{
char bros[512];

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список заказов такси"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zaktaxi_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список заказов такси"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start(GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);


data.hboxradio = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(data.hboxradio),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start(GTK_BOX(vbox2), data.hboxradio, FALSE, TRUE, 0);

data.metkarr=0;

//Вставляем радиокнопки
GSList *group=NULL;

data.radiobutton0=gtk_radio_button_new_with_label(NULL,"Незавершенные заказы");
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton0, TRUE, TRUE, 0);
g_signal_connect(data.radiobutton0, "clicked",G_CALLBACK(l_zaktaxi_radio0),&data);
//gtk_widget_set_name(data.radiobutton0,iceb_u_inttochar("0"));

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton0));

data.radiobutton1=gtk_radio_button_new_with_label(group,"Все заказы");
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton1, TRUE, TRUE, 0);
g_signal_connect(data.radiobutton1, "clicked",G_CALLBACK(l_zaktaxi_radio1),&data);

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton1));

data.radiobutton2=gtk_radio_button_new_with_label(group,"Сутки");
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton2, TRUE, TRUE, 0);
g_signal_connect(data.radiobutton2, "clicked",G_CALLBACK(l_zaktaxi_radio2),&data);

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton2));

data.radiobutton3=gtk_radio_button_new_with_label(group,"Первые");
g_signal_connect(data.radiobutton3, "clicked",G_CALLBACK(l_zaktaxi_radio3),&data);
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton3, TRUE, TRUE, 0);
//кнопку устанавливаем активной после отображения окна
//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton3),TRUE); //Устанавливем активной кнопку

//Вставляем настройку количества записей
GtkAdjustment *adjustment1=gtk_adjustment_new (10.,1.,1000.,1.,0.,0.);
data.spin1=gtk_spin_button_new(GTK_ADJUSTMENT(adjustment1),1.0,0);
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.spin1, TRUE, TRUE, 0);
gtk_widget_set_tooltip_text(data.spin1,"Установка количества записей для просмотра");

//Вставляем кнопку автоматического обновления списка
GtkWidget *knopaos=gtk_check_button_new_with_label("Автоматическое обновление списка");
g_signal_connect(knopaos, "toggled",G_CALLBACK(l_zaktaxi_knopaos),&data);
gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(knopaos),FALSE);
gtk_box_pack_start (GTK_BOX (data.hboxradio),knopaos, TRUE, TRUE, 0);

//Вставляем настройку автоматического обновления списка
GtkAdjustment *adjustment=gtk_adjustment_new (30.,1.,1000.,1.,0.,0.);
data.spin=gtk_spin_button_new(GTK_ADJUSTMENT(adjustment),1.0,0);
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.spin, TRUE, TRUE, 0);

gtk_widget_set_tooltip_text(data.spin,"Установка интервала автоматического обновления списка в секундах");

sprintf(bros,"К/з: 0");
data.labelkz=gtk_label_new(bros);
gtk_box_pack_end(GTK_BOX(data.hboxradio),data.labelkz, TRUE, TRUE, 0);






//Кнопки

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_zaktaxi_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_zaktaxi_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_zaktaxi_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_zaktaxi_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_zaktaxi_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_zaktaxi_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
//gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_zaktaxi_create_list(&data);

gtk_widget_show_all(data.window);
gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.metka_voz);

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  l_zaktaxi_radio0(GtkWidget *widget,class l_zaktaxi_data *data)
{
if(data->metkarr == 0)
  return;
data->metkarr=0;

l_zaktaxi_create_list(data);

}
void  l_zaktaxi_radio1(GtkWidget *widget,class l_zaktaxi_data *data)
{
if(data->metkarr == 1)
  return;
data->metkarr=1;
l_zaktaxi_create_list(data);
}

void  l_zaktaxi_radio2(GtkWidget *widget,class l_zaktaxi_data *data)
{
if(data->metkarr == 2)
  return;

data->metkarr=2;
l_zaktaxi_create_list(data);

}
void  l_zaktaxi_radio3(GtkWidget *widget,class l_zaktaxi_data *data)
{
if(data->metkarr == 3)
  return;

data->metkarr=3;
l_zaktaxi_create_list(data);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
gboolean l_zaktaxi_create_list (class l_zaktaxi_data *data)
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

printf("%s\n",__FUNCTION__);

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_zaktaxi_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_zaktaxi_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_COLUMNS+2, 
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
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT,
G_TYPE_INT);


l_zaktaxi_zapros(strsql,data);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  gdite.close();
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(FALSE);
 }
//gtk_list_store_clear(model);

data->kolzap=0;
float kolstr1=0. ;
class iceb_u_str metka_nz("");
class iceb_u_str data_time("");
class iceb_u_str voditel("");
class iceb_u_str gosnomer("");
class iceb_u_str kod_zaver("");
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(l_zaktaxi_prov_row(row,data) != 0)
    continue;

  if(row[0][0] == '0')
   metka_nz.new_plus("?");
  else
   metka_nz.new_plus("");

  data_time.new_plus(iceb_u_datzap(row[6]));
  data_time.plus(" ",row[7]);
  /*код водителя*/  
  if(row[2][0] != '\0')
   {
    sprintf(strsql,"select fio,gosn from Taxivod where kod='%s'",row[2]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      voditel.new_plus(row[2]);
      voditel.plus(" ",row1[0]);
      gosnomer.new_plus(row1[1]);
     }
   }
  else
   {
    voditel.new_plus("");
    gosnomer.new_plus("");
   }

  //Код завершения
  if(atoi(row[0]) != 0)
   { 
    sprintf(strsql,"select naik from Taxikzz where kod=%s",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     kod_zaver.new_plus(row1[0]);
   }
  else
    kod_zaver.new_plus("");

  //Дата и время заказа
  class iceb_u_str data_time_zak(iceb_u_datzap(row[8]));
  data_time_zak.plus(" ",row[9]);


  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_M,metka_nz.ravno(),
  COL_DATA_TIME,data_time.ravno(),
  COL_ADRES_START,row[10],
  COL_ADRES_END,row[11],
  COL_TELEF,row[4],
  COL_KOD_KLIENTA,row[1],
  COL_VODITEL,voditel.ravno(),
  COL_GOS_NOMER,gosnomer.ravno(),
  COL_KOD_ZAVER,kod_zaver.ravno(),
  COL_KOLIH_PAS,row[5],
  COL_KOMENT,row[12],
  COL_FIO,row[3],
  COL_SUMA,row[13],
  COL_DATA_TIME_ZAK,data_time_zak.ravno(),  
  COL_DATA_TIME_UKZ,iceb_u_vremzap(row[14]),
  COL_DATA_TIME_WRITE,iceb_u_vremzap(row[16]),
  COL_OPERATOR,iceb_kszap(row[15],data->window),
  NUM_COLUMNS,atoi(row[17]),
  NUM_COLUMNS+1,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_zaktaxi_add_columns (GTK_TREE_VIEW (data->treeview));


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
zagolov.plus(gettext("Список заказов такси"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->metkapoi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->rp.telef.ravno(),gettext("Телефон клиента"));
  iceb_str_poisk(&zagolov,data->rp.kodk.ravno(),gettext("Код клиента"));
  iceb_str_poisk(&zagolov,data->rp.kv.ravno(),gettext("Код водителя"));
  iceb_str_poisk(&zagolov,data->rp.fio.ravno(),gettext("Фамилия клиента"));
  iceb_str_poisk(&zagolov,data->rp.kolp.ravno(),gettext("Количество пассажиров"));
  iceb_str_poisk(&zagolov,data->rp.adreso.ravno(),gettext("Адрес посадки"));
  iceb_str_poisk(&zagolov,data->rp.adresk.ravno(),gettext("Адрес поездки"));
  iceb_str_poisk(&zagolov,data->rp.koment.ravno(),gettext("Коментарий"));
  iceb_str_poisk(&zagolov,data->rp.suma.ravno(),gettext("Сумма"));
  iceb_str_poisk(&zagolov,data->rp.kodzav.ravno(),gettext("Код завершения"));
  iceb_str_poisk(&zagolov,data->rp.datan.ravno(),gettext("Дата начала поиска"));
  iceb_str_poisk(&zagolov,data->rp.datak.ravno(),gettext("Дата конца поиска"));
  iceb_str_poisk(&zagolov,data->rp.kod_operatora.ravno(),gettext("Код оператора"));
 

  iceb_label_set_text_color(data->label_poisk,zagolov.ravno(),"red");
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

sprintf(strsql,"К/з: %d",data->kolzap);

gtk_label_set_text(GTK_LABEL(data->labelkz),strsql);

gtk_widget_show(data->label_kolstr);
return(TRUE);
}

/*****************/
/*Создаем колонки*/
/*****************/

static void l_zaktaxi_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("M",renderer,"text",COL_M,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_M);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата/время заказа"),renderer,"text",COL_DATA_TIME,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_TIME);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Адрес подачи"),renderer,"text",COL_ADRES_START,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ADRES_START);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Адрес поездки"),renderer,"text",COL_ADRES_END,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ADRES_END);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Телефон"),renderer,"text",COL_TELEF,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_TELEF);
gtk_tree_view_append_column (treeview, column);



renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код клиента"),renderer,"text",COL_KOD_KLIENTA,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOD_KLIENTA);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Водитель"),renderer,"text",COL_VODITEL,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_VODITEL);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Гос.номер"),renderer,"text",COL_GOS_NOMER,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_GOS_NOMER);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код завер."),renderer,"text",COL_KOD_ZAVER,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOD_ZAVER);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кол.пас."),renderer,"text",COL_KOLIH_PAS,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOLIH_PAS);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Коментарий"),renderer,"text",COL_KOMENT,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOMENT);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Фамилия"),renderer,"text",COL_FIO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_FIO);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Сумма"),renderer,"text",COL_SUMA,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_SUMA);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Время звонка"),renderer,"text",COL_DATA_TIME_ZAK,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_TIME_ZAK);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Время завершения"),renderer,"text",COL_DATA_TIME_UKZ,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_TIME_UKZ);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Время записи"),renderer,"text",COL_DATA_TIME_WRITE,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_TIME_WRITE);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Оператор"),renderer,"text",COL_OPERATOR,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_OPERATOR);
gtk_tree_view_append_column (treeview, column);
}

/****************************/
/*Выбор строки*/
/**********************/

void l_zaktaxi_vibor(GtkTreeSelection *selection,class l_zaktaxi_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gint nomer=0;
gint unz=0;

gtk_tree_model_get(model,&iter,NUM_COLUMNS,&unz,NUM_COLUMNS+1,&nomer,-1);

data->snanomer=nomer;
data->unz=unz; 


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_zaktaxi_knopka(GtkWidget *widget,class l_zaktaxi_data *data)
{
iceb_u_str repl;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_zaktaxi_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    lzaktaxiv(0,data->window); /*обновлениесписка не запускаем так как из меню после записис не выходим и обновление запускаем там*/
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;

    if(lzaktaxiv(data->unz,data->window) == 0)
      l_zaktaxi_create_list(data);
    
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_zaktaxi_udzap(data);
    l_zaktaxi_create_list(data);
    return;  
  


  case FK4:
    if(lzaktaxi_p(&data->rp,data->window) == 0)
     data->metkapoi=1;
    else
     data->metkapoi=0;
    l_zaktaxi_create_list(data);
    return;  

  case FK5:
    l_zaktaxi_ras(data);
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

gboolean   l_zaktaxi_key_press(GtkWidget *widget,GdkEventKey *event,class l_zaktaxi_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {

  case GDK_KEY_F2:

    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    else
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    data->kl_shift=0; /*обязательно сбрасываем*/
    return(TRUE);
   
  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    data->kl_shift=0; /*обязательно сбрасываем*/
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
void l_zaktaxi_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zaktaxi_data *data)
{
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");
  return;


}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_zaktaxi_udzap(class l_zaktaxi_data *data)
{
char strsql[512];
sprintf(strsql,"delete from Taxizak where nz=%d",data->unz);
iceb_sql_zapis(strsql,1,0,data->window);

}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_zaktaxi_prov_row(SQL_str row,class l_zaktaxi_data *data)
{
if(data->metkapoi == 0)
 return(0);
//Полное сравнение
if(iceb_u_proverka(data->rp.kodk.ravno(),row[1],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rp.kv.ravno(),row[2],0,0) != 0)
   return(1);

//Поиск образца в строке
if(data->rp.fio.getdlinna() > 1 && iceb_u_strstrm(row[3],data->rp.fio.ravno()) == 0)
 return(1);

if(data->rp.telef.getdlinna() > 1 && iceb_u_strstrm(row[4],data->rp.telef.ravno()) == 0)
 return(1);

if(data->rp.kolp.getdlinna() > 1)
 if(atoi(data->rp.kolp.ravno()) != atoi(row[5]))
  return(1);

if(data->rp.adreso.getdlinna() > 1 && iceb_u_strstrm(row[10],data->rp.adreso.ravno()) == 0)
 return(1);

if(data->rp.adresk.getdlinna() > 1 && iceb_u_strstrm(row[11],data->rp.adresk.ravno()) == 0)
 return(1);

if(data->rp.koment.getdlinna() > 1 && iceb_u_strstrm(row[12],data->rp.koment.ravno()) == 0)
 return(1);

if(data->rp.suma.getdlinna() > 1)
 if(fabs(atof(data->rp.suma.ravno())-atof(row[13])) > 0.009)
  return(1);

if(iceb_u_proverka(data->rp.kodzav.ravno(),row[0],0,0) != 0)
   return(1);


//Полное сравнение
if(iceb_u_proverka(data->rp.kod_operatora.ravno(),row[15],0,0) != 0)
 return(1);

return(0);
}

/*********************************/
/*Формирование строки запроса    */
/**********************************/

void l_zaktaxi_zapros(char *strsql,class  l_zaktaxi_data *data)
{
char  bros[300];

if(data->metkapoi == 0)
 {
  gtk_widget_show(data->hboxradio); //Показать

  if(data->metkarr == 0 ) //Только не завершенные
    sprintf(strsql,"select * from Taxizak where kz=0 order by datvz asc,vremvz asc");

  if(data->metkarr == 1) //Все
    sprintf(strsql,"select * from Taxizak order by datvz desc,vremvz desc");

  if(data->metkarr == 2) //За последние сутки
   {
    time_t vrem;
    time(&vrem);
    vrem-=24*60*60;

    sprintf(strsql,"select * from Taxizak where vrem >=%ld order by datvz desc,vremvz desc",
    vrem);
    printf("%s\n",strsql);          
   }

  if(data->metkarr == 3) //Первые 10
   {
    int kolzap=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(data->spin1));
    sprintf(strsql,"select * from Taxizak order by datvz desc,vremvz desc limit %d",kolzap);
   }
 }
else
 {

  gtk_widget_hide(data->hboxradio); //Скрываем от показа виджет

  sprintf(strsql,"select * from Taxizak ");
  memset(bros,'\0',sizeof(bros));  
  if(data->rp.datan.getdlinna() > 1)
   {
    sprintf(bros,"where datvz >= '%s' ",data->rp.datan.ravno_sqldata());
    strcat(strsql,bros);
    if(data->rp.datak.getdlinna() > 1)
     {
      sprintf(bros,"and datvz <= '%s' ",data->rp.datak.ravno_sqldata());
      strcat(strsql,bros);
     }
   }
  else
    if(data->rp.datak.getdlinna() > 1)
     {
      sprintf(bros,"where datvz <= '%s' ",data->rp.datak.ravno_sqldata());
      strcat(strsql,bros);
     }
  

  sprintf(bros,"order by datvz,vremvz desc");
  strcat(strsql,bros);
  
 } 
//printf("zapros-%s\n",strsql);
}
/************************************************************************/
/*Обработчик кнопки влючения/выключения автомтического обновления списка*/
/************************************************************************/
static void  l_zaktaxi_knopaos(GtkWidget *widget,class l_zaktaxi_data *data)
{

//printf("lzaktaxi_knopaos\n");

int vrem=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(data->spin));
printf("%s-vrem=%d\n",__FUNCTION__,vrem);
//vrem*=1000;
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE)
 {
  printf("%s-Включено\n",__FUNCTION__);
  data->timer_read=g_timeout_add_seconds(vrem,(GSourceFunc)l_zaktaxi_create_list,data);
 }
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == FALSE)
 {
  printf("%s-Выключено\n",__FUNCTION__);
  g_source_remove(data->timer_read);
  data->timer_read=0;
 }

}
/******************************************/
/*РАСПЕЧАТАТЬ*/
/*******************************************/

void l_zaktaxi_ras(class l_zaktaxi_data *data)
{
char		strsql[512];
SQL_str		row;
SQL_str		row1;
int		kolstr=0;
FILE		*ff;
char		imaf[40];
short 	        d,m,g;
SQLCURSOR cur;
SQLCURSOR cur1;



//Формируем запрос к базе данных
l_zaktaxi_zapros(strsql,data);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return;
 }

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

iceb_u_rsdat(&dn,&mn,&gn,data->rp.datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->rp.datak.ravno(),1);


sprintf(imaf,"vod%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

iceb_u_zagolov("Список заказов",dn,mn,gn,dk,mk,gk,"",ff);

char imaf_1[64];

FILE *ff_1;
sprintf(imaf_1,"vodd%d.lst",getpid());

if((ff_1 = fopen(imaf_1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_1,"",errno,data->window);
  return;
 }

iceb_u_zagolov("Список заказов",dn,mn,gn,dk,mk,gk,"",ff_1);

fprintf(ff,"\
-----------------------------------------------------------------------------\n\
Дата и время заказа|      Фамилия                 |    Телефон    |   Адрес \n\
-----------------------------------------------------------------------------\n");

fprintf(ff_1,"\
--------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_1,"\
Код водит.|Код клиен.|Фамилия клиента     |Дата и время заказа|   Откуда           |     Куда           |  Сумма   |\n");

fprintf(ff_1,"\
--------------------------------------------------------------------------------------------------------------------\n");

class iceb_u_str fio_kl("");
double isuma=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  if(l_zaktaxi_prov_row(row,data) != 0)
    continue;
  sprintf(strsql,"select fio from Taxiklient where kod='%s'",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   fio_kl.new_plus(row1[0]);
  else
   fio_kl.new_plus("");

  iceb_u_rsdat(&d,&m,&g,row[6],2);

  fprintf(ff,"%02d.%02d.%04d %s %-*.*s %-*s %s\n",
  d,m,g,
  row[9],
  iceb_u_kolbait(30,row[3]),
  iceb_u_kolbait(30,row[3]),
  row[3],
  iceb_u_kolbait(15,row[4]),
  row[4],
  row[10]);

  fprintf(ff_1,"%-10s %-10s %-*.*s %02d.%02d.%04d %s %-*.*s %-*.*s %10s\n",
  row[2],
  row[1],
  iceb_u_kolbait(20,fio_kl.ravno()),
  iceb_u_kolbait(20,fio_kl.ravno()),
  fio_kl.ravno(),
  d,m,g,
  row[9],
  iceb_u_kolbait(20,row[10]),
  iceb_u_kolbait(20,row[10]),
  row[10],
  iceb_u_kolbait(20,row[11]),
  iceb_u_kolbait(20,row[11]),
  row[11],
  row[13]);
  isuma+=atof(row[13]);
  
 }
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff_1,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_1,"%105s:%10.2f\n","Итого",isuma);

iceb_podpis(ff,data->window);
fclose(ff);

iceb_podpis(ff_1,data->window);
fclose(ff_1);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список заказов форма 1"));
oth.spis_imaf.plus(imaf_1);
oth.spis_naim.plus(gettext("Список заказов форма 2"));

for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
 iceb_ustpeh(oth.spis_imaf.ravno(nom),3,data->window);

iceb_rabfil(&oth,data->window);


}
