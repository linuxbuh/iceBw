/*$Id: vkasord1.c,v 1.27 2013/10/04 07:27:17 sasa Exp $*/
/*09.06.2010	23.01.2006	Белых А.И.	vkasord1.c
Ввод и корректировка шапки для приходного кассового ордера
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
  E_KASSA,
  E_DATAD,
  E_NOMDOK,
  E_KODOP,
  E_SHET,
  E_OSNOV,
  E_DOPOL,
  E_FIO,
  E_NOMER_BL,
  KOLENTER  
 };

class vkasord1_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_naim_shet;
  GtkWidget *label_naim_kodop;
  GtkWidget *label_naim_kassa;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  //Ключевые реквизиты корректируемого документа
  class iceb_u_str datad_k;
  class iceb_u_str nomdok_k;
  class iceb_u_str kassa_k;
  int tipz; //не корректируемый реквизит  
  
  //Реквизиты шапки документа
  class iceb_u_str kassa;
  class iceb_u_str datad;
  class iceb_u_str nomdok;
  class iceb_u_str kodop;
  class iceb_u_str shet;
  class iceb_u_str osnov;
  class iceb_u_str dopol;
  class iceb_u_str fio;
  class iceb_u_str nomer_bl;
  class iceb_u_str nomer_blz; /*запомненный номер бланка до корректировки*/
  
  vkasord1_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    nomer_blz.plus("");
    clear_data();
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_data()
   {
    kassa.new_plus("");
    datad.new_plus("");
    nomdok.new_plus("");
    kodop.new_plus("");
    shet.new_plus("");
    osnov.new_plus("");
    dopol.new_plus("");
    fio.new_plus("");
    nomer_bl.new_plus("");
   }
 };

gboolean   vkasord1_v_key_press(GtkWidget *widget,GdkEventKey *event,class vkasord1_data *data);
void    vkasord1_v_vvod(GtkWidget *widget,class vkasord1_data *data);
void  vkasord1_v_knopka(GtkWidget *widget,class vkasord1_data *data);
void vkasord1_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int vkasord1_pk(char *kod,GtkWidget *wpredok);

void  vkasord1_v_e_knopka(GtkWidget *widget,class vkasord1_data *data);
int vkasord1_zap(class vkasord1_data *data);

extern SQL_baza bd;
//extern double	okrcn;  /*Округление цены*/

