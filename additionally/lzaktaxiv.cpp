/*$Id: lzaktaxiv.c,v 1.47 2013/08/13 06:10:25 sasa Exp $*/
/*16.04.2017	10.10.2003	Белых А.И.	lzaktaxiv.c
Ввод новой записи
*/
#include        <time.h>
#include        <stdlib.h>
#include        <unistd.h>
#include "taxi.h"
//#include "lzaktaxi.h"
#include "l_zaktaxi.h"

enum
{
 FK2,
 FK4,
 FK10,
 KOL_F_KL
};

enum
 {
  E_TELEF,
  E_KODKL,
  E_ADRES_POD,
  E_ADRES_POEZ,
  E_KOMENT,
  E_SUMA,
  E_KOLIH_PAS,
  E_FIO,
  E_DATA_ZAK,
  E_VREM_ZAK,
  KOLENTER
 };


class lzaktaxiv_data
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *entry[KOLENTER];
  GtkWidget *windowkodzav;
  GtkWidget *windowvoditel;
  GtkWidget *label_saldo; //Вывод сальдо
  short     kl_shift; //0-отжата 1-нажата  
  int       voz;
  int      metkavz; //0-запись не выбрана 1-выбрана
  int      metkarr; //0-только не завершенные заказы 1-все
//  int       metkazapisi; //0-новая запись 1-корректировка
  time_t vremnz;       

//  class zaktaxi_rek zap_zaktaxi;
  class l_zaktaxi_rek zap_zaktaxi;
      
  //реквизиты для корректировки строки
  class iceb_u_str vrem;
  class iceb_u_str datz;
  class iceb_u_str vremz;
  unsigned int un_nom_zap;
    
  lzaktaxiv_data()
   {
    un_nom_zap=0;
    voz=0;
    kl_shift=0;
   }
  void read_rek()
   {

    zap_zaktaxi.telef.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TELEF])));
    zap_zaktaxi.kodk.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODKL])));
    zap_zaktaxi.fio.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FIO])));
    zap_zaktaxi.kolp.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIH_PAS])));
    zap_zaktaxi.adreso.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ADRES_POD])));
    zap_zaktaxi.adresk.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ADRES_POEZ])));
    zap_zaktaxi.koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
    zap_zaktaxi.suma.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA])));
    zap_zaktaxi.datvz.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_ZAK])));
    zap_zaktaxi.vremvz.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_VREM_ZAK])));


    zap_zaktaxi.kodzav.new_plus(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(windowkodzav)));
    zap_zaktaxi.kv.new_plus(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(windowvoditel)));

   }
  
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    zap_zaktaxi.clear_data();
   }
 };

void get_telef(GtkWidget *,class lzaktaxiv_data *);
void clear_menu(class lzaktaxiv_data *);
gboolean   vzaktaxi_key_press(GtkWidget *widget,GdkEventKey *event,class lzaktaxiv_data *v_data);
void       kodkl_get(GtkWidget *,class lzaktaxiv_data*);
void       voditel_get(GtkWidget*,class lzaktaxiv_data*);
void       vzaktaxi_knopka(GtkWidget *,class lzaktaxiv_data *);
int    zzaktaxi(class lzaktaxiv_data *v_data);
void  kodzav_get(GtkWidget *widget,class lzaktaxiv_data *v_data);

extern SQL_baza	bd;
extern char *name_system;

