/*$Id: vdnd2.c,v 1.44 2014/07/31 07:08:27 sasa Exp $*/
/*13.07.2015	19.07.2004	Белых А.И.	vdnd2.c
Ввод и корректировка шапки для расходного документа
Если вернули 0-записали
             1-не записали
*/
#include "buhg_g.h"
#include "zapdokw.h"
enum
{
  FK2,
  FK4,
  FK5,
  FK10,
  KOL_F_KL
};

enum
 {
//Обязательные реквизиты для заполнения
  E_KONTR,
  E_DATAD,
  E_SKLAD,
  E_NOMDOK,
//Необязательные реквизиты для заполнения
  E_DOVER,
  E_DATA_DOVER,
  E_SHEREZ,
  E_NOMDOK_P,
  E_NOMNALNAK,
  E_DATA_VNN,
  E_DATA_OPL,

  E_OSNOV,
  E_USLOV_PROD,
  E_NOMER_MAH,
//***********
  E_FORMA_OPL,
  E_NDS,

  KOLENTER  
 };

class vdnd2_data
 {
  public:
    
  GtkWidget *entry[KOLENTER-2];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_naim_kontr;
  GtkWidget *label_naim_sklad;
  GtkWidget *label_naim_kodop;
  GtkWidget *label_k00;
  GtkWidget *opt_foroplat;
  GtkWidget *opt;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  class zapdokw_data rk;
  
  class iceb_u_str kontr_pred;
   
  class iceb_u_str naim_kontr;
  class iceb_u_str naim_sklad;
  class iceb_u_str naim_kodop;
  class iceb_u_spisok menu_forop;
  int           vib_forop;    
  //Полученные реквизиты для корректировки шапки документа
  short god_nd; /*Год нумерации документов*/    
  vdnd2_data() //Конструктор
   {
    vib_forop=0;
    kl_shift=0;
    voz=1;
    clear();
    god_nd=0;
   }

  void clear()
   {
    kontr_pred.new_plus("");
    naim_sklad.new_plus("");
    naim_kontr.new_plus("");
    naim_kodop.new_plus("");    
   }
  void read_rek()
   {

    rk.kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR])));
    rk.datad.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAD])));
    rk.sklad.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SKLAD])));
    rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    rk.dover.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DOVER])));
    rk.data_dover.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_DOVER])));
    rk.sherez.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHEREZ])));
    rk.nomdok_p.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK_P])));
    rk.nomnalnak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMNALNAK])));
    rk.data_pnn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_VNN])));
    rk.data_opl.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_OPL])));

    rk.osnov.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_OSNOV])));
    rk.uslov_prod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_USLOV_PROD])));
    rk.nomer_mah.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMER_MAH])));
    
    rk.nds=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));
    rk.forma_oplat.new_plus(menu_forop.ravno(gtk_combo_box_get_active (GTK_COMBO_BOX(opt_foroplat))));

   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER-2; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
    clear();
    rk.clear();
    
//    gtk_option_menu_set_history(GTK_OPTION_MENU(opt),0);
//    gtk_option_menu_set_history(GTK_OPTION_MENU(opt_foroplat),0);

    gtk_label_set_text(GTK_LABEL(label_naim_kontr),naim_kontr.ravno());
    gtk_label_set_text(GTK_LABEL(label_naim_sklad),naim_sklad.ravno());
   }
 };

void    vdnd2_v_vvod(GtkWidget *widget,class vdnd2_data *data);
void  vdnd2_v_knopka(GtkWidget *widget,class vdnd2_data *data);
gboolean   vdnd2_v_key_press(GtkWidget *widget,GdkEventKey *event,class vdnd2_data *data);
void  vdnd2_v_e_knopka(GtkWidget *widget,class vdnd2_data *data);

void   vdnd2_get_pm0(GtkWidget *widget,int *data);
int   vdnd2_prov_kontr(class vdnd2_data *data);
int   vdnd2_prov_datad(class vdnd2_data *data);
int   vdnd2_prov_sklad(class vdnd2_data *data,int);
int   vdnd2_prov_nomdok(class vdnd2_data *data,int);
int   vdnd2_zapis(class vdnd2_data *data);
void vdnd2_menu_fo(class vdnd2_data *data);

extern SQL_baza bd;
extern short    mvnp; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
extern short    mdd;  /*0-обычный документ 1-двойной*/

extern class iceb_u_str bnds1; /*0% НДС реализация на територии Украины.*/
extern class iceb_u_str bnds2; /*0% НДС экспорт.*/
extern class iceb_u_str bnds3; /*Освобождение от НДС статья 5.*/
extern class iceb_u_str nds_mp; /*НДС на мед. препараты*/

