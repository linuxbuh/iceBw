/*$Id: iceb_l_kontrv.c,v 1.64 2014/02/28 05:21:03 sasa Exp $*/
/*31.03.2016	03.12.2003	Белых А.И.	iceb_l_kontrv.c
Ввод и корректировка записей в список контрагентов
*/
#include <errno.h>
#include "iceb_libbuh.h"
#include "iceb_l_kontr.h"
enum
 {
  E_KONTR,
  E_NAIMK,
  E_ADRES,
  E_KOD,
  E_NOMSH,
  E_MFO,
  E_NAIMB,
  E_ADRESB,
  E_NSPNDS,
  E_GRUP,
  E_TELEF,
  E_NALADRES,
  E_REGNOM,
  E_INNN,
  E_NAIMP,
  E_GK,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK5,
  FK6,
  FK7,
  FK10,
  KOL_FK
 };

class kontr_korkod_data
 {
  public:
  GtkWidget *window;
  iceb_u_str skod;//Старый код контрагента
  iceb_u_str nkod;//Новый код контрагента
 };

class iceb_l_kontrv_data
 {
  public:
    
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *opt1;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  class iceb_u_str name_window;

  class kontr_rek rk;
  
  class iceb_u_str kontr_i;
 
  class iceb_u_str shetsk; //Счёт списка контрагента    
  
  iceb_l_kontrv_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kodkontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR])));
    rk.naimkon.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIMK])));
    rk.naimp.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIMP])));
    rk.naimban.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIMB])));
    rk.adres.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ADRES])));
    rk.adresb.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ADRESB])));
    rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    rk.mfo.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MFO])));
    rk.nomsh.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMSH])));
    rk.nspnds.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NSPNDS])));
    rk.telef.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TELEF])));
    rk.grup.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GRUP])));
    rk.regnom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_REGNOM])));
    rk.innn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_INNN])));
    rk.na.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NALADRES])));
    rk.gk.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GK])));
    rk.metka_nal=gtk_combo_box_get_active (GTK_COMBO_BOX(opt1));

/*******
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
************/
   }
 };

void       kontr_vdi(const char *kodkon,GtkWidget*);
gboolean   kontrv_delete_event(GtkWidget *widget,GdkEvent *event,class iceb_l_kontrv_data *data);
gboolean   kontrv_key_press(GtkWidget *,GdkEventKey *,class iceb_l_kontrv_data*);
void       kontrv_knopka(GtkWidget *widget,class iceb_l_kontrv_data*);
int        kontr_zap(class iceb_l_kontrv_data*);
int        kontr_pk(class iceb_l_kontrv_data *data);
void       kontr_vvod(GtkWidget *widget,class iceb_l_kontrv_data *data);
gint       kontr_korkod1(class kontr_korkod_data *data);
void  iceb_l_kontrv_v_e_knopka(GtkWidget *widget,class iceb_l_kontrv_data *data);


void iceb_l_kontrv_rk(const char *kontr,GtkWidget *wpredok);


extern SQL_baza bd;