int  lzaktaxiv(int un_nom_zap,GtkWidget *wpredok)
{
class lzaktaxiv_data v_data;
v_data.zap_zaktaxi.clear_data();

char    strsql[512];
SQL_str row;
short d,m,g;
time_t vrem;

v_data.un_nom_zap=un_nom_zap; 

time(&v_data.vremnz);

//v_data->zap_zaktaxi.clear_data();

v_data.zap_zaktaxi.suma.new_plus("1");

if(un_nom_zap > 0)
 {
  SQL_str row,row1;
  class SQLCURSOR cur,cur1;
  sprintf(strsql,"select * from Taxizak where nz=%d",un_nom_zap);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s %d!",gettext("Не найдена запись с номером"),un_nom_zap);
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }

  v_data.vrem.new_plus(row[16]);
  v_data.datz.new_plus(row[8]);
  v_data.vremz.new_plus(row[9]);


  if(row[0][0] != '\0')
   {
    sprintf(strsql,"select naik from Taxikzz where kod=%s",row[0]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
     {
      sprintf(strsql,"%s %s",row[0],row1[0]);
      v_data.zap_zaktaxi.kodzav.new_plus(strsql);

     }
   }


  v_data.zap_zaktaxi.kodk.new_plus(row[1]);
  if(row[2][0] != '\0')
   {
    sprintf(strsql,"select fio,gosn from Taxivod where kod='%s'",row[2]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
     {    
      sprintf(strsql,"%s %s %s",row[2],row1[0],row1[1]);
      v_data.zap_zaktaxi.kv.new_plus(strsql);
     }
   }


  v_data.zap_zaktaxi.fio.new_plus(row[3]);
  v_data.zap_zaktaxi.telef.new_plus(row[4]);
  v_data.zap_zaktaxi.kolp.new_plus(row[5]);

  v_data.zap_zaktaxi.datvz.new_plus(iceb_u_datzap(row[6]));

  v_data.zap_zaktaxi.vremvz.new_plus(row[7]);
  v_data.zap_zaktaxi.adreso.new_plus(row[10]);
  v_data.zap_zaktaxi.adresk.new_plus(row[11]);
  v_data.zap_zaktaxi.koment.new_plus(row[12]);
  v_data.zap_zaktaxi.suma.new_plus(row[13]);
  
 }

v_data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
//gtk_window_set_position( GTK_WINDOW(v_data.window),ICEB_POS_CENTER);
//if(v_data.metkazapisi == 1)

gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
//Удерживать окно над породившем его окном всегда
gtk_window_set_transient_for(GTK_WINDOW(v_data.window),GTK_WINDOW(wpredok));

gtk_window_set_modal( GTK_WINDOW(v_data.window) ,TRUE ); 

if(un_nom_zap == 0)
  sprintf(strsql,"%s %s",NAME_SYSTEM,"Ввод новой записи");
else
  sprintf(strsql,"%s %s",NAME_SYSTEM,"Корректировка записи");
gtk_window_set_title(GTK_WINDOW(v_data.window),strsql);

g_signal_connect(v_data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(v_data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(v_data.window,"key_press_event",G_CALLBACK(vzaktaxi_key_press),&v_data);

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox2),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox3),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox4),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox5),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox6),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox7 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox7),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox8 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox8),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (v_data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1); //Адрес подачи
gtk_container_add (GTK_CONTAINER (vbox), hbox2); //Адрес поездки
gtk_container_add (GTK_CONTAINER (vbox), hbox3); //Код водителя
gtk_container_add (GTK_CONTAINER (vbox), hbox5); //Код завершения
gtk_container_add (GTK_CONTAINER (vbox), hbox); //Телефон, код клиента
gtk_container_add (GTK_CONTAINER (vbox), hbox4); //Коментарий
gtk_container_add (GTK_CONTAINER (vbox), hbox6); //Сумма, количество пассажиров
gtk_container_add (GTK_CONTAINER (vbox), hbox7); //Фамилия клиента
gtk_container_add (GTK_CONTAINER (vbox), hbox8); //Дата , время выполнения заказа
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new("Телефон");
v_data.entry[E_TELEF] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), v_data.entry[E_TELEF], TRUE, TRUE, 0);
g_signal_connect(v_data.entry[E_TELEF], "activate",G_CALLBACK(get_telef),&v_data);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_TELEF]),v_data.zap_zaktaxi.telef.ravno());

label=gtk_label_new("Код клиента");
v_data.entry[E_KODKL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), v_data.entry[E_KODKL], TRUE, TRUE, 0);
g_signal_connect(v_data.entry[E_KODKL], "activate",G_CALLBACK(kodkl_get),&v_data);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_KODKL]),v_data.zap_zaktaxi.kodk.ravno());


label=gtk_label_new("Адрес подачи");
v_data.entry[E_ADRES_POD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), v_data.entry[E_ADRES_POD], TRUE, TRUE, 0);
g_signal_connect(v_data.entry[E_ADRES_POD], "activate",G_CALLBACK(iceb_get_text_str),&v_data.zap_zaktaxi.adreso);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_ADRES_POD]),v_data.zap_zaktaxi.adreso.ravno());


