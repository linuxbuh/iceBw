/*$Id: zar_eks_ko_v.c,v 1.17 2013/08/25 08:27:00 sasa Exp $*/
/*13.07.2015	20.10.2006	Белых А.И.	zar_eks_ko_v.c
Ввод реквизитов для ескпорта кассовых ордеров в подсистему "Учёт кассовых ордеров".
*/
#include "buhg_g.h"
#include "zar_eks_ko.h"
enum
 {
  E_KASSA,
  E_DATA,
  E_KODOP,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class zar_eks_ko_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *radiobutton2[3];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class zar_eks_ko_rek *rk;
  
  zar_eks_ko_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->kassa.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KASSA])));
    rk->data.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA])));
    rk->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP])));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[0])) == TRUE)
     rk->metka_r=0;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[1])) == TRUE)
     rk->metka_r=1;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[2])) == TRUE)
     rk->metka_r=2;
   }
 };


gboolean   zar_eks_ko_v_key_press(GtkWidget *widget,GdkEventKey *event,class zar_eks_ko_v_data *data);
void  zar_eks_ko_v_knopka(GtkWidget *widget,class zar_eks_ko_v_data *data);
void    zar_eks_ko_v_vvod(GtkWidget *widget,class zar_eks_ko_v_data *data);
void  zar_eks_ko_v_e_knopka(GtkWidget *widget,class zar_eks_ko_v_data *data);

int zar_eks_ko_prov(class zar_eks_ko_v_data *data);

extern SQL_baza  bd;

int zar_eks_ko_v(class zar_eks_ko_rek *rek,GtkWidget *wpredok)
{

class zar_eks_ko_v_data data;
char strsql[512];
data.rk=rek;
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Экспорт кассовых оредров в подсистему \"Учёт кассовых ордеров\""));
label=gtk_label_new(gettext("Экспорт кассовых оредров в подсистему \"Учёт кассовых ордеров\""));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_eks_ko_v_key_press),&data);

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

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator3=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator1);

data.radiobutton2[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Кассовые ордера по подразделениям"));
data.radiobutton2[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[0]),gettext("Кассовые ордера по каждому человеку отдельно"));
data.radiobutton2[2]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[1]),gettext("Один кассовый ордер на всех"));

gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[1], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[2], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk->metka_r]),TRUE); //Устанавливем активной кнопку

gtk_container_add (GTK_CONTAINER (vbox), separator3);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Код кассы"));
data.knopka_enter[E_KASSA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.knopka_enter[E_KASSA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KASSA],"clicked",G_CALLBACK(zar_eks_ko_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KASSA],iceb_u_inttochar(E_KASSA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KASSA],gettext("Выбор кассы"));

data.entry[E_KASSA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KASSA]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.entry[E_KASSA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KASSA], "activate",G_CALLBACK(zar_eks_ko_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KASSA]),data.rk->kassa.ravno());
gtk_widget_set_name(data.entry[E_KASSA],iceb_u_inttochar(E_KASSA));

sprintf(strsql,"%s",gettext("Дата"));
data.knopka_enter[E_DATA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.knopka_enter[E_DATA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATA],"clicked",G_CALLBACK(zar_eks_ko_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA],iceb_u_inttochar(E_DATA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA],gettext("Выбор даты"));

data.entry[E_DATA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATA], "activate",G_CALLBACK(zar_eks_ko_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.rk->data.ravno());
gtk_widget_set_name(data.entry[E_DATA],iceb_u_inttochar(E_DATA));


sprintf(strsql,"%s",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODOP],"clicked",G_CALLBACK(zar_eks_ko_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODOP],iceb_u_inttochar(E_KODOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODOP],gettext("Выбор операции"));

data.entry[E_KODOP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODOP]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(zar_eks_ko_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));


sprintf(strsql,"F2 %s",gettext("Экспорт"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[PFK2],"clicked",G_CALLBACK(zar_eks_ko_v_knopka),&data);
gtk_widget_set_name(data.knopka[PFK2],iceb_u_inttochar(PFK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[PFK10],"clicked",G_CALLBACK(zar_eks_ko_v_knopka),&data);
gtk_widget_set_name(data.knopka[PFK10],iceb_u_inttochar(PFK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

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
void  zar_eks_ko_v_e_knopka(GtkWidget *widget,class zar_eks_ko_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("zar_eks_ko_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATA:

    if(iceb_calendar(&data->rk->data,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA]),data->rk->data.ravno());
      
    return;  

  case E_KASSA:

    if(l_spis_kas(1,&kod,&naim,data->window) == 0)
      data->rk->kassa.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KASSA]),data->rk->kassa.ravno());
      
    return;  

  case E_KODOP:
    if(data->rk->prn == 1)
     if(l_kasopp(1,&kod,&naim,data->window) == 0)
       data->rk->kodop.new_plus(kod.ravno());
    if(data->rk->prn == 2)
     if(l_kasopr(1,&kod,&naim,data->window) == 0)
       data->rk->kodop.new_plus(kod.ravno());

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk->kodop.ravno());
      
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_eks_ko_v_key_press(GtkWidget *widget,GdkEventKey *event,class zar_eks_ko_v_data *data)
{
//char  bros[512];

//printf("zar_eks_ko_v_key_press\n");
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
void  zar_eks_ko_v_knopka(GtkWidget *widget,class zar_eks_ko_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("zar_eks_ko_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
  
  if(zar_eks_ko_prov(data) != 0)
   return;
  data->voz=0;
    gtk_widget_destroy(data->window);
          
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

void    zar_eks_ko_v_vvod(GtkWidget *widget,class zar_eks_ko_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("zar_eks_ko_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KASSA:
    data->rk->kassa.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATA:
    data->rk->data.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KODOP:
    data->rk->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/*******************************/
/*Проверка правильности ввода*/
/****************************/
int zar_eks_ko_prov(class zar_eks_ko_v_data *data)
{
char strsql[512];
//проверка кода кассы
sprintf(strsql,"select naik from Kas where kod=%d",data->rk->kassa.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код кассы"),data->rk->kassa.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
//проверка даты
if(data->rk->data.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата !"),data->window);
  return(1);
  
 }
SQL_str row;
class SQLCURSOR	cur;
//проверка кода операции
if(data->rk->prn == 1)
  sprintf(strsql,"select naik,shetkor from Kasop1 where kod='%s'",data->rk->kodop.ravno());
if(data->rk->prn == 2)
  sprintf(strsql,"select naik,shetkor from Kasop2 where kod='%s'",data->rk->kodop.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код операции"),data->rk->kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
data->rk->naimop.new_plus(row[0]);
data->rk->shetk.new_plus(row[1]);
return(0);
}
