/*$Id: udvhdw.c,v 1.16 2014/08/31 06:19:22 sasa Exp $*/
/*25.07.2019	20.04.2009	Белых А.И.	udvhdw.c
Ввод и корректировка шапки доверенности
*/

#include <ctype.h>
#include "buhg_g.h"
enum
 {
  E_DATAV,
  E_NOMDOV,
  E_DATADO,
  E_KOMU,
  E_POST,
  E_DATANAR,
  E_NOMERNAR,
  E_OTMISP,
  E_PODOK,
  E_SERIQ,
  E_NOMDOK,
  E_DATAVD,
  E_KEMVID,
  E_DOLG,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class udvhdw_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str datav;
  class iceb_u_str nomdov;
  class iceb_u_str datado;
  class iceb_u_str komu;
  class iceb_u_str post;
  class iceb_u_str datanar;
  class iceb_u_str nomernar;
  class iceb_u_str otmisp;
  class iceb_u_str podok;
  class iceb_u_str seriq;
  class iceb_u_str nomdok;
  class iceb_u_str datavd;
  class iceb_u_str kemvid;
  class iceb_u_str dolg;
  
  class iceb_u_str datakor;
  class iceb_u_str nomdovkor;

  udvhdw_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();
   }

  void read_rek()
   {
    datav.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAV])));
    nomdov.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOV])));
    datado.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATADO])));
    komu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMU])));
    post.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_POST])));
    datanar.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATANAR])));
    nomernar.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMERNAR])));
    otmisp.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_OTMISP])));
    podok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODOK])));
    seriq.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SERIQ])));
    nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    datavd.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAVD])));
    kemvid.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KEMVID])));
    dolg.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DOLG])));
   }
  void clear()
   {
    datav.new_plus("");
    nomdov.new_plus("");
    datado.new_plus("");
    komu.new_plus("");
    post.new_plus("");
    datanar.new_plus("");
    nomernar.new_plus("");
    otmisp.new_plus("");
    podok.new_plus("");
    seriq.new_plus("");
    nomdok.new_plus("");
    datavd.new_plus("");
    kemvid.new_plus("");
    dolg.new_plus("");
   }
 };


gboolean   udvhdw_key_press(GtkWidget *widget,GdkEventKey *event,class udvhdw_data *data);
void  udvhdw_knopka(GtkWidget *widget,class udvhdw_data *data);
void    udvhdw_vvod(GtkWidget *widget,class udvhdw_data *data);
int udvhdw_zap(class udvhdw_data *data);
void  udvhdw_e_knopka(GtkWidget *widget,class udvhdw_data *data);

extern SQL_baza  bd;

int udvhdw(class iceb_u_str *datadov,class iceb_u_str *nomerdov,GtkWidget *wpredok)
{

class udvhdw_data data;
char strsql[512];
iceb_u_str kikz;
data.datav.new_plus(datadov->ravno());
data.nomdov.new_plus(nomerdov->ravno());
data.datakor.new_plus(datadov->ravno());
data.nomdovkor.new_plus(nomerdov->ravno());

if(data.datav.getdlinna() >  1)
 {
  sprintf(strsql,"select * from Uddok where datd='%s' and nomd='%s'",data.datav.ravno_sqldata(),data.nomdov.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.datado.new_plus(iceb_u_datzap(row[2]));
  data.komu.new_plus(row[3]);
  data.post.new_plus(row[4]);
  data.podok.new_plus(row[5]);
  data.otmisp.new_plus(row[6]);
  data.datanar.new_plus(iceb_u_datzap(row[7]));
  data.nomernar.new_plus(row[8]);
  data.seriq.new_plus(row[9]);
  data.nomdok.new_plus(row[10]);
  data.datavd.new_plus(iceb_u_datzap(row[11]));
  data.kemvid.new_plus(row[12]);
  data.dolg.new_plus(row[13]);


  kikz.plus(iceb_kikz(row[14],row[15],wpredok));
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.datav.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод нового документа"));
  label=gtk_label_new(gettext("Ввод нового документа"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка шапки доверенности"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка шапки доверенности"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(udvhdw_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s (%s)",gettext("Дата выдачи доверенности"),gettext("д.м.г"));
data.knopka_enter[E_DATAV]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAV]), data.knopka_enter[E_DATAV], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAV],"clicked",G_CALLBACK(udvhdw_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAV],iceb_u_inttochar(E_DATAV));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAV],gettext("Выбор даты"));

