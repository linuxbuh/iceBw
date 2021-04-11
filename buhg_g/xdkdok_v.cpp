/*$Id: xdkdok_v.c,v 1.23 2013/09/26 09:46:59 sasa Exp $*/
/*28.07.2012	13.04.2006	Белых А.И.	xdkdok_v.c
Корректировка реквизитов платёжного документа
*/
#include "buhg_g.h"
#include "dok4w.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SUMA_PL,
  E_SHET_USL_BANKA,
  E_SUMA_USL_BANKA,
  E_NOMDOK,
  E_DATA_DOK,
  E_KODOP,
  E_NDS,
  E_SHET_DEBET,
  E_MFO_DEBET,
  E_KOL_KOP,
  E_KOD_POL,
  E_MFO_KREDIT,
  E_SHET_KREDIT,
  E_NAIBANK_KREDIT,
  E_KOD_KONTR,
  E_KOD_NE_REZ,
  KOLENTER  
 };

class xdkdok_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_naim_kontr;
  GtkWidget *label_naim_kodop;
  GtkTextBuffer *buffer;
  GtkWidget *ramka;
  short kl_shift;
  int voz;
    
  //Реквизиты меню
  class iceb_u_str suma_pl;
  class iceb_u_str shet_kredit;
  class iceb_u_str shet_usl_banka;
  class iceb_u_str suma_usl_banka;
  class iceb_u_str nomdok;
  class iceb_u_str data_dok;
  class iceb_u_str kodop;
  class iceb_u_str nds;
  class iceb_u_str shet_debet;
  class iceb_u_str mfo_debet;
  class iceb_u_str kol_kop;
  class iceb_u_str kod_pol;
  class iceb_u_str mfo_kredit;
  class iceb_u_str kod_ne_rez;
  class iceb_u_str kod_kontr;
  class iceb_u_str naz_plat;
  class iceb_u_str naibank_kredit;
    
  char tablica[64];  
  

  xdkdok_v_data() //Конструктор
   {
    kl_shift=0;
    voz=0;
   }

  void read_rek()
   {
    suma_pl.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA_PL])));
    shet_kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_KREDIT])));
    shet_usl_banka.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_USL_BANKA])));
    suma_usl_banka.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA_USL_BANKA])));
    nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    data_dok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_DOK])));
    kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP])));
    nds.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NDS])));
    shet_debet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_DEBET])));
    mfo_debet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MFO_DEBET])));
    kol_kop.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_KOP])));
    kod_pol.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_POL])));
    mfo_kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MFO_KREDIT])));
    kod_ne_rez.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_NE_REZ])));
    kod_kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_KONTR])));
    naibank_kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIBANK_KREDIT])));

    xdkvnp_rt(&naz_plat,buffer); /*чтение назначения платежа*/

//    for(int i=0; i < KOLENTER; i++)
//      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };

gboolean   xdkdok_v_key_press(GtkWidget *widget,GdkEventKey *event,class xdkdok_v_data *data);
void    xdkdok_v_vvod(GtkWidget *widget,class xdkdok_v_data *data);
void  xdkdok_v_knopka(GtkWidget *widget,class xdkdok_v_data *data);

void  xdkdoks_v_e_knopka(GtkWidget *widget,class xdkdok_p_data *data);
void  xdkdok_v_e_knopka(GtkWidget *widget,class xdkdok_v_data *data);
int xdkdok_v_zapis(class xdkdok_v_data *data);

extern SQL_baza bd;
extern class REC rec;
extern short kp;

