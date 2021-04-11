/*$Id: l_otrvr_t_v.c,v 1.21 2014/02/28 05:20:59 sasa Exp $*/
/*30.11.2016	08.09.2006	Белых А.И.	l_otrvr_t_v.c
Ввод и корректировка вида табеля
*/
#include "buhg_g.h"
enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOL_OTR_DNEI,
  E_KOL_OTR_HAS,
  E_KOL_KAL_DNEI,
  E_KOL_RAB_DNEI,
  E_KOL_RAB_HAS,
  E_DATAN,
  E_DATAK,
  E_KOMENT,
  E_NOM_ZAP,
  KOLENTER  
 };

class l_otrvr_t_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton1[2];
  GtkWidget *radiobutton2[2];
  GtkWidget *label_naishet;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  
  //Реквизиты меню
  class iceb_u_str kol_otr_dnei;
  class iceb_u_str kol_otr_has;
  class iceb_u_str kol_kal_dnei;
  class iceb_u_str kol_rab_dnei;
  class iceb_u_str kol_rab_has;
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str koment;
  class iceb_u_str nom_zap;
    
  //Реквизиты корректируемой записи
  int tabnom;
  short mp,gp;
  int kodk; //Код записи которую корректируют
  int nom_zapk;
  int metka_rr;
      
  float dnei; //Количество рабочих дней в просмотриваемом месяце
  float hasov; //Количество рабочих часов в просмотриваемом месяце
  short	maxkkd; /*Максимальное количество календарных дней*/
  
  double oklad; /*Найденный оклад*/
  int metka_sp;
  float has1;   /*Количество часов в одном рабочем дне*/
  float kof;    /*Коэффициент оплаты ставки*/
//  int met;      /*0-оклад 1-часовая ставка 2-не пересчитываемый оклад*/
//  int koddo;    /*Код должносного оклада*/
//  int metka;    /*0-нет метки 1-есть Метка пересчета на процент выполнения плана подразделением*/
//  int metka1;    /*0-расчёт выполнять по отработанным часам 1-дням*/
//  class iceb_u_str shet_do;
  
  l_otrvr_t_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
    maxkkd=1;
    metka_sp=0;
    has1=0.;
    kof=0.;
    
   }

  void read_rek()
   {
    kol_otr_dnei.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_OTR_DNEI])));
    kol_otr_has.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_OTR_HAS])));
    kol_kal_dnei.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_KAL_DNEI])));
    kol_rab_dnei.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_RAB_DNEI])));
    kol_rab_has.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_RAB_HAS])));
    datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
    nom_zap.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOM_ZAP])));
   }

  void clear_rek()
   {
  
    kol_otr_dnei.new_plus("");
    kol_otr_has.new_plus("");
    kol_kal_dnei.new_plus("");
    kol_rab_dnei.new_plus("");
    kol_rab_has.new_plus("");
    datan.new_plus("");
    datak.new_plus("");
    koment.new_plus("");
    nom_zap.new_plus("");
  
   }
 };

gboolean   l_otrvr_t_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_otrvr_t_v_data *data);
void    l_otrvr_t_v_v_vvod(GtkWidget *widget,class l_otrvr_t_v_data *data);
void  l_otrvr_t_v_v_knopka(GtkWidget *widget,class l_otrvr_t_v_data *data);
void otrvr_t_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int otrvr_t_zap(class l_otrvr_t_v_data *data);
int otrvr_t_pk(char *kod,GtkWidget *wpredok);
void  otrvr_t_v_e_knopka(GtkWidget *widget,class l_otrvr_t_v_data *data);

int l_otrvr_t_v_zap(class l_otrvr_t_v_data *data);




extern SQL_baza bd;