data.entry[E_DATAV] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAV]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAV]), data.entry[E_DATAV], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAV], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAV]),data.datav.ravno());
gtk_widget_set_name(data.entry[E_DATAV],iceb_u_inttochar(E_DATAV));


sprintf(strsql,"%s",gettext("Номер доверенности"));
data.knopka_enter[E_NOMDOV]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOV]), data.knopka_enter[E_NOMDOV], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NOMDOV],"clicked",G_CALLBACK(udvhdw_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMDOV],iceb_u_inttochar(E_NOMDOV));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NOMDOV],gettext("Получение номера документа"));

data.entry[E_NOMDOV] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOV]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOV]), data.entry[E_NOMDOV], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOV], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOV]),data.nomdov.ravno());
gtk_widget_set_name(data.entry[E_NOMDOV],iceb_u_inttochar(E_NOMDOV));


sprintf(strsql,"%s (%s)",gettext("Действительна до"),gettext("д.м.г"));
data.knopka_enter[E_DATADO]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATADO]), data.knopka_enter[E_DATADO], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATADO],"clicked",G_CALLBACK(udvhdw_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATADO],iceb_u_inttochar(E_DATADO));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATADO],gettext("Выбор даты"));

data.entry[E_DATADO] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATADO]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATADO]), data.entry[E_DATADO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATADO], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATADO]),data.datado.ravno());
gtk_widget_set_name(data.entry[E_DATADO],iceb_u_inttochar(E_DATADO));

sprintf(strsql,"%s",gettext("Кому"));
data.knopka_enter[E_KOMU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMU]), data.knopka_enter[E_KOMU], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOMU],"clicked",G_CALLBACK(udvhdw_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOMU],iceb_u_inttochar(E_KOMU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOMU],gettext("Выбор работника"));

data.entry[E_KOMU] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMU]),79);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMU]), data.entry[E_KOMU], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMU], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMU]),data.komu.ravno());
gtk_widget_set_name(data.entry[E_KOMU],iceb_u_inttochar(E_KOMU));


sprintf(strsql,"%s",gettext("Поставщик"));
data.knopka_enter[E_POST]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_POST]), data.knopka_enter[E_POST], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_POST],"clicked",G_CALLBACK(udvhdw_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_POST],iceb_u_inttochar(E_POST));
gtk_widget_set_tooltip_text(data.knopka_enter[E_POST],gettext("Выбор поставщика"));

data.entry[E_POST] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_POST]),255);
gtk_box_pack_start (GTK_BOX (hbox[E_POST]), data.entry[E_POST], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_POST], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_POST]),data.post.ravno());
gtk_widget_set_name(data.entry[E_POST],iceb_u_inttochar(E_POST));


sprintf(strsql,"%s (%s)",gettext("Дата наряда"),gettext("д.м.г"));
data.knopka_enter[E_DATANAR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATANAR]), data.knopka_enter[E_DATANAR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATANAR],"clicked",G_CALLBACK(udvhdw_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATANAR],iceb_u_inttochar(E_DATANAR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATANAR],gettext("Выбор даты"));

data.entry[E_DATANAR] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATANAR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATANAR]), data.entry[E_DATANAR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATANAR], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATANAR]),data.datanar.ravno());
gtk_widget_set_name(data.entry[E_DATANAR],iceb_u_inttochar(E_DATANAR));

label=gtk_label_new(gettext("Номер наряда"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERNAR]), label, FALSE, FALSE, 0);

data.entry[E_NOMERNAR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMERNAR]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERNAR]), data.entry[E_NOMERNAR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMERNAR], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMERNAR]),data.nomernar.ravno());
gtk_widget_set_name(data.entry[E_NOMERNAR],iceb_u_inttochar(E_NOMERNAR));


