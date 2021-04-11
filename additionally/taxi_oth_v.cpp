/*$Id: taxi_oth_v.c,v 1.32 2013/12/31 11:49:23 sasa Exp $*/
/*09.12.2013	08.12.2003	Белых А.И.	taxi_oth_v.c
Меню ввода данных для получения отчёта
*/
#include        <time.h>
#include        <stdlib.h>
#include        <unistd.h>
#include "taxi.h"
#include "taxi_oth.h"

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };
 
void  taxi_oth_v_knopka(GtkWidget *widget,class taxi_oth_data *data);
gboolean   taxi_oth_v_key_press(GtkWidget *widget,GdkEventKey *event,class taxi_oth_data *data);
void    taxi_oth_v_vvod(GtkWidget *widget,class taxi_oth_data *data);
int    taxi_oth_prov(class taxi_oth_data *data);
void taxi_oth_clear(class taxi_oth_data *data);

void       taxi_oth_v_radio0(GtkWidget *,class taxi_oth_data *);
void       taxi_oth_v_radio1(GtkWidget *,class taxi_oth_data *);

void  taxi_oth_v_v_e_knopka(GtkWidget *widget,class taxi_oth_data *data);

//extern SQL_baza	bd;
extern char *name_system;

int taxi_oth_v(class taxi_oth_data *data)
{
char strsql[300];

data->window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data->window),ICEB_POS_CENTER);
//gtk_window_set_modal( GTK_WINDOW(data->window) ,TRUE ); 

sprintf(strsql,"%s %s",NAME_SYSTEM,"Получение отчётов");
gtk_window_set_title(GTK_WINDOW(data->window),strsql);
g_signal_connect(data->window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data->window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data->window,"key_press_event",G_CALLBACK(taxi_oth_v_key_press),data);


GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
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
GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *hboxradio = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxradio),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data->window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox5);
gtk_container_add (GTK_CONTAINER (vbox), separator);
gtk_container_add (GTK_CONTAINER (vbox), hboxradio);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);



//Вставляем радиокнопки
GSList *group;

data->radiobutton0=gtk_radio_button_new_with_label(NULL,"По времени записи заказа");
gtk_box_pack_start (GTK_BOX (hboxradio),data->radiobutton0, TRUE, TRUE, 0);
g_signal_connect(data->radiobutton0, "clicked",G_CALLBACK(taxi_oth_v_radio0),data);
//gtk_widget_set_name(data->radiobutton0,iceb_u_inttochar("0"));

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data->radiobutton0));


data->radiobutton1=gtk_radio_button_new_with_label(group,"По дате заказа");
//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data->radiobutton3),TRUE); //Устанавливем активной кнопку
g_signal_connect(data->radiobutton1, "clicked",G_CALLBACK(taxi_oth_v_radio1),data);
gtk_box_pack_start (GTK_BOX (hboxradio),data->radiobutton1, TRUE, TRUE, 0);

GtkWidget *label;

data->knopka_enter[E_DATAN]=gtk_button_new_with_label("Дата начала периода");
gtk_box_pack_start (GTK_BOX (hbox1), data->knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data->knopka_enter[E_DATAN],"clicked",G_CALLBACK(taxi_oth_v_v_e_knopka),data);
gtk_widget_set_name(data->knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data->knopka_enter[E_DATAN],"Выбор даты в календаре");

//gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);

data->entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data->entry[E_DATAN]),11);
data->entry[E_VREMN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data->entry[E_VREMN]),9);
gtk_box_pack_start (GTK_BOX (hbox1), data->entry[E_DATAN], TRUE, TRUE, 0);

label=gtk_label_new("Время начала периода");
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data->entry[E_VREMN], TRUE, TRUE, 0);

g_signal_connect(data->entry[E_DATAN], "activate",G_CALLBACK(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->datn.ravno());
gtk_widget_set_name(data->entry[E_DATAN],iceb_u_inttochar(E_DATAN));

g_signal_connect(data->entry[E_VREMN], "activate",G_CALLBACK(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMN]),data->vremn.ravno());
gtk_widget_set_name(data->entry[E_VREMN],iceb_u_inttochar(E_VREMN));

data->knopka_enter[E_DATAK]=gtk_button_new_with_label("Дата конца периода");
gtk_box_pack_start (GTK_BOX (hbox2), data->knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data->knopka_enter[E_DATAK],"clicked",G_CALLBACK(taxi_oth_v_v_e_knopka),data);
gtk_widget_set_name(data->knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data->knopka_enter[E_DATAK],"Выбор даты в календаре");
//label=gtk_label_new("Дата конца периода");
//gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);