label=gtk_label_new("Адрес поездки");
v_data.entry[E_ADRES_POEZ] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), v_data.entry[E_ADRES_POEZ], TRUE, TRUE, 0);
g_signal_connect(v_data.entry[E_ADRES_POEZ], "activate",G_CALLBACK(iceb_get_text_str),&v_data.zap_zaktaxi.adresk);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_ADRES_POEZ]),v_data.zap_zaktaxi.adresk.ravno());



label=gtk_label_new("Код водителя");
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);

//Создаем выбор водителя
//GList *glist=NULL;

SQLCURSOR cur;
sprintf(strsql,"select kod,fio,gosn from Taxivod where sm=0 order by fio asc");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);

class iceb_u_spisok VOD;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s %s",row[0],row[1],row[2]);
  VOD.plus(strsql);
 }
if(kolstr == 0)
 VOD.plus("Не введен список водителей !!!");
/*******************
v_data.windowvoditel=gtk_combo_new();


for(i=0 ; i < kolstr; i++)
  glist=g_list_append(glist,(void*)VOD.ravno(i));

gtk_combo_set_popdown_strings(GTK_COMBO(v_data.windowvoditel),glist);
gtk_combo_disable_activate(GTK_COMBO(v_data.windowvoditel));
g_signal_connect(GTK_COMBO(v_data.windowvoditel->entry), "activate",G_CALLBACK(voditel_get),&v_data);
gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(v_data.windowvoditel)->entry),v_data.zap_zaktaxi.kv.ravno());
**************************/

iceb_pm_vibor_enter(&VOD,&v_data.windowvoditel,v_data.zap_zaktaxi.kv.ravno());
gtk_box_pack_start (GTK_BOX (hbox3), v_data.windowvoditel, TRUE, TRUE, 0);

label=gtk_label_new("Коментарий");
v_data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), v_data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(v_data.entry[E_KOMENT], "activate",G_CALLBACK(iceb_get_text_str),&v_data.zap_zaktaxi.koment);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_KOMENT]),v_data.zap_zaktaxi.koment.ravno());


label=gtk_label_new("Код завершения");
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE, 0);
//Создаем меню кодов завершения

//glist=NULL;

sprintf(strsql,"select kod,naik from Taxikzz order by kod asc");
kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);

iceb_u_spisok KZV;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s",row[0],row[1]);
  KZV.plus(strsql);
 }
if(kolstr == 0)
  KZV.plus("Не введен список кодов завершения !!!");

/********************
v_data.windowkodzav=gtk_combo_new();


for(i=0 ; i < kolstr; i++)
  glist=g_list_append(glist,(void*)KZV.ravno(i));

gtk_combo_set_popdown_strings(GTK_COMBO(v_data.windowkodzav),glist);
gtk_combo_disable_activate(GTK_COMBO(v_data.windowkodzav));
g_signal_connect(GTK_COMBO(v_data.windowkodzav->entry), "activate",G_CALLBACK(kodzav_get),&v_data);
//Если не выводить то в меню будет первая строка из списка
//if(gl_data->metkazapisi == 1)
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(v_data.windowkodzav)->entry),v_data.zap_zaktaxi.kodzav.ravno());
*******************************/

iceb_pm_vibor_enter(&VOD,&v_data.windowkodzav,v_data.zap_zaktaxi.kodzav.ravno());
gtk_box_pack_start (GTK_BOX (hbox5), v_data.windowkodzav, TRUE, TRUE, 0);

label=gtk_label_new("Сумма");
v_data.entry[E_SUMA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), v_data.entry[E_SUMA], TRUE, TRUE, 0);
g_signal_connect(v_data.entry[E_SUMA], "activate",G_CALLBACK(iceb_get_text_str),&v_data.zap_zaktaxi.suma);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_SUMA]),v_data.zap_zaktaxi.suma.ravno());

double suma=0.;

if(v_data.zap_zaktaxi.kodk.getdlinna() > 1)
 suma=taxi_saldo(v_data.zap_zaktaxi.kodk.ravno());

