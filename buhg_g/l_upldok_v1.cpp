/*$Id: l_upldok_v1.c,v 1.3 2013/08/25 08:26:42 sasa Exp $*/
/*01.03.2016	12.03.2008	Белых А.И.	l_upldok_v1.c
Ввод и корректировка шапки документа для "Учёта путевых листов"
*/
#include "buhg_g.h"
#include "l_upldok_rekh.h"

enum
 {
  E_DATA_DOK,
  E_KOD_POD,
  E_NOMDOK,
  E_KOD_AVT,
  E_KOD_VOD,
  E_PS_VIEZD,
  E_PS_VOZV,
  E_DATA_VIEZD,
  E_VREM_VIEZD,
  E_DATA_VOZV,
  E_VREM_VOZV,
  E_OSOB_OTM,
  E_NBSO,
  KOLENTER  
 };

enum
 {
  FK2,
  FK10,
  KOL_FK
 };


class l_upldok_v1_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_vod;
  GtkWidget *label_pod;
  GtkWidget *label_avt;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str data_dokz;
  class iceb_u_str kod_podz;
  class iceb_u_str nomdokz;

  class l_upldok_rekh rk;
  
  l_upldok_v1_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    rk.clear();
   }

  void read_rek()
   {
    rk.data_dok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_DOK])));
    rk.kod_pod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_POD])));
    rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    rk.kod_avt.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_AVT])));
    rk.kod_vod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_VOD])));
    rk.ps_viezd.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PS_VIEZD])));
    rk.ps_vozv.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PS_VOZV])));
    rk.data_viezd.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_VIEZD])));

    rk.vrem_viezd.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_VREM_VIEZD])));

    rk.data_vozv.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_VOZV])));
    rk.vrem_vozv.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_VREM_VOZV])));

    rk.osob_otm.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_OSOB_OTM])));
    rk.nbso.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NBSO])));

   }

 };


gboolean   l_upldok_v1_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok_v1_data *data);
void  l_upldok_v1_knopka(GtkWidget *widget,class l_upldok_v1_data *data);
void    l_upldok_v1_vvod(GtkWidget *widget,class l_upldok_v1_data *data);
int l_upldok_zap(class l_upldok_v1_data *data);

void  l_upldok_v1_e_knopka(GtkWidget *widget,class l_upldok_v1_data *data);
void l_upldok_v1_avt(class l_upldok_v1_data *data);
void uplnomdokw(short god,const char *podr,class iceb_u_str *nomdok,GtkWidget *wpredok);
void l_upldok_v1_pod(class l_upldok_v1_data *data);
int l_upldok_v1_poippl(class iceb_u_str *data_dok,const char *kodavt,GtkWidget*);

extern SQL_baza  bd;