int l_otrvr_t_v(int metka_rr, //0-ввод новой записи 1- корректировка
int tabnom,short mp,short gp,int vid_tab,int nom_zap,GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
class iceb_u_str kikz;
class l_otrvr_t_v_data data;
data.tabnom=tabnom;
data.mp=mp;
data.gp=gp;
data.kodk=vid_tab;
data.nom_zapk=nom_zap;
data.metka_rr=metka_rr;
class iceb_u_str naim_tab("");

redkalw(data.mp,data.gp,&data.dnei,&data.hasov,wpredok);

iceb_u_dpm(&data.maxkkd,&data.mp,&data.gp,5);
//int metka_no=0;
data.oklad=okladw(data.tabnom,&data.metka_sp,wpredok);
//Читаем наименование табеля
sprintf(strsql,"select naik from Tabel where kod=%d",data.kodk);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_tab.new_plus(row[0]);




if(data.metka_rr != 0)
 { 
  sprintf(strsql,"select * from Ztab where tabn=%d and god=%d and mes=%d and kodt=%d and nomz=%d",
  data.tabnom,data.gp,data.mp,data.kodk,data.nom_zapk);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    class iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  if(row[4][0] != '0')  
   data.kol_otr_dnei.new_plus(atof(row[4]));
  if(row[5][0] != '0')  
   data.kol_otr_has.new_plus(atof(row[5]));
  if(row[6][0] != '0')  
   data.kol_kal_dnei.new_plus(atof(row[6]));
  if(row[11][0] != '0')  
   data.kol_rab_dnei.new_plus(atof(row[11]));
  if(row[12][0] != '0')  
   data.kol_rab_has.new_plus(atof(row[12]));
  data.datan.new_plus(iceb_u_datzap(row[7]));
  data.datak.new_plus(iceb_u_datzap(row[8]));
  data.koment.new_plus(row[13]);
  data.nom_zap.new_plus(row[14]);

  kikz.plus(iceb_kikz(row[9],row[10],wpredok));
  
 }







data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_rr != 0)
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
else
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_otrvr_t_v_v_key_press),&data);

iceb_u_str zagolov;
if(data.metka_rr == 0)
 zagolov.new_plus(gettext("Ввод новой записи"));
else
 {
  zagolov.new_plus(gettext("Корректировка записи"));
  zagolov.ps_plus(kikz.ravno());
 }
zagolov.ps_plus(data.kodk);
zagolov.plus(" ");
zagolov.plus(naim_tab.ravno());

GtkWidget *label=gtk_label_new(zagolov.ravno());

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator1);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator2=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator2);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

/**********
sprintf(strsql,"%s",gettext("Количество рабочих дней"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_OTR_DNEI]), label, FALSE, FALSE, 0);
****************/
sprintf(strsql,"%s",gettext("Количество рабочих дней"));
data.knopka_enter[E_KOL_OTR_DNEI]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_OTR_DNEI]), data.knopka_enter[E_KOL_OTR_DNEI], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOL_OTR_DNEI],"clicked",G_CALLBACK(otrvr_t_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOL_OTR_DNEI],iceb_u_inttochar(E_KOL_OTR_DNEI));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOL_OTR_DNEI],gettext("Вычисление количества дней если введена дата начала и дата конца"));

data.entry[E_KOL_OTR_DNEI] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOL_OTR_DNEI]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_OTR_DNEI]), data.entry[E_KOL_OTR_DNEI],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOL_OTR_DNEI], "activate",G_CALLBACK(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_OTR_DNEI]),data.kol_otr_dnei.ravno());
gtk_widget_set_name(data.entry[E_KOL_OTR_DNEI],iceb_u_inttochar(E_KOL_OTR_DNEI));

sprintf(strsql,"%s",gettext("Количество часов"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_OTR_HAS]), label, FALSE, FALSE, 0);

data.entry[E_KOL_OTR_HAS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOL_OTR_HAS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_OTR_HAS]), data.entry[E_KOL_OTR_HAS],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOL_OTR_HAS], "activate",G_CALLBACK(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_OTR_HAS]),data.kol_otr_has.ravno());
gtk_widget_set_name(data.entry[E_KOL_OTR_HAS],iceb_u_inttochar(E_KOL_OTR_HAS));
/****************
sprintf(strsql,"%s",gettext("Количество календарних дней"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_KAL_DNEI]), label, FALSE, FALSE, 0);
*****************/
sprintf(strsql,"%s",gettext("Количество календарних дней"));
data.knopka_enter[E_KOL_KAL_DNEI]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_KAL_DNEI]), data.knopka_enter[E_KOL_KAL_DNEI], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOL_KAL_DNEI],"clicked",G_CALLBACK(otrvr_t_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOL_KAL_DNEI],iceb_u_inttochar(E_KOL_KAL_DNEI));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOL_KAL_DNEI],gettext("Вычисление количества дней если введена дата начала и дата конца"));

