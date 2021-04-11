/*$Id: shetsrs_m.c,v 1.1 2013/09/26 09:46:55 sasa Exp $*/
/*28.04.2016	01.03.2009	Белых А.И.	shetsrs_m.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "go.h"

enum
 {
  PFK2,
  PFK10,
  KOL_PFK
 };

enum
 {
  E_KODGR,
  KOLENTER  
 };

class shetsrs_m_data
 {
  public:
    
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *radiobutton1[2];
  GtkWidget *radiobutton2[4];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class go_rr *rk;
  class iceb_u_spisok sp_val;
  
  
  shetsrs_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->kodgr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODGR])));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[0])) == TRUE)
     rk->metka_oth=0;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[1])) == TRUE)
     rk->metka_oth=1;

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[0])) == TRUE)
     rk->metkasort=0;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[1])) == TRUE)
     rk->metkasort=1;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[2])) == TRUE)
     rk->metkasort=2;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[3])) == TRUE)
     rk->metkasort=3;

   }

 };

void shetsrs_m_clear(class shetsrs_m_data *data);
void  shetsrs_m_knopka(GtkWidget *widget,class shetsrs_m_data *data);
gboolean   shetsrs_m_key_press(GtkWidget *widget,GdkEventKey *event,class shetsrs_m_data *data);
void  shetsrs_m_v_e_knopka(GtkWidget *widget,class shetsrs_m_data *data);
void  shetsrs_m_v_vvod(GtkWidget *widget,class shetsrs_m_data *data);

extern SQL_baza bd;

int shetsrs_m(class go_rr *rek_r,GtkWidget *wpredok)
{
class shetsrs_m_data data;
char strsql[1024];
data.rk=rek_r;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(shetsrs_m_key_press),&data);

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

sprintf(strsql,"%s\n%s %s\n",gettext("Расчёт по счёту с развёрнутым сальдо"),rek_r->shet.ravno(),rek_r->naimshet.ravno());


GtkWidget *label=gtk_label_new(strsql);

GtkWidget *hbox_kodgr = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox_kodgr),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);

//for(int i=0; i < KOLENTER; i++)
gtk_box_pack_start (GTK_BOX (vbox),hbox_kodgr, TRUE, TRUE, 0);

sprintf(strsql,"%s (,,)",gettext("Код группы контрагента"));
data.knopka_enter[E_KODGR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox_kodgr), data.knopka_enter[E_KODGR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODGR],"clicked",G_CALLBACK(shetsrs_m_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODGR],iceb_u_inttochar(E_KODGR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODGR],gettext("Выбор группы контрагента"));

data.entry[E_KODGR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox_kodgr), data.entry[E_KODGR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODGR], "activate",G_CALLBACK(shetsrs_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODGR]),data.rk->kodgr.ravno());
gtk_widget_set_name(data.entry[E_KODGR],iceb_u_inttochar(E_KODGR));


GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator2=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

//Вставляем радиокнопки

data.radiobutton1[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Расчёт журнала-ордера и оборотного баланса по счёту"));
data.radiobutton1[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton1[0]),gettext("Распечатка проводок по контрагентам"));

gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton1[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton1[1], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[data.rk->metka_oth]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox),separator1, TRUE, TRUE, 2);


data.radiobutton2[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Сортировка по коду контрагента в символическом виде"));
data.radiobutton2[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[0]),gettext("Сортировка по коду контрагента в цифровом виде"));
data.radiobutton2[2]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[1]),gettext("Сортировка по наименованию контрагента"));
data.radiobutton2[3]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[2]),gettext("Сортировка по группе контрагента"));

gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[1], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[2], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[3], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk->metkasort]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox),separator2, TRUE, TRUE, 2);

GtkWidget *hbox_val = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox_val),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start (GTK_BOX (vbox),hbox_val, TRUE, TRUE, 0);


GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);

sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[PFK2],"clicked",G_CALLBACK(shetsrs_m_knopka),&data);
gtk_widget_set_name(data.knopka[PFK2],iceb_u_inttochar(PFK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK10],gettext("Отказатся от выполнения расчёта"));
g_signal_connect(data.knopka[PFK10],"clicked",G_CALLBACK(shetsrs_m_knopka),&data);
gtk_widget_set_name(data.knopka[PFK10],iceb_u_inttochar(PFK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.knopka[PFK2]);

gtk_widget_show_all (data.window);

gtk_main();

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  shetsrs_m_v_e_knopka(GtkWidget *widget,class shetsrs_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("go_v_e_knopka knop=%d\n",knop);
class iceb_u_str naim("");
switch (knop)
 {

  case E_KODGR:
    iceb_vibrek(0,"Gkont",&data->rk->kodgr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGR]),data->rk->kodgr.ravno());
    return;

 }
}
/***************************************/
void shetsrs_m_v_vvod(GtkWidget *widget,class shetsrs_m_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("go_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KODGR:
    data->rk->kodgr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   shetsrs_m_key_press(GtkWidget *widget,GdkEventKey *event,class shetsrs_m_data *data)
{
//char  bros[512];

//printf("vl_uosdm_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[PFK2],"clicked");
    return(FALSE);


    
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
void  shetsrs_m_knopka(GtkWidget *widget,class shetsrs_m_data *data)
{
//char bros[512];

int knop=atoi(gtk_widget_get_name(widget));
//g_print("shetsrs_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