label=gtk_label_new(gettext("Отметка использования"));
gtk_box_pack_start (GTK_BOX (hbox[E_OTMISP]), label, FALSE, FALSE, 0);

data.entry[E_OTMISP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_OTMISP]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_OTMISP]), data.entry[E_OTMISP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_OTMISP], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OTMISP]),data.otmisp.ravno());
gtk_widget_set_name(data.entry[E_OTMISP],iceb_u_inttochar(E_OTMISP));

label=gtk_label_new(gettext("По документу"));
gtk_box_pack_start (GTK_BOX (hbox[E_PODOK]), label, FALSE, FALSE, 0);

data.entry[E_PODOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PODOK]),50);
gtk_box_pack_start (GTK_BOX (hbox[E_PODOK]), data.entry[E_PODOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODOK], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODOK]),data.podok.ravno());
gtk_widget_set_name(data.entry[E_PODOK],iceb_u_inttochar(E_PODOK));


label=gtk_label_new(gettext("Серия документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_SERIQ]), label, FALSE, FALSE, 0);

data.entry[E_SERIQ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SERIQ]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SERIQ]), data.entry[E_SERIQ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SERIQ], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SERIQ]),data.seriq.ravno());
gtk_widget_set_name(data.entry[E_SERIQ],iceb_u_inttochar(E_SERIQ));

label=gtk_label_new(gettext("Номер документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE, 0);

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));


sprintf(strsql,"%s (%s)",gettext("Дата выдачи документа"),gettext("д.м.г"));
data.knopka_enter[E_DATAVD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAVD]), data.knopka_enter[E_DATAVD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAVD],"clicked",G_CALLBACK(udvhdw_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAVD],iceb_u_inttochar(E_DATAVD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAVD],gettext("Выбор даты"));

data.entry[E_DATAVD] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAVD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAVD]), data.entry[E_DATAVD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAVD], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAVD]),data.datavd.ravno());
gtk_widget_set_name(data.entry[E_DATAVD],iceb_u_inttochar(E_DATAVD));


label=gtk_label_new(gettext("Кем видан документ"));
gtk_box_pack_start (GTK_BOX (hbox[E_KEMVID]), label, FALSE, FALSE, 0);

data.entry[E_KEMVID] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KEMVID]),200);
gtk_box_pack_start (GTK_BOX (hbox[E_KEMVID]), data.entry[E_KEMVID], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KEMVID], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KEMVID]),data.kemvid.ravno());
gtk_widget_set_name(data.entry[E_KEMVID],iceb_u_inttochar(E_KEMVID));


label=gtk_label_new(gettext("Должность"));
gtk_box_pack_start (GTK_BOX (hbox[E_DOLG]), label, FALSE, FALSE, 0);