int  vdnd2(iceb_u_str *datad,
iceb_u_str *skl,
iceb_u_str *nomdok,
const char *kodop, //Код операции для нового документа
int metkapr, //0-работает i_xmu 1-xpro
GtkWidget *wpredok)
{
vdnd2_data data;
static iceb_u_str data_zap;
char strsql[512];
SQL_str row;
SQLCURSOR cur;
iceb_u_str kikz;

data.rk.tipz=2;
data.rk.metkapr=metkapr;
iceb_u_rsdat(&data.rk.dd_kor,&data.rk.md_kor,&data.rk.gd_kor,datad->ravno(),1);
data.rk.nomdok_kor.new_plus(nomdok->ravno());
data.rk.skl_kor=skl->ravno_atoi();
data.rk.kodop.new_plus(kodop);


if(data.rk.dd_kor == 0)
 {
  mdd=0;
  if(data_zap.getdlinna() > 1)
   data.rk.datad.new_plus(data_zap.ravno());
  else
   {
    short d,m,g;
    iceb_u_poltekdat(&d,&m,&g);
    sprintf(strsql,"%d.%d.%d",d,m,g);
    data.rk.datad.new_plus(strsql);
   }
  data.rk.nds=0;
  if(iceb_u_proverka(bnds1.ravno(),data.rk.kodop.ravno(),0,1) == 0 || bnds1.ravno()[0] == '*')
       data.rk.nds=1;
  if(iceb_u_proverka(bnds2.ravno(),data.rk.kodop.ravno(),0,1) == 0 || bnds2.ravno()[0] == '*')
       data.rk.nds=2;
  if(iceb_u_proverka(bnds3.ravno(),data.rk.kodop.ravno(),0,1) == 0 || bnds3.ravno()[0] == '*')
       data.rk.nds=3;
  if(iceb_u_proverka(nds_mp.ravno(),data.rk.kodop.ravno(),0,1) == 0 || nds_mp.ravno()[0] == '*')
       data.rk.nds=3;
 }
else
 {
  data.rk.datad.new_plus(datad->ravno());
  data.rk.nomdok.new_plus(nomdok->ravno());
  data.rk.sklad.new_plus(skl->ravno());

  //Чтение реквизитов для просмотра или корректировки
  sprintf(strsql,"select kontr,nomnn,kodop,ktoi,vrem,nomon,datpnn,k00 from \
Dokummat where datd='%s' and sklad=%s and nomd='%s' and tip=2",
  data.rk.datad.ravno_sqldata(),data.rk.sklad.ravno(),data.rk.nomdok.ravno_filtr());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден документ"));
    repl.plus(" !!!");

    sprintf(strsql,"%s:%s %s:%s %s:%s",
    gettext("Дата"),
    data.rk.datad.ravno(),
    gettext("Номер документа"),
    data.rk.nomdok.ravno(),
    gettext("Склад"),
    data.rk.sklad.ravno());
    
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);  
    
    return(1);
   } 
  data.rk.kontr.new_plus(row[0]);
  data.rk.kontr_kor.new_plus(row[0]);
  
  data.rk.nomnalnak.new_plus(row[1]);
  data.rk.kodop.new_plus(row[2]);
  
  data.rk.nomdok_p.new_plus(row[5]);
  data.rk.nomdok_p_kor.new_plus(row[5]);
  
  data.rk.data_pnn.new_plus_sqldata(row[6]);
  kikz.plus(iceb_kikz(row[3],row[4],wpredok));   
  data.rk.k00.new_plus(row[7]);  
  iceb_u_str kontr;
  
  data.rk.sklad_par=iceb_00_skl(data.rk.kontr.ravno());
  iceb_00_kontr(data.rk.kontr.ravno(),&kontr);
  
  //читаем наименование контрагента
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno_filtr());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    data.naim_kontr.new_plus(row[0]);
   
  //читаем наименование склада
  sprintf(strsql,"select naik from Sklad where kod=%d",data.rk.skl_kor);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    data.naim_sklad.new_plus(row[0]);


  //читаем наименование операции
  sprintf(strsql,"select naik from Rashod where kod='%s'",data.rk.kodop.ravno_filtr());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    data.naim_kodop.new_plus(row[0]);

  //читаем доверенность
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=%d",
  data.rk.datad.ravno_god(),data.rk.sklad.ravno_atoi(),data.rk.nomdok.ravno_filtr(),1);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    iceb_u_polen(row[0],strsql,sizeof(strsql),1,'#');
    data.rk.dover.new_plus(strsql);
    iceb_u_polen(row[0],strsql,sizeof(strsql),2,'#');
    data.rk.data_dover.new_plus(strsql);
   }   
  //читаем через кого
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=%d",
  data.rk.datad.ravno_god(),data.rk.sklad.ravno_atoi(),data.rk.nomdok.ravno_filtr(),2);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   data.rk.sherez.new_plus(row[0]);

  //читаем дату оплаты
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=%d",
  data.rk.datad.ravno_god(),data.rk.sklad.ravno_atoi(),data.rk.nomdok.ravno_filtr(),9);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   data.rk.data_opl.new_plus(row[0]);

  //читаем метку НДС
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=%d",
  data.rk.datad.ravno_god(),data.rk.sklad.ravno_atoi(),data.rk.nomdok.ravno_filtr(),11);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   data.rk.nds=atoi(row[0]);
  
  //читаем дату накладной поставщика
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=%d",
  data.rk.datad.ravno_god(),data.rk.sklad.ravno_atoi(),data.rk.nomdok.ravno_filtr(),14);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   data.rk.data_dok_p.new_plus(row[0]);

  //читаем основание
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=%d",
  data.rk.datad.ravno_god(),data.rk.sklad.ravno_atoi(),data.rk.nomdok.ravno_filtr(),3);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   data.rk.osnov.new_plus(row[0]);
 
  //читаем условие продажи
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=%d",
  data.rk.datad.ravno_god(),data.rk.sklad.ravno_atoi(),data.rk.nomdok.ravno_filtr(),7);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   data.rk.uslov_prod.new_plus(row[0]);

  //читаем форму оплаты
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=%d",
  data.rk.datad.ravno_god(),data.rk.sklad.ravno_atoi(),data.rk.nomdok.ravno_filtr(),8);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   data.rk.forma_oplat.new_plus(row[0]);

  //читаем номер машины
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=%d",
  data.rk.datad.ravno_god(),data.rk.sklad.ravno_atoi(),data.rk.nomdok.ravno_filtr(),10);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   data.rk.nomer_mah.new_plus(row[0]);
 
 }
 
//Читаем код операции
sprintf(strsql,"select naik,vido from Rashod where kod='%s'",kodop);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  data.naim_kodop.new_plus(row[0]);
  mvnp=atoi(row[1]);
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.rk.dd_kor == 0)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод расходного документа"));
else
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка шапки расходного документа"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vdnd2_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
iceb_u_str repl;
if(data.rk.dd_kor == 0)
  repl.new_plus(gettext("Ввод расходного документа"));
else
  repl.new_plus(gettext("Корректировка шапки расходного документа"));