sprintf(strsql,"Сальдо: %.2f ",suma);
v_data.label_saldo=gtk_label_new(strsql);

gtk_box_pack_start (GTK_BOX (hbox6), v_data.label_saldo, FALSE, FALSE, 0);

label=gtk_label_new("Количество пассажиров");
v_data.entry[E_KOLIH_PAS] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), v_data.entry[E_KOLIH_PAS], TRUE, TRUE, 0);
g_signal_connect(v_data.entry[E_KOLIH_PAS], "activate",G_CALLBACK(iceb_get_text_str),&v_data.zap_zaktaxi.kolp);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_KOLIH_PAS]),v_data.zap_zaktaxi.kolp.ravno());


/********************************/

label=gtk_label_new("ФИО");
v_data.entry[E_FIO] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox7), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox7), v_data.entry[E_FIO], TRUE, TRUE, 0);
g_signal_connect(v_data.entry[E_FIO], "activate",G_CALLBACK(iceb_get_text_str),&v_data.zap_zaktaxi.fio);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_FIO]),v_data.zap_zaktaxi.fio.ravno());


label=gtk_label_new("Дата вып.заказа");
v_data.entry[E_DATA_ZAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(v_data.entry[E_DATA_ZAK]),11);
gtk_box_pack_start (GTK_BOX (hbox8), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox8), v_data.entry[E_DATA_ZAK], TRUE, TRUE, 0);
g_signal_connect(v_data.entry[E_DATA_ZAK], "activate",G_CALLBACK(iceb_get_text_str),&v_data.zap_zaktaxi.datvz);
time(&vrem);
//bf=localtime(&vrem);  

if(un_nom_zap > 0)
 {
  gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_DATA_ZAK]),v_data.zap_zaktaxi.datvz.ravno());
 }

label=gtk_label_new("Время вып.заказа");
v_data.entry[E_VREM_ZAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(v_data.entry[E_VREM_ZAK]),9);
gtk_box_pack_start (GTK_BOX (hbox8), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox8), v_data.entry[E_VREM_ZAK], TRUE, TRUE, 0);
g_signal_connect(v_data.entry[E_VREM_ZAK], "activate",G_CALLBACK(iceb_get_text_str),&v_data.zap_zaktaxi.vremvz);

if(un_nom_zap > 0)
 {
  iceb_u_rstime(&d,&m,&g,v_data.zap_zaktaxi.vremvz.ravno());
  if(d != 0)
   sprintf(strsql,"%02d:%02d",d,m);
  gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_VREM_ZAK]),strsql);
 }

sprintf(strsql,"F2 %s",gettext("Запись"));
v_data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(v_data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(v_data.knopka[FK2],"clicked",G_CALLBACK(vzaktaxi_knopka),&v_data);
gtk_widget_set_name(v_data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), v_data.knopka[FK2], TRUE, TRUE, 0);

if(un_nom_zap == 0)
 {
  sprintf(strsql,"F4 %s",gettext("Очистить"));
  v_data.knopka[FK4]=gtk_button_new_with_label(strsql);
  gtk_widget_set_tooltip_text(v_data.knopka[FK4],gettext("Очистить меню от введеноой информации. Установить текущее время и дату"));
  g_signal_connect(v_data.knopka[FK4],"clicked",G_CALLBACK(vzaktaxi_knopka),&v_data);
  gtk_widget_set_name(v_data.knopka[FK4],iceb_u_inttochar(FK4));
  gtk_box_pack_start(GTK_BOX(hboxknop), v_data.knopka[FK4], TRUE, TRUE, 0);
 }

