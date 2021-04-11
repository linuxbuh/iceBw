/*$Id: iceb_l_kontrsh.c,v 1.54 2013/12/31 11:49:18 sasa Exp $*/
/*27.02.2016	24.11.2003	Белых А.И.	iceb_l_kontrsh.c
Работа со списком контрагентов для бухгалтерского счёта с развёрнутым сальдо
*/
#include        <errno.h>
#include        "iceb_libbuh.h"
#include        "iceb_l_kontr.h"

class kontrsh_udnz_data
 {
  public:
  iceb_u_str shetsk;
  SQLCURSOR cur;
  GtkWidget *window;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество прочитанных строк
  int    kolstr;  //Количество строк в курсоре
  int koludk;

  //Конструктор
  kontrsh_udnz_data()
   {
    koludk=0;
    kolstr1=0.;
   }
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

class kontr_data_sh
 {
  public:
  

  kontr_rek poisk;
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
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или 0
  int       kolzap;     //Количество записей
  short     metka_rr;   //0-ввод и корректировка 1-чистый выбор
  int       metka_voz;  //0-выбрано 1-не выбрано
  int metka_rk;      
  //Конструктор
  kontr_data_sh()
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

gboolean   kontrsh_key_press(GtkWidget *,GdkEventKey *,class kontr_data_sh *);
void       kontrsh_knopka(GtkWidget *,class kontr_data_sh *);
void       kontrsh_v_row(GtkTreeView *,GtkTreePath *,GtkTreeViewColumn *,class kontr_data_sh *);
void       kontrsh_udzap(class kontr_data_sh *data);
int        kontrsh_provvudzap(const char *shet,const char *kontr,int met,GtkWidget*);
void       kontrsh_vibor(GtkTreeSelection *,class kontr_data_sh *);
void       kontrsh_udnz(const char*);
gint       kontrsh_udnz11(class kontrsh_udnz_data *data);
void       kontrsh_rasspis(class kontr_data_sh *data);
void       kontrsh_snkod(class kontr_data_sh *data);
void       kontrsh_rosk(class kontr_data_sh *data,int);
void       iceb_spks(char*,char*,GtkWidget*);

void       kontrsh_create_list (class kontr_data_sh *);

extern SQL_baza bd;

int iceb_l_kontrsh(int metka_rr,//0-ввод и корректировка 1-чистый выбор
const char       *shetsk,
class iceb_u_str *kod,
class iceb_u_str *naikod,
GtkWidget  *wpredok)
{
kontr_data_sh data;
char bros[512];

data.shetsk.new_plus(shetsk);
data.metka_rr=metka_rr;
if(naikod->getdlinna() > 1)
 {
  data.poisk.metka_poi=1;
  data.poisk.naimkon.plus(naikod->ravno());
 }

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(kontrsh_key_press),&data);
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
gtk_widget_show (vbox1);
gtk_widget_show (vbox2);

gtk_widget_show (hbox);

data.label_kolstr=gtk_label_new (gettext("Общий список контрагентов"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);


//Кнопки

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(kontrsh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show (data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[SFK2]),FALSE);//Недоступна
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(kontrsh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной запси"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show (data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK3]),FALSE);//Недоступна
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(kontrsh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной запси"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show (data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удал.неисп."));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[SFK3]),FALSE);//Недоступна
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(kontrsh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удаление неиспользованных контрагентов"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show (data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(kontrsh_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show (data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(kontrsh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show (data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Най.код"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(kontrsh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Найти запись по коду контрагента"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show (data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Сальдо"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(kontrsh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Получить сальдо по выбранному контрагенту"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show (data.knopka[FK7]);

sprintf(bros,"F8 %s",gettext("Общ.список"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(kontrsh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Просмотр общего списка контрагентов"));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));
gtk_widget_show (data.knopka[FK8]);

sprintf(bros,"F9 %s",gettext("Реквизиты"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(kontrsh_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Включить/выключить просмотр списка контрагентов со всеми реквизитами"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show (data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(kontrsh_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show (data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

kontrsh_create_list (&data);

gtk_widget_show (data.window);

if(metka_rr == 0)
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
void kontrsh_create_list (class kontr_data_sh *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR curos;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;


gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(kontrsh_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(kontrsh_vibor),data);

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


sprintf(strsql,"select kodkon,ktoi,vrem from Skontr where ns='%s' order by kodkon asc",data->shetsk.ravno());

//printf("*******%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str s1;
iceb_u_str s2;
iceb_u_str s3;
iceb_u_str s4;
iceb_u_str s5;
iceb_u_str s6;
iceb_u_str s7;
iceb_u_str s8;
iceb_u_str s9;
iceb_u_str s10;
iceb_u_str s11;
iceb_u_str s12;
iceb_u_str s13;
iceb_u_str s14;
iceb_u_str s15;
iceb_u_str s16;

data->kolzap=0;
float kolstr1=0.;
while(cur.read_cursor(&row1) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  //Код контрагента
  s1.new_plus(row1[0]);
      
  sprintf(strsql,"select * from Kontragent where kodkon='%s'",row1[0]);
  if(sql_readkey(&bd,strsql,&row,&curos) == 1)
   {
    if(kontr_prov_row(row,&data->poisk) != 0)
      continue;


    if(iceb_u_SRAV(data->kontr_v.ravno(),row[0],0) == 0)
      data->snanomer=data->kolzap;

    
    //Наименование контрагента
    s2.new_plus(row[1]);

    //Телефон
    s3.new_plus(row[10]);

    //Адрес контрагента
    s4.new_plus(row[3]);

    //Счет
    s5.new_plus(row[7]);

    //МФО
    s6.new_plus(row[6]);

    //Банк
    s7.new_plus(row[2]);

    //Адрес банка
    s8.new_plus(row[4]);

    //Код ЕДРПОУ
    s9.new_plus(row[5]);

    //Налоговый номер
    s10.new_plus(row[8]);

    //Номер свидетельства плательщика ПДВ
    s11.new_plus(row[9]);

    //Группа
    s12.new_plus(row[11]);

    //Договор
    s13.new_plus(row[14]);

    //Регистрациооный номер частного предпринимателя
    s14.new_plus(row[15]);

    //Дата и время записи
    s15.new_plus(iceb_u_vremzap(row[13]));

    //Кто записал
    s16.new_plus(iceb_kszap(row[12],data->window));
   }
  else
   {
    class iceb_u_str repl;
    repl.plus_ps(gettext("Не найден код контрагента в общем списке !"));
    repl.plus(row1[0]);
    iceb_menu_soob(&repl,data->window);

    s2.new_plus(gettext("Не найден код контрагента в общем списке!"));
    s3.new_plus("");
    s4.new_plus("");
    s5.new_plus("");
    s6.new_plus("");
    s7.new_plus("");
    s8.new_plus("");
    s9.new_plus("");
    s10.new_plus("");
    s11.new_plus("");
    s12.new_plus("");
    s13.new_plus("");
    s14.new_plus("");
    s15.new_plus("");
    s16.new_plus("");
   }


  
  gtk_list_store_append (model, &iter);
 
  if(data->metka_rk == 0)
   gtk_list_store_set (model, &iter,
  COL_KONTR,row[0],
  COL_NAIMKONTR,row[1],
  2,data->kolzap,
  -1);
  else
   gtk_list_store_set (model, &iter,
  COL_KONTR,s1.ravno(),
  COL_NAIMKONTR,s2.ravno(),
  COL_TELEF,s3.ravno(),
  COL_ADRES,s4.ravno(),
  COL_SHET,s5.ravno(),
  COL_MFO,s6.ravno(),
  COL_BANK,s7.ravno(),
  COL_ADRESB,s8.ravno(),
  COL_KOD,s9.ravno(),
  COL_NALNOM,s10.ravno(),
  COL_NSPDV,s11.ravno(),
  COL_GRUPA,s12.ravno(),
  COL_DOG,s13.ravno(),
  COL_RNHP,s14.ravno(),
  COL_DATA_VREM,s15.ravno(),
  COL_KTO,s16.ravno(),
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
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data->treeview,GTK_STATE_NORMAL,&color);

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список контрагентоа для счёта"));
zagolov.plus(": ");
zagolov.plus(data->shetsk.ravno());



sprintf(strsql," %s:%d",gettext("Количество контрагентов"),data->kolzap);
zagolov.plus(strsql);
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());


if(data->poisk.metka_poi == 1)
 {
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");
  
  if(data->poisk.kodkontr.ravno()[0] != '\0')  
   {
    stroka.plus(gettext("Код контрагента"));
    stroka.plus(": ");
    stroka.plus(data->poisk.kodkontr.ravno());
    zagolov.ps_plus(stroka.ravno());

   }
  if(data->poisk.naimkon.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("Наименование контрагента"));
    stroka.plus(": ");
    stroka.plus(data->poisk.naimkon.ravno());
    zagolov.ps_plus(stroka.ravno());

   }

  if(data->poisk.naimban.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("Наименование банка"));
    stroka.plus(": ");
    stroka.plus(data->poisk.naimban.ravno());
    zagolov.ps_plus(stroka.ravno());

   }

  if(data->poisk.adres.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("Адрес"));
    stroka.plus(": ");
    stroka.plus(data->poisk.adres.ravno());
    zagolov.ps_plus(stroka.ravno());

   }

  if(data->poisk.adresb.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("Город банка"));
    stroka.plus(": ");
    stroka.plus(data->poisk.adresb.ravno());
    zagolov.ps_plus(stroka.ravno());

   }

  if(data->poisk.kod.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("Код ЕГРПУ"));
    stroka.plus(": ");
    stroka.plus(data->poisk.kod.ravno());
    zagolov.ps_plus(stroka.ravno());

   }

  if(data->poisk.mfo.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("МФО"));
    stroka.plus(": ");
    stroka.plus(data->poisk.mfo.ravno());
    zagolov.ps_plus(stroka.ravno());

   }

  if(data->poisk.nomsh.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("Номер счета"));
    stroka.plus(": ");
    stroka.plus(data->poisk.nomsh.ravno());
    zagolov.ps_plus(stroka.ravno());

   }

  if(data->poisk.innn.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("Инд.нал.номер"));
    stroka.plus(": ");
    stroka.plus(data->poisk.innn.ravno());
    zagolov.ps_plus(stroka.ravno());
   }

  if(data->poisk.nspnds.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("Ном.свид.пл.НДС"));
    stroka.plus(": ");
    stroka.plus(data->poisk.nspnds.ravno());
    zagolov.ps_plus(stroka.ravno());
   }

  if(data->poisk.telef.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("Телефон"));
    stroka.plus(": ");
    stroka.plus(data->poisk.telef.ravno());
    zagolov.ps_plus(stroka.ravno());
   }

  if(data->poisk.grup.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("Группа"));
    stroka.plus(": ");
    stroka.plus(data->poisk.grup.ravno());
    zagolov.ps_plus(stroka.ravno());
   }

  if(data->poisk.na.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("Налоговый адрес"));
    stroka.plus(": ");
    stroka.plus(data->poisk.na.ravno());
    zagolov.ps_plus(stroka.ravno());
   }

  if(data->poisk.regnom.ravno()[0] != '\0')  
   {
    stroka.new_plus(gettext("Гегист.ном.ч/п"));
    stroka.plus(": ");
    stroka.plus(data->poisk.regnom.ravno());
    zagolov.ps_plus(stroka.ravno());
   }

  iceb_label_set_text_color(data->label_poisk,zagolov.ravno(),"#F90101");