GtkWidget *label=gtk_label_new(repl.ravno());

repl.new_plus(gettext("Операция"));
repl.plus(":");
repl.plus(kodop);
repl.plus(" ");
repl.plus(data.naim_kodop.ravno());
repl.plus(" (");
if(mvnp == 0)
  repl.plus(gettext("Внешняя"));
if(mvnp == 1)
  repl.plus(gettext("Внутренняя"));
if(mvnp == 2)
  repl.plus(gettext("Изменения стоимости"));
repl.plus(")");
repl.plus(" ",data.rk.k00.ravno());
if(kikz.getdlinna() > 1)
 repl.ps_plus(kikz.ravno());
 
data.label_naim_kodop=gtk_label_new(repl.ravno());
data.label_k00=gtk_label_new(data.rk.k00.ravno());

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
GtkWidget *hbox1=gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (hbox1), data.label_naim_kodop);
gtk_container_add (GTK_CONTAINER (hbox1), data.label_k00);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(vdnd2_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR],gettext("Выбор контрагента"));

data.entry[E_KONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KONTR]),20);
data.label_naim_kontr=gtk_label_new(data.naim_kontr.ravno());
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.label_naim_kontr,FALSE,FALSE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk.kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));

sprintf(strsql,"%s",gettext("Дата"));
data.knopka_enter[E_DATAD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.knopka_enter[E_DATAD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAD],"clicked",G_CALLBACK(vdnd2_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAD],iceb_u_inttochar(E_DATAD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAD],gettext("Выбор даты"));

data.entry[E_DATAD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.entry[E_DATAD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAD], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAD]),data.rk.datad.ravno());
gtk_widget_set_name(data.entry[E_DATAD],iceb_u_inttochar(E_DATAD));

sprintf(strsql,"%s",gettext("Склад поставщик"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SKLAD],"clicked",G_CALLBACK(vdnd2_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SKLAD],iceb_u_inttochar(E_SKLAD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SKLAD],gettext("Выбор склада"));

data.entry[E_SKLAD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SKLAD]),10);
data.label_naim_sklad=gtk_label_new(data.naim_sklad.ravno());
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.label_naim_sklad, FALSE, FALSE, 0);
g_signal_connect(data.entry[E_SKLAD], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk.sklad.ravno());
gtk_widget_set_name(data.entry[E_SKLAD],iceb_u_inttochar(E_SKLAD));

sprintf(strsql,"%s",gettext("Номер документа"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NOMDOK],"clicked",G_CALLBACK(vdnd2_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NOMDOK],gettext("Получение номера документа"));

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk.nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));


sprintf(strsql,"%s",gettext("Доверенность"));
label=gtk_label_new(strsql);
data.entry[E_DOVER] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DOVER]),50);
gtk_box_pack_start (GTK_BOX (hbox[E_DOVER]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DOVER]), data.entry[E_DOVER], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DOVER], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOVER]),data.rk.dover.ravno());
gtk_widget_set_name(data.entry[E_DOVER],iceb_u_inttochar(E_DOVER));

sprintf(strsql,"%s",gettext("Дата выдачи доверенности"));
data.knopka_enter[E_DATA_DOVER]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DOVER]), data.knopka_enter[E_DATA_DOVER], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATA_DOVER],"clicked",G_CALLBACK(vdnd2_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_DOVER],iceb_u_inttochar(E_DATA_DOVER));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_DOVER],gettext("Выбор даты"));

data.entry[E_DATA_DOVER] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_DOVER]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DOVER]), data.entry[E_DATA_DOVER], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATA_DOVER], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_DOVER]),data.rk.data_dover.ravno());
gtk_widget_set_name(data.entry[E_DATA_DOVER],iceb_u_inttochar(E_DATA_DOVER));

sprintf(strsql,"%s",gettext("Через кого"));
label=gtk_label_new(strsql);
data.entry[E_SHEREZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHEREZ]),79);
gtk_box_pack_start (GTK_BOX (hbox[E_SHEREZ]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SHEREZ]), data.entry[E_SHEREZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHEREZ], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHEREZ]),data.rk.sherez.ravno());
gtk_widget_set_name(data.entry[E_SHEREZ],iceb_u_inttochar(E_SHEREZ));

sprintf(strsql,"%s",gettext("Номер парного документа"));
label=gtk_label_new(strsql);
data.entry[E_NOMDOK_P] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK_P]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK_P]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK_P]), data.entry[E_NOMDOK_P], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK_P], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK_P]),data.rk.nomdok_p.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK_P],iceb_u_inttochar(E_NOMDOK_P));

sprintf(strsql,"%s",gettext("Номер налоговой накладной"));
data.knopka_enter[E_NOMNALNAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMNALNAK]), data.knopka_enter[E_NOMNALNAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NOMNALNAK],"clicked",G_CALLBACK(vdnd2_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMNALNAK],iceb_u_inttochar(E_NOMNALNAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NOMNALNAK],gettext("Получить номер документа"));


data.entry[E_NOMNALNAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMNALNAK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMNALNAK]), data.entry[E_NOMNALNAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMNALNAK], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMNALNAK]),data.rk.nomnalnak.ravno());
gtk_widget_set_name(data.entry[E_NOMNALNAK],iceb_u_inttochar(E_NOMNALNAK));

sprintf(strsql,"%s",gettext("Дата оплаты"));
data.knopka_enter[E_DATA_OPL]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OPL]), data.knopka_enter[E_DATA_OPL], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATA_OPL],"clicked",G_CALLBACK(vdnd2_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_OPL],iceb_u_inttochar(E_DATA_OPL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_OPL],gettext("Выбор даты"));

data.entry[E_DATA_OPL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_OPL]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OPL]), data.entry[E_DATA_OPL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATA_OPL], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_OPL]),data.rk.data_opl.ravno());
gtk_widget_set_name(data.entry[E_DATA_OPL],iceb_u_inttochar(E_DATA_OPL));