int xdkdok_v(char *tablica,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str naim_kontr("");
class iceb_u_str naim_kodop("");
SQL_str row;
class SQLCURSOR cur;
class xdkdok_v_data data;
strcpy(data.tablica,tablica);

data.suma_pl.plus(rec.sumd);
data.shet_kredit.plus(rec.nsh1.ravno());
data.shet_usl_banka.plus(rec.shbzu.ravno());
data.suma_usl_banka.plus(rec.uslb);
data.nomdok.plus(rec.nomdk.ravno());
sprintf(strsql,"%d.%d.%d",rec.dd,rec.md,rec.gd);
data.data_dok.plus(strsql);
data.kodop.plus(rec.kodop.ravno());
data.nds.plus(rec.nds);
data.shet_debet.plus(rec.nsh.ravno());
data.mfo_debet.plus(rec.mfo.ravno());
data.kol_kop.plus(kp);
data.kod_pol.plus(rec.kod1.ravno());
data.mfo_kredit.plus(rec.mfo1.ravno());
data.kod_ne_rez.plus(rec.kodnr.ravno());
data.naz_plat.plus(rec.naz_plat.ravno());
data.kod_kontr.new_plus(rec.kodor1.ravno());
data.naibank_kredit.new_plus(rec.naiban1.ravno());


sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data.kod_kontr.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_kontr.new_plus(row[0]);

if(data.kodop.getdlinna() > 1) /*может быть пустым при создании типового платёжного поручения*/
 {
  if(iceb_u_SRAV(data.tablica,"Pltp",0) == 0 || iceb_u_SRAV(data.tablica,"Tpltp",0) == 0)
   sprintf(strsql,"select naik from Opltp where kod='%s'",data.kodop.ravno());

  if(iceb_u_SRAV(data.tablica,"Pltt",0) == 0 || iceb_u_SRAV(data.tablica,"Tpltt",0) == 0)
   sprintf(strsql,"select naik from Opltp where kod='%s'",data.kodop.ravno());

  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код операции"),data.kodop.ravno());
    iceb_menu_soob(strsql,wpredok);
   }
  else
   naim_kodop.new_plus(row[0]);
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка реквизитов документа"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(xdkdok_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Корректировка реквизитов документа"));

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
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], FALSE, FALSE, 0);

/*назначение платежа*/
data.ramka=xkdvnp(data.naz_plat.ravno(),&data.buffer);
gtk_box_pack_start (GTK_BOX (vbox), data.ramka, FALSE, FALSE, 5);

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE, 0);


label=gtk_label_new(gettext("Сумма платежа"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_PL]), label, FALSE, FALSE, 0);

data.entry[E_SUMA_PL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA_PL]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_PL]), data.entry[E_SUMA_PL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA_PL], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA_PL]),data.suma_pl.ravno());
gtk_widget_set_name(data.entry[E_SUMA_PL],iceb_u_inttochar(E_SUMA_PL));



label=gtk_label_new(gettext("Счёт оплаты за услуги банка"));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_USL_BANKA]), label, FALSE, FALSE, 0);

data.entry[E_SHET_USL_BANKA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET_USL_BANKA]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_USL_BANKA]), data.entry[E_SHET_USL_BANKA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET_USL_BANKA], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_USL_BANKA]),data.shet_usl_banka.ravno());
gtk_widget_set_name(data.entry[E_SHET_USL_BANKA],iceb_u_inttochar(E_SHET_USL_BANKA));

label=gtk_label_new(gettext("Сумма оплаты за услуги банка"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_USL_BANKA]), label, FALSE, FALSE, 0);

data.entry[E_SUMA_USL_BANKA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA_USL_BANKA]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_USL_BANKA]), data.entry[E_SUMA_USL_BANKA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA_USL_BANKA], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA_USL_BANKA]),data.suma_usl_banka.ravno());
gtk_widget_set_name(data.entry[E_SUMA_USL_BANKA],iceb_u_inttochar(E_SUMA_USL_BANKA));


label=gtk_label_new(gettext("Номер документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE, 0);

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));

//label=gtk_label_new(gettext("Дата документа"));
//gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DOK]), label, FALSE, FALSE, 0);
data.knopka_enter[E_DATA_DOK]=gtk_button_new_with_label(gettext("Дата документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DOK]), data.knopka_enter[E_DATA_DOK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATA_DOK],"clicked",G_CALLBACK(xdkdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_DOK],iceb_u_inttochar(E_DATA_DOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_DOK],gettext("Выбор даты"));

data.entry[E_DATA_DOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_DOK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DOK]), data.entry[E_DATA_DOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATA_DOK], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_DOK]),data.data_dok.ravno());
gtk_widget_set_name(data.entry[E_DATA_DOK],iceb_u_inttochar(E_DATA_DOK));


data.knopka_enter[E_KODOP]=gtk_button_new_with_label(gettext("Код операции"));
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODOP],"clicked",G_CALLBACK(xdkdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODOP],iceb_u_inttochar(E_KODOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODOP],gettext("Выбор операции"));

data.entry[E_KODOP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODOP]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));

data.label_naim_kodop=gtk_label_new(naim_kodop.ravno());
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.label_naim_kodop, TRUE, TRUE, 0);