int vkasord1(class iceb_u_str *datad, //дата документа
class iceb_u_str *nomdok,
class iceb_u_str *kassa,
GtkWidget *wpredok)
{
static class iceb_u_str datadok_zap("");
static class iceb_u_str kodkas_zap("");

class vkasord1_data data;
char strsql[512];

if(datadok_zap.getdlinna() <= 1)
 {
  short dt,mt,gt;
  iceb_u_poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%02d.%02d.%04d",dt,mt,gt);
  datadok_zap.plus(strsql);
 }


data.datad_k.plus(datad->ravno());
data.nomdok_k.plus(nomdok->ravno());
data.kassa_k.plus(kassa->ravno());
data.datad.plus(datad->ravno());
data.nomdok.plus(nomdok->ravno());
data.kassa.plus(kassa->ravno());
data.tipz=1; //меню для работы только с приходными документами
iceb_u_str kikz;
iceb_u_str naim_shet;
iceb_u_str naim_kodop;
iceb_u_str naim_kassa;
naim_shet.plus("");
naim_kodop.plus("");
naim_kassa.plus("");

if(data.nomdok_k.getdlinna() > 1) //корректировка шапки документа
 {
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Kasord where nomd='%s' and kassa=%d and \
tp=%d and god=%d",data.nomdok_k.ravno(),data.kassa_k.ravno_atoi(),data.tipz,data.datad_k.ravno_god());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.shet.new_plus(row[4]);
  data.kodop.new_plus(row[5]);
  data.osnov.new_plus(row[8]);
  data.dopol.new_plus(row[9]);
  data.fio.new_plus(row[10]);
  data.nomer_bl.new_plus(row[15]);
  data.nomer_blz.new_plus(row[15]);

  kikz.plus(iceb_kikz(row[13],row[14],wpredok));

  //Узнаём наименование кассы
  sprintf(strsql,"select naik from Kas where kod=%d",data.kassa.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_kassa.new_plus(row[0]);

  //Узнаём наименование счёта
  if(data.shet.getdlinna() > 1)
   {
    sprintf(strsql,"select nais from Plansh where ns='%s'",data.shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_shet.new_plus(row[0]);
   }

  //Узнаём наименование операции
  if(data.kodop.getdlinna() > 1)
   {
    sprintf(strsql,"select naik from Kasop1 where kod='%s'",data.kodop.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_kodop.new_plus(row[0]);
   }
  
 }
else
 {
  data.kassa.plus(kodkas_zap.ravno());
  data.datad.plus(datadok_zap.ravno());
  if(data.kassa.getdlinna() > 1 && data.datad.getdlinna() > 1)
     iceb_nomnak(data.datad.ravno_god(),data.kassa.ravno(),&data.nomdok,data.tipz,2,1,wpredok);

 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.nomdok.getdlinna() > 1)
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи."));
else
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи."));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vkasord1_v_key_press),&data);

iceb_u_str zagolov;
if(data.nomdok.getdlinna() <= 1)
 zagolov.new_plus(gettext("Ввод нового ордера на приход."));
else
 {
  zagolov.new_plus(gettext("Корректировка шапки приходного документа."));
  zagolov.ps_plus(kikz.ravno());
 }

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

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator1);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);



sprintf(strsql,"%s",gettext("Код кассы"));
data.knopka_enter[E_KASSA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.knopka_enter[E_KASSA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KASSA],"clicked",G_CALLBACK(vkasord1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KASSA],iceb_u_inttochar(E_KASSA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KASSA],gettext("Выбор кассы."));

data.entry[E_KASSA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KASSA]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.entry[E_KASSA],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KASSA], "activate",G_CALLBACK(vkasord1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KASSA]),data.kassa.ravno());
gtk_widget_set_name(data.entry[E_KASSA],iceb_u_inttochar(E_KASSA));

data.label_naim_kassa=gtk_label_new(naim_kassa.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.label_naim_kassa,TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Дата"));
data.knopka_enter[E_DATAD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.knopka_enter[E_DATAD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAD],"clicked",G_CALLBACK(vkasord1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAD],iceb_u_inttochar(E_DATAD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAD],gettext("Выбор даты."));

data.entry[E_DATAD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.entry[E_DATAD],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAD], "activate",G_CALLBACK(vkasord1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAD]),data.datad.ravno());
gtk_widget_set_name(data.entry[E_DATAD],iceb_u_inttochar(E_DATAD));

sprintf(strsql,"%s",gettext("Номер кассового ордера"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NOMDOK],"clicked",G_CALLBACK(vkasord1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NOMDOK],gettext("Получение номера документа."));

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(vkasord1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));

sprintf(strsql,"%s",gettext("Счёт кассы"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(vkasord1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов."));

data.entry[E_SHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(vkasord1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

data.label_naim_shet=gtk_label_new(naim_shet.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.label_naim_shet,TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODOP],"clicked",G_CALLBACK(vkasord1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODOP],iceb_u_inttochar(E_KODOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODOP],gettext("Выбор операции"));

data.entry[E_KODOP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODOP]),9);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(vkasord1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));

data.label_naim_kodop=gtk_label_new(naim_kodop.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.label_naim_kodop,TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Основание"));
label=gtk_label_new(strsql);
data.entry[E_OSNOV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_OSNOV]),249);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), data.entry[E_OSNOV],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_OSNOV], "activate",G_CALLBACK(vkasord1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OSNOV]),data.osnov.ravno());
gtk_widget_set_name(data.entry[E_OSNOV],iceb_u_inttochar(E_OSNOV));

