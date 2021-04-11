/*$Id: iceb_l_plansh_vksb.c,v 1.14 2013/08/25 08:27:06 sasa Exp $*/
/*03.12.2014	29.04.2007	Белых А.И.	iceb_l_plansh_vksb.c
Меню для ввода код суббаланса
*/
#include "iceb_libbuh.h"
enum
 {
  E_KOD_SUBBAL,
  KOLENTER  
 };

enum
 {
  FK2,
  FK10,
  KOL_FK
 };

class iceb_l_plansh_vksb_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str kod_subbal;
        
  iceb_l_plansh_vksb_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    kod_subbal.plus("");
   }

  void read_rek()
   {
    kod_subbal.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_SUBBAL])));
   }
  
 };


gboolean   iceb_l_plansh_vksb_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_plansh_vksb_data *data);
void  iceb_l_plansh_vksb_knopka(GtkWidget *widget,class iceb_l_plansh_vksb_data *data);
void    iceb_l_plansh_vksb_vvod(GtkWidget *widget,class iceb_l_plansh_vksb_data *data);


void  iceb_l_plansh_vksb_e_knopka(GtkWidget *widget,class iceb_l_plansh_vksb_data *data);

extern SQL_baza  bd;

int iceb_l_plansh_vksb(class iceb_u_str *kod_subbal,char *nadpis,GtkWidget *wpredok)
{
class iceb_l_plansh_vksb_data data;

if(kod_subbal->getdlinna() > 1)
 data.kod_subbal.new_plus(kod_subbal->ravno());

char strsql[512];
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод кода суббаланса"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

sprintf(strsql,"%s\n%s",gettext("Ввод кода суббаланса"),nadpis);

label=gtk_label_new(nadpis);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_l_plansh_vksb_key_press),&data);

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




sprintf(strsql,"%s",gettext("Код суббаланса"));
data.knopka_enter[E_KOD_SUBBAL]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_SUBBAL]), data.knopka_enter[E_KOD_SUBBAL], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_SUBBAL],"clicked",G_CALLBACK(iceb_l_plansh_vksb_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_SUBBAL],iceb_u_inttochar(E_KOD_SUBBAL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_SUBBAL],gettext("Выбор кода суббаланса"));

data.entry[E_KOD_SUBBAL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_SUBBAL]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_SUBBAL]), data.entry[E_KOD_SUBBAL], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_KOD_SUBBAL], "activate",G_CALLBACK(iceb_l_plansh_vksb_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_SUBBAL]),data.kod_subbal.ravno());
gtk_widget_set_name(data.entry[E_KOD_SUBBAL],iceb_u_inttochar(E_KOD_SUBBAL));





sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Записать введённую в меню информацию"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(iceb_l_plansh_vksb_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(iceb_l_plansh_vksb_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 kod_subbal->new_plus(data.kod_subbal.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  iceb_l_plansh_vksb_e_knopka(GtkWidget *widget,class iceb_l_plansh_vksb_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_KOD_SUBBAL:
    if(iceb_l_subbal(1,&kod,&naim,data->window) == 0)
     data->kod_subbal.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_SUBBAL]),data->kod_subbal.ravno());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_l_plansh_vksb_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_plansh_vksb_data *data)
{
//char  bros[512];

//printf("iceb_l_plansh_vksb_key_press\n");
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
void  iceb_l_plansh_vksb_knopka(GtkWidget *widget,class iceb_l_plansh_vksb_data *data)
{
char strsql[500];
int knop=atoi(gtk_widget_get_name(widget));
//g_print("iceb_l_plansh_vksb_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:

    data->read_rek();
    if(data->kod_subbal.getdlinna() > 1)
     {
      sprintf(strsql,"select kod from Glksubbal where kod=%d",data->kod_subbal.ravno_atoi());
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        sprintf(strsql,"%s %d !",gettext("Не найден код суббаланса"),data->kod_subbal.ravno_atoi());
        iceb_menu_soob(strsql,data->window);
        return;
       }
     }

    data->voz=0;
    gtk_widget_destroy(data->window);
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

void    iceb_l_plansh_vksb_vvod(GtkWidget *widget,class iceb_l_plansh_vksb_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("iceb_l_plansh_vksb_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD_SUBBAL:
    data->kod_subbal.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