sprintf(strsql,"%s",gettext("Дата выдачи налоговой накладной"));
data.knopka_enter[E_DATA_VNN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VNN]), data.knopka_enter[E_DATA_VNN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATA_VNN],"clicked",G_CALLBACK(vdnd2_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_VNN],iceb_u_inttochar(E_DATA_VNN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_VNN],gettext("Выбор даты"));

data.entry[E_DATA_VNN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_VNN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VNN]), data.entry[E_DATA_VNN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATA_VNN], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_VNN]),data.rk.data_pnn.ravno());
gtk_widget_set_name(data.entry[E_DATA_VNN],iceb_u_inttochar(E_DATA_VNN));


sprintf(strsql,"%s",gettext("Основание"));
label=gtk_label_new(strsql);
data.entry[E_OSNOV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_OSNOV]),79);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), data.entry[E_OSNOV], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_OSNOV], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OSNOV]),data.rk.osnov.ravno());
gtk_widget_set_name(data.entry[E_OSNOV],iceb_u_inttochar(E_OSNOV));


sprintf(strsql,"%s",gettext("Вид договора"));
data.knopka_enter[E_USLOV_PROD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_USLOV_PROD]), data.knopka_enter[E_USLOV_PROD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_USLOV_PROD],"clicked",G_CALLBACK(vdnd2_v_e_knopka),&data); 
gtk_widget_set_name(data.knopka_enter[E_USLOV_PROD],iceb_u_inttochar(E_USLOV_PROD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_USLOV_PROD],gettext("Получить вид договора, дату договора, номер договора"));

data.entry[E_USLOV_PROD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_USLOV_PROD]),79);
gtk_box_pack_start (GTK_BOX (hbox[E_USLOV_PROD]), data.entry[E_USLOV_PROD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_USLOV_PROD], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_USLOV_PROD]),data.rk.uslov_prod.ravno());
gtk_widget_set_name(data.entry[E_USLOV_PROD],iceb_u_inttochar(E_USLOV_PROD));

sprintf(strsql,"%s",gettext("Номер машини"));
label=gtk_label_new(strsql);
data.entry[E_NOMER_MAH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMER_MAH]),79);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_MAH]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_MAH]), data.entry[E_NOMER_MAH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMER_MAH], "activate",G_CALLBACK(vdnd2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_MAH]),data.rk.nomer_mah.ravno());
gtk_widget_set_name(data.entry[E_NOMER_MAH],iceb_u_inttochar(E_NOMER_MAH));


label=gtk_label_new(gettext("Форма оплаты"));
gtk_box_pack_start (GTK_BOX (hbox[E_FORMA_OPL]), label, FALSE, FALSE, 0);

vdnd2_menu_fo(&data);
gtk_box_pack_start (GTK_BOX (hbox[E_FORMA_OPL]), data.opt_foroplat, FALSE, FALSE, 0);



label=gtk_label_new(gettext("НДС"));
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), label, FALSE, FALSE, 0);
//создание пункта меню
iceb_pm_nds(data.rk.datad.ravno(),&data.opt,data.rk.nds);

gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), data.opt, FALSE, FALSE, 0);


sprintf(strsql,"F2 %s",gettext("Записать"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Записать введенную в меню информацию"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vdnd2_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(vdnd2_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F5 %s",gettext("конт.00"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Ввести код контрагента реквизиты которого будут использоваться в распечатках"));
g_signal_connect(data.knopka[FK5],"clicked",G_CALLBACK(vdnd2_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK5], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vdnd2_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
if(data.voz == 0)
 {
  datad->new_plus(data.rk.datad.ravno());
  skl->new_plus(data.rk.sklad.ravno());
  nomdok->new_plus(data.rk.nomdok.ravno());
 }
return(data.voz);





}


/***********************************************/
/*обработка даты*/
/***********************************************/
void vdnd2_od(class vdnd2_data *data)
{
    if(vdnd2_prov_datad(data) != 0)
      return;
    data->rk.sklad.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_SKLAD])));
    data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK])));

    if(data->rk.sklad.getdlinna() > 1)
     if( data->rk.nomdok.getdlinna() <= 1  ||  data->god_nd != data->rk.datad.ravno_god()) /*Если меняется год то меняется и порядок нумерации документов*/
      {
       data->god_nd=data->rk.datad.ravno_god();
       iceb_nomnak(data->rk.datad.ravno_god(),data->rk.sklad.ravno(),&data->rk.nomdok,1,0,0,data->window);
       gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno());

      }


    if(mdd == 1 && data->rk.sklad_par != 0 && data->rk.dd_kor == 0 )
     {
      char sklad[64];
      sprintf(sklad,"%d",data->rk.sklad_par);

      iceb_nomnak(data->rk.datad.ravno_god(),sklad,&data->rk.nomdok_p,1,0,0,data->window);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK_P]),data->rk.nomdok_p.ravno());
     }

}