//  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));


if(data->kolzap == 0 && data->poisk.metka_poi == 1 && data->poisk.naimkon.getdlinna() > 1)
 {
  gdite.close();
  class iceb_u_str repl;
  repl.plus_ps(gettext("Не найдено ни одной записи !"));
  repl.plus(gettext("Выполнить поиск в общем списке контрагентов ?"));
  if(iceb_menu_danet(&repl,2,data->window) == 2)
   return;

  kontrsh_rosk(data,1);
 }

//printf("kontrsh_create_list end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void kontrsh_vibor(GtkTreeSelection *selection,class kontr_data_sh *data)
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
void  kontrsh_knopka(GtkWidget *widget,class kontr_data_sh *data)
{
class iceb_u_str kod("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    
    if(iceb_l_kontrv(&kod,data->shetsk.ravno(),data->window) == 0)
     {
      data->kontr_v.new_plus(kod.ravno());
      kontrsh_create_list (data);
     }
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(iceb_l_kontrv(&data->kodv,data->shetsk.ravno(),data->window) == 0)
     {
      data->kontr_v.new_plus(data->kodv.ravno());
      kontrsh_create_list (data);
     }
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    kontrsh_udzap(data);
    return;  

  case SFK3:
    if(data->kolzap == 0)
      return;

    if(iceb_menu_danet(gettext("Удалить не используемые контрагеты ? Вы уверены ?"),2,NULL) == 2)
      return;

    gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
    kontrsh_udnz(data->shetsk.ravno());
    kontrsh_create_list(data);
    return;  

  case FK4:
    if(iceb_l_kontrp(&data->poisk,data->window) == 0)
     data->poisk.metka_poi=1; //искать
    else
     data->poisk.metka_poi=0; //не искать
     
    kontrsh_create_list(data);
    return;  

  case FK5:
//    gtk_widget_hide(data->window);
    kontrsh_rasspis(data);
//    gtk_widget_show(data->window);
    return;  

  case FK6:
    kontrsh_snkod(data);
    return;  

  case FK7:
   iceb_spks(data->shetsk.ravno(),data->kodv.ravno(),data->window);
   return;

  case FK8:
    kontrsh_rosk(data,0);
    return;

  case FK9:
    data->metka_rk++;
    if(data->metka_rk > 1)
     data->metka_rk=0;
    kontrsh_create_list(data);
    return;
   
    
  case FK10:
//    printf("kontrsh_knopka F10\n");
    gtk_widget_destroy(data->window);
    data->metka_voz=1;
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kontrsh_key_press(GtkWidget *widget,GdkEventKey *event,class kontr_data_sh *data)
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
//    printf("kontrsh_key_press-Нажата клавиша Shift\n");

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
void kontrsh_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class kontr_data_sh *data)
{
//printf("kontrsh_v_row data->metka_rr=%d\n",data->metka_rr);
if(data->metka_rr == 0)
 {
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");
  return;
 }

//printf("kontrsh_v_row выход\n");

gtk_widget_destroy(data->window);

data->metka_voz=0;

}
/***********************/
/*Удаление записи      */
/***********************/
void   kontrsh_udzap(class kontr_data_sh *data)
{
char  strsql[512];

 
iceb_u_str sp;
sp.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&sp,2,data->window) == 2)
 return;