data->entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data->entry[E_DATAK]),11);
data->entry[E_VREMK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data->entry[E_VREMK]),9);
gtk_box_pack_start (GTK_BOX (hbox2), data->entry[E_DATAK], TRUE, TRUE, 0);
label=gtk_label_new("Время конца периода");
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data->entry[E_VREMK], TRUE, TRUE, 0);

g_signal_connect(data->entry[E_DATAK], "activate",G_CALLBACK(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->datk.ravno());
gtk_widget_set_name(data->entry[E_DATAK],iceb_u_inttochar(E_DATAK));

g_signal_connect(data->entry[E_VREMK], "activate",G_CALLBACK(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMK]),data->vremk.ravno());
gtk_widget_set_name(data->entry[E_VREMK],iceb_u_inttochar(E_VREMK));

data->knopka_enter[E_KODVOD]=gtk_button_new_with_label("Код водителя (,,");
gtk_box_pack_start (GTK_BOX (hbox3), data->knopka_enter[E_KODVOD], FALSE, FALSE, 0);
g_signal_connect(data->knopka_enter[E_KODVOD],"clicked",G_CALLBACK(taxi_oth_v_v_e_knopka),data);
gtk_widget_set_name(data->knopka_enter[E_KODVOD],iceb_u_inttochar(E_KODVOD));
gtk_widget_set_tooltip_text(data->knopka_enter[E_KODVOD],"Выбор кода водителя в списке");

//label=gtk_label_new("Код водителя (,,"));
//gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);

data->entry[E_KODVOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox3), data->entry[E_KODVOD], TRUE, TRUE, 0);
g_signal_connect(data->entry[E_KODVOD], "activate",G_CALLBACK(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODVOD]),data->kodvod.ravno());
gtk_widget_set_name(data->entry[E_KODVOD],iceb_u_inttochar(E_KODVOD));

label=gtk_label_new("Код оператора (,,");
data->entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data->entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data->entry[E_KODOP], "activate",G_CALLBACK(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->kodop.ravno());
gtk_widget_set_name(data->entry[E_KODOP],iceb_u_inttochar(E_KODOP));

data->knopka_enter[E_KODZAV]=gtk_button_new_with_label("Код завершения (,,");
gtk_box_pack_start (GTK_BOX (hbox5), data->knopka_enter[E_KODZAV], FALSE, FALSE, 0);
g_signal_connect(data->knopka_enter[E_KODZAV],"clicked",G_CALLBACK(taxi_oth_v_v_e_knopka),data);
gtk_widget_set_name(data->knopka_enter[E_KODZAV],iceb_u_inttochar(E_KODZAV));
gtk_widget_set_tooltip_text(data->knopka_enter[E_KODZAV],"Выбор кода завершения в списке");

data->entry[E_KODZAV] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox5), data->entry[E_KODZAV], TRUE, TRUE, 0);
g_signal_connect(data->entry[E_KODZAV], "activate",G_CALLBACK(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODZAV]),data->kodzav.ravno());
gtk_widget_set_name(data->entry[E_KODZAV],iceb_u_inttochar(E_KODZAV));


data->knopka_enter[E_KODKLIENTA]=gtk_button_new_with_label("Код клиента (,,");
gtk_box_pack_start (GTK_BOX (hbox5), data->knopka_enter[E_KODKLIENTA], FALSE, FALSE, 0);
g_signal_connect(data->knopka_enter[E_KODKLIENTA],"clicked",G_CALLBACK(taxi_oth_v_v_e_knopka),data);
gtk_widget_set_name(data->knopka_enter[E_KODKLIENTA],iceb_u_inttochar(E_KODKLIENTA));
gtk_widget_set_tooltip_text(data->knopka_enter[E_KODKLIENTA],"Выбор клиента в списке клиентов");


data->entry[E_KODKLIENTA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox5), data->entry[E_KODKLIENTA], TRUE, TRUE, 0);
g_signal_connect(data->entry[E_KODKLIENTA], "activate",G_CALLBACK(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKLIENTA]),data->kodklienta.ravno());
gtk_widget_set_name(data->entry[E_KODKLIENTA],iceb_u_inttochar(E_KODKLIENTA));


GtkWidget *knopka[KOL_PFK];

