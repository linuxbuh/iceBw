/*$Id: l_kv_v.c,v 1.1 2013/09/26 09:48:37 sasa Exp $*/
/*10.01.2018	05.09.2013	Белых А.И.	l_kv_v.c
Ввод и корректировка справочника курсов валют
*/
#include "buhg_g.h"

enum
 {
  E_DATAK,
  E_KURS,
  E_KOMENT,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class l_kv_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  
  class iceb_u_str datak;
  class iceb_u_str kurs;
  class iceb_u_str koment;
  
  class iceb_u_str datak_start;
  class iceb_u_str kod_val;
  l_kv_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    datak.plus("");
    kurs.plus("");
    koment.plus("");
   }

  void read_rek()
   {
    datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    kurs.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KURS])));
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
   }
 };


gboolean   l_kv_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_kv_v_data *data);
void  l_kv_v_knopka(GtkWidget *widget,class l_kv_v_data *data);
void    l_kv_v_vvod(GtkWidget *widget,class l_kv_v_data *data);
int l_kv_pk(class l_kv_v_data *data);
int l_kv_zap(class l_kv_v_data *data);
void  l_kv_v_e_knopka(GtkWidget *widget,class l_kv_v_data *data);


extern SQL_baza  bd;

int l_kv_v(const char *kod_val,class iceb_u_str *datak_start,GtkWidget *wpredok)
{

class l_kv_v_data data;
char strsql[512];
class iceb_u_str kikz;

data.datak_start.new_plus(datak_start->ravno());
data.kod_val.new_plus(kod_val);


if(data.datak_start.getdlinna() >  1)
 {
  data.datak.new_plus(datak_start->ravno());
  sprintf(strsql,"select * from Glkkv where kod='%s' and datk='%s'",data.kod_val.ravno(),data.datak_start.ravno_sqldata());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.kurs.new_plus(row[2]);
  data.koment.new_plus(row[3]);
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  
 }
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.datak_start.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_kv_v_key_press),&data);

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



data.knopka_enter[E_DATAK]=gtk_button_new_with_label(gettext("Дата"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(l_kv_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(l_kv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));

label=gtk_label_new(gettext("Курс"));
gtk_box_pack_start (GTK_BOX (hbox[E_KURS]), label, FALSE, FALSE, 0);

data.entry[E_KURS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KURS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KURS]), data.entry[E_KURS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KURS], "activate",G_CALLBACK(l_kv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KURS]),data.kurs.ravno());
gtk_widget_set_name(data.entry[E_KURS],iceb_u_inttochar(E_KURS));


label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);


data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),255);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_kv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));





sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[PFK2],"clicked",G_CALLBACK(l_kv_v_knopka),&data);
gtk_widget_set_name(data.knopka[PFK2],iceb_u_inttochar(PFK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[PFK10],"clicked",G_CALLBACK(l_kv_v_knopka),&data);
gtk_widget_set_name(data.knopka[PFK10],iceb_u_inttochar(PFK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 datak_start->new_plus(data.datak.ravno());
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_kv_v_e_knopka(GtkWidget *widget,class l_kv_v_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_kv_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_DATAK:
    if(iceb_calendar(&data->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->datak.ravno());

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_kv_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_kv_v_data *data)
{
//char  bros[512];

//printf("l_kv_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[PFK2],"clicked");

    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[PFK10],"clicked");

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
void  l_kv_v_knopka(GtkWidget *widget,class l_kv_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_kv_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    if(l_kv_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  


  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_kv_v_vvod(GtkWidget *widget,class l_kv_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_kv_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAK:
    data->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KURS:
    data->kurs.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int l_kv_zap(class l_kv_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);


if(data->datak.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена дата !"),data->window);
  return(1);
 }

if(data->datak.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
  return(1);
 }

if(iceb_u_SRAV(data->datak_start.ravno(),data->datak.ravno(),0) != 0)
  if(l_kv_pk(data) != 0)
     return(1);

if(data->datak_start.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Glkkv \
values ('%s','%s',%.10g,'%s',%d,%ld)",
   data->kod_val.ravno_filtr(),
   data->datak.ravno_sqldata(),
   data->kurs.ravno_atof(),
   data->koment.ravno_filtr(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Glkkv \
set \
datk='%s',\
kurs=%.10g,\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where kod='%s' and datk='%s'",
   data->datak.ravno_sqldata(),
   data->kurs.ravno_atof(),
   data->koment.ravno_filtr(),
   iceb_getuid(data->window),
   vrem,
   data->kod_val.ravno_filtr(),
   data->datak_start.ravno_sqldata());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_kv_pk(class l_kv_v_data *data)
{
char strsql[512];

sprintf(strsql,"select kod from Glkkv where kod='%s' and datk='%s'",data->kod_val.ravno(),data->datak.ravno_sqldata());
//printf("l_kv_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),data->window);
  return(1);
 }

return(0);

}