int l_upldok_v1(class iceb_u_str *data_dok,class iceb_u_str *kod_pod,class iceb_u_str *nomdok,GtkWidget *wpredok)
{
class iceb_u_str naim_pod("");
class iceb_u_str naim_vod("");
class iceb_u_str naim_avt("");

class l_upldok_v1_data data;
char strsql[512];
class iceb_u_str kikz;
int gor=0,ver=0;
data.data_dokz.new_plus(data_dok->ravno());
data.kod_podz.new_plus(kod_pod->ravno());
data.nomdokz.new_plus(nomdok->ravno());
data.name_window.plus(__FUNCTION__);

if(data.data_dokz.getdlinna() >  1)
 {
  sprintf(strsql,"select ka,kv,psv,psz,denn,denk,vremn,vremk,otmet,nomb,ktoz,vrem from Upldok where god=%d and kp=%d and nomd='%s'",
  data.data_dokz.ravno_god(),data.kod_podz.ravno_atoi(),data.nomdokz.ravno());
  
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.rk.data_dok.new_plus(data_dok->ravno());
  data.rk.kod_pod.new_plus(data.kod_podz.ravno());
  data.rk.nomdok.new_plus(data.nomdokz.ravno());
  data.rk.kod_avt.new_plus(row[0]);
  data.rk.kod_vod.new_plus(row[1]);
  data.rk.ps_viezd.new_plus(row[2]);
  data.rk.ps_vozv.new_plus(row[3]);

  kikz.plus(iceb_kikz(row[10],row[11],wpredok));

  data.rk.data_viezd.new_plus(iceb_u_datzap(row[4]));
  data.rk.vrem_viezd.new_plus(row[6]);
  data.rk.data_vozv.new_plus(iceb_u_datzap(row[5]));
  data.rk.vrem_vozv.new_plus(row[7]);
  data.rk.osob_otm.new_plus(row[8]);
  data.rk.nbso.new_plus(row[9]);
      
  sprintf(strsql,"select naik from Uplpodr where kod=%d",data.rk.kod_pod.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_pod.new_plus(row[0]);

  sprintf(strsql,"select naik from Uplouot where kod=%d",data.rk.kod_vod.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_vod.new_plus(row[0]);

  sprintf(strsql,"select naik from Uplavt where kod=%d",data.rk.kod_avt.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_avt.new_plus(row[0]);
   
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(data.data_dokz.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод нового путевого листа"));
  label=gtk_label_new(gettext("Ввод нового путевого листа"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка шапки путевого листа"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка шапки путевого листа"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_upldok_v1_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL,1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);



sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("д.м.г"));
data.knopka_enter[E_DATA_DOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DOK]), data.knopka_enter[E_DATA_DOK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATA_DOK],"clicked",G_CALLBACK(l_upldok_v1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_DOK],iceb_u_inttochar(E_DATA_DOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_DOK],gettext("Выбор даты"));

data.entry[E_DATA_DOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_DOK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DOK]), data.entry[E_DATA_DOK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATA_DOK], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_DOK]),data.rk.data_dok.ravno());
gtk_widget_set_name(data.entry[E_DATA_DOK],iceb_u_inttochar(E_DATA_DOK));


sprintf(strsql,"%s",gettext("Код подразделения"));
data.knopka_enter[E_KOD_POD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POD]), data.knopka_enter[E_KOD_POD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_POD],"clicked",G_CALLBACK(l_upldok_v1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_POD],iceb_u_inttochar(E_KOD_POD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_POD],gettext("Выбор даты"));

data.entry[E_KOD_POD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_POD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POD]), data.entry[E_KOD_POD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_POD], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_POD]),data.rk.kod_pod.ravno());
gtk_widget_set_name(data.entry[E_KOD_POD],iceb_u_inttochar(E_KOD_POD));

data.label_pod=gtk_label_new(naim_pod.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POD]), data.label_pod, FALSE, FALSE,1);


sprintf(strsql,"%s",gettext("Номер документа"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NOMDOK],"clicked",G_CALLBACK(l_upldok_v1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NOMDOK],gettext("Получение номера документа"));

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk.nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));


sprintf(strsql,"%s",gettext("Код автомобиля"));
data.knopka_enter[E_KOD_AVT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.knopka_enter[E_KOD_AVT], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_AVT],"clicked",G_CALLBACK(l_upldok_v1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_AVT],iceb_u_inttochar(E_KOD_AVT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_AVT],gettext("Выбор автомобиля"));

data.entry[E_KOD_AVT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_AVT]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.entry[E_KOD_AVT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_AVT], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_AVT]),data.rk.kod_avt.ravno());
gtk_widget_set_name(data.entry[E_KOD_AVT],iceb_u_inttochar(E_KOD_AVT));

data.label_avt=gtk_label_new(naim_avt.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.label_avt, FALSE, FALSE,1);



sprintf(strsql,"%s",gettext("Код водителя"));
data.knopka_enter[E_KOD_VOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.knopka_enter[E_KOD_VOD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_VOD],"clicked",G_CALLBACK(l_upldok_v1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_VOD],iceb_u_inttochar(E_KOD_VOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_VOD],gettext("Выбор водителя"));

