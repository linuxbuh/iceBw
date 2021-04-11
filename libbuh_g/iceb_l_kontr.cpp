/*$Id: iceb_l_kontr.c,v 1.80 2014/07/31 07:08:28 sasa Exp $*/
/*10.03.2019	24.11.2003	Белых А.И.	iceb_l_kontr.c
Работа с общим списком контрагентов
Если вернули 0- выбрали контрагента
             1- нет
*/
#include        <errno.h>
#include        "iceb_libbuh.h"
#include        "iceb_l_kontr.h"
enum
{
  COL_KONTR,
  COL_NAIMKONTR,
  COL_TELEF,
  COL_ADRES,
  COL_SHET,
  COL_MFO,
  COL_BANK,
  COL_ADRESB,
  COL_KOD,
  COL_NALNOM,
  COL_NSPDV,
  COL_GRUPA,
  COL_DOG,
  COL_RNHP,
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
  FK7,
  FK8,
  FK9,
  FK10,
  SFK2,
  SFK3,
  KOL_F_KL
};

class kontr_udnz_data
 {
  public:
  SQLCURSOR cur;
  GtkWidget *window;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество прочитанных строк
  int    kolstr;  //Количество строк в курсоре
  int koludk;


  //Конструктор
  kontr_udnz_data()
   {
    koludk=0;
    kolstr1=0.;
   }
 };


class kontr_data
 {
  public:
  

  class kontr_rek poisk;
  short metka_sort; //0-по кодам контрагента 1-по наименованию
    
  //Выбранный код контрагента
  class iceb_u_str shetsk;  //Счет списка контрагентов
  class iceb_u_str kodv; //выбранный код контрагента
  class iceb_u_str kontr_v; //только что введённый код контрагента
  class iceb_u_str naim;
      
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  class iceb_u_str name_window;
  
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или 0
  int       kolzap;     //Количество записей
  short     metka_rr;   //0-ввод и корректировка 1-чистый выбор
  int       metka_voz;  //0-выбрано 1-не выбрано
  int metka_rk; /*0-только наименование 1-всё*/      
  //Конструктор
  kontr_data()
   {
    metka_rk=0;
    shetsk.plus("");
    metka_voz=1;
    metka_rr=0;
    metka_sort=0;
    snanomer=0;
    kolzap=kl_shift=0;
    window=treeview=NULL;
   }  

 };


extern SQL_baza	bd;


gboolean   kontr_key_press(GtkWidget *,GdkEventKey *,class kontr_data *);
void       kontr_knopka(GtkWidget *,class kontr_data *);
void       kontr_v_row(GtkTreeView *,GtkTreePath *,GtkTreeViewColumn *,class kontr_data *);
void       kontr_udzap(class kontr_data *data);
int        kontr_provvudzap(const char *kontr,int met,GtkWidget*);
void       kontr_vibor(GtkTreeSelection *,class kontr_data *);
void       kontr_udnz(GtkWidget*);
gint       kontr_udnz11(class kontr_udnz_data *data);
void       kontr_rasspis(class kontr_data *data);
void       kontr_snkod(class kontr_data *data);
void       kontr_sm_kod(const char *kodkon,GtkWidget *);

void       kontr_create_list (class kontr_data *);
int iceb_l_kontr_uk(const char *kod_kontr,GtkWidget *wpredok);

int iceb_l_kontr_ik(const char *imaf,GtkWidget *wpredok);
void iceb_l_kontr_ik_f9(class kontr_data *data);


int iceb_l_kontr(int metka_rr,//0-ввод и корректировка 1-чистый выбор
iceb_u_str *kod,
iceb_u_str *naikod,
GtkWidget  *wpredok)
{
int gor=0;
int ver=0;
class kontr_data data;
char bros[512];

data.metka_rr=metka_rr;
data.name_window.plus(__FUNCTION__);

if(naikod->getdlinna() > 1)
 {
  data.poisk.metka_poi=1;
  data.poisk.naimkon.plus(naikod->ravno());
 }
//printf("l_kontr-data.metka_rr=%d data.metka_poi=%d\n",data.metka_rr,data.metka_poi);
 
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_rr == 1)
 {
  if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
 }


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Контрагенты"));

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(kontr_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Общий список контрагентов"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);
gtk_widget_show (data.label_kolstr);

data.label_poisk=gtk_label_new (gettext("Ждите !"));

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);
gtk_widget_show (data.label_poisk);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_widget_show (data.sw);