int iceb_l_kontrv(class iceb_u_str *kod_kontr,const char *shetsk,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class iceb_l_kontrv_data data;
char    strsql[1024];
SQL_str row;
iceb_u_str stroka;
iceb_u_str strokatit;
iceb_u_str kikz;
class iceb_u_str kod("");
static int new_kodkontr=1;
data.kontr_i.new_plus(kod_kontr->ravno());    
kod.new_plus(kod_kontr->ravno());
data.shetsk.new_plus(shetsk);

if(data.shetsk.getdlinna() > 1 && data.kontr_i.getdlinna() <= 1)
 if(iceb_menu_vvod1(gettext("Введите код контрагента"),&kod,20,"",wpredok) != 0)
     return(1);

data.kl_shift=0;
data.name_window.plus(__FUNCTION__);

data.rk.clear_zero();
if(kod.getdlinna() > 1)
 {

  if(kod.ravno()[0] == '\0')
   {
    iceb_menu_soob(gettext("Не выбрана запись !"),wpredok);
    return(1);
   }
  SQLCURSOR cur;
  sprintf(strsql,"select * from Kontragent where kodkon='%s'",kod.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.kodkontr.new_plus(row[0]);
  data.rk.naimkon.new_plus(row[1]);
  data.rk.naimban.new_plus(row[2]);
  data.rk.adres.new_plus(row[3]);
  data.rk.adresb.new_plus(row[4]);
  data.rk.kod.new_plus(row[5]);
  data.rk.mfo.new_plus(row[6]);
  data.rk.nomsh.new_plus(row[7]);
  data.rk.innn.new_plus(row[8]);
  data.rk.nspnds.new_plus(row[9]);
  data.rk.telef.new_plus(row[10]);
  data.rk.grup.new_plus(row[11]);
  data.rk.na.new_plus(row[14]);
  data.rk.regnom.new_plus(row[15]);
  data.rk.naimp.new_plus(row[16]);
  data.rk.gk.new_plus(row[17]);
  data.rk.metka_nal=atoi(row[18]);
  kikz.plus(iceb_kikz(row[12],row[13],wpredok));
  
 }
else
 {
//  sprintf(strsql,"%d",iceb_get_new_kod("Kontragent",1,wpredok));
  sprintf(strsql,"%d",iceb_getnkontr(new_kodkontr,wpredok));
  data.rk.kodkontr.new_plus(strsql);
  new_kodkontr=atoi(strsql);
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

if(kod.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
  strokatit.new_plus(strsql);
  stroka.new_plus(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
  strokatit.new_plus(strsql);

  stroka.new_plus(gettext("Корректировка записи"));

  stroka.ps_plus(kikz.ravno());  

 }

gtk_window_set_title(GTK_WINDOW(data.window),strokatit.ravno());

/************
//g_signal_connect(data.window,"delete_event",G_CALLBACK(kontrv_delete_event),data);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_widget_destroyed),&data.window);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(kontrv_key_press),data);
****************/

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(kontrv_key_press),&data);



if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label=gtk_label_new(stroka.ravno());


GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox3),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox4),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox5),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox6),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox7 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox7),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox8 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox8),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox9),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox10 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox10),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox11 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox11),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox_en = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox_en),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_container_add (GTK_CONTAINER (vbox), hbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox5);
gtk_container_add (GTK_CONTAINER (vbox), hbox6);
gtk_container_add (GTK_CONTAINER (vbox), hbox7);
gtk_container_add (GTK_CONTAINER (vbox), hbox8);
gtk_container_add (GTK_CONTAINER (vbox), hbox9);
gtk_container_add (GTK_CONTAINER (vbox), hbox10);
gtk_container_add (GTK_CONTAINER (vbox), hbox11);
gtk_container_add (GTK_CONTAINER (vbox), hbox_en);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Код контрагента"));
gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);

data.entry[E_KONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KONTR]),19);
gtk_box_pack_start (GTK_BOX (hbox), data.entry[E_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk.kodkontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));

label=gtk_label_new(gettext("Короткое наименование"));
data.entry[E_NAIMK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIMK]),255);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_NAIMK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIMK], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMK]),data.rk.naimkon.ravno());
gtk_widget_set_name(data.entry[E_NAIMK],iceb_u_inttochar(E_NAIMK));


label=gtk_label_new(gettext("Адрес"));
data.entry[E_ADRES] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ADRES]),99);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_ADRES], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ADRES], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRES]),data.rk.adres.ravno());
gtk_widget_set_name(data.entry[E_ADRES],iceb_u_inttochar(E_ADRES));

label=gtk_label_new(gettext("Код ЕГРПОУ"));
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),19);
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Счет"));
data.entry[E_NOMSH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMSH]),29);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_NOMSH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMSH], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMSH]),data.rk.nomsh.ravno());
gtk_widget_set_name(data.entry[E_NOMSH],iceb_u_inttochar(E_NOMSH));

label=gtk_label_new(gettext("МФО"));
data.entry[E_MFO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MFO]),19);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_MFO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_MFO], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MFO]),data.rk.mfo.ravno());
gtk_widget_set_name(data.entry[E_MFO],iceb_u_inttochar(E_MFO));

label=gtk_label_new(gettext("Наименование банка"));
data.entry[E_NAIMB] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIMB]),79);
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox5), data.entry[E_NAIMB], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIMB], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMB]),data.rk.naimban.ravno());
gtk_widget_set_name(data.entry[E_NAIMB],iceb_u_inttochar(E_NAIMB));