label=gtk_label_new(gettext("Сумма НДС"));
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), label, FALSE, FALSE, 0);

data.entry[E_NDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), data.entry[E_NDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NDS], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NDS]),data.nds.ravno());
gtk_widget_set_name(data.entry[E_NDS],iceb_u_inttochar(E_NDS));


label=gtk_label_new(gettext("Счёт дебета"));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_DEBET]), label, FALSE, FALSE, 0);

data.entry[E_SHET_DEBET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET_DEBET]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_DEBET]), data.entry[E_SHET_DEBET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET_DEBET], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_DEBET]),data.shet_debet.ravno());
gtk_widget_set_name(data.entry[E_SHET_DEBET],iceb_u_inttochar(E_SHET_DEBET));

label=gtk_label_new(gettext("МФО дебета"));
gtk_box_pack_start (GTK_BOX (hbox[E_MFO_DEBET]), label, FALSE, FALSE, 0);

data.entry[E_MFO_DEBET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MFO_DEBET]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_MFO_DEBET]), data.entry[E_MFO_DEBET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_MFO_DEBET], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MFO_DEBET]),data.mfo_debet.ravno());
gtk_widget_set_name(data.entry[E_MFO_DEBET],iceb_u_inttochar(E_MFO_DEBET));


label=gtk_label_new(gettext("Количество копий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_KOP]), label, FALSE, FALSE, 0);

data.entry[E_KOL_KOP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOL_KOP]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_KOP]), data.entry[E_KOL_KOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOL_KOP], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_KOP]),data.kol_kop.ravno());
gtk_widget_set_name(data.entry[E_KOL_KOP],iceb_u_inttochar(E_KOL_KOP));

label=gtk_label_new(gettext("Код получателя"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POL]), label, FALSE, FALSE, 0);

data.entry[E_KOD_POL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_POL]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POL]), data.entry[E_KOD_POL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_POL], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_POL]),data.kod_pol.ravno());
gtk_widget_set_name(data.entry[E_KOD_POL],iceb_u_inttochar(E_KOD_POL));

label=gtk_label_new(gettext("МФО кредита"));
gtk_box_pack_start (GTK_BOX (hbox[E_MFO_KREDIT]), label, FALSE, FALSE, 0);

data.entry[E_MFO_KREDIT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MFO_KREDIT]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_MFO_KREDIT]), data.entry[E_MFO_KREDIT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_MFO_KREDIT], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MFO_KREDIT]),data.mfo_kredit.ravno());
gtk_widget_set_name(data.entry[E_MFO_KREDIT],iceb_u_inttochar(E_MFO_KREDIT));

label=gtk_label_new(gettext("Счёт кредита"));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_KREDIT]), label, FALSE, FALSE, 0);

data.entry[E_SHET_KREDIT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET_KREDIT]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_KREDIT]), data.entry[E_SHET_KREDIT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET_KREDIT], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_KREDIT]),data.shet_kredit.ravno());
gtk_widget_set_name(data.entry[E_SHET_KREDIT],iceb_u_inttochar(E_SHET_KREDIT));

label=gtk_label_new(gettext("Наименование банка кредита"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIBANK_KREDIT]), label, FALSE, FALSE, 0);

data.entry[E_NAIBANK_KREDIT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NAIBANK_KREDIT]), data.entry[E_NAIBANK_KREDIT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIBANK_KREDIT], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIBANK_KREDIT]),data.naibank_kredit.ravno());
gtk_widget_set_name(data.entry[E_NAIBANK_KREDIT],iceb_u_inttochar(E_NAIBANK_KREDIT));


data.knopka_enter[E_KOD_KONTR]=gtk_button_new_with_label(gettext("Код контрагента"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KONTR]), data.knopka_enter[E_KOD_KONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_KONTR],"clicked",G_CALLBACK(xdkdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_KONTR],iceb_u_inttochar(E_KOD_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_KONTR],gettext("Выбор контрагента"));

data.entry[E_KOD_KONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_KONTR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KONTR]), data.entry[E_KOD_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_KONTR], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_KONTR]),data.kod_kontr.ravno());
gtk_widget_set_name(data.entry[E_KOD_KONTR],iceb_u_inttochar(E_KOD_KONTR));