sprintf(strsql,"F10 %s",gettext("Выход"));
v_data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(v_data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(v_data.knopka[FK10],"clicked",G_CALLBACK(vzaktaxi_knopka),&v_data);
gtk_widget_set_name(v_data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), v_data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(v_data.entry[E_ADRES_POD]);

if(un_nom_zap == 0)
  clear_menu(&v_data);
gtk_widget_show_all (v_data.window);

gtk_main();

gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
return(v_data.voz);
}
/*************************/
/*Обработчик кода клиента*/
/**************************/
void  kodkl_get(GtkWidget *widget,class lzaktaxiv_data *v_data)
{
char kod[300];
char strsql[512];
SQL_str row;

//printf("kodkl_get\n");

strcpy(kod,gtk_entry_get_text(GTK_ENTRY(widget)));

  v_data->zap_zaktaxi.kodk.new_plus(kod);
  if(v_data->zap_zaktaxi.kodk.getdlinna() <= 1)
   return;

sprintf(strsql,"select fio,adres,telef from Taxiklient where kod='%s'",kod);
//printf("%s\n",strsql);
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"Нет клиента с кодом %s !!!",kod);
  repl.plus(strsql);
  iceb_menu_soob(&repl,NULL);
  return; 
 }


v_data->zap_zaktaxi.fio.new_plus(row[0]);

v_data->zap_zaktaxi.adreso.new_plus(gtk_entry_get_text(GTK_ENTRY(v_data->entry[E_ADRES_POD])));

if(v_data->zap_zaktaxi.adreso.getdlinna() <= 1)
  v_data->zap_zaktaxi.adreso.new_plus(row[1]);

v_data->zap_zaktaxi.telef.new_plus(gtk_entry_get_text(GTK_ENTRY(v_data->entry[E_TELEF])));

if(v_data->zap_zaktaxi.telef.getdlinna() <= 1)
  v_data->zap_zaktaxi.telef.new_plus(row[2]);

iceb_u_str stroka;

stroka.plus(v_data->zap_zaktaxi.fio.ravno());
gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_FIO]),stroka.ravno());

stroka.new_plus(v_data->zap_zaktaxi.adreso.ravno());
gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_ADRES_POD]),stroka.ravno());

stroka.new_plus(v_data->zap_zaktaxi.telef.ravno());
gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_TELEF]),stroka.ravno());


double suma=taxi_saldo(v_data->zap_zaktaxi.kodk.ravno());
char bros[50];
sprintf(bros,"Сальдо: %.2f ",suma);
stroka.new_plus(bros);
gtk_label_set_text(GTK_LABEL(v_data->label_saldo),stroka.ravno());

gtk_editable_select_region(GTK_EDITABLE(widget),0,strlen(kod));

}

/*************************************/
/*Обработчик сигнала выбора водителя*/
/************************************/
void  voditel_get(GtkWidget *widget,class lzaktaxiv_data *v_data)
{
char bros[300];
char strsql[1024];
SQL_str row;
class iceb_u_str kod;

//printf("voditel_get\n");

kod.plus(gtk_entry_get_text(GTK_ENTRY(widget)));

  v_data->zap_zaktaxi.kv.new_plus(kod.ravno());
  if(v_data->zap_zaktaxi.kv.getdlinna() <= 1)
   return;


if(kod.ravno()[0] == '\0')
 return;

memset(bros,'\0',sizeof(bros));
if(iceb_u_polen(kod.ravno(),bros,sizeof(bros),1,' ') != 0)
  strncpy(bros,kod.ravno(),sizeof(bros)-1);

sprintf(strsql,"select fio,gosn,sm from Taxivod where kod='%s'",bros);
//printf("%s\n",strsql);
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"Нет водителя с кодом %s !!!",bros);
  repl.plus(strsql);
  iceb_menu_soob(&repl,NULL);
  return; 
 }

sprintf(strsql,"%s %s %s",bros,row[0],row[1]);

if(atoi(row[2]) != 0)
 {
  iceb_u_str repl;
  repl.plus("Этого водителя нет на смене !!!");
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,NULL);
  gtk_entry_set_text(GTK_ENTRY(widget),"");
  return; 

 }
 
gtk_entry_set_text(GTK_ENTRY(widget),strsql);
gtk_editable_select_region(GTK_EDITABLE(widget),0,strlen(strsql));

}