data.entry[E_KOL_KAL_DNEI] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOL_KAL_DNEI]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_KAL_DNEI]), data.entry[E_KOL_KAL_DNEI],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOL_KAL_DNEI], "activate",G_CALLBACK(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_KAL_DNEI]),data.kol_kal_dnei.ravno());
gtk_widget_set_name(data.entry[E_KOL_KAL_DNEI],iceb_u_inttochar(E_KOL_KAL_DNEI));

sprintf(strsql,"%s",gettext("Количество рабочих дней в месяце"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_RAB_DNEI]), label, FALSE, FALSE, 0);

data.entry[E_KOL_RAB_DNEI] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOL_RAB_DNEI]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_RAB_DNEI]), data.entry[E_KOL_RAB_DNEI],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOL_RAB_DNEI], "activate",G_CALLBACK(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_RAB_DNEI]),data.kol_rab_dnei.ravno());
gtk_widget_set_name(data.entry[E_KOL_RAB_DNEI],iceb_u_inttochar(E_KOL_RAB_DNEI));

sprintf(strsql,"%s",gettext("Количество рабочих часов в дне"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_RAB_HAS]), label, FALSE, FALSE, 0);

data.entry[E_KOL_RAB_HAS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOL_RAB_HAS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_RAB_HAS]), data.entry[E_KOL_RAB_HAS],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOL_RAB_HAS], "activate",G_CALLBACK(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_RAB_HAS]),data.kol_rab_has.ravno());
gtk_widget_set_name(data.entry[E_KOL_RAB_HAS],iceb_u_inttochar(E_KOL_RAB_HAS));

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(otrvr_t_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

//sprintf(strsql,"%s",gettext("Дата конца"));
//label=gtk_label_new(strsql);
//gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), label, FALSE, FALSE, 0);
sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(otrvr_t_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));

sprintf(strsql,"%s",gettext("Коментарий"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),40);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));

sprintf(strsql,"%s",gettext("Номер записи"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_ZAP]), label, FALSE, FALSE, 0);