data.label_naim_kontr=gtk_label_new(naim_kontr.ravno());
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KONTR]), data.label_naim_kontr, TRUE, TRUE, 0);





sprintf(strsql,"%s",gettext("Код не резидента"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NE_REZ]), label, FALSE, FALSE, 0);

data.entry[E_KOD_NE_REZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_NE_REZ]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NE_REZ]), data.entry[E_KOD_NE_REZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_NE_REZ], "activate",G_CALLBACK(xdkdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_NE_REZ]),data.kod_ne_rez.ravno());
gtk_widget_set_name(data.entry[E_KOD_NE_REZ],iceb_u_inttochar(E_KOD_NE_REZ));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации."));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(xdkdok_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации."));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(xdkdok_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне."));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(xdkdok_v_knopka),&data);
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
void  xdkdok_v_e_knopka(GtkWidget *widget,class xdkdok_v_data *data)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("xdkdoks_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATA_DOK:

    if(iceb_calendar(&data->data_dok,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_DOK]),data->data_dok.ravno());
    return;  

  case E_KODOP:
//    printf("%s-data->tablica=%s\n",__FUNCTION__,data->tablica);
    if(l_xdkop(1,&kod,&naim,data->tablica,data->window) == 0)
     {
      data->kodop.new_plus(kod.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naim_kodop),naim.ravno());
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->kodop.ravno());
    break;
    
  case E_KOD_KONTR:
    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
     {
      data->kod_kontr.new_plus(kod.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),naim.ravno());
      sprintf(strsql,"select naikon,kod,mfo,nomsh,naiban from Kontragent where kodkon='%s'",data->kod_kontr.ravno());
      
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),row[0]);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_POL]),row[1]);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_MFO_KREDIT]),row[2]);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_KREDIT]),row[3]);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_NAIBANK_KREDIT]),row[4]);
       }
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_KONTR]),data->kod_kontr.ravno());

    break;
    
    
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   xdkdok_v_key_press(GtkWidget *widget,GdkEventKey *event,class xdkdok_v_data *data)
{

//printf("xdkdok_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
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
void  xdkdok_v_knopka(GtkWidget *widget,class xdkdok_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(xdkdok_v_zapis(data) != 0)
     return;

    gtk_widget_destroy(data->window);
    data->voz=0;
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    xdkdok_v_vvod(GtkWidget *widget,class xdkdok_v_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("xdkdok_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_NOMDOK:
    data->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATA_DOK:
    data->data_dok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODOP:
    data->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    if(data->kodop.getdlinna() > 1)
     {
      if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltp",0) == 0)
       sprintf(strsql,"select naik from Opltp where kod='%s'",data->kodop.ravno());

      if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltt",0) == 0)
       sprintf(strsql,"select naik from Opltp where kod='%s'",data->kodop.ravno());

      if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код операции"),data->kodop.ravno());
        iceb_menu_soob(strsql,data->window);
       }
      else
        gtk_label_set_text(GTK_LABEL(data->label_naim_kodop),row[0]);

     }

    break;

  case E_NDS:
    data->nds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET_DEBET:
    data->shet_debet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_MFO_DEBET:
    data->mfo_debet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOL_KOP:
    data->kol_kop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOD_POL:
    data->kod_pol.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_MFO_KREDIT:
    data->mfo_kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  
  case E_SHET_KREDIT:
    data->shet_kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  
  case E_SHET_USL_BANKA:
    data->shet_usl_banka.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SUMA_USL_BANKA:
    data->suma_usl_banka.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_SUMA_PL:
    data->suma_pl.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOD_NE_REZ:
    data->kod_ne_rez.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOD_KONTR:
    data->kod_kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select naikon,kod,mfo,nomsh,naiban from Kontragent where kodkon='%s'",data->kod_kontr.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),row[0]);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_POL]),row[1]);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_MFO_KREDIT]),row[2]);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_KREDIT]),row[3]);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NAIBANK_KREDIT]),row[4]);
     }
    break;




 }

enter+=1;

if(enter >= KOLENTER)
enter=0;


gtk_widget_grab_focus(data->entry[enter]);
 
}
/***************************************/
/*Запись информации*/
/***************************************/