/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vdnd2_v_e_knopka(GtkWidget *widget,class vdnd2_data *data)
{
short d,m,g;
int nom_nal_nak=0;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATAD:

    if(iceb_calendar(&data->rk.datad,data->window) == 0)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->rk.datad.ravno());
      g_signal_emit_by_name(data->entry[E_DATAD],"activate"); /*чтобы произошёл ввод и были отработы все проверки*/
     }      
    return;  

  case E_NOMDOK:
    data->rk.datad.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAD])));
    data->rk.sklad.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_SKLAD])));
    data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK])));
    if(data->rk.nomdok.getdlinna() > 1)
     {
      iceb_menu_soob(gettext("Номер документа уже введён!"),data->window);
      return;
     }
    if(data->rk.datad.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата документа!"),data->window);
      return;
     }
    if(data->rk.sklad.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введён код склада!"),data->window);
      return;
     }
    if(iceb_u_rsdat(&d,&m,&g,data->rk.datad.ravno(),1) != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата документа!"),data->window);
      return;
     }

    iceb_nomnak(g,data->rk.sklad.ravno(),&data->rk.nomdok,2,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno());
    if(data->rk.sklad_par > 0)
     {
      char bros[32];
      sprintf(bros,"%d",data->rk.sklad_par);
      iceb_nomnak(g,bros,&data->rk.nomdok_p,1,0,0,data->window);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK_P]),data->rk.nomdok_p.ravno());
     }
      
    return;  

  case E_NOMNALNAK:
    data->rk.datad.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAD])));
    data->rk.nomnalnak.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMNALNAK])));
    if(data->rk.nomnalnak.getdlinna() > 1)
     {
      iceb_menu_soob(gettext("Номер документа уже введён!"),data->window);
      return;
     }
    if(data->rk.datad.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата документа!"),data->window);
      return;
     }
    if(iceb_u_rsdat(&d,&m,&g,data->rk.datad.ravno(),1) != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата документа!"),data->window);
      return;
     }
    
    data->god_nd=data->rk.datad.ravno_god();
    nom_nal_nak=iceb_nomnalnak(m,g,data->window);  
    data->rk.nomnalnak.new_plus(nom_nal_nak);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMNALNAK]),data->rk.nomnalnak.ravno());
    
    return;  
  case E_DATA_DOVER:

    if(iceb_calendar(&data->rk.data_dover,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_DOVER]),data->rk.data_dover.ravno());
      
    return;  


  case E_DATA_OPL:

    if(iceb_calendar(&data->rk.data_opl,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_OPL]),data->rk.data_opl.ravno());
      
    return;  

  case E_DATA_VNN:

    if(iceb_calendar(&data->rk.data_pnn,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_VNN]),data->rk.data_pnn.ravno());
      
    return;  

  case E_USLOV_PROD:
    data->rk.kontr.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_KONTR])));
    data->rk.uslov_prod.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_USLOV_PROD])));
//    if(data->rk.uslov_prod.getdlinna() > 1)
//     return;
    if(data->rk.kontr.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не ввели код контрагента!"),data->window);
      return;
     }
    
    
    if(iceb_getuslp_m(data->rk.kontr.ravno(),&data->rk.uslov_prod,data->window) == 0)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_USLOV_PROD]),data->rk.uslov_prod.ravno());
     }      
    return;  

   
  case E_KONTR:
    if(iceb_poi_kontr(&data->rk.kontr,&data->naim_kontr,data->window) == 0)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk.kontr.ravno());
      g_signal_emit_by_name(data->entry[0],"activate");
     }
    return;  

  case E_SKLAD:

    iceb_vibrek(1,"Sklad",&data->rk.sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk.sklad.ravno());
    g_signal_emit_by_name(data->entry[E_SKLAD],"activate"); /*чтобы произошёл ввод и были отработы все проверки*/

    return;  

 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vdnd2_v_key_press(GtkWidget *widget,GdkEventKey *event,class vdnd2_data *data)
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
void  vdnd2_v_knopka(GtkWidget *widget,class vdnd2_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(vdnd2_zapis(data) != 0)
     return;
    
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  


  case FK4:
    data->clear_rek();
    return;

  case FK5:
    iceb_vk00(&data->rk.k00,data->window);
    gtk_label_set_text(GTK_LABEL(data->label_k00),data->rk.k00.ravno());
    return;

    
  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vdnd2_v_vvod(GtkWidget *widget,class vdnd2_data *data)
{
iceb_u_str repl;

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KONTR:
    data->kontr_pred.new_plus(data->rk.kontr.ravno());
    data->rk.kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    if(vdnd2_prov_kontr(data) != 0)
      return;

    break;

  case E_DATAD:
    data->rk.datad.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    vdnd2_od(data);

    break;

  case E_SKLAD:
    data->rk.sklad.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(vdnd2_prov_sklad(data,0) != 0)
      return;

    break;

  case E_NOMDOK:
    data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(vdnd2_prov_nomdok(data,0) != 0)
      return;
    data->rk.datad.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAD])));
    data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK])));

    if(data->rk.sklad.getdlinna() > 1)
     if( data->rk.nomdok.getdlinna() <= 1  ||  data->god_nd != data->rk.datad.ravno_god()) /*Если меняется год то меняется и порядок нумерации документов*/
      {
       data->god_nd=data->rk.datad.ravno_god();
       iceb_nomnak(data->rk.datad.ravno_god(),data->rk.sklad.ravno(),&data->rk.nomdok,1,0,0,data->window);
       gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno());

      }
    break;

  case E_DOVER:
    data->rk.dover.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATA_DOVER:
    data->rk.data_dover.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHEREZ:
    data->rk.sherez.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMDOK_P:
    data->rk.nomdok_p.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMNALNAK:
    data->rk.nomnalnak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATA_OPL:
    data->rk.data_opl.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATA_VNN:
    data->rk.data_pnn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_OSNOV:
    data->rk.osnov.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_USLOV_PROD:
    data->rk.uslov_prod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMER_MAH:
    data->rk.nomer_mah.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;



 }

enter+=1;
  
if(enter >= KOLENTER-2)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**********************/
/*Читаем              */
/**********************/

void   vdnd2_get_pm0(GtkWidget *widget,int *data)
{
int enter=atoi(gtk_widget_get_name(widget));
*data=enter;
/*printf("vdnd2_get_pm0-%d\n",*data);*/
}
/***************************/
/*Проверка кода контрагента*/
/****************************/
//Если вернули 0-все впорядке 1-нет

int   vdnd2_prov_kontr(class vdnd2_data *data)
{

   
iceb_u_str repl;

if(data->rk.dd_kor != 0 && iceb_00_skl(data->rk.kontr_kor.ravno()) > 0 && 
iceb_u_SRAV(data->rk.kontr.ravno(),data->rk.kontr_kor.ravno(),0) != 0)
 {
  repl.new_plus(gettext("Для этого документа код контрагента изменить нельзя !"));
  iceb_menu_soob(&repl,data->window);
  data->rk.kontr.new_plus(data->rk.kontr_kor.ravno());
  gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk.kontr.ravno());
  return(1);
 }