data.entry[E_DOLG] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DOLG]),50);
gtk_box_pack_start (GTK_BOX (hbox[E_DOLG]), data.entry[E_DOLG], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DOLG], "activate",G_CALLBACK(udvhdw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOLG]),data.dolg.ravno());
gtk_widget_set_name(data.entry[E_DOLG],iceb_u_inttochar(E_DOLG));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(udvhdw_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(udvhdw_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  datadov->new_plus(data.datav.ravno());
  nomerdov->new_plus(data.nomdov.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  udvhdw_e_knopka(GtkWidget *widget,class udvhdw_data *data)
{
char strsql[512];
class iceb_u_str kod("");
class iceb_u_str naim("");
class SQLCURSOR cur;
SQL_str row;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_zarnah_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_DATAV:
    if(iceb_calendar(&data->datav,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAV]),data->datav.ravno());
    return;  
  case E_DATAVD:
    if(iceb_calendar(&data->datavd,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAVD]),data->datavd.ravno());
    return;  

  case E_DATANAR:
    if(iceb_calendar(&data->datanar,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATANAR]),data->datanar.ravno());
    return;  
      
  case E_DATADO:
    if(iceb_calendar(&data->datado,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATADO]),data->datado.ravno());
    return;  

  case E_KOMU:
    if(l_sptbn1(&kod,&naim,0,data->window) == 0)
     {
      data->komu.new_plus(naim.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOMU]),data->komu.ravno());

      sprintf(strsql,"select nomp,vidan,datvd,dolg from Kartb where tabn=%d",kod.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {

        iceb_u_polen(row[0],strsql,sizeof(strsql),1,' ');
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SERIQ]),strsql);

        iceb_u_polen(row[0],strsql,sizeof(strsql),2,' ');
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),strsql);

        gtk_entry_set_text(GTK_ENTRY(data->entry[E_KEMVID]),row[1]);

        gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAVD]),iceb_u_datzap(row[2]));

        gtk_entry_set_text(GTK_ENTRY(data->entry[E_DOLG]),row[3]);

 
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODOK]),"Паспорт");
        
       }

     }
    return;  

  case E_POST:
    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
     {
      data->post.new_plus(iceb_get_pnk(kod.ravno(),1,data->window));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_POST]),data->post.ravno());

     }
    return;  

  case E_NOMDOV:
    data->datav.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAV])));
    if(data->datav.ravno_god() != 0)
     {
      data->nomdov.new_plus(iceb_nomdok(data->datav.ravno_god(),"Uddok",data->window));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOV]),data->nomdov.ravno());
     }      

    return;  



 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   udvhdw_key_press(GtkWidget *widget,GdkEventKey *event,class udvhdw_data *data)
{
//char  bros[512];

//printf("udvhdw_key_press\n");
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
void  udvhdw_knopka(GtkWidget *widget,class udvhdw_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("udvhdw_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(udvhdw_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    udvhdw_vvod(GtkWidget *widget,class udvhdw_data *data)
{
class iceb_u_str kkk("");
char strsql[1024];
class SQLCURSOR cur;
SQL_str row;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("udvhdw_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAV:
    data->datav.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    if(data->nomdov.getdlinna() <= 1)
    if(data->datav.ravno_god() != 0)
     {
      data->nomdov.new_plus(iceb_nomdok(data->datav.ravno_god(),"Uddok",data->window));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOV]),data->nomdov.ravno());
     }      
    break;
  case E_DATADO:
    data->datado.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATANAR:
    data->datanar.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOMU:
    data->komu.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select nomp,vidan,datvd,dolg from Kartb where tabn=%d",data->komu.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {

      iceb_u_polen(row[0],&kkk,1,' ');
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SERIQ]),kkk.ravno());

      if(iceb_u_polen(row[0],&kkk,2,' ') != 0)
       kkk.new_plus(row[0]);
      
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),kkk.ravno());

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KEMVID]),row[1]);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAVD]),iceb_u_datzap(row[2]));

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_DOLG]),row[3]);


      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODOK]),"Паспорт");
      
     }
    break;

  case E_POST:
    data->post.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(isdigit(data->post.ravno()[0]) != 0) //Значит символ число
     {
      sprintf(strsql,"select naikon,pnaim from Kontragent where kodkon='%d'",data->post.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        if(row[1][0] == '\0')
         data->post.new_plus(row[0]);
        else
         data->post.new_plus(row[1]);
  
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_POST]),data->post.ravno());

       }
     }

    break;

  case E_NOMDOV:
    data->nomdov.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_OTMISP:
    data->otmisp.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NOMERNAR:
    data->nomernar.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_PODOK:
    data->podok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_SERIQ:
    data->seriq.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NOMDOK:
    data->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KEMVID:
    data->kemvid.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DOLG:
    data->dolg.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATAVD:
    data->datavd.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int udvhdw_zap(class udvhdw_data *data)
{
char strsql[2048];

if(iceb_pbpds(data->datakor.ravno(),data->window) != 0)
 return(1);
if(iceb_pbpds(data->datav.ravno(),data->window) != 0)
 return(1);

if(data->datav.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата выдачи доверенности!"),data->window);
  return(1);  
 }

if(data->datado.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата \"Действительна до\"!"),data->window);
  return(1);  
 }

if(data->datanar.getdlinna() > 1)
if(data->datanar.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата наряда!"),data->window);
  return(1);  
 }
if(data->datavd.getdlinna() > 1)
if(data->datavd.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата выдачи документа!"),data->window);
  return(1);  
 }