sprintf(strsql,"%s",gettext("Дополнение"));
label=gtk_label_new(strsql);
data.entry[E_DOPOL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DOPOL]),79);
gtk_box_pack_start (GTK_BOX (hbox[E_DOPOL]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DOPOL]), data.entry[E_DOPOL],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DOPOL], "activate",G_CALLBACK(vkasord1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOPOL]),data.dopol.ravno());
gtk_widget_set_name(data.entry[E_DOPOL],iceb_u_inttochar(E_DOPOL));

sprintf(strsql,"%s",gettext("Фамилия"));
label=gtk_label_new(strsql);
data.entry[E_FIO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FIO]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO], "activate",G_CALLBACK(vkasord1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.fio.ravno());
gtk_widget_set_name(data.entry[E_FIO],iceb_u_inttochar(E_FIO));

sprintf(strsql,"%s",gettext("Номер бланка"));
label=gtk_label_new(strsql);
data.entry[E_NOMER_BL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMER_BL]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_BL]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_BL]), data.entry[E_NOMER_BL],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMER_BL], "activate",G_CALLBACK(vkasord1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_BL]),data.nomer_bl.ravno());
gtk_widget_set_name(data.entry[E_NOMER_BL],iceb_u_inttochar(E_NOMER_BL));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации."));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vkasord1_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне."));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vkasord1_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 {
  datad->new_plus(data.datad.ravno());
  nomdok->new_plus(data.nomdok.ravno());
  kassa->new_plus(data.kassa.ravno());

  datadok_zap.new_plus(data.datad.ravno());
  kodkas_zap.new_plus(data.kassa.ravno());
 }