data.entry[E_KOD_VOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_VOD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.entry[E_KOD_VOD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_VOD], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_VOD]),data.rk.kod_vod.ravno());
gtk_widget_set_name(data.entry[E_KOD_VOD],iceb_u_inttochar(E_KOD_VOD));

data.label_vod=gtk_label_new(naim_vod.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.label_vod, FALSE, FALSE,1);


label=gtk_label_new(gettext("Показания спидометра при выезде"));
gtk_box_pack_start (GTK_BOX (hbox[E_PS_VIEZD]), label, FALSE, FALSE,1);

data.entry[E_PS_VIEZD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PS_VIEZD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_PS_VIEZD]), data.entry[E_PS_VIEZD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_PS_VIEZD], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PS_VIEZD]),data.rk.ps_viezd.ravno());
gtk_widget_set_name(data.entry[E_PS_VIEZD],iceb_u_inttochar(E_PS_VIEZD));


label=gtk_label_new(gettext("Показания спидометра при возвращении"));
gtk_box_pack_start (GTK_BOX (hbox[E_PS_VOZV]), label, FALSE, FALSE,1);

data.entry[E_PS_VOZV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PS_VOZV]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_PS_VOZV]), data.entry[E_PS_VOZV], TRUE, TRUE,1);
g_signal_connect(data.entry[E_PS_VOZV], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PS_VOZV]),data.rk.ps_vozv.ravno());
gtk_widget_set_name(data.entry[E_PS_VOZV],iceb_u_inttochar(E_PS_VOZV));