if(data->nomdov.getdlinna() <= 0)
 {
  iceb_menu_soob(gettext("Не ввели номер доверенности!"),data->window);
  return(1);  

 }
if(data->komu.getdlinna() <= 0)
 {
  iceb_menu_soob(gettext("Не ввели кому!"),data->window);
  return(1);  

 }
if(data->post.getdlinna() <= 0)
 {
  iceb_menu_soob(gettext("Не ввели поставщика!"),data->window);
  return(1);  

 }

class iceb_lock_tables KKK("LOCK TABLES Uddok WRITE,Uddok1 WRITE,icebuser READ");
short dd=0,md=0,gd=0;
iceb_u_rsdat(&dd,&md,&gd,data->datav.ravno(),1);

if(gd != data->datakor.ravno_god() || iceb_u_SRAV(data->nomdovkor.ravno(),data->nomdov.ravno(),0) != 0)
 {
  /*Проверяем есть ли такой номер документа*/
  sprintf(strsql,"select nomd from Uddok where datd >= '%04d-01-01' and datd <= '%04d-12-31' \
  and nomd = '%s'",gd,gd,data->nomdov.ravno());
  if(iceb_sql_readkey(strsql,data->window) > 0)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть!"),data->nomdov.ravno());    
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }
if(data->datakor.getdlinna() <= 1) /*Новый документ*/
 {
  sprintf(strsql,"insert into Uddok values('%s','%s','%s','%s','%s','%s','%s','%s','%s',\
'%s','%s','%s','%s','%s',%d,%ld)",
  data->datav.ravno_sqldata(),
  data->nomdov.ravno_filtr(),
  data->datado.ravno_sqldata(),
  data->komu.ravno_filtr(),
  data->post.ravno_filtr(),
  data->podok.ravno_filtr(),
  data->otmisp.ravno_filtr(),
  data->datanar.ravno_sqldata(),
  data->nomernar.ravno_filtr(),
  data->seriq.ravno_filtr(),
  data->nomdok.ravno_filtr(),
  data->datavd.ravno_sqldata(),
  data->kemvid.ravno_filtr(),
  data->dolg.ravno_filtr(),
  iceb_getuid(data->window),
  time(NULL));
        
 }
else /*Корректировка уже введённого документа*/
 {
  sprintf(strsql,"update Uddok set \
datd='%s',\
nomd='%s',\
datds='%s',\
komu='%s',\
post='%s',\
podok='%s',\
otis='%s',\
datn='%s',\
nomn='%s',\
seriq='%s',\
nomerd='%s',\
datavd='%s',\
vidan='%s',\
dolg='%s',\
ktoz=%d,\
vrem=%ld \
where datd='%s' and nomd='%s'",
  data->datav.ravno_sqldata(),
  data->nomdov.ravno_filtr(),
  data->datado.ravno_sqldata(),
  data->komu.ravno_filtr(),
  data->post.ravno_filtr(),
  data->podok.ravno_filtr(),
  data->otmisp.ravno_filtr(),
  data->datanar.ravno_sqldata(),
  data->nomernar.ravno_filtr(),
  data->seriq.ravno_filtr(),
  data->nomdok.ravno_filtr(),
  data->datavd.ravno_sqldata(),
  data->kemvid.ravno_filtr(),
  data->dolg.ravno_filtr(),
  iceb_getuid(data->window),
  time(NULL),
  data->datakor.ravno_sqldata(),
  data->nomdovkor.ravno_filtr());  
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

/*Корректируем записи в документах*/
if(iceb_u_sravmydat(data->datav.ravno(),data->datakor.ravno()) != 0 || iceb_u_SRAV(data->nomdovkor.ravno(),data->nomdov.ravno(),0) != 0)
 {
  sprintf(strsql,"update Uddok1 set datd='%s', nomd='%s' where datd='%s' and nomd='%s'",
  data->datav.ravno_sqldata(),
  data->nomdov.ravno(),
  data->datakor.ravno_sqldata(),
  data->nomdovkor.ravno());

  iceb_sql_zapis(strsql,0,0,data->window); 
 }


return(0);

}