sprintf(strsql,"F2 %s","Расчет");
knopka[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(knopka[PFK2],"Начать расчет отчёта");
g_signal_connect(knopka[PFK2],"clicked",G_CALLBACK(taxi_oth_v_knopka),data);
gtk_widget_set_name(knopka[PFK2],iceb_u_inttochar(PFK2));

sprintf(strsql,"F4 %s","Очистить");
knopka[PFK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(knopka[PFK4],"Очистить меню от введенной информации");
g_signal_connect(knopka[PFK4],"clicked",G_CALLBACK(taxi_oth_v_knopka),data);
gtk_widget_set_name(knopka[PFK4],iceb_u_inttochar(PFK4));

sprintf(strsql,"F10 %s","Выход");
knopka[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(knopka[PFK10],"Выйти из меню без получения отчёта");
g_signal_connect(knopka[PFK10],"clicked",G_CALLBACK(taxi_oth_v_knopka),data);
gtk_widget_set_name(knopka[PFK10],iceb_u_inttochar(PFK10));

gtk_box_pack_start(GTK_BOX(hboxknop), knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), knopka[PFK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), knopka[PFK10], TRUE, TRUE, 0);


gtk_widget_grab_focus(data->entry[0]);

gtk_widget_show_all(data->window);
gtk_main();

return(data->metka_voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  taxi_oth_v_v_e_knopka(GtkWidget *widget,class taxi_oth_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->datn,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->datn.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->datk,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->datk.ravno());
      
    return;  

  case E_KODKLIENTA:
    if(l_taxisk(1,&kod,&naim,data->window) == 0)
     {
      data->kodklienta.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKLIENTA]),data->kodklienta.ravno());
     }
    return;  

  case E_KODVOD:
    if(l_vod(1,&kod,&naim,data->window) == 0)
     {
      data->kodvod.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODVOD]),data->kodvod.ravno());
     }
    return;  

  case E_KODZAV:
    if(l_taxikz(1,&kod,&naim,data->window) == 0)
     {
      data->kodzav.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODZAV]),data->kodzav.ravno());
     }
    return;  

 }
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  taxi_oth_v_knopka(GtkWidget *widget,class taxi_oth_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case PFK2:
    taxi_oth_prov(data);
    data->metka_voz=1;
    return;  

  case PFK4:
    taxi_oth_clear(data);
    return;
   
  case PFK10:
    gtk_widget_destroy(data->window);
    data->metka_voz=0;
    return;
 }
}
 
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   taxi_oth_v_key_press(GtkWidget *widget,GdkEventKey *event,class taxi_oth_data *data)
{
//char  bros[300];

//printf("vzaktaxi_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    taxi_oth_prov(data);
    data->metka_voz=1;
    return(TRUE);
    
  case GDK_KEY_F4:
    taxi_oth_clear(data);
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    gtk_widget_destroy(data->window);
    data->metka_voz=0;

    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

//    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    taxi_oth_v_vvod(GtkWidget *widget,class taxi_oth_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("kontr_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->datn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_VREMN:
    data->vremn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->datk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_VREMK:
    data->vremk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODVOD:
    data->kodvod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODOP:
    data->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODZAV:
    data->kodzav.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODKLIENTA:
    data->kodklienta.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/************************************/
/*Чтение введенной информации и проверка на ошибки*/
/**************************************************/
//Если вернули 0 - все впорядке

int    taxi_oth_prov(class taxi_oth_data *data)
{
for(int i=0; i < KOLENTER; i++)
  g_signal_emit_by_name(data->entry[i],"activate");

short d,m,g;

if(iceb_u_rsdat(&d,&m,&g,data->datn.ravno(),1) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не верно введена дата начала !"));
  iceb_menu_soob(&repl,NULL);
  return(1);
 }

if(iceb_u_rsdat(&d,&m,&g,data->datk.ravno(),1) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не верно введена дата конца !"));
  iceb_menu_soob(&repl,NULL);
  return(1);
 }

if(iceb_u_rstime(&d,&m,&g,data->vremn.ravno()) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не верно ввели время начала !"));
  iceb_menu_soob(&repl,NULL);
  return(1);
 }

if(iceb_u_rstime(&d,&m,&g,data->vremk.ravno()) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не верно ввели время конца !"));
  iceb_menu_soob(&repl,NULL);
  return(1);
 }

gtk_widget_destroy(data->window);

return(0);

}
/****************************/
/*Очистка меню              */
/*****************************/

void taxi_oth_clear(class taxi_oth_data *data)
{

data->clear_zero();

gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->datn.ravno());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->datk.ravno());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMN]),data->vremn.ravno());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMK]),data->vremk.ravno());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->kodop.ravno());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODVOD]),data->kodvod.ravno());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKLIENTA]),data->kodklienta.ravno());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODZAV]),data->kodzav.ravno());

gtk_widget_grab_focus(data->entry[0]);

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  taxi_oth_v_radio0(GtkWidget *widget,class taxi_oth_data *data)
{
//g_print("taxi_oth_v_radio0\n");
/*
int knop=atoi(gtk_widget_get_name(widget));
//g_print("taxi_oth_v_radio knop=%s\n",knop);

data->metkarr=atoi(knop);
*/
if(data->metkarr == 0)
  return;
data->metkarr=0;

}
void  taxi_oth_v_radio1(GtkWidget *widget,class taxi_oth_data *data)
{
//g_print("taxi_oth_v_radio1\n");
if(data->metkarr == 1)
  return;
data->metkarr=1;
}