char strsql[512];
SQL_str row;
SQLCURSOR cur;


if(data->rk.dd_kor == 0 && (data->rk.sklad_par=iceb_00_skl(data->rk.kontr.ravno())) > 0)
 {
  if(mvnp != 0)
    mdd=1;

  if(mdd == 0)
   {
    repl.new_plus(gettext("Двойной документ можно сделать только для внутренней операции !"));
    iceb_menu_soob(&repl,data->window);

    data->rk.kontr.new_plus("");
    data->naim_kontr.new_plus("");
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk.kontr.ravno());
    gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),data->naim_kontr.ravno());
    return(1);
   }
  
  sprintf(strsql,"select * from Sklad where kod=%d",
  data->rk.sklad_par);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    repl.new_plus (gettext("Не найден код склада"));
    repl.plus(" ");
    repl.plus(data->rk.sklad_par);
    repl.plus(" !");
    
    iceb_menu_soob(&repl,data->window);
    return(1);
   }

  if(mdd == 1 && data->rk.sklad_par != 0 && data->rk.dd_kor == 0 && data->rk.datad.getdlinna() > 1)
   {
    if(iceb_u_SRAV(data->kontr_pred.ravno(),data->rk.kontr.ravno(),0) != 0)
      data->rk.nomdok_p.new_plus("");
    sprintf(strsql,"%d",data->rk.sklad_par);
    iceb_nomnak(data->rk.datad.ravno(),strsql,&data->rk.nomdok_p,1,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK_P]),data->rk.nomdok_p.ravno());
   }

 }

data->naim_kontr.new_plus("");

iceb_u_str kontr;
iceb_00_kontr(data->rk.kontr.ravno(),&kontr);
int voz=0;
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno_filtr());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  data->naim_kontr.new_plus(row[0]);
//  if(data->rk.osnov.getdlinna() <= 1)
//   data->rk.osnov.new_plus(row[1]);
 }
else
 { 
  voz=1;
  data->naim_kontr.new_plus("");
  repl.new_plus(gettext("Не нашли код контрагента"));
  repl.plus(" ");
  repl.plus(kontr.ravno());
  repl.plus(" !");
  iceb_menu_soob(&repl,data->window);  
  
 }
gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMER_MAH]),data->rk.nomer_mah.ravno());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_OSNOV]),data->rk.osnov.ravno());
gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),data->naim_kontr.ravno(20));

data->rk.sklad_par=iceb_00_skl(data->rk.kontr.ravno());

return(voz);
}
/******************************/
/*Проверка даты документа*/
/**************************/