/*************************************/
/*Обработчик сигнала выбора кода завершения*/
/************************************/
void  kodzav_get(GtkWidget *widget,class lzaktaxiv_data *v_data)
{
char bros[1025];
char strsql[2048];
SQL_str row;
SQLCURSOR cur;
char  kod[1024];

//printf("kodzav_get**********************************\n");

strcpy(kod,gtk_entry_get_text(GTK_ENTRY(widget)));

  v_data->zap_zaktaxi.kodzav.new_plus(kod);
  if(v_data->zap_zaktaxi.kodzav.getdlinna() <= 1)
   return;


if(kod[0] == '\0')
 return;

memset(bros,'\0',sizeof(bros));
if(iceb_u_polen(kod,bros,sizeof(bros),1,' ') != 0)
  strncpy(bros,kod,sizeof(bros)-1);

sprintf(strsql,"select naik from Taxikzz where kod=%s",bros);
//printf("%s\n",strsql);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"Нет такого кода завершения !!!");
  repl.plus(strsql);
  iceb_menu_soob(&repl,NULL);
  return; 
 }

sprintf(strsql,"%s %s",bros,row[0]);

gtk_entry_set_text(GTK_ENTRY(widget),strsql);
gtk_editable_select_region(GTK_EDITABLE(widget),0,strlen(strsql));

}


