/*$Id: iceb_menu_mes_god.c,v 1.15 2013/08/25 08:27:06 sasa Exp $*/
/*02.12.2014	05.12.2006	Белых А.И.	iceb_menu_mes_god.c
Меню для ввода даты в формате "месяц.год"
*/
#include "iceb_libbuh.h"
enum
 {
  E_MES_GOD,
  KOLENTER  
 };

enum
 {
  FK2,
  FK5,
  FK10,
  KOL_FK
 };

class iceb_menu_mes_god_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str mes_god;
  class iceb_u_str imaf_nast;
        
  iceb_menu_mes_god_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    imaf_nast.plus("");
   }

  void read_rek()
   {
    mes_god.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MES_GOD])));
   }
  
 };


gboolean   iceb_menu_mes_god_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_menu_mes_god_data *data);
void  iceb_menu_mes_god_knopka(GtkWidget *widget,class iceb_menu_mes_god_data *data);
void    iceb_menu_mes_god_vvod(GtkWidget *widget,class iceb_menu_mes_god_data *data);


void  iceb_menu_mes_god_e_knopka(GtkWidget *widget,class iceb_menu_mes_god_data *data);

extern SQL_baza  bd;

int iceb_menu_mes_god(class iceb_u_str *mes_god,const char *nadpis,const char *imaf_nast,GtkWidget *wpredok)
{
class iceb_menu_mes_god_data data;
data.imaf_nast.new_plus(imaf_nast);

if(mes_god->getdlinna() > 1)
 data.mes_god.new_plus(mes_god->ravno());
else
 {
  data.mes_god.poltekdat1();
 }

char strsql[512];
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),nadpis);
label=gtk_label_new(nadpis);

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_menu_mes_god_key_press),&data);

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
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 2);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE, 2);



gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 2);




sprintf(strsql,"%s %s",gettext("Введите дату"),gettext("(м.г)"));
data.knopka_enter[E_MES_GOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_MES_GOD]), data.knopka_enter[E_MES_GOD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_MES_GOD],"clicked",G_CALLBACK(iceb_menu_mes_god_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_MES_GOD],iceb_u_inttochar(E_MES_GOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_MES_GOD],gettext("Выбор даты."));

data.entry[E_MES_GOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MES_GOD]),7);
gtk_box_pack_start (GTK_BOX (hbox[E_MES_GOD]), data.entry[E_MES_GOD], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_MES_GOD], "activate",G_CALLBACK(iceb_menu_mes_god_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MES_GOD]),data.mes_god.ravno());
gtk_widget_set_name(data.entry[E_MES_GOD],iceb_u_inttochar(E_MES_GOD));





sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(iceb_menu_mes_god_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


if(data.imaf_nast.getdlinna() > 1)
 {
  sprintf(strsql,"F5 %s",gettext("Настройка"));
  data.knopka[FK5]=gtk_button_new_with_label(strsql);
  gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Настройка расчёта"));
  g_signal_connect(data.knopka[FK5],"clicked",G_CALLBACK(iceb_menu_mes_god_knopka),&data);
  gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
  gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK5], TRUE, TRUE, 0);
 }

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(iceb_menu_mes_god_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 mes_god->new_plus(data.mes_god.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  iceb_menu_mes_god_e_knopka(GtkWidget *widget,class iceb_menu_mes_god_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_MES_GOD:
    iceb_calendar1(&data->mes_god,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_MES_GOD]),data->mes_god.ravno());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_menu_mes_god_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_menu_mes_god_data *data)
{
//char  bros[512];

//printf("iceb_menu_mes_god_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");

    return(TRUE);

  case GDK_KEY_F5:
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
void  iceb_menu_mes_god_knopka(GtkWidget *widget,class iceb_menu_mes_god_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("iceb_menu_mes_god_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:

    data->read_rek();
    if(data->mes_god.prov_dat1() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата  !"),data->window);
      return;
     }

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK5:
    iceb_f_redfil(data->imaf_nast.ravno(),0,data->window);
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

void    iceb_menu_mes_god_vvod(GtkWidget *widget,class iceb_menu_mes_god_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("iceb_menu_mes_god_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_MES_GOD:
    data->mes_god.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    break;

 }
/***********
enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
**************/ 
}