/*******************************
label=gtk_label_new(gettext("Пробег по городу"));
gtk_box_pack_start (GTK_BOX (hbox[E_PROBEG_PO_GOR]), label, FALSE, FALSE,1);

data.entry[E_PROBEG_PO_GOR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PROBEG_PO_GOR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_PROBEG_PO_GOR]), data.entry[E_PROBEG_PO_GOR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_PROBEG_PO_GOR], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROBEG_PO_GOR]),data.rk.probeg_po_gor.ravno());
gtk_widget_set_name(data.entry[E_PROBEG_PO_GOR],iceb_u_inttochar(E_PROBEG_PO_GOR));


label=gtk_label_new(gettext("Норма списания топлива по городу"));
gtk_box_pack_start (GTK_BOX (hbox[E_NST_PO_GOR]), label, FALSE, FALSE,1);

data.entry[E_NST_PO_GOR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NST_PO_GOR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_NST_PO_GOR]), data.entry[E_NST_PO_GOR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NST_PO_GOR], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NST_PO_GOR]),data.rk.nst_po_gor.ravno());
gtk_widget_set_name(data.entry[E_NST_PO_GOR],iceb_u_inttochar(E_NST_PO_GOR));


label=gtk_label_new(gettext("Затраты топлива по норме по городу"));
gtk_box_pack_start (GTK_BOX (hbox[E_VTPN_PO_GOR]), label, FALSE, FALSE,1);

data.entry[E_VTPN_PO_GOR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VTPN_PO_GOR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_VTPN_PO_GOR]), data.entry[E_VTPN_PO_GOR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_VTPN_PO_GOR], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VTPN_PO_GOR]),data.rk.vtpn_po_gor.ravno());
gtk_widget_set_name(data.entry[E_VTPN_PO_GOR],iceb_u_inttochar(E_VTPN_PO_GOR));


label=gtk_label_new(gettext("Затраты топлива фактические по городу"));
gtk_box_pack_start (GTK_BOX (hbox[E_VTF_PO_GOR]), label, FALSE, FALSE,1);

data.entry[E_VTF_PO_GOR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VTF_PO_GOR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_VTF_PO_GOR]), data.entry[E_VTF_PO_GOR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_VTF_PO_GOR], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VTF_PO_GOR]),data.rk.vtf_po_gor.ravno());
gtk_widget_set_name(data.entry[E_VTF_PO_GOR],iceb_u_inttochar(E_VTF_PO_GOR));


label=gtk_label_new(gettext("Пробег за городом"));
gtk_box_pack_start (GTK_BOX (hbox[E_PROBEG_ZA_GOR]), label, FALSE, FALSE,1);

data.entry[E_PROBEG_ZA_GOR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PROBEG_ZA_GOR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_PROBEG_ZA_GOR]), data.entry[E_PROBEG_ZA_GOR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_PROBEG_ZA_GOR], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROBEG_ZA_GOR]),data.rk.probeg_za_gor.ravno());
gtk_widget_set_name(data.entry[E_PROBEG_ZA_GOR],iceb_u_inttochar(E_PROBEG_ZA_GOR));


label=gtk_label_new(gettext("Норма списания топлива за городом"));
gtk_box_pack_start (GTK_BOX (hbox[E_NST_ZA_GOR]), label, FALSE, FALSE,1);

data.entry[E_NST_ZA_GOR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NST_ZA_GOR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_NST_ZA_GOR]), data.entry[E_NST_ZA_GOR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NST_ZA_GOR], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NST_ZA_GOR]),data.rk.nst_za_gor.ravno());
gtk_widget_set_name(data.entry[E_NST_ZA_GOR],iceb_u_inttochar(E_NST_ZA_GOR));


label=gtk_label_new(gettext("Затраты топлива по норме за городом"));
gtk_box_pack_start (GTK_BOX (hbox[E_VTPN_ZA_GOR]), label, FALSE, FALSE,1);

data.entry[E_VTPN_ZA_GOR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VTPN_ZA_GOR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_VTPN_ZA_GOR]), data.entry[E_VTPN_ZA_GOR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_VTPN_ZA_GOR], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VTPN_ZA_GOR]),data.rk.vtpn_za_gor.ravno());
gtk_widget_set_name(data.entry[E_VTPN_ZA_GOR],iceb_u_inttochar(E_VTPN_ZA_GOR));


label=gtk_label_new(gettext("Затраты топлива фактические за городом"));
gtk_box_pack_start (GTK_BOX (hbox[E_VTF_ZA_GOR]), label, FALSE, FALSE,1);

data.entry[E_VTF_ZA_GOR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VTF_ZA_GOR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_VTF_ZA_GOR]), data.entry[E_VTF_ZA_GOR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_VTF_ZA_GOR], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VTF_ZA_GOR]),data.rk.vtf_za_gor.ravno());
gtk_widget_set_name(data.entry[E_VTF_ZA_GOR],iceb_u_inttochar(E_VTF_ZA_GOR));
******************************/
sprintf(strsql,"%s (%s)",gettext("Дата выезда"),gettext("д.м.г"));
data.knopka_enter[E_DATA_VIEZD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VIEZD]), data.knopka_enter[E_DATA_VIEZD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATA_VIEZD],"clicked",G_CALLBACK(l_upldok_v1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_VIEZD],iceb_u_inttochar(E_DATA_VIEZD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_VIEZD],gettext("Выбор даты"));

data.entry[E_DATA_VIEZD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_VIEZD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VIEZD]), data.entry[E_DATA_VIEZD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATA_VIEZD], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_VIEZD]),data.rk.data_viezd.ravno());
gtk_widget_set_name(data.entry[E_DATA_VIEZD],iceb_u_inttochar(E_DATA_VIEZD));


sprintf(strsql,"%s (%s)",gettext("Время выезда"),gettext("ч.м.с"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VIEZD]), label, FALSE, FALSE,1);

data.entry[E_VREM_VIEZD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VREM_VIEZD]),8);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VIEZD]), data.entry[E_VREM_VIEZD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_VREM_VIEZD], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREM_VIEZD]),data.rk.vrem_viezd.ravno());
gtk_widget_set_name(data.entry[E_VREM_VIEZD],iceb_u_inttochar(E_VREM_VIEZD));