return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vkasord1_v_e_knopka(GtkWidget *widget,class vkasord1_data *data)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_NOMDOK:

     //Читаем кассу
     data->kassa.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_KASSA])));
     //Читаем дату
     data->datad.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAD])));
     
     iceb_nomnak(data->datad.ravno_god(),data->kassa.ravno(),&data->nomdok,data->tipz,2,1,data->window);
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->nomdok.ravno());
      
    return;  

  case E_DATAD:

    if(iceb_calendar(&data->datad,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->datad.ravno());

    if(data->datad.getdlinna() > 1 && data->kassa.getdlinna() > 1 && data->nomdok.getdlinna() <= 1)
     {
      iceb_nomnak(data->datad.ravno_god(),data->kassa.ravno(),&data->nomdok,data->tipz,2,1,data->window);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->nomdok.ravno());
     }      
    return;  

  case E_SHET:
    if(iceb_vibrek(1,"Plansh",&data->shet,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naim_shet),naim.ravno(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno());
    return;

  case E_KASSA:
    if(l_spis_kas(1,&kod,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim_kassa),naim.ravno(20));
      data->kassa.new_plus(kod.ravno());
     }    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KASSA]),data->kassa.ravno());
    return;

  case E_KODOP:

     if(l_kasopp(1,&kod,&naim,data->window) == 0)
      {
       gtk_label_set_text(GTK_LABEL(data->label_naim_kodop),naim.ravno(20));
       data->kodop.new_plus(kod.ravno());
       sprintf(strsql,"select shetkor from Kasop1 where kod='%s'",kod.ravno());
       if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
        {
         gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),row[0]);
         sprintf(strsql,"select nais from Plansh where ns='%s'",row[0]);

         if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
           {
            sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
            gtk_label_set_text(GTK_LABEL(data->label_naim_shet),strsql);
           }    
        }
      }
      

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->kodop.ravno());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vkasord1_v_key_press(GtkWidget *widget,GdkEventKey *event,class vkasord1_data *data)
{

//printf("vkasord1_v_key_press\n");
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
void  vkasord1_v_knopka(GtkWidget *widget,class vkasord1_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(vkasord1_zap(data) != 0)
      return;

    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
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

void    vkasord1_v_vvod(GtkWidget *widget,class vkasord1_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int enter=atoi(gtk_widget_get_name(widget));
//g_print("vkasord1_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_FIO:
    data->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMDOK:
    data->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMER_BL:
    data->nomer_bl.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_OSNOV:
    data->osnov.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DOPOL:
    data->dopol.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAD:
    data->datad.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->datad.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
      break;
     }    
 
    /*Читаем номер документа*/ 
    data->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK])));
    if(data->nomdok.getdlinna() <= 1)
     {
      /*Читаем кассу*/
      data->kassa.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_KASSA])));
      if(data->kassa.getdlinna() > 1 && data->datad.getdlinna() > 1)
       {
        iceb_nomnak(data->datad.ravno_god(),data->kassa.ravno(),&data->nomdok,data->tipz,2,1,data->window);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->nomdok.ravno());
       }
     }
    break;

  case E_SHET:

    data->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    sprintf(strsql,"select nais from Plansh where ns='%s'",data->shet.ravno());

    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_shet),strsql);
     }    
    break;

  case E_KASSA:

    data->kassa.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    sprintf(strsql,"select naik from Kasop1 where kod='%s'",data->kassa.ravno());


    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_kassa),strsql);
     }    
    break;

  case E_KODOP:

    data->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    sprintf(strsql,"select naik,shetkor from Kasop1 where kod='%s'",data->kodop.ravno());

    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_kodop),strsql);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),row[1]);

      sprintf(strsql,"select nais from Plansh where ns='%s'",row[1]);
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
        gtk_label_set_text(GTK_LABEL(data->label_naim_shet),strsql);
       }    
     }    
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************/
/*Запись шапки кассового ордера*/
/*******************************/
int vkasord1_zap(class vkasord1_data *data)
{
data->read_rek(); //читаем все реквизиты из меню


if(data->datad.getdlinna() <= 1 || data->kassa.getdlinna() <= 1 || \
data->nomdok.getdlinna() <= 1 || data->kodop.getdlinna() <= 1 || 
data->shet.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введены все обязательные реквизиты !"),data->window);
  return(1);
 }

short d,m,g;

if(iceb_u_rsdat(&d,&m,&g,data->datad.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата документа !"),data->window);
  return(1);
 }
if(pbkasw(m,g,data->window) != 0)
 return(1);

char strsql[1024];

//Проверяем код кассы
sprintf(strsql,"select kod from Kas where kod=%d",data->kassa.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код кассы"),data->kassa.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

//проверяем код операции
sprintf(strsql,"select kod from Kasop1 where kod='%s'",data->kodop.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код операции"),data->kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
//проверяем счёт
class OPSHET opsh;

if(data->shet.getdlinna() > 1)
 if(iceb_prsh1(data->shet.ravno(),&opsh,data->window) != 0)
  return(1);
SQL_str row;
class SQLCURSOR cur;
/*Проверяем номер бланка*/
if(data->nomer_bl.getdlinna() > 1)
 if(iceb_u_SRAV(data->nomer_bl.ravno(),data->nomer_blz.ravno(),0) != 0)
  {
   sprintf(strsql,"select nomd from Kasord where nb='%s'",data->nomer_bl.ravno());
   if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
    {
     sprintf(strsql,gettext("Номер бланка %s уже есть у документа %s !"),data->nomer_bl.ravno(),row[0]);
     iceb_menu_soob(strsql,data->window);
     return(1);     
    } 

  }

int kodv=0;
int mkor=0;
if(data->nomdok_k.getdlinna() > 1)
  mkor=1;

short dz,mz,gz;
iceb_u_rsdat(&dz,&mz,&gz,data->datad_k.ravno(),1);
  
if((kodv=zaphkorw(mkor,
data->kassa.ravno(),
data->tipz,
d,m,g,
data->nomdok.ravno_filtr(),
data->shet.ravno_filtr(),
data->kodop.ravno_filtr(),
data->osnov.ravno_filtr(),
data->dopol.ravno_filtr(),
data->fio.ravno_filtr(),
"",
data->kassa_k.ravno(),data->nomdok_k.ravno(),dz,mz,gz,data->nomer_bl.ravno(),"",data->window)) != 0)
 {
  if(kodv == 1)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),data->nomdok.ravno());
    iceb_menu_soob(strsql,data->window);
   }
  return(1);
 }
return(0);

}