/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  vzaktaxi_knopka(GtkWidget *widget,class lzaktaxiv_data *v_data)
{

int knop=atoi(gtk_widget_get_name(widget));
//printf("vzaktaxi_knopka knop=%s\n",knop);
switch ((gint)knop)
 {
  case FK2:
    if(zzaktaxi(v_data) == 0)
     {
      if(v_data->un_nom_zap > 0)
       {
        gtk_widget_destroy(v_data->window);
       }
      else
       clear_menu(v_data);
     }
    return;  

  case FK4:
   clear_menu(v_data);
   return;
   
  case FK10:
    gtk_widget_destroy(v_data->window);
    return;
 }
}
 
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vzaktaxi_key_press(GtkWidget *widget,GdkEventKey *event,class lzaktaxiv_data *v_data)
{
//char  bros[300];

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(v_data->knopka[FK2],"clicked");
    return(TRUE);
    
  case GDK_KEY_F4:
    g_signal_emit_by_name(v_data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(v_data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    v_data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}


/**************************/
/*Запись строки в таблицу*/
/**************************/
//Если вернули 0-записали 1-нет
int    zzaktaxi(class lzaktaxiv_data *v_data)
{
char bros[300];
char strsql[2048];
time_t   vrem;
struct tm *bf;
SQL_str row;
SQLCURSOR cur;


v_data->read_rek();

if(v_data->zap_zaktaxi.datvz.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена дата выполнения заказа !"),v_data->window);
  return(1);
 }

if(v_data->zap_zaktaxi.vremvz.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введено время выполнения заказа !"),v_data->window);
  return(1);
 }

 
short  gvz,mvz,dvz;
short  hvz,mnvz,svz;

if(iceb_u_rsdat(&dvz,&mvz,&gvz,v_data->zap_zaktaxi.datvz.ravno(),1) != 0)
 {
  iceb_u_str sp;
  sp.plus_ps(gettext("Не верно введена дата выполнения заказа !"));
  sp.plus(v_data->zap_zaktaxi.datvz.ravno());
  iceb_menu_soob(&sp,v_data->window);
  return(1);
 }

if(iceb_u_rstime(&hvz,&mnvz,&svz,v_data->zap_zaktaxi.vremvz.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Не верно введено время выполнения заказа !"),v_data->window);
  return(1);
 }

if(v_data->zap_zaktaxi.kodk.getdlinna() > 1)
 {

  sprintf(strsql,"select fio,adres,telef from Taxiklient where kod='%s'",v_data->zap_zaktaxi.kodk.ravno());
  //printf("%s\n",strsql);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !!!",gettext("Нет клиента с кодом"),v_data->zap_zaktaxi.kodk.ravno());
    iceb_menu_soob(strsql,v_data->window);
    return(1); 
   }
 }
 
if(v_data->zap_zaktaxi.kv.getdlinna() > 1)
 {
  memset(bros,'\0',sizeof(bros));
  if(iceb_u_polen(v_data->zap_zaktaxi.kv.ravno(),bros,sizeof(bros),1,' ') != 0)
    strncpy(bros,v_data->zap_zaktaxi.kv.ravno(),sizeof(bros)-1);

  sprintf(strsql,"select fio,sm from Taxivod where kod='%s'",bros);
  //printf("%s\n",strsql);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    iceb_u_str repl;
    sprintf(strsql,"%s %s !!!",gettext("Нет водителя с кодом"),bros);
    repl.plus(strsql);
    iceb_menu_soob(&repl,v_data->window);
    return(1); 
   }

  if(v_data->un_nom_zap == 0)
   if(atoi(row[1]) != 0)
    {
     iceb_u_str repl;
     repl.plus(gettext("Этого водителя нет на смене!"));
     repl.ps_plus(bros);
     repl.plus(" ");
     repl.plus(row[0]);
     iceb_menu_soob(&repl,v_data->window);
     return(1); 
    }

  v_data->zap_zaktaxi.kv.new_plus(bros);
 }

if(v_data->zap_zaktaxi.kodzav.getdlinna() > 1)
 {
  memset(bros,'\0',sizeof(bros));
  if(iceb_u_polen(v_data->zap_zaktaxi.kodzav.ravno(),bros,sizeof(bros),1,' ') != 0)
    strncpy(bros,v_data->zap_zaktaxi.kodzav.ravno(),sizeof(bros)-1);

  sprintf(strsql,"select naik from Taxikzz where kod=%s",bros);
  //printf("%s\n",strsql);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"Нет кода завершения %s !!!",bros);
    iceb_menu_soob(strsql,v_data->window);
    return(1); 
   }
  v_data->zap_zaktaxi.kodzav.new_plus(bros);
 }



time(&vrem);
bf=localtime(&vrem);
if(v_data->un_nom_zap == 0)
 {
  bf=localtime(&v_data->vremnz);

    
  sprintf(strsql,"insert into Taxizak (kz,kodk,kv,fio,telef,kolp,datvz,vremvz,datz,vremz,adreso,adresk,koment,suma,vremzz,ktoi,vrem) \
values (%d,'%s','%s','%s','%s',%d,\
'%04d-%02d-%02d','%02d:%02d:%02d',\
'%04d-%02d-%02d','%02d:%02d:%02d',\
'%s','%s','%s',%.2f,%d,%d,%ld)",
  v_data->zap_zaktaxi.kodzav.ravno_atoi(),\
  v_data->zap_zaktaxi.kodk.ravno(),\
  v_data->zap_zaktaxi.kv.ravno(),\
  v_data->zap_zaktaxi.fio.ravno_filtr(),\
  v_data->zap_zaktaxi.telef.ravno(),\
  v_data->zap_zaktaxi.kolp.ravno_atoi(),\
  gvz,mvz,dvz,\
  hvz,mnvz,svz,\
  bf->tm_year+1900,\
  bf->tm_mon+1,\
  bf->tm_mday,\
  bf->tm_hour,
  bf->tm_min,
  bf->tm_sec,
  v_data->zap_zaktaxi.adreso.ravno_filtr(),\
  v_data->zap_zaktaxi.adresk.ravno_filtr(),\
  v_data->zap_zaktaxi.koment.ravno_filtr(),\
  v_data->zap_zaktaxi.suma.ravno_atof(),\
  0,\
  iceb_getuid(v_data->window),\
  vrem);
  
 }
else
 {

  sprintf(strsql,"update Taxizak \
set \
kz=%d,\
kodk='%s',\
kv='%s',\
fio='%s',\
telef='%s',\
kolp='%d',\
datvz='%04d-%02d-%02d',\
vremvz='%02d:%02d:%02d',\
adreso='%s',\
adresk='%s',\
koment='%s',\
suma=%.2f,\
ktoi=%d,\
vrem=%ld \
where nz=%d",\
  atoi(v_data->zap_zaktaxi.kodzav.ravno()),\
  v_data->zap_zaktaxi.kodk.ravno(),\
  v_data->zap_zaktaxi.kv.ravno(),\
  v_data->zap_zaktaxi.fio.ravno(),\
  v_data->zap_zaktaxi.telef.ravno(),\
  atoi(v_data->zap_zaktaxi.kolp.ravno()),\
  gvz,mvz,dvz,\
  hvz,mnvz,svz,\
  v_data->zap_zaktaxi.adreso.ravno(),\
  v_data->zap_zaktaxi.adresk.ravno(),\
  v_data->zap_zaktaxi.koment.ravno(),\
  atof(v_data->zap_zaktaxi.suma.ravno()),\
  iceb_getuid(v_data->window),\
  vrem,\
  v_data->un_nom_zap);
 }

printf("%s-%s\n",__FUNCTION__,strsql);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),v_data->window);
   return(1);
  }
 else
  {
   iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,v_data->window);
   return(1);
  }
 }