sprintf(strsql,"%s (%s)",gettext("Дата возвращения"),gettext("д.м.г"));
data.knopka_enter[E_DATA_VOZV]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VOZV]), data.knopka_enter[E_DATA_VOZV], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATA_VOZV],"clicked",G_CALLBACK(l_upldok_v1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_VOZV],iceb_u_inttochar(E_DATA_VOZV));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_VOZV],gettext("Выбор даты"));

data.entry[E_DATA_VOZV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_VOZV]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VOZV]), data.entry[E_DATA_VOZV], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATA_VOZV], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_VOZV]),data.rk.data_vozv.ravno());
gtk_widget_set_name(data.entry[E_DATA_VOZV],iceb_u_inttochar(E_DATA_VOZV));


sprintf(strsql,"%s (%s)",gettext("Время возвращения"),gettext("ч.м.с"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VOZV]), label, FALSE, FALSE,1);

data.entry[E_VREM_VOZV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VREM_VOZV]),8);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VOZV]), data.entry[E_VREM_VOZV], TRUE, TRUE,1);
g_signal_connect(data.entry[E_VREM_VOZV], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREM_VOZV]),data.rk.vrem_vozv.ravno());
gtk_widget_set_name(data.entry[E_VREM_VOZV],iceb_u_inttochar(E_VREM_VOZV));


sprintf(strsql,"%s",gettext("Особые отметки"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_OSOB_OTM]), label, FALSE, FALSE,1);

data.entry[E_OSOB_OTM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_OSOB_OTM]),249);
gtk_box_pack_start (GTK_BOX (hbox[E_OSOB_OTM]), data.entry[E_OSOB_OTM], TRUE, TRUE,1);
g_signal_connect(data.entry[E_OSOB_OTM], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OSOB_OTM]),data.rk.osob_otm.ravno());
gtk_widget_set_name(data.entry[E_OSOB_OTM],iceb_u_inttochar(E_OSOB_OTM));


sprintf(strsql,"%s",gettext("Номер бланка строгой отчётности"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NBSO]), label, FALSE, FALSE,1);

data.entry[E_NBSO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NBSO]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_NBSO]), data.entry[E_NBSO], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NBSO], "activate",G_CALLBACK(l_upldok_v1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NBSO]),data.rk.nbso.ravno());
gtk_widget_set_name(data.entry[E_NBSO],iceb_u_inttochar(E_NBSO));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_upldok_v1_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_upldok_v1_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 {
  data_dok->new_plus(data.rk.data_dok.ravno());
  kod_pod->new_plus(data.rk.kod_pod.ravno());
  nomdok->new_plus(data.rk.nomdok.ravno());
 }
 
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_upldok_v1_e_knopka(GtkWidget *widget,class l_upldok_v1_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATA_DOK:

    if(iceb_calendar(&data->rk.data_dok,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_DOK]),data->rk.data_dok.ravno());
      
    return;  

  case E_DATA_VIEZD:

    if(iceb_calendar(&data->rk.data_viezd,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_VIEZD]),data->rk.data_viezd.ravno());
      
    return;  

  case E_DATA_VOZV:

    if(iceb_calendar(&data->rk.data_vozv,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_VOZV]),data->rk.data_vozv.ravno());
      
    return;  

  case E_KOD_POD:
    if(l_uplpod(1,&kod,&naim,data->window) == 0)
     {
      data->rk.kod_pod.new_plus(kod.ravno());
      l_upldok_v1_pod(data);
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_POD]),data->rk.kod_pod.ravno());
      
    return;  

  case E_KOD_AVT:
    if(l_uplavt0(1,&kod,&naim,data->window) == 0)
     {
      data->rk.kod_avt.new_plus(kod.ravno());
      l_upldok_v1_avt(data);     
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_AVT]),data->rk.kod_avt.ravno());
      
    return;  


  case E_KOD_VOD:
    if(l_uplout(1,&kod,&naim,1,data->window) == 0)
     {
      data->rk.kod_vod.new_plus(kod.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_vod),naim.ravno(20));
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VOD]),data->rk.kod_vod.ravno());
      
    return;  

  case E_NOMDOK:
    data->rk.data_dok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATA_DOK])));
    data->rk.kod_pod.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_KOD_POD])));
    data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK])));

    if(data->rk.data_dok.getdlinna() <= 1)
     return;
     
    if(data->rk.kod_pod.getdlinna() <= 1)
     return;

    uplnomdokw(data->rk.data_dok.ravno_god(),data->rk.kod_pod.ravno(),&data->rk.nomdok,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno());
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_upldok_v1_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok_v1_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");

    return(TRUE);