label=gtk_label_new(gettext("Город банка"));
data.entry[E_ADRESB] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ADRESB]),99);
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), data.entry[E_ADRESB], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ADRESB], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRESB]),data.rk.adresb.ravno());
gtk_widget_set_name(data.entry[E_ADRESB],iceb_u_inttochar(E_ADRESB));

label=gtk_label_new(gettext("Номер св. НДС"));
data.entry[E_NSPNDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NSPNDS]),19);
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), data.entry[E_NSPNDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NSPNDS], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NSPNDS]),data.rk.nspnds.ravno());
gtk_widget_set_name(data.entry[E_NSPNDS],iceb_u_inttochar(E_NSPNDS));


sprintf(strsql,"%s",gettext("Группа"));
data.knopka_enter[E_GRUP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox7), data.knopka_enter[E_GRUP], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_GRUP],"clicked",G_CALLBACK(iceb_l_kontrv_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_GRUP],iceb_u_inttochar(E_GRUP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_GRUP],gettext("Просмотр списка групп контрагентов"));

data.entry[E_GRUP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GRUP]),9);
gtk_box_pack_start (GTK_BOX (hbox7), data.entry[E_GRUP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GRUP], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP]),data.rk.grup.ravno());
gtk_widget_set_name(data.entry[E_GRUP],iceb_u_inttochar(E_GRUP));


label=gtk_label_new(gettext("Телефон"));
data.entry[E_TELEF] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_TELEF]),79);
gtk_box_pack_start (GTK_BOX (hbox7), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox7), data.entry[E_TELEF], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TELEF], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TELEF]),data.rk.telef.ravno());
gtk_widget_set_name(data.entry[E_TELEF],iceb_u_inttochar(E_TELEF));

label=gtk_label_new(gettext("Налоговый адрес"));
data.entry[E_NALADRES] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NALADRES]),255);
gtk_box_pack_start (GTK_BOX (hbox8), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox8), data.entry[E_NALADRES], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NALADRES], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NALADRES]),data.rk.na.ravno());
gtk_widget_set_name(data.entry[E_NALADRES],iceb_u_inttochar(E_NALADRES));

label=gtk_label_new(gettext("Реги.ном.ч/п"));
data.entry[E_REGNOM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_REGNOM]),29);
gtk_box_pack_start (GTK_BOX (hbox9), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox9), data.entry[E_REGNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_REGNOM], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_REGNOM]),data.rk.regnom.ravno());
gtk_widget_set_name(data.entry[E_REGNOM],iceb_u_inttochar(E_REGNOM));

label=gtk_label_new(gettext("Инд. налог.номер"));
data.entry[E_INNN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_INNN]),19);
gtk_box_pack_start (GTK_BOX (hbox9), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox9), data.entry[E_INNN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_INNN], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INNN]),data.rk.innn.ravno());
gtk_widget_set_name(data.entry[E_INNN],iceb_u_inttochar(E_INNN));

label=gtk_label_new(gettext("Полное наименование"));
data.entry[E_NAIMP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIMP]),255);
gtk_box_pack_start (GTK_BOX (hbox10), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox10), data.entry[E_NAIMP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIMP], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMP]),data.rk.naimp.ravno());
gtk_widget_set_name(data.entry[E_NAIMP],iceb_u_inttochar(E_NAIMP));

label=gtk_label_new(gettext("Населённый пунк"));
data.entry[E_GK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GK]),255);
gtk_box_pack_start (GTK_BOX (hbox11), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox11), data.entry[E_GK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GK], "activate",G_CALLBACK(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GK]),data.rk.gk.ravno());
gtk_widget_set_name(data.entry[E_GK],iceb_u_inttochar(E_GK));


label=gtk_label_new(gettext("Система налога"));
gtk_box_pack_start (GTK_BOX (hbox_en), label, FALSE, FALSE, 1);

class iceb_u_spisok bal_st;
bal_st.plus(gettext("Общая система"));
bal_st.plus(gettext("Единый налог"));

iceb_pm_vibor(&bal_st,&data.opt1,data.rk.metka_nal);
gtk_box_pack_start (GTK_BOX (hbox_en), data.opt1, TRUE, TRUE,1);


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(kontrv_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));


sprintf(strsql,"F4 %s",gettext("Код контрагента"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Получить новый код контрагента (В случае если контрагент с таким кодом уже введен.)"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(kontrv_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатать реквизиты контрагента"));
g_signal_connect(data.knopka[FK5],"clicked",G_CALLBACK(kontrv_knopka),&data);
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));