int xdkdok_v_zapis(class xdkdok_v_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

if((iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Pltt",0) == 0) &&
 data->data_dok.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена дата документа !"),data->window);
  return(1);
 }  


short dd=0,md=0,gd=0;


if(iceb_u_SRAV(data->tablica,"Tpltp",0) != 0 && iceb_u_SRAV(data->tablica,"Tpltt",0) != 0)
 {
  if(data->data_dok.prov_dat() != 0)
   {
    iceb_menu_soob(gettext("Не правильно введена дата документа !"),data->window);
    return(1);
   }  
  else
    iceb_u_rsdat(&dd,&md,&gd,data->data_dok.ravno(),0);
 }
rec.dd=dd;
rec.md=md;
rec.gd=gd;

if(rec.md != rec.mdi || rec.gd != rec.gdi)
if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Pltt",0) == 0 )
 {
  int metka_t=0;
  if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0 )
   metka_t=5;
  if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0 )
   metka_t=4;

  if(iceb_pvkdd(metka_t,rec.ddi,rec.mdi,rec.gdi,rec.md,rec.gd,0,rec.nomdk_i.ravno(),rec.tipz,data->window) != 0)
    return(1);

 }

/***************
if(VV.VVOD_SPISOK_return_data(10)[0] != '\0')  
 {
  if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(10),0) != 0)
   {
    SOOB.VVOD_delete();;
    SOOB.VVOD_SPISOK_add_MD(gettext("Не верно введена дата валютирования !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto obr;
   }  

  rec.dv=d;
  rec.mv=m;
  rec.gv=g;
 }
******************/

if((iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Pltt",0) == 0) &&
 data->kodop.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена операция !"),data->window);
  return(1);
 }  

if(data->kodop.getdlinna() > 1)
 {
  if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltp",0) == 0)
   sprintf(strsql,"select naik from Opltp where kod='%s'",data->kodop.ravno());

  if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltt",0) == 0)
   sprintf(strsql,"select naik from Opltp where kod='%s'",data->kodop.ravno());

  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код операции"),data->kodop.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }

if(data->shet_debet.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён номер дебетового счёта !"),data->window);
  return(1);
 }  

if(iceb_u_SRAV(data->nomdok.ravno(),rec.nomdk.ravno(),0) != 0)
 {
  if(rec.nomdk.getdlinna() > 1)
  if(iceb_u_SRAV(data->tablica,"Tpltt",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltp",0) == 0)
   {
    iceb_menu_soob(gettext("Для типовых документов, номер документа корректировать нельзя !"),data->window);
    return(1);
   }
  
  /*Проверяем может документ с таким номером уже есть*/
  sprintf(strsql,"select nomd from %s where datd >= '%04d-01-01' and \
  datd <= '%04d-12-31' and nomd='%s'",
  data->tablica,
  data->data_dok.ravno_god(),
  data->data_dok.ravno_god(),
  data->nomdok.ravno());
  
  if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
   {
    sprintf(strsql,gettext("Документ с номером %s уже есть !"),data->nomdok.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }          
 }

if(data->kod_kontr.getdlinna() > 1)
 {
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kod_kontr.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не нашли код контрагента"),data->kod_kontr.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }
else
 {
  iceb_menu_soob(gettext("Не введён код контрагента!"),data->window);
  return(1);
 }
rec.kodor1.new_plus(data->kod_kontr.ravno());
rec.naior1.new_plus(row[0]);

rec.nomdk.new_plus(data->nomdok.ravno());

rec.sumd=data->suma_pl.ravno_atof();

rec.nsh1.new_plus(data->shet_kredit.ravno());

rec.shbzu.new_plus(data->shet_usl_banka.ravno());

rec.uslb=data->suma_usl_banka.ravno_atof(); 

rec.kodop.new_plus(data->kodop.ravno());

rec.nds=data->nds.ravno_atof();
rec.nds=iceb_u_okrug(rec.nds,0.01);

rec.nsh.new_plus(data->shet_debet.ravno());

rec.mfo.new_plus(data->mfo_debet.ravno());


rec.dd=dd;
rec.md=md;
rec.gd=gd;

kp=data->kol_kop.ravno_atoi();
rec.kod1.new_plus(data->kod_pol.ravno());
rec.mfo1.new_plus(data->mfo_kredit.ravno());
rec.kodnr.new_plus(data->kod_ne_rez.ravno());
rec.naz_plat.new_plus(data->naz_plat.ravno());
rec.naiban1.new_plus(data->naibank_kredit.ravno());
return(0);

}