//  case GDK_KEY_F3:
//    g_signal_emit_by_name(data->knopka[FK3],"clicked");

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
void  l_upldok_v1_knopka(GtkWidget *widget,class l_upldok_v1_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    if(l_upldok_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  


  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_upldok_v1_vvod(GtkWidget *widget,class l_upldok_v1_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
char strsql[512];
class iceb_u_str naim("");
SQL_str row;
class SQLCURSOR cur;

switch (enter)
 {
  case E_DATA_DOK:
    data->rk.data_dok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOD_POD:
    data->rk.kod_pod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->rk.kod_pod.getdlinna() > 1)
     {
      sprintf(strsql,"select naik from Uplpodr where kod=%d",data->rk.kod_pod.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       naim.new_plus(row[0]);

      gtk_label_set_text(GTK_LABEL(data->label_pod),naim.ravno(20));
     }
    break;

  case E_KOD_AVT:
    data->rk.kod_avt.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    l_upldok_v1_avt(data);

    break;

  case E_KOD_VOD:
    data->rk.kod_vod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    sprintf(strsql,"select naik from Uplouot where kod=%d",data->rk.kod_vod.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);

    gtk_label_set_text(GTK_LABEL(data->label_vod),naim.ravno(20));

    break;

  case E_PS_VIEZD:
    data->rk.ps_viezd.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PS_VOZV:
    data->rk.ps_vozv.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_upldok_zap(class l_upldok_v1_data *data)
{
char strsql[1024];
short dv=0,mv=0,gv=0;  

if(iceb_u_rsdat(&dv,&mv,&gv,data->rk.data_dok.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели дату документа!"),data->window);
  return(1);
 }

if(l_upldok_pb(data->rk.data_dok.ravno(),data->window) != 0)
 return(1);

if(data->data_dokz.getdlinna() > 1)
 if(l_upldok_pb(data->data_dokz.ravno(),data->window) != 0)
  return(1);

if(data->rk.data_viezd.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели дату выезда!"),data->window);
  return(1);
 }

if(data->rk.data_vozv.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели дату возвращения!"),data->window);
  return(1);
 }

if(data->rk.vrem_vozv.prov_time() != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели время возвращения!"),data->window);
  return(1);
 }

if(data->rk.vrem_viezd.prov_time() != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели время выезда!"),data->window);
  return(1);
 }

if(data->rk.kod_pod.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели код подразделения!"),data->window);
  return(1);
 }
if(data->rk.kod_avt.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели код автомобиля!"),data->window);
  return(1);
 }

sprintf(strsql,"select naik from Uplpodr where kod=%d",data->rk.kod_pod.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код подразделения"),  data->rk.kod_pod.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(iceb_u_SRAV(data->nomdokz.ravno(),data->rk.nomdok.ravno(),0) != 0)
 {
  sprintf(strsql,"select datd from Upldok where god=%d and kp=%d and nomd='%s'",
  gv,data->rk.kod_pod.ravno_atoi(),data->rk.nomdok.ravno());
  if(iceb_sql_readkey(strsql,data->window) > 0)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),
    data->rk.nomdok.ravno());
    iceb_menu_soob(strsql,data->window);

    return(1);
   }
 }

sprintf(strsql,"select naik from Uplavt where kod=%d",data->rk.kod_avt.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код автомобиля"),
  data->rk.kod_avt.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }


sprintf(strsql,"select naik from Uplouot where kod=%d",data->rk.kod_vod.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код водителя"),
  data->rk.kod_vod.ravno());

  iceb_menu_soob(strsql,data->window);
  return(1);
 }


time_t   vrem;
time(&vrem);
SQL_str row;
class SQLCURSOR curr;
if(data->data_dokz.getdlinna() <= 1) //Ввод нового документа
 {
  int nom_zap=0;
  //Определяем номер записи для документа
  sprintf(strsql,"select nz from Upldok where datd='%d-%d-%d' and ka=%d order by nz desc limit 1",
  gv,mv,dv,atoi(data->rk.kod_avt.ravno()));
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) > 0)
   nom_zap=atoi(row[0])+1;
  sprintf(strsql,"insert into Upldok \
values ('%04d-%d-%d',\
%s,\
'%s',\
%d,\
'%s',\
%s,\
%s,\
%d,\
%d,\
'%s',\
'%s',\
'%s',\
'%s',\
0,0,\
'%s',\
%d,\
%ld,\
%d)",
  gv,mv,dv,
  data->rk.kod_pod.ravno(),
  data->rk.nomdok.ravno(),
  gv,
  data->rk.nbso.ravno_filtr(),
  data->rk.kod_avt.ravno(),
  data->rk.kod_vod.ravno(),
  data->rk.ps_viezd.ravno_atoi(),
  data->rk.ps_vozv.ravno_atoi(),
  data->rk.data_viezd.ravno_sqldata(),
  data->rk.data_vozv.ravno_sqldata(),
  data->rk.vrem_viezd.ravno_time(),
  data->rk.vrem_vozv.ravno_time(),
  data->rk.osob_otm.ravno_filtr(),
  iceb_getuid(data->window),vrem,
  nom_zap);

    
 }
else  //Корректировка старого документа 
 {
  //исправляем записи в таблице списания топлива по счетам и объектам
  sprintf(strsql,"update Upldok2a set \
datd='%d-%d-%d',\
kp=%d,\
nomd='%s' \
where datd='%s' and kp=%d and nomd='%s'",
  gv,mv,dv,
  data->rk.kod_pod.ravno_atoi(),
  data->rk.nomdok.ravno(),
  data->data_dokz.ravno_sqldata(),data->kod_podz.ravno_atoi(),data->nomdokz.ravno());

  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   return(1);

  sprintf(strsql,"update Upldok3 set \
datd='%d-%d-%d',\
kp=%d,\
nomd='%s' \
where datd='%s' and kp=%d and nomd='%s'",
  gv,mv,dv,
  data->rk.kod_pod.ravno_atoi(),
  data->rk.nomdok.ravno(),
  data->data_dokz.ravno_sqldata(),data->kod_podz.ravno_atoi(),data->nomdokz.ravno());

  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   return(1);

  //Исправлям записи в документе
  
  sprintf(strsql,"update Upldok1 set \
datd='%d-%d-%d',\
kp=%d,\
nomd='%s',\
ka=%d,\
kv=%d \
where datd='%s' and kp=%d and nomd='%s'",
  gv,mv,dv,
  atoi(data->rk.kod_pod.ravno()),
  data->rk.nomdok.ravno(),
  data->rk.kod_avt.ravno_atoi(),
  data->rk.kod_vod.ravno_atoi(),
  data->data_dokz.ravno_sqldata(),data->kod_podz.ravno_atoi(),data->nomdokz.ravno());

  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   return(1);




  //Исправляем шапку документа

  sprintf(strsql,"update Upldok set \
datd='%d-%d-%d',\
kp=%d,\
nomd='%s',\
god=%d,\
nomb='%s',\
ka=%d,\
kv=%d,\
psv=%d,\
psz=%d,\
denn='%s',\
denk='%s',\
vremn='%s',\
vremk='%s',\
otmet='%s',\
ktoz=%d,\
vrem=%ld \
where god=%d and kp=%d and nomd='%s'",
  gv,mv,dv,
  data->rk.kod_pod.ravno_atoi(),
  data->rk.nomdok.ravno(),
  gv,
  data->rk.nbso.ravno_filtr(),
  data->rk.kod_avt.ravno_atoi(),
  data->rk.kod_vod.ravno_atoi(),
  data->rk.ps_viezd.ravno_atoi(),
  data->rk.ps_vozv.ravno_atoi(),
  data->rk.data_viezd.ravno_sqldata(),
  data->rk.data_vozv.ravno_sqldata(),
  data->rk.vrem_viezd.ravno_time(),
  data->rk.vrem_vozv.ravno_time(),
  data->rk.osob_otm.ravno_filtr(),
  iceb_getuid(data->window),vrem,
  data->data_dokz.ravno_god(),data->kod_podz.ravno_atoi(),data->nomdokz.ravno());
  

  
 }

 
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


gtk_widget_hide(data->window);


return(0);

}
/**********************************/
/*установки после чтения кода автомобиля*/
/****************************************/
void l_upldok_v1_avt(class l_upldok_v1_data *data)
{
char strsql[512];
class iceb_u_str naim("");
SQL_str row;
class SQLCURSOR cur;
int kod_vod=0;

sprintf(strsql,"select * from Uplavt where kod=%d",data->rk.kod_avt.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  naim.new_plus(row[1]);
  naim.plus(" ");
  naim.plus(row[3]);
  
   
  kod_vod=atoi(row[2]);
  

  if(kod_vod != 0)
   {
    data->rk.kod_vod.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_KOD_VOD])));
    if(data->rk.kod_vod.getdlinna() <= 1)
     {     
      data->rk.kod_vod.new_plus(kod_vod);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VOD]),data->rk.kod_vod.ravno());
      class iceb_u_str naim_vod("");   
      sprintf(strsql,"select naik from Uplouot where kod=%d",kod_vod);
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        naim_vod.new_plus(row[0]);
       }
      gtk_label_set_text(GTK_LABEL(data->label_vod),naim_vod.ravno(20));
     }
   }

  int psp=l_upldok_v1_poippl(&data->rk.data_dok,data->rk.kod_avt.ravno(),data->window);
  if(psp != 0)
   {
    data->rk.ps_viezd.new_plus(psp);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PS_VIEZD]),data->rk.ps_viezd.ravno());
   }  
 }