sprintf(strsql,"F6 %s",gettext("Доп.инф."));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Ввод дополнительной информации для контрагента"));
g_signal_connect(data.knopka[FK6],"clicked",G_CALLBACK(kontrv_knopka),&data);
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));

sprintf(strsql,"F7 %s",gettext("Договора"));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Ввод списка договоров"));
g_signal_connect(data.knopka[FK7],"clicked",G_CALLBACK(kontrv_knopka),&data);
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(kontrv_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

for(int ii=0; ii < KOL_FK; ii++)
 gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[ii], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 {
  kod_kontr->new_plus(data.rk.kodkontr.ravno());
 }
 
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  iceb_l_kontrv_v_e_knopka(GtkWidget *widget,class iceb_l_kontrv_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_GRUP:
   
    if(iceb_l_gkontr(1,&kod,&naim,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP]),kod.ravno());
      
    return;  
 }
}


/*****************************/
/*Обработчик сигнала delete_event*/
/****************************/
gboolean kontrv_delete_event(GtkWidget *widget,GdkEvent *event,class kontr_data *data)
{
//printf("vkontr_delete_event\n");
gtk_widget_destroy(widget);
return(FALSE);
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kontrv_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_kontrv_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
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
    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
//    printf("vkontr_key_press F10\n");
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
void  kontrv_knopka(GtkWidget *widget,class iceb_l_kontrv_data *data)
{
char bros[512];
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

data->kl_shift=0;

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    if(kontr_zap(data) == 0)
     {
      gtk_widget_destroy(data->window);
      data->voz=0;
     }
    return;  


  case FK4:
    
    sprintf(bros,"%d",iceb_get_new_kod("Kontragent",1,data->window));
    data->rk.kodkontr.new_plus(bros);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk.kodkontr.ravno());
    return;  

  case FK5:
    iceb_l_kontrv_rk(data->rk.kodkontr.ravno(),data->window);
    return;

  case FK6:
    kontr_vdi(data->rk.kodkontr.ravno(),data->window);
    return;

  case FK7:
    iceb_l_kontdog(0,data->rk.kodkontr.ravno(),data->window);
    return;

  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/************************************/
/*Запись контрагента в таблицу      */
/*************************************/
int kontr_zap(class iceb_l_kontrv_data *data)
{
char strsql[4096];
time_t   vrem;
time(&vrem);
short metkakor=0;

data->read_rek();

if(data->rk.kodkontr.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код контрагента !"),data->window);
  return(1);
 }

if(data->rk.naimkon.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование контрагента !"),data->window);
  
  return(1);
 }

if(data->rk.grup.ravno()[0] != '\0')
 {
  sprintf(strsql,"select kod from Gkont where kod=%s",data->rk.grup.ravno());
  if(sql_readkey(&bd,strsql) < 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найдена группа"),data->rk.grup.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }

 }

if(kontr_pk(data) != 0)
   return(1);

if(data->kontr_i.getdlinna() <= 1)
 {
  sprintf(strsql,"insert into Kontragent \
values ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',\
%d,%ld,'%s','%s','%s','%s',%d)",
   data->rk.kodkontr.ravno_filtr(),
   data->rk.naimkon.ravno_filtr(),
   data->rk.naimban.ravno_filtr(),
   data->rk.adres.ravno_filtr(),
   data->rk.adresb.ravno_filtr(),
   data->rk.kod.ravno_filtr(),
   data->rk.mfo.ravno_filtr(),
   data->rk.nomsh.ravno_filtr(),
   data->rk.innn.ravno_filtr(),
   data->rk.nspnds.ravno_filtr(),
   data->rk.telef.ravno_filtr(),
   data->rk.grup.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.na.ravno_filtr(),
   data->rk.regnom.ravno_filtr(),
   data->rk.naimp.ravno_filtr(),
   data->rk.gk.ravno_filtr(),
   data->rk.metka_nal);
 }
else
 {

  if(iceb_u_SRAV(data->kontr_i.ravno(),data->rk.kodkontr.ravno(),0) != 0)
   {
  
    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Kontragent \
set \
kodkon='%s',\
naikon='%s',\
naiban='%s',\
adres='%s',\
adresb='%s',\
kod='%s',\
mfo='%s',\
nomsh='%s',\
innn='%s',\
nspnds='%s',\
telef='%s',\
grup='%s',\
ktoz=%d,\
vrem=%ld,\
na='%s', \
regnom='%s',\
pnaim='%s',\
gk='%s',\
en=%d \
where kodkon='%s'",
   data->rk.kodkontr.ravno_filtr(),
   data->rk.naimkon.ravno_filtr(),
   data->rk.naimban.ravno_filtr(),
   data->rk.adres.ravno_filtr(),
   data->rk.adresb.ravno_filtr(),
   data->rk.kod.ravno_filtr(),
   data->rk.mfo.ravno_filtr(),
   data->rk.nomsh.ravno_filtr(),
   data->rk.innn.ravno_filtr(),
   data->rk.nspnds.ravno_filtr(),
   data->rk.telef.ravno_filtr(),
   data->rk.grup.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.na.ravno_filtr(),
   data->rk.regnom.ravno_filtr(),
   data->rk.naimp.ravno_filtr(),
   data->rk.gk.ravno_filtr(),
   data->rk.metka_nal,
   data->kontr_i.ravno_filtr());
 }

//printf("%s-strsql=%s\n",__FUNCTION__,strsql);
int voz=0;
if((voz=iceb_sql_zapis(strsql,0,1,data->window)) != 0)
 {
  if(voz !=  ER_DUP_ENTRY) //такая запись уже есть при вводе контрагента в список счёта с развёрнутым сальдо с общем списке контраген уже может быть
    return(1);
 }
if(data->shetsk.getdlinna() > 1)
 {
  if(data->kontr_i.getdlinna() <= 1)
    sprintf(strsql,"insert into Skontr \
values ('%s','%s',%d,%ld)",
    data->shetsk.ravno_filtr(),
    data->rk.kodkontr.ravno_filtr(),
    iceb_getuid(data->window),vrem);
  else
    sprintf(strsql,"update Skontr \
set \
kodkon='%s',\
ktoi=%d,\
vrem=%ld \
where ns='%s' and kodkon='%s'",
    data->rk.kodkontr.ravno_filtr(),iceb_getuid(data->window),vrem,
    data->shetsk.ravno_filtr(),
    data->kontr_i.ravno_filtr());


  iceb_sql_zapis(strsql,0,0,data->window);
 }

gtk_widget_hide(data->window);

if(metkakor == 1 && data->shetsk.ravno()[0] == '\0')
 {
  gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
  printf("Корректировка кода %s %s\n",data->kontr_i.ravno(),data->rk.kodkontr.ravno());
  kontr_korkod(data->kontr_i.ravno(),data->rk.kodkontr.ravno(),data->window);
 }

return(0);
}
/******************************/
/*Пооверка кода контрагента*/
/*****************************/

int kontr_pk(class iceb_l_kontrv_data *data)
{
char strsql[512];

if(iceb_u_SRAV(data->rk.kodkontr.ravno(),data->kontr_i.ravno(),0) == 0)
 return(0);

if(data->shetsk.getdlinna() <= 1)
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->rk.kodkontr.ravno());
else
  sprintf(strsql,"select ns from Skontr where ns='%s' and kodkon='%s'",data->shetsk.ravno(),data->rk.kodkontr.ravno());
//printf("%s-%s\n",__FUNCTION__,strsql); 
if(iceb_sql_readkey(strsql,data->window) >= 1)
 {
  iceb_menu_soob(gettext("Такой код контрагента уже есть !"),NULL);  
  return(1);
 }
return(0);

}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    kontr_vvod(GtkWidget *widget,class iceb_l_kontrv_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("kontr_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KONTR:
    data->rk.kodkontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NAIMK:
    data->rk.naimkon.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_ADRES:
    data->rk.adres.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_ADRESB:
    data->rk.adresb.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOD:
    data->rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMSH:
    data->rk.nomsh.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_MFO:
    data->rk.mfo.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NAIMB:
    data->rk.naimban.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NSPNDS:
    data->rk.nspnds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GRUP:
    data->rk.grup.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_TELEF:
    data->rk.telef.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NALADRES:
    data->rk.na.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_REGNOM:
    data->rk.regnom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_INNN:
    data->rk.innn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NAIMP:
    data->rk.naimp.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GK:
    data->rk.gk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }
enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/**********************************/
/*Ввод дополнительной информации*/
/*********************************/

void  kontr_vdi(const char *kodkon,GtkWidget *wpredok)
{
char strsql[312];

//Установить флаг работы с карточкой
sprintf(strsql,"k%s%s",iceb_get_namebase(),kodkon);
if(sql_flag(&bd,strsql,0,0) != 0)
 {
  iceb_u_str rep;
  rep.plus(gettext("С карточкой уже работает другой оператор !"));
  iceb_menu_soob(&rep,NULL);
  return;
 }


sprintf(strsql,"kontr%d.tmp",getpid());
iceb_dikont('-',strsql,kodkon,"Kontragent1");
iceb_vizred(strsql,wpredok);

iceb_dikont('+',strsql,kodkon,"Kontragent1");
unlink(strsql);

/*Снять флаг работы с карточкой*/
sprintf(strsql,"k%s%s",iceb_get_namebase(),kodkon);
sql_flag(&bd,strsql,0,1);

}
/*************************************/
/*Меню корректировки кода контрагента*/
/*************************************/


void  kontr_korkod(const char *skod,const char *nkod,GtkWidget *wpredok)
{
char strsql[312];

kontr_korkod_data data;
data.skod.plus(skod);
data.nkod.plus(nkod);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

//g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

GtkWidget *label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

label=gtk_label_new(gettext("Выполняется каскадная корректировка в таблицах"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

/**************
data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); //Показывать текст в строке



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);
**************/


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна
gtk_widget_show_all(data.window);

g_idle_add((GSourceFunc)kontr_korkod1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}

/********************************/
/*Корректировка кода контрагента*/
/********************************/
gint  kontr_korkod1(class kontr_korkod_data *data)
{
iceb_kor_kod_kontr(data->skod.ravno(),data->nkod.ravno(),data->window);

return(FALSE);

}
/******************************/
/*распечатка реквизитов контрагента*/
/*********************************/
void iceb_l_kontrv_rk(const char *kontr,GtkWidget *wpredok)
{
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
FILE *ff;
char imaf[64];


sprintf(strsql,"select * from Kontragent where kodkon='%s'",kontr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден контрагент"),kontr);
  
  iceb_menu_soob(strsql,wpredok);
  return;
 }


sprintf(imaf,"kontr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }


iceb_u_zagolov(gettext("Реквизиты контрагента"),0,0,0,0,0,0,iceb_get_pnk("00",0,wpredok),ff);
fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),kontr,row[1]);
fprintf(ff,"---------------------------------------------------------\n");

fprintf(ff,"%s%s\n",gettext("Адрес..............."),row[3]);
fprintf(ff,"%s%s\n",gettext("Код ЕГРПОУ.........."),row[5]);
fprintf(ff,"%s%s\n",gettext("Наименование банка.."),row[2]);
fprintf(ff,"%s%s\n",gettext("МФО................."),row[6]);
fprintf(ff,"%s%s\n",gettext("Номер счета........."),row[7]);
fprintf(ff,"%s%s\n",gettext("Инд. нал. номер....."),row[8]);
fprintf(ff,"%s%s\n",gettext("Город (банка)......."),row[4]);
fprintf(ff,"%s%s\n",gettext("Номер св. пл. НДС..."),row[9]);
fprintf(ff,"%s%s\n",gettext("Номер телефона......"),row[10]);
fprintf(ff,"%s%s\n",gettext("Группа контрагента.."),row[11]);
fprintf(ff,"%s%s\n",gettext("Договор............."),row[14]);
fprintf(ff,"%s%s\n",gettext("Рег. номер Ч.П. ...."),row[15]);
fprintf(ff,"%s%s\n",gettext("Полное наименование."),row[16]);


sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' order by nomz asc",kontr);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
 {
  fprintf(ff,"\n%s\n",gettext("Дополнительная информация"));
  fprintf(ff,"-----------------------------------------------\n");
  while(cur.read_cursor(&row) != 0)
   fprintf(ff,"%s\n",row[0]);
 }
iceb_podpis(ff,wpredok);
fclose(ff);


iceb_u_spisok fil;
iceb_u_spisok naimf;
fil.plus(imaf);
naimf.plus(gettext("Реквизиты контрагента"));
iceb_ustpeh(imaf,3,wpredok);

iceb_rabfil(&fil,&naimf,wpredok);

}