int   vdnd2_prov_datad(class vdnd2_data *data)
{

if(data->rk.datad.getdlinna() <= 1)
 return(0);
 
iceb_u_str repl;
short d=0,m=0,g=0;

if(iceb_u_rsdat(&d,&m,&g,data->rk.datad.ravno(),0) != 0)
 {
  repl.new_plus(gettext("Не верно введена дата !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

if(data->rk.loginrash != 0)
if(data->rk.dd_kor != d || data->rk.md_kor != m || data->rk.gd_kor != g) 
 {
  repl.new_plus(gettext("К документа распечатан чек ! Дату изменить нельзя !"));
  iceb_menu_soob(&repl,data->window);
  char bros[64];
  sprintf(bros,"%d.%d.%d",data->rk.dd_kor,data->rk.md_kor,data->rk.gd_kor);
  data->rk.datad.new_plus(bros);
  gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->rk.datad.ravno());
  return(1);
 }


//iceb_u_str imaf;
//iceb_imafn("blokmak.alx",&imaf);

if(data->rk.dd_kor != 0 && iceb_u_sravmydat(d,m,g,data->rk.dd_kor,data->rk.md_kor,data->rk.gd_kor) != 0)
if(iceb_pbpds(data->rk.dd_kor,data->rk.gd_kor,data->window) != 0)
 {
  char bros[64];

  sprintf(bros,"%d.%d.%d",data->rk.dd_kor,data->rk.md_kor,data->rk.gd_kor);
  data->rk.datad.new_plus(bros);
  gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->rk.datad.ravno());
  
  return(1);
 }

if(iceb_pbpds(m,g,data->window) != 0)
  return(1);


/*Автоматическая установка номера накладной*/
/********************************************
if(metka == 0)
 {
  data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK])));
  data->rk.sklad.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_SKLAD])));

  if(data->rk.nomdok.getdlinna() <= 1 && data->rk.sklad.getdlinna() > 1 )
   {
    iceb_nomnak(g,data->rk.sklad.ravno(),&data->rk.nomdok,2,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno());
   }

  char strsql[1024];

  if(mdd == 1 && data->rk.sklad_par != 0 && data->rk.dd_kor == 0 )
   {
    sprintf(strsql,"%d",data->rk.sklad_par);

    iceb_nomnak(g,strsql,&data->rk.nomdok_p,1,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK_P]),data->rk.nomdok_p.ravno());
   }


  data->rk.uslov_prod.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_USLOV_PROD])));
  data->rk.datad.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAD])));
  if(data->rk.uslov_prod.getdlinna() <= 1 && mvnp == 0 &&  data->rk.nomdok.getdlinna() > 1)
   {
    sprintf(strsql,"%s N%s %s %s%s",
    gettext("Накладная"),
    data->rk.nomdok.ravno(),
    gettext("от"),
    data->rk.datad.ravno(),
    gettext("г."));
    data->rk.uslov_prod.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_USLOV_PROD]),data->rk.uslov_prod.ravno());
   }  
 }
************************************/
return(0);

}

/********************************/
/*Проверка склада*/
/********************************/

int   vdnd2_prov_sklad(class vdnd2_data *data,int metka)
{
if(data->rk.sklad.getdlinna() <= 1)
  return(0);

iceb_u_str repl;
  
if(data->rk.skl_kor != 0 && data->rk.skl_kor != data->rk.sklad.ravno_atoi())
 {
  repl.new_plus(gettext("Склад изменить нельзя !"));
  repl.ps_plus(gettext("Нужно удалить документ и ввести с нужным складом !"));
  iceb_menu_soob(&repl,data->window);
  data->rk.sklad.new_plus(data->rk.skl_kor);
  gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk.sklad.ravno());
  return(1);
 }

SQL_str row;
SQLCURSOR cur;
char strsql[512];

sprintf(strsql,"select naik from Sklad where kod=%s",data->rk.sklad.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  repl.new_plus(gettext("Не найден код склада"));
  repl.plus(" ");
  repl.plus(data->rk.sklad.ravno());
  repl.plus(" !");
  iceb_menu_soob(&repl,data->window);
  return(1);
 }
data->naim_sklad.new_plus(row[0]);
gtk_label_set_text(GTK_LABEL(data->label_naim_sklad),data->naim_sklad.ravno());


data->naim_sklad.new_plus(row[0]);
gtk_label_set_text(GTK_LABEL(data->label_naim_sklad),data->naim_sklad.ravno());

//if(atoi(VV->VVOD_SPISOK_return_data(2)) == skl1)
if(data->rk.sklad.ravno_atoi() == data->rk.sklad_par)
 {
  if(mvnp == 1)
   {

    repl.new_plus(gettext("Код входного и выходного склада равны !"));
    repl.ps_plus(gettext("Это правильно ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
     {
      data->rk.sklad.new_plus("");
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk.sklad.ravno());

      data->naim_sklad.new_plus("");
      gtk_label_set_text(GTK_LABEL(data->label_naim_sklad),data->naim_sklad.ravno());
      
      return(1);
     }
   }
 }


if(mvnp == 2 && data->rk.sklad_par != 0)
if(data->rk.sklad_par != data->rk.sklad.ravno_atoi())
 {
  repl.new_plus(gettext("Для операции изменения стоимости код входного и выходного склада\nдолжны быть одинаковы !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }
data->naim_sklad.new_plus(row[0]);
gtk_label_set_text(GTK_LABEL(data->label_naim_sklad),data->naim_sklad.ravno());

/*Автоматическая установка номера накладной*/
if(metka == 0)
 {
  data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK])));
  if(data->rk.nomdok.getdlinna() <= 1)
   {
    iceb_nomnak(data->rk.datad.ravno(),data->rk.sklad.ravno(),&data->rk.nomdok,2,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno());
   }

  data->rk.uslov_prod.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_USLOV_PROD])));
  data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK])));
  data->rk.datad.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAD])));
  if(data->rk.uslov_prod.getdlinna() <= 1 && mvnp == 0 && data->rk.nomdok.getdlinna() > 1)
   {
    sprintf(strsql,"%s N%s %s %s%s",
    gettext("Накладная"),
    data->rk.nomdok.ravno(),
    gettext("от"),
    data->rk.datad.ravno(),
    gettext("г."));
    data->rk.uslov_prod.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_USLOV_PROD]),data->rk.uslov_prod.ravno());
   }  
 }
return(0);

}
/********************************/
/*Проверка номера документа*/
/********************************/

int   vdnd2_prov_nomdok(class vdnd2_data *data,int metka)
{
if(data->rk.nomdok.getdlinna() <= 1)
  return(0);


/*Проверяем есть ли такой номер документа*/
if(iceb_u_SRAV(data->rk.nomdok_kor.ravno(),data->rk.nomdok.ravno(),0) == 0)
 return(0);

short d,m,g;

iceb_u_poltekdat(&d,&m,&g);

if(data->rk.datad.getdlinna() > 1)
 iceb_u_rsdat(&d,&m,&g,data->rk.datad.ravno(),1);

char strsql[512];
//Уникальный номер документа для прихода и расхода
sprintf(strsql,"select nomd from Dokummat where \
datd >= '%d-01-01' and datd <= '%d-12-31' and sklad=%s and nomd='%s'",
g,g,data->rk.sklad.ravno(),data->rk.nomdok.ravno());

  
if(sql_readkey(&bd,strsql) >= 1)
 {
  sprintf(strsql,gettext("С номером %s документ уже есть !"),data->rk.nomdok.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
  
 }

if(metka == 0 && mvnp == 0)
 {
  data->rk.uslov_prod.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_USLOV_PROD])));
  if(data->rk.uslov_prod.getdlinna() <= 1)
   {
    data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK])));
    data->rk.datad.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAD])));
 
    sprintf(strsql,"%s N%s %s %s%s",
    gettext("Накладная"),
    data->rk.nomdok.ravno(),
    gettext("от"),
    data->rk.datad.ravno(),
    gettext("г."));
    data->rk.uslov_prod.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_USLOV_PROD]),data->rk.uslov_prod.ravno());
   }  
 }