gtk_label_set_text(GTK_LABEL(data->label_avt),naim.ravno(20));

}
/********************************************/
/*установки после чтения кода подразделения*/
/*******************************************/ 
void l_upldok_v1_pod(class l_upldok_v1_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naim("");

sprintf(strsql,"select naik from Uplpodr where kod=%d",data->rk.kod_pod.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim.new_plus(row[0]);

gtk_label_set_text(GTK_LABEL(data->label_pod),naim.ravno(20));

data->rk.data_dok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATA_DOK])));
data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK])));

if(data->rk.nomdok.getdlinna() > 1 )
 return;
if(data->rk.data_dok.getdlinna() <= 1)
 return;
 
if(data->rk.kod_pod.getdlinna() <= 1)
 return;

uplnomdokw(data->rk.data_dok.ravno_god(),data->rk.kod_pod.ravno(),&data->rk.nomdok,data->window);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno());


}
/******************************************************************/
/*Поиск предыдущего путевого листа и возврат показания спидометра*/
/******************************************************************/

int l_upldok_v1_poippl(class iceb_u_str *data_dok,const char *kodavt,GtkWidget *wpredok)
{
int	poksp=0;
char	strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select psz from Upldok where datd <= '%s' and ka=%s and psz != 0 order by datd desc,nz desc limit 1",
data_dok->ravno_sqldata(),kodavt);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
 poksp=atoi(row[0]);

return(poksp);

}