if(kontrsh_provvudzap(data->shetsk.ravno(),data->kodv.ravno(),1,data->window) != 0)
  return;

sprintf(strsql,"delete from Skontr where ns='%s' and kodkon='%s'",
data->shetsk.ravno_filtr(),
data->kodv.ravno_filtr());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;

kontrsh_create_list(data);

}

/********************************/
/*Проверка возможности удаления записи*/
/***************************************/

int kontrsh_provvudzap(const char *shet,const char *kontr,int met,GtkWidget *wpredok) //0-молча 1-сообщения
{
char strsql[512];

sprintf(strsql,"select kodkon from Saldo where ns='%s' and kodkon='%s' limit 1",shet,kontr);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Saldo",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);

   }
  return(1);
 }    

sprintf(strsql,"select kodkon from Prov where kodkon='%s' and sh='%s' limit 1",kontr,shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    sprintf(strsql,"%s Prov",gettext("Удалить невозможно ! Есть записи в таблице"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }    


return(0);
}

/*************************************/
/*Удаление не используемых кодов контрагентов*/
/*********************************************/

void  kontrsh_udnz(const char *shet)
{
char strsql[512];
kontrsh_udnz_data data;

data.shetsk.plus(shet);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Удаление контрагентов, которые не используются"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

//g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

GtkWidget *label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

label=gtk_label_new(gettext("Удаление контрагентов, которые не используются"));
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

g_idle_add((GSourceFunc)kontrsh_udnz11,&data);

gtk_main();

}
/**********************************/
/**********************************/
gint   kontrsh_udnz11(class kontrsh_udnz_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur1;
GtkTextIter iter;

//printf("kontr_udnz11 %f\n",data->kolstr1);

if(data->kolstr1 == 0)
 {
  sprintf(strsql,"select * from Skontr where ns='%s'",data->shetsk.ravno());
  if((data->kolstr=data->cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(FALSE);
   }
  
 }
iceb_u_str stroka;
while(data->cur.read_cursor(&row) != 0)
 {
//  printf("%-5s %-40.40s ",row[0],row[1]);

  sprintf(strsql,"%s ",row[0]);    
  stroka.new_plus(strsql);
  sprintf(strsql,"%s\n",row[1]);    
  stroka.plus(strsql);
  
//  g_print("%s",stroka.ravno());
    
  gtk_text_buffer_get_iter_at_offset(data->buffer,&iter,-1);
  gtk_text_buffer_insert(data->buffer,&iter,stroka.ravno(),-1);
  gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(data->view),&iter,0.0,TRUE,0.,1.);
  
  iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

  if(kontrsh_provvudzap(row[0],row[1],0,data->window) != 0)
   {
//    printf("используется\n");
    return(TRUE);
   }

//  printf("удаляем\n");


  sprintf(strsql,"delete from Skontr where ns='%s' and kodkon='%s'",row[0],row[1]);
  if(iceb_sql_zapis(strsql,1,1,data->window) != 0)
   continue;
  
  data->koludk++;

  return(TRUE);
  
 }




int		metkazap=0;

strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur1);
if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
//printf("VERSION:%s\n",row[0]);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table %s","Skontr");

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
void  kontrsh_rasspis(class kontr_data_sh *data)
{
char imaf[64];
FILE *ff;
int kolstr=0;
char strsql[512];
SQLCURSOR cur;
SQLCURSOR curos;
SQL_str row;
SQL_str row1;

sprintf(strsql,"select kodkon from Skontr where ns='%s' order by kodkon asc",
data->shetsk.ravno_filtr());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr == 0)
  return;
  
sprintf(imaf,"Skontr%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

iceb_u_startfil(ff);

time_t tmm;
struct tm *bf;

time(&tmm);
bf=localtime(&tmm);


fprintf(ff,"%s\n\n%s:%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
iceb_get_pnk("00",0,data->window),
gettext("Список контрагентов для счёта"),
data->shetsk.ravno(),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Код |           Наименование  контрагента    |   Наименование банка         |  Адрес контрагента           |\
  Адрес банка              |  Код     |   MФО    | Номер счета   |Нал. номер     |Н.пл. НДС |Телефон   |Группа|Рег.н.ЧП| Договор\n"));
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
int kl=0;
while(cur.read_cursor(&row1) != 0)
 {
  sprintf(strsql,"select * from Kontragent where kodkon='%s'",row1[0]);
  if(sql_readkey(&bd,strsql,&row,&curos) != 1)
   {
    iceb_u_str repl;
    repl.plus_ps(gettext("Не найден код контрагента в общем списке !"));
    repl.plus(row1[0]);
    iceb_menu_soob(&repl,data->window);
    continue;
   }

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
  row[5],row[6],row[7],row[8],
  row[9],row[10],row[11],row[15],row[14]);
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
%s:%d\n",gettext("Количество контрагентов"),kl);

iceb_podpis(ff,data->window);

fclose(ff);

iceb_u_spisok fil;
iceb_u_spisok naim;
fil.plus(imaf);
naim.plus(gettext("Список контрагентов"));
iceb_ustpeh(imaf,3,data->window);
iceb_rabfil(&fil,&naim,data->window);
}
/**************************************/
/*Стать на код контрагента           */
/*************************************/

void    kontrsh_snkod(class kontr_data_sh *data)
{
iceb_u_str zapros;
class iceb_u_str kodkon("");


zapros.plus(gettext("Введите нужный код контрагента"));
if(iceb_menu_vvod1(zapros.ravno(),&kodkon,62,"",data->window) != 0)
  return;

if(kodkon.ravno()[0] == '\0')
  return;

char strsql[512];

sprintf(strsql,"select kodkon from Skontr where ns='%s' and kodkon='%s'",data->shetsk.ravno(),kodkon.ravno());
printf("%s\n",strsql);
if(sql_readkey(&bd,strsql) != 1)
 {
  iceb_menu_soob(gettext("Нет такого кода в списке контрагентов !"),data->window);
  return;
 }


data->kontr_v.new_plus(kodkon.ravno());
kontrsh_create_list(data);

}
/*******************************/
/*Работа с общим списком контрагентов*/
/*************************************/
void  kontrsh_rosk(class kontr_data_sh *data,int metka_poi)
{
iceb_u_str kod;
iceb_u_str naikod;

if(metka_poi != 0)
 if(data->poisk.naimkon.getdlinna() > 1)
  naikod.plus(data->poisk.naimkon.ravno());
 
if(iceb_l_kontr(1,&kod,&naikod,data->window) != 0)
  return;

//Прверяем есть ли выбранный контрагент в списке счета
char strsql[512];

sprintf(strsql,"select kodkon from Skontr where ns='%s' and kodkon='%s'",
data->shetsk.ravno_filtr(),kod.ravno_filtr());
if(sql_readkey(&bd,strsql) == 1)
 {
  data->poisk.metka_poi=0;
  data->kontr_v.new_plus(kod.ravno());
  return;
 } 

iceb_u_str repl;
repl.plus(gettext("Добавить код контрагента"));
repl.plus(" ");
repl.plus(kod.ravno());
repl.plus(" ");
repl.plus(gettext("в список счета"));
repl.plus(" ");
repl.plus(data->shetsk.ravno());
repl.plus(" ?");

if(iceb_menu_danet(&repl,2,data->window) == 2)
  return;


time_t vrem;
time(&vrem);

sprintf(strsql,"insert into Skontr values('%s','%s',%d,%ld)",
data->shetsk.ravno_filtr(),kod.ravno_filtr(),iceb_getuid(data->window),vrem);

iceb_sql_zapis(strsql,0,0,data->window);

data->poisk.metka_poi=0;
data->kontr_v.new_plus(kod.ravno());
kontrsh_create_list (data);

}