//Кнопки

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(kontr_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(kontr_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной запси"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(kontr_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной запси"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удал.неисп"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(kontr_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удаление неиспользованных контрагентов"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(kontr_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(kontr_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Най.код"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(kontr_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Найти запись по коду контрагента"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Сальдо"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(kontr_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Получить сальдо по выбранному контрагенту"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F8 %s",gettext("Сме.код"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(kontr_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Сменить код контрагента на другой с удалением записи"));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));
gtk_widget_show(data.knopka[FK8]);

sprintf(bros,"F9 %s",gettext("Меню"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(kontr_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Получить меню для выбора нужного режима работы"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show(data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(kontr_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
//gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

kontr_create_list (&data);

gtk_widget_show(data.window);
if(data.metka_rr == 0)
 gtk_window_maximize(GTK_WINDOW(data.window));



//gtk_window_fullscreen(GTK_WINDOW(data.window));

//gtk_widget_hide (data.label_poisk);

gtk_main();

if(data.metka_voz == 0)
 {
  kod->new_plus(data.kodv.ravno());
  naikod->new_plus(data.naim.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.metka_voz);

}

/***********************************/
/*Создаем список для просмотра */
/***********************************/
void kontr_create_list (class kontr_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

class iceb_clock kk(data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;

//printf("kontr_create_list %d\n",data->snanomer);

//gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
//iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);
gtk_widget_show (data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(kontr_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(kontr_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


if(data->metka_rk == 0)
 model = gtk_list_store_new (3, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);
else
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
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING,
G_TYPE_INT);


if(data->metka_sort == 0)
  sprintf(strsql,"select * from Kontragent order by kodkon asc");
else
  sprintf(strsql,"select * from Kontragent order by naikon asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

data->kolzap=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s\n",row[0],row[1]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(kontr_prov_row(row,&data->poisk) != 0)
    continue;


  if(iceb_u_SRAV(data->kontr_v.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  
  gtk_list_store_append (model, &iter);

  if(data->metka_rk == 0)
   gtk_list_store_set (model, &iter,
  COL_KONTR,row[0],
  COL_NAIMKONTR,row[1],
  2,data->kolzap,
  -1);
  else
   gtk_list_store_set (model, &iter,
  COL_KONTR,row[0],
  COL_NAIMKONTR,row[1],
  COL_TELEF,row[10],
  COL_ADRES,row[3],
  COL_SHET,row[7],
  COL_MFO,row[6],
  COL_BANK,row[2],
  COL_ADRESB,row[4],
  COL_KOD,row[5],
  COL_NALNOM,row[8],
  COL_NSPDV,row[9],
  COL_GRUPA,row[11],
  COL_DOG,row[14],
  COL_RNHP,row[15],
  COL_DATA_VREM,iceb_u_vremzap(row[13]),
  COL_KTO,iceb_kszap(row[12],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kontr_v.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

kontr_add_columns (GTK_TREE_VIEW (data->treeview),data->shetsk.ravno(),data->metka_rk);


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data->treeview,GTK_STATE_NORMAL,&color);


//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Общий список контрагентов"));

sprintf(strsql," %s:%d",gettext("Количество контрагентов"),data->kolzap);
zagolov.plus(strsql);
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->poisk.metka_poi == 1)
 {
//  printf("Формирование заголовка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kodkontr.ravno(),gettext("Код контрагента"));
  iceb_str_poisk(&zagolov,data->poisk.naimkon.ravno(),gettext("Наименование контрагента"));
  iceb_str_poisk(&zagolov,data->poisk.naimp.ravno(),gettext("Полное наименование"));
  iceb_str_poisk(&zagolov,data->poisk.gk.ravno(),gettext("Город контрагента"));
  iceb_str_poisk(&zagolov,data->poisk.naimban.ravno(),gettext("Наименование банка"));
  iceb_str_poisk(&zagolov,data->poisk.adresb.ravno(),gettext("Город банка"));
  iceb_str_poisk(&zagolov,data->poisk.kod.ravno(),gettext("Код ЕГРПУ"));
  iceb_str_poisk(&zagolov,data->poisk.mfo.ravno(),gettext("МФО"));
  iceb_str_poisk(&zagolov,data->poisk.nomsh.ravno(),gettext("Номер счета"));
  iceb_str_poisk(&zagolov,data->poisk.innn.ravno(),gettext("Инд.нал.номер"));
  iceb_str_poisk(&zagolov,data->poisk.nspnds.ravno(),gettext("Ном.свид.пл.НДС"));
  iceb_str_poisk(&zagolov,data->poisk.telef.ravno(),gettext("Телефон"));
  iceb_str_poisk(&zagolov,data->poisk.grup.ravno(),gettext("Группа"));
  iceb_str_poisk(&zagolov,data->poisk.na.ravno(),gettext("Нал.адрес"));
  iceb_str_poisk(&zagolov,data->poisk.regnom.ravno(),gettext("Гегист.ном.ч/п"));
  if(data->poisk.metka_nal == 1)  
    iceb_str_poisk(&zagolov,gettext("Общая система"),gettext("Cистема налога"));
  if(data->poisk.metka_nal == 2)  
    iceb_str_poisk(&zagolov,gettext("Единый налог"),gettext("Cистема налога"));
   


  iceb_label_set_text_color(data->label_poisk,zagolov.ravno(),"#F90101");
//  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 


//gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));
//printf("kontr_create_list - end\n");


}

/*****************/
/*Создаем колонки*/
/*****************/

void kontr_add_columns (GtkTreeView *treeview,const char *shet,int metka_rk)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;

renderer = gtk_cell_renderer_text_new ();
if(shet[0] == '\0')
  g_object_set(G_OBJECT(renderer),"foreground","red",NULL);
else
  g_object_set(G_OBJECT(renderer),"foreground","blue",NULL);

column = gtk_tree_view_column_new_with_attributes (gettext("Код"),renderer, "text",COL_KONTR,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KONTR);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();


column = gtk_tree_view_column_new_with_attributes (gettext("Наименование"),renderer, "text",COL_NAIMKONTR,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAIMKONTR); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);


if(metka_rk == 0) /*только наименования*/
 return;

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Телефон"),renderer, "text",COL_TELEF,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_TELEF); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Адрес"),renderer, "text",COL_ADRES,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ADRES); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Счет"),renderer, "text",COL_SHET,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_SHET); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("МФО"),renderer, "text",COL_MFO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_MFO); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Банк"),renderer, "text",COL_BANK,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_BANK); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Город банка"),renderer, "text",COL_ADRESB,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ADRESB); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Код ЕГРПОУ"),renderer, "text",COL_KOD,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOD); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Инд.нал.номер"),renderer, "text",COL_NALNOM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NALNOM); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Ном.свид.пл.НДС"),renderer, "text",COL_NSPDV,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NSPDV); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);



renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Группа"),renderer, "text",COL_GRUPA,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_GRUPA); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Договор"),renderer, "text",COL_DOG,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DOG); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Рег.ном.ч/п"),renderer, "text",COL_RNHP,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_RNHP); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Дата и время записи"),renderer, "text",COL_DATA_VREM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_VREM); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кто записал"),renderer, "text",COL_KTO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KTO); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);
 
}
/****************************/
/*Выбор строки*/
/**********************/

void kontr_vibor(GtkTreeSelection *selection,class kontr_data *data)
{
//printf("kontr_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;
if(data->metka_rk == 0)
 gtk_tree_model_get(model,&iter,COL_KONTR,&kod,COL_NAIMKONTR,&naim,2,&nomer,-1);
else
 gtk_tree_model_get(model,&iter,COL_KONTR,&kod,COL_NAIMKONTR,&naim,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(kod);
data->naim.new_plus(naim);
data->snanomer=nomer;

g_free(kod);
g_free(naim);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  kontr_knopka(GtkWidget *widget,class kontr_data *data)
{
class iceb_u_str kod("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("kontr_knopka knop=%d\n",knop);
data->kl_shift=0;

switch (knop)
 {
  case FK2:

    if(iceb_l_kontrv(&kod,"",data->window) == 0)
     {
      data->kontr_v.new_plus(kod.ravno());
      kontr_create_list (data);
     }
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(iceb_l_kontrv(&data->kodv,"",data->window) == 0)
     {
      data->kontr_v.new_plus(data->kodv.ravno());
      kontr_create_list (data);
     }

    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    kontr_udzap(data);
    return;  

  case SFK3:
    if(data->kolzap == 0)
      return;

    if(iceb_menu_danet(gettext("Удалить не используемые контрагенты ? Вы уверены ?"),2,data->window) == 2)
      return;

    kontr_udnz(data->window);
    kontr_create_list(data);
    return;  

  case FK4:
    if(iceb_l_kontrp(&data->poisk,data->window) == 0)
     data->poisk.metka_poi=1;
    else
     data->poisk.metka_poi=0;
    kontr_create_list(data);
    return;  

  case FK5:
//    gtk_widget_hide(data->window);
    kontr_rasspis(data);
//    gtk_widget_show(data->window);
    return;  

  case FK6:
    kontr_snkod(data);
    return;  

  case FK7:
   iceb_salorok(data->kodv.ravno(),data->window);
   return;

  case FK8:
   kontr_sm_kod(data->kodv.ravno(),data->window);
   kontr_create_list (data);
   return;
   
  case FK9:
    iceb_l_kontr_ik_f9(data);
    return;
    
  case FK10:
    if(data->metka_rr == 1)
      iceb_sizww(data->name_window.ravno(),data->window);
    gtk_widget_destroy(data->window);
    data->metka_voz=1;
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kontr_key_press(GtkWidget *widget,GdkEventKey *event,class kontr_data *data)
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

  case GDK_KEY_F9:
    g_signal_emit_by_name(data->knopka[FK9],"clicked");
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
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void kontr_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class kontr_data *data)
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
/***********************/
/*Удаление записи      */
/***********************/
void   kontr_udzap(class kontr_data *data)
{

if(iceb_u_SRAV(data->kodv.ravno(),"00",0) == 0)
 {
  iceb_menu_soob(gettext("Код контрагента 00 удалять нельзя!"),data->window);
  return;
 }
 
iceb_u_str sp;
sp.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&sp,2,data->window) == 2)
 return;

if(kontr_provvudzap(data->kodv.ravno(),1,data->window) != 0)
  return;

iceb_l_kontr_uk(data->kodv.ravno(),data->window);

/**************************
char  strsql[312];
sprintf(strsql,"delete from Kontragent where kodkon='%s'",data->kodv.ravno());
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;

sprintf(strsql,"delete from Skontr where kodkon='%s'",data->kodv.ravno());
iceb_sql_zapis(strsql,0,0,data->window);

sprintf(strsql,"delete from Kontragent1 where kodkon='%s'",data->kodv.ravno());
iceb_sql_zapis(strsql,0,0,data->window);

sprintf(strsql,"delete from Kontragent2 where kodkon='%s'",data->kodv.ravno());
iceb_sql_zapis(strsql,0,0,data->window);
*******************************/
kontr_create_list(data);

}

/********************************/
/*Проверка возможности удаления записи*/
/***************************************/

int kontr_provvudzap(const char *kontr,int met,GtkWidget *wpredok) //0-молча 1-сообщения
{
char strsql[1024];

sprintf(strsql,"select kodkon from Saldo where kodkon='%s' limit 1",
kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Saldo",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kodkon from Prov where kodkon='%s'",
kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Prov",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kontr from Dokummat where kontr='%s' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Dokummat",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kontr from Dokummat where k00='%s' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Dokummat",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kontr from Usldokum where kontr='%s' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Usldokum",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kontr from Usldokum where k00='%s' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Usldokum",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kontr from Uosdok where kontr='%s' limit 1",
kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Uosdok",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }    


sprintf(strsql,"select polu from Pltp where polu like '%s#%%' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Pltp",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }    

sprintf(strsql,"select polu from Pltt where polu like '%s#%%' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Pltt",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }    

sprintf(strsql,"select polu from Tpltt where polu like '%s#%%' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Tpltt",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }    

sprintf(strsql,"select polu from Tpltp where polu like '%s#%%' limit 1",
kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Tpltp",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }    


memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select kontr from Kasord1 where kontr='%s' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Kasord1",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }    


memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select kont from Ukrdok where kont='%s' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      sprintf(strsql,"%s Ukrdok",gettext("Удалить невозможно ! Есть записи в таблице"));
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }    


memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select kontr from Uplouot where kontr='%s' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  
  if(met != 0)
   {
    sprintf(strsql,"%s Uplouot",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select kontr from Ukrdok1 where kontr='%s' or kdrnn='%s' limit 1",kontr,kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  
  if(met != 0)
   {
    sprintf(strsql,"%s Ukrdok1",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }


return(0);
}

/*************************************/
/*Удаление не используемых кодов контрагентов*/
/*********************************************/

void  kontr_udnz(GtkWidget *wpredok)
{
char strsql[512];
kontr_udnz_data data;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_deletable(GTK_WINDOW(data.window),FALSE); /*Выключить кнопку close в рамке окна*/
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Удаление контрагентов, которые не используются"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

//g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

GtkWidget *label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

label=gtk_label_new(gettext("Удаление контрагентов, которые не используются."));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна
gtk_widget_show_all(data.window);

g_idle_add((GSourceFunc)kontr_udnz11,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}
/**********************************/
/**********************************/
gint   kontr_udnz11(class kontr_udnz_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur1;
GtkTextIter iter;

//printf("kontr_udnz11 %f\n",data->kolstr1);

if(data->kolstr1 == 0)
 {
  sprintf(strsql,"select * from Kontragent");
  if((data->kolstr=data->cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(FALSE);
   }
  
 }
class iceb_u_str stroka;
while(data->cur.read_cursor(&row) != 0)
 {
//  printf("%-5s %-40.40s ",row[0],row[1]);

  sprintf(strsql,"%-*s ",iceb_u_kolbait(6,row[0]),row[0]);    
  stroka.new_plus(strsql);
  sprintf(strsql,"%s\n",row[1]);    
  stroka.plus(strsql);
  
//  g_print("%s",stroka.ravno());
    
  gtk_text_buffer_get_iter_at_offset(data->buffer,&iter,-1);
  gtk_text_buffer_insert(data->buffer,&iter,stroka.ravno(),-1);
  gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(data->view),&iter,0.0,TRUE,0.,1.);
  
  iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

  if(iceb_u_SRAV("00",row[0],0) == 0)
   continue;
   
  if(kontr_provvudzap(row[0],0,data->window) != 0)
   {
//    printf("используется\n");
    return(TRUE);
   }

//  printf("удаляем\n");

  iceb_l_kontr_uk(row[0],data->window);
/***********************************
  sprintf(strsql,"delete from Skontr where kodkon='%s'",row[0]);
  if(iceb_sql_zapis(strsql,1,1,data->window) != 0)
   continue;
  
  sprintf(strsql,"delete from Kontragent where kodkon='%s'",row[0]);
  iceb_sql_zapis(strsql,1,1,data->window);

  sprintf(strsql,"delete from Kontragent1 where kodkon='%s'",row[0]);
  iceb_sql_zapis(strsql,1,1,data->window);
****************************/
  data->koludk++;

  return(TRUE);
  
 }


printf("%s: %d\n",gettext("Количество удаленных контрагентов"),data->koludk);
//if(koludk != 0)
//  printf("%s:\n%s\n",gettext("Список удаленных контрагентов выгружено в файл"),imaf);

char imatab[32];

strcpy(imatab,"Kontragent");
//printf(gettext("Оптимизация таблицы %s !\n"),imatab);


int		metkazap=0;

strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur1);
if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
//printf("VERSION:%s\n",row[0]);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table %s",imatab);

if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
 }
else
  sql_readkey(&bd,strsql);

//printf(gettext("Оптимизацию таблицы закончено.\n"));

strcpy(imatab,"Skontr");
//printf(gettext("Оптимизация таблицы %s !\n"),imatab);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table %s",imatab);

if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
 }
else
  sql_readkey(&bd,strsql);

strcpy(imatab,"Kontragent1");
//printf(gettext("Оптимизация таблицы %s !\n"),imatab);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table %s",imatab);

if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
 }
else
  sql_readkey(&bd,strsql);



//printf(gettext("Оптимизацию таблицы закончено.\n"));
gtk_widget_destroy(data->window);

return(FALSE);
}
/*****************************************/
/*Распечатка списка контрагентов         */
/*****************************************/
void  kontr_rasspis(class kontr_data *data)
{
char imaf[64];
FILE *ff;
int kolstr=0;
char strsql[512];
SQLCURSOR cur;
SQL_str row;

sprintf(strsql,"select * from Kontragent order by kodkon asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr == 0)
  return;
  
sprintf(imaf,"Kontragent.lst");
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }
char imafz[64];
sprintf(imafz,"kontz%d.csv",getpid());
class iceb_fopen ffz;
if(ffz.start(imafz,"w",data->window) != 0)
 return;

time_t tmm;
struct tm *bf;

time(&tmm);
bf=localtime(&tmm);


fprintf(ff,"%s\n\n%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
iceb_get_pnk("00",0,data->window),
gettext("Список контрагентов"),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Код |           Наименование  контрагента    |   Наименование банка         |  Адрес контрагента           |  Адрес банка              |  Код     |   MФО    | Номер счета   |Нал. номер     |Н.пл. НДС |Телефон   |Группа|Рег.н.ЧП| Налоговый адрес\n"));
//    1234567890123456789012345678901234567890 123456789012345678901234567890 123456789012345678901234567890 123456789012345678901234567 1234567890
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");


fprintf(ffz.ff,"#Список контрагентов для загрузки в базу\n#Выгружено из базы %s %s\n",iceb_get_namebase(),iceb_get_pnk("00",1,data->window));
fprintf(ffz.ff,"#Расшифровка полей в записи:\n\
#1  Код контрагента\n\
#2  Наименование контрагента\n\
#3  Наименование банка\n\
#4  Адрес контрагента\n\
#5  Адрес банка\n\
#6  Код ЕГРПОУ\n\
#7  МФО\n\
#8  Номер счета\n\
#9  Индивидуальный налоговый номер\n\
#10 Номер свидетельства плательщика НДС\n\
#11 Телефон\n\
#12 Группа\n\
#13 Кто записал\n\
#14 Время записи\n\
#15 Налоговый адес\n\
#16 регистрационный номер частного предпринимателя\n\
#17 полное наименование контрагента\n\
#18 город контрагента\n\
#19 0-общая система налогообложения 1-единый налог\n");



int kl=0;
while(cur.read_cursor(&row) != 0)
 {
   if(kontr_prov_row(row,&data->poisk) != 0)
    continue;

  kl++;

  fprintf(ff,"%-*s %-*.*s %-*.*s %-*.*s %-*.*s %-10s %-10s\
 %-15s %-15s %-10s %-10.10s %-6s %-8s %s\n",
  iceb_u_kolbait(5,row[0]),row[0],
  iceb_u_kolbait(40,row[1]),iceb_u_kolbait(40,row[1]),row[1],
  iceb_u_kolbait(30,row[2]),iceb_u_kolbait(30,row[2]),row[2],
  iceb_u_kolbait(30,row[3]),iceb_u_kolbait(30,row[3]),row[3],
  iceb_u_kolbait(27,row[4]),iceb_u_kolbait(27,row[4]),row[4],
  row[5],
  row[6],
  row[7],
  row[8],
  row[9],row[10],row[11],row[15],row[14]);

  fprintf(ffz.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[14],row[15],row[16],row[17],row[18]);
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
%s - %d\n",gettext("Итого"),kl);

iceb_podpis(ff,data->window);

fclose(ff);
ffz.end();

class iceb_u_spisok fil;
class iceb_u_spisok naim;

fil.plus(imaf);
naim.plus(gettext("Список контрагентов"));

iceb_ustpeh(imaf,3,data->window);

fil.plus(imafz);
naim.plus(gettext("Список контрагентов для загрузки в базу"));

iceb_rabfil(&fil,&naim,data->window);
}
/**************************************/
/*Стать на код контрагента           */
/*************************************/

void    kontr_snkod(class kontr_data *data)
{
//iceb_u_str zapros;
char    kodkon[32];

memset(kodkon,'\0',sizeof(kodkon));

//zapros.plus(gettext("Введите нужный код контрагента"));
if(iceb_menu_vvod1(gettext("Введите нужный код контрагента"),kodkon,sizeof(kodkon),"",data->window) != 0)
  return;

if(kodkon[0] == '\0')
  return;

char strsql[512];

sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kodkon);
printf("%s\n",strsql);
if(sql_readkey(&bd,strsql) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Нет такого кода в списке контрагентов !"));
  iceb_menu_soob(&repl,data->window);
  return;
 }


data->kontr_v.new_plus(kodkon);
kontr_create_list(data);

}
/*************************************/
/*Проверка записей на реквизиты поиска*/
/**************************************/

int  kontr_prov_row(SQL_str row,class kontr_rek *data)
{
if(data->metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->kodkontr.ravno(),row[0],0,0) != 0)
 return(1);

//Поиск образца в строке
if(data->naimkon.ravno()[0] != '\0' && iceb_u_strstrm(row[1],data->naimkon.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->naimp.ravno()[0] != '\0' && iceb_u_strstrm(row[16],data->naimp.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->naimban.ravno()[0] != '\0' && iceb_u_strstrm(row[2],data->naimban.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->adres.ravno()[0] != '\0' && iceb_u_strstrm(row[3],data->adres.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->adresb.ravno()[0] != '\0' && iceb_u_strstrm(row[4],data->adresb.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->kod.ravno()[0] != '\0' && iceb_u_strstrm(row[5],data->kod.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->mfo.ravno()[0] != '\0' && iceb_u_strstrm(row[6],data->mfo.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->nomsh.ravno()[0] != '\0' && iceb_u_strstrm(row[7],data->nomsh.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->innn.ravno()[0] != '\0' && iceb_u_strstrm(row[8],data->innn.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->nspnds.ravno()[0] != '\0' && iceb_u_strstrm(row[9],data->nspnds.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->telef.ravno()[0] != '\0' && iceb_u_strstrm(row[10],data->telef.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->grup.ravno()[0] != '\0' && iceb_u_strstrm(row[11],data->grup.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->na.ravno()[0] != '\0' && iceb_u_strstrm(row[14],data->na.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->regnom.ravno()[0] != '\0' && iceb_u_strstrm(row[15],data->regnom.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->gk.ravno()[0] != '\0' && iceb_u_strstrm(row[17],data->gk.ravno()) == 0)
 return(1);

if(data->metka_nal != 0)
 {
  if(data->metka_nal == 1 && atoi(row[18]) != 0)
    return(1);
  if(data->metka_nal == 2 && atoi(row[18]) != 1)
    return(1);
 }
return(0);
}
/************************************/
/*Смена кода контрагента с удалением*/
/************************************/
void kontr_sm_kod(const char *kodkons,GtkWidget *wpredok)
{
if(iceb_u_SRAV("00",kodkons,0) == 0)
 {
  iceb_menu_soob(gettext("Код контрагента 00 нельзя заменять!"),wpredok);
  return;
 }
iceb_u_str repl;
repl.plus(gettext("Заменить код контрагента"));
repl.plus(" ");
repl.plus(kodkons);
repl.plus(" ");
repl.plus(gettext("Вы уверены ?"));
if(iceb_menu_danet(&repl,2,wpredok) == 2)
  return;

char kodkonn[64];
memset(kodkonn,'\0',sizeof(kodkonn));

repl.new_plus(gettext("Введите код контрагента на который нужно заменить"));
if(iceb_menu_vvod1(repl.ravno(),kodkonn,sizeof(kodkonn),"",wpredok) != 0)
  return;

//Проверяем есть ли этот код в списке контрагентов
char strsql[512];
//SQL_str row;

sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kodkonn);
if(sql_readkey(&bd,strsql) != 1)
 {
  repl.new_plus(gettext("Нет такого кода в списке контрагентов !"));
  iceb_menu_soob(&repl,wpredok);
  return;
 }

if(iceb_parol(0,wpredok) != 0)
  return;

//Выполняем коректировку 
kontr_korkod(kodkons,kodkonn,wpredok);

//Проверяем возможность удаления
if(kontr_provvudzap(kodkons,1,wpredok) != 0)
  return;

//Удаляем из списка счетов
sprintf(strsql,"delete from Skontr where kodkon='%s'",kodkons);
iceb_sql_zapis(strsql,1,1,wpredok);

//Удаляем из списка конрагентов
sprintf(strsql,"delete from Kontragent where kodkon='%s'",kodkons);
iceb_sql_zapis(strsql,1,1,wpredok);

}
/************************************/
/*загрузка контрагентов из файла в базу*/
/**************************************/
void iceb_l_kontr_infil(class kontr_data  *data)
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");
 
if(iceb_menu_import(&imaf,gettext("Импорт контрагентов из файла"),"",__FUNCTION__,data->window) != 0)
 return;


iceb_l_kontr_ik(imaf.ravno(),data->window);

}
/**********************************/
/*мень*/
/***********************************/
void iceb_l_kontr_ik_f9(class kontr_data *data)
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Меню выбора"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Изменить порядок сортировки"));//0
punkt_m.plus(gettext("Загрузить котрагентов из файла"));//1
punkt_m.plus(gettext("Показать/скрыть все реквизиты контрагетов"));//1


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      data->metka_sort++;
      if(data->metka_sort > 1)
        data->metka_sort=0;
        
      kontr_create_list(data);
      break;       

    case 1:
      iceb_l_kontr_infil(data);
      kontr_create_list(data);
      return;

    case 2:
      data->metka_rk++;
      if(data->metka_rk > 1)
       data->metka_rk=0;
       
      kontr_create_list(data);
      return;

   }
   
 }
}
/********************************************/
/*Удаление контрагента*/
/*******************************************/
int iceb_l_kontr_uk(const char *kod_kontr,GtkWidget *wpredok)
{
char strsql[1024];

sprintf(strsql,"delete from Kontragent where kodkon='%s'",kod_kontr);
if(iceb_sql_zapis(strsql,0,0,wpredok) != 0)
 return(1);

sprintf(strsql,"delete from Skontr where kodkon='%s'",kod_kontr);
iceb_sql_zapis(strsql,0,0,wpredok);

sprintf(strsql,"delete from Kontragent1 where kodkon='%s'",kod_kontr);
iceb_sql_zapis(strsql,0,0,wpredok);

sprintf(strsql,"delete from Kontragent2 where kodkon='%s'",kod_kontr);
iceb_sql_zapis(strsql,0,0,wpredok);

return(0);
}