data.entry[E_NOM_ZAP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOM_ZAP]),40);
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_ZAP]), data.entry[E_NOM_ZAP],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOM_ZAP], "activate",G_CALLBACK(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOM_ZAP]),data.nom_zap.ravno());
gtk_widget_set_name(data.entry[E_NOM_ZAP],iceb_u_inttochar(E_NOM_ZAP));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_otrvr_t_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_otrvr_t_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);



}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  otrvr_t_v_e_knopka(GtkWidget *widget,class l_otrvr_t_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");
short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;
char bros[112];
int nom=0;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_KOL_OTR_DNEI:
    data->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAN])));
    data->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAK])));
    if(data->datan.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата начала!"),data->window);
      return;
     }      
    if(data->datak.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата конца!"),data->window);
      return;
     }      
    if(iceb_rsdatp(&dn,&mn,&gn,data->datan.ravno(),&dk,&mk,&gk,data->datak.ravno(),data->window) != 0)
     return;
       
    for(nom=0; iceb_u_sravmydat(dn,mn,gn,dk,mk,gk) <= 0; nom++)
      {
       iceb_u_dpm(&dn,&mn,&gn,1);     
       if(iceb_vixod(dn,mn,gn,data->window) == 0)
        nom--;
      }

    sprintf(bros,"%d",nom);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOL_OTR_DNEI]),bros);
        
    return;  

  case E_KOL_KAL_DNEI:
    data->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAN])));
    data->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAK])));
    if(data->datan.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата начала!"),data->window);
      return;
     }      
    if(data->datak.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата конца!"),data->window);
      return;
     }      
    if(iceb_rsdatp(&dn,&mn,&gn,data->datan.ravno(),&dk,&mk,&gk,data->datak.ravno(),data->window) != 0)
     return;
       
    for(nom=0; iceb_u_sravmydat(dn,mn,gn,dk,mk,gk) <= 0; nom++)
     iceb_u_dpm(&dn,&mn,&gn,1);     
    sprintf(bros,"%d",nom);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOL_KAL_DNEI]),bros);
        
    return;  
  case E_DATAN:

    if(iceb_calendar(&data->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->datak.ravno());
      
    return;  
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_otrvr_t_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_otrvr_t_v_data *data)
{

//printf("l_otrvr_t_v_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);


  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_otrvr_t_v_v_knopka(GtkWidget *widget,class l_otrvr_t_v_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    if(iceb_pbpds(data->mp,data->gp,data->window) != 0)
     return;
    if(l_otrvr_t_v_zap(data) != 0)
     return;

    gtk_widget_destroy(data->window);

    data->voz=0;
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_otrvr_t_v_v_vvod(GtkWidget *widget,class l_otrvr_t_v_data *data)
{
double bb;
char strsql[512];
int d;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_otrvr_t_v_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOL_OTR_DNEI:
    data->kol_otr_dnei.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    if(data->kol_otr_dnei.ravno_atoi() != 0)
     {
      data->kol_otr_has.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_KOL_OTR_HAS])));
      data->kol_kal_dnei.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_KOL_KAL_DNEI])));
      if(data->kol_otr_has.getdlinna() <= 1)
       {
        if(data->has1 != 0.)
          bb=data->kol_otr_dnei.ravno_atof()*data->has1*data->kof;
        else
         {
          if(data->kol_otr_dnei.ravno_atof() == data->dnei && data->hasov > 0.)
           bb=data->hasov;
          else
           bb=data->kol_otr_dnei.ravno_atof()*8;
         }
        bb=iceb_u_okrug(bb,0.01);
        sprintf(strsql,"%.6g",bb);
        data->kol_otr_has.new_plus(strsql);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOL_OTR_HAS]),strsql);
       }
      if(data->kol_kal_dnei.getdlinna() <= 1 && data->dnei == data->kol_otr_dnei.ravno_atof())
       {
        sprintf(strsql,"%d",data->maxkkd);
        data->kol_kal_dnei.new_plus(strsql);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOL_KAL_DNEI]),strsql);
       }
     }
    break;

  case E_KOL_OTR_HAS:
    data->kol_otr_has.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    bb=data->kol_otr_has.ravno_atof();
    if(iceb_u_polen(data->kol_otr_has.ravno(),strsql,sizeof(strsql),2,':') == 0)
     {
      bb=(bb*60+iceb_u_atof(strsql))/60;
     }
    if(bb != 0.)
     {
      sprintf(strsql,"%.10g",bb);
      data->kol_otr_has.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOL_OTR_HAS]),strsql);
     }
    break;

  case E_KOL_KAL_DNEI:
    data->kol_kal_dnei.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    d=data->kol_kal_dnei.ravno_atoi();
    if(d > data->maxkkd)
     {
      
      sprintf(strsql,"%s %d !",gettext("В этом месяце календарных дней не может быть больше"),data->maxkkd);
      iceb_menu_soob(strsql,data->window);
      
     }
    break;

  case E_KOL_RAB_DNEI:
    data->kol_rab_dnei.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOL_RAB_HAS:
    data->kol_rab_has.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATAN:
    data->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATAK:
    data->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOMENT:
    data->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NOM_ZAP:
    data->nom_zap.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/*********************/
/*запись*/
/*********************/

int l_otrvr_t_v_zap(class l_otrvr_t_v_data *data)
{
data->read_rek();

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(data->datan.getdlinna() > 1)
 if(iceb_u_rsdat(&dn,&mn,&gn,data->datan.ravno(),1) != 0)
  {
   iceb_menu_soob(gettext("Не правильно введена дата начала !"),data->window);
   return(1);
  }

if(data->datak.getdlinna() > 1)
 if(iceb_u_rsdat(&dk,&mk,&gk,data->datak.ravno(),1) != 0)
  {
   iceb_menu_soob(gettext("Не правильно введена дата конца !"),data->window);
   return(1);
  }

int voz=zaptabelw(data->metka_rr,data->mp,data->gp,data->tabnom,data->kodk,
data->kol_otr_dnei.ravno_atof(),
data->kol_otr_has.ravno_atof(),
data->kol_kal_dnei.ravno_atof(),
dn,mn,gn,
dk,mk,gk,
data->kol_rab_dnei.ravno_atof(),
data->kol_rab_has.ravno_atof(),
data->koment.ravno(),
data->nom_zap.ravno_atoi(),
data->nom_zapk,
data->window);
 
return(voz);

}