return(0);
}
/***************************/
/*Запись информации*/
/**************************/
int   vdnd2_zapis(class vdnd2_data *data)
{


if(vdnd2_prov_kontr(data) != 0)
 return(1);

if(vdnd2_prov_datad(data) != 0)
 return(1);
if(vdnd2_prov_sklad(data,1) != 0)
 return(1); 
if(vdnd2_prov_nomdok(data,1) != 0)
 return(1);



iceb_u_str repl;

if(data->rk.kontr.getdlinna() <= 1 || data->rk.sklad.getdlinna() <= 1 || data->rk.nomdok.getdlinna() <= 1 || \
data->rk.datad.getdlinna() <= 1) 
 {
  repl.new_plus(gettext("Не введены все обязательные реквизиты !"));
  iceb_menu_soob(&repl,data->window);
  return(1);  
 }


if(mdd == 1 && data->rk.nomdok_p.getdlinna() <= 1)
 {
  repl.new_plus(gettext("Не введён номер парного документа !"));
  iceb_menu_soob(&repl,data->window);
  return(1);  
 }


if(data->rk.nomnalnak.getdlinna() > 1 && data->rk.data_pnn.getdlinna() <= 1)
 {
  repl.new_plus(gettext("Не введена дата выдачи налоговой накладной !"));
  iceb_menu_soob(&repl,data->window);
  return(1);  

 }
short d,m,g;
if(data->rk.data_pnn.getdlinna() > 1)
if(iceb_u_rsdat(&d,&m,&g,data->rk.data_pnn.ravno(),0) != 0)
 {
  repl.new_plus(gettext("Не верно введена дата выдачи налоговой накладной !"));
  iceb_menu_soob(&repl,data->window);
  return(1);  
 }


if(mvnp == 2 && data->rk.sklad_par != 0)
 if(data->rk.sklad_par != data->rk.sklad.ravno_atoi())
  {
   repl.new_plus(gettext("Для операции изменения стоимости код входного и выходного склада\nдолжны быть одинаковы !"));
   iceb_menu_soob(&repl,data->window);
   return(1);  
  }


if(iceb_pbpds(data->rk.datad.ravno(),data->window) != 0)
  return(1);  

//Проверяем дату выдачи доверенности
if(data->rk.data_dover.prov_dat() != 0)
 {
  repl.new_plus(gettext("Не правильно введена дата выдачи доверенности !"));
  iceb_menu_soob(&repl,data->window);
  return(1);  
 }
//Проверяем дату получения налоговой накладной
if(data->rk.data_pnn.prov_dat() != 0)
 {
  repl.new_plus(gettext("Не правильно введена дата выдачи налоговой накладной !"));
  iceb_menu_soob(&repl,data->window);
  return(1);  
 }
//Проверяем дату оплаты
if(data->rk.data_opl.prov_dat() != 0)
 {
  repl.new_plus(gettext("Не правильно введена дата оплаты !"));
  iceb_menu_soob(&repl,data->window);
  return(1);  
 }

char strsql[512];
SQL_str row;
SQLCURSOR cur;
if(data->rk.dd_kor != 0) //Корректировка, проверяем блокировку документа
 {
  if(iceb_pbpds(data->rk.md_kor,data->rk.gd_kor,data->window) != 0)
    return(1);  

  short dd=0,md=0,gd=0;
  iceb_u_rsdat(&dd,&md,&gd,data->rk.datad.ravno(),1);  

  if(md != data->rk.md_kor || gd != data->rk.gd_kor)
   if(iceb_pvkdd(1,data->rk.dd_kor,data->rk.md_kor,data->rk.gd_kor,md,gd,data->rk.skl_kor,data->rk.nomdok_kor.ravno(),data->rk.tipz,data->window) != 0)
    return(1);

  sprintf(strsql,"select sodz from Dokummat2 where god=%d and \
sklad=%d and nomd='%s' and nomerz=12",data->rk.gd_kor,data->rk.skl_kor,data->rk.nomdok_kor.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) > 0)
   {
    repl.new_plus(gettext("Документ заблокирован"));
    repl.ps_plus(iceb_kszap(atoi(row[0]),data->window));
    iceb_menu_soob(&repl,data->window);
    return(1);  

   }
 }

/****************************************************/

/*Блокируем таблицу*/
iceb_lock_tables lokt("LOCK TABLES Dokummat WRITE, Prihod READ, Rashod READ,Blok READ,Alx READ,icebuser READ,Nalog READ");

if(iceb_u_SRAV(data->rk.nomdok_kor.ravno(),data->rk.nomdok.ravno(),0) != 0 )
 {  

  /*Проверяем есть ли такой номер документа*/
  sprintf(strsql,"select nomd from Dokummat where \
datd >= '%04d-01-01' and datd <= '%d-12-31' and sklad=%s and nomd='%s'",
  data->rk.datad.ravno_god(),data->rk.datad.ravno_god(),data->rk.sklad.ravno(),data->rk.nomdok.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),data->rk.nomdok.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);  
   }
 }
/*Проверяем номер встречного документа*/
if(iceb_u_SRAV(data->rk.nomdok_p_kor.ravno(),data->rk.nomdok_p.ravno(),0) != 0 && \
data->rk.nomdok_p.getdlinna() > 1)
 {  
  sprintf(strsql,"select nomd from Dokummat where tip=%d and \
datd >= '%d-01-01' and datd <= '%d-12-31' and sklad=%d and nomd='%s'",
  1,data->rk.datad.ravno_god(),data->rk.datad.ravno_god(),
  data->rk.sklad_par,data->rk.nomdok_p.ravno());
  
  if(iceb_sql_readkey(strsql,data->window) >= 1)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),data->rk.nomdok_p.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);  
   }
 }

zapdokw(&data->rk,&lokt,data->window);


return(0);



}


/*********************************/
/*создание меню для выбота формы оплаты*/
/****************************************/
void vdnd2_menu_fo(class vdnd2_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

data->opt_foroplat=gtk_combo_box_text_new();
int nom=0;

data->menu_forop.plus("");
gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(data->opt_foroplat),nom++,"");

sprintf(strsql,"select kod,naik from Foroplat");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  data->menu_forop.plus(row[0]);
  sprintf(strsql,"%s %s",row[0],row[1]);
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(data->opt_foroplat),nom++,strsql);

 }
/*определяем какая строка должна быть активной*/
int nomer_as=0;
for(nom=0; nom < data->menu_forop.kolih(); nom++)
 if(iceb_u_SRAV(data->rk.forma_oplat.ravno(),data->menu_forop.ravno(nom),0) == 0)
  {
   nomer_as=nom;
   break;
  }
gtk_combo_box_set_active(GTK_COMBO_BOX(data->opt_foroplat),nomer_as);

}