printf("%s\n",__FUNCTION__);
//Проверяем есть ли телефон в списке телефонов
if(v_data->zap_zaktaxi.telef.getdlinna() > 1 && v_data->zap_zaktaxi.telef.ravno()[0] != '8')
 {
  sprintf(strsql,"select tel from Taxitel where tel='%s'",v_data->zap_zaktaxi.telef.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) == 0)
   {
    sprintf(strsql,"insert into Taxitel \
values ('%s','%s',%d,%ld)",v_data->zap_zaktaxi.telef.ravno(),v_data->zap_zaktaxi.adreso.ravno(),iceb_getuid(v_data->window),vrem);
    if(sql_zap(&bd,strsql) != 0)
       iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,v_data->window);
   }
 }


if(v_data->un_nom_zap > 0)
  gtk_widget_hide(v_data->window); //Скрываем от показа виджет

/*обновляем список и не выходим из меню*/
extern class l_zaktaxi_data data;
l_zaktaxi_create_list (&data);

return(0);
}
/**************************************/
/*Проверка телефона в списке телефонов*/
/**************************************/

void get_telef(GtkWidget *widget,class lzaktaxiv_data *v_data)
{

//char bros[300];
char strsql[300];
SQL_str row;
char  kod[100];

//printf("kodzav_get\n");

strcpy(kod,gtk_entry_get_text(GTK_ENTRY(widget)));

v_data->zap_zaktaxi.telef.new_plus(kod);

if(kod[0] == '\0')
 return;



sprintf(strsql,"select adres from Taxitel where tel='%s'",kod);
//printf("%s\n",strsql);
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  v_data->zap_zaktaxi.adreso.new_plus(gtk_entry_get_text(GTK_ENTRY(v_data->entry[E_ADRES_POD])));

  if(v_data->zap_zaktaxi.adreso.getdlinna() <= 1)
   gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_ADRES_POD]),row[0]);
 }

gtk_entry_set_text(GTK_ENTRY(widget),kod);
//gtk_editable_select_region(GTK_EDITABLE(widget),0,strlen(kod));

}
/************************/
/*Очистка меню и запись нового времени*/
/**************************************/

void clear_menu(class lzaktaxiv_data *v_data)
{

time(&v_data->vremnz);
//??memset(&v_data->zap_zaktaxi,'\0',sizeof(v_data->zap_zaktaxi));

for(int i=0; i < KOLENTER; i++)
 gtk_entry_set_text(GTK_ENTRY(v_data->entry[i]),"");

//Код водителя
//gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(v_data->windowvoditel)->entry),"");
gtk_combo_box_set_active_id(GTK_COMBO_BOX(v_data->windowvoditel),"");
//Код завершения
//gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(v_data->windowkodzav)->entry),"");
gtk_combo_box_set_active_id(GTK_COMBO_BOX(v_data->windowkodzav),"");
struct tm *bf;

bf=localtime(&v_data->vremnz);  
char bros[100];
sprintf(bros,"%02d.%02d.%04d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
v_data->zap_zaktaxi.datvz.new_plus(bros);

gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_DATA_ZAK]),v_data->zap_zaktaxi.datvz.ravno());

sprintf(bros,"%02d:%02d:%02d",bf->tm_hour,bf->tm_min,bf->tm_sec);
v_data->zap_zaktaxi.vremvz.new_plus(bros);
gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_VREM_ZAK]),v_data->zap_zaktaxi.vremvz.ravno());

v_data->zap_zaktaxi.suma.new_plus("1");
gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_SUMA]),v_data->zap_zaktaxi.suma.ravno());

gtk_widget_grab_focus(v_data->entry[E_ADRES_POD]);
//gtk_widget_grab_focus(v_data->entry[E_TELEF]);


}
