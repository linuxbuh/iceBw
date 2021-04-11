/*$Id: l_vrint_k.c,v 1.9 2013/08/13 06:10:25 sasa Exp $*/
/*04.01.2007	01.03.2004	Белых А.И.	l_vrint_k.c
Ввод дат для копирования записей
*/
#include <time.h>
#include <unistd.h>
#include "i_rest.h"
//#include "klient.h"

enum
{
 FK2,
// FK4,
 FK10,
 KOL_F_KL
};

enum
{
 E_DATAZ,
 E_DATAN,
 KOLENTER
};


class l_vrint_k_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *opt;
  short     kl_shift; //0-отжата 1-нажата  

  int       voz;
  class iceb_u_str dataz;
  class iceb_u_str datan;
       
  l_vrint_k_data()
   {
    voz=0;
    kl_shift=0;
    dataz.plus("");
    datan.plus("");
    
   }
  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
  
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };


gboolean   l_vrint_k_key_press(GtkWidget *widget,GdkEventKey *event,class l_vrint_k_data *data);
void  l_vrint_k_knopka(GtkWidget *widget,class l_vrint_k_data *data);
void    l_vrint_k_vvod(GtkWidget *widget,class l_vrint_k_data *data);
int l_vrint_zap(class l_vrint_k_data *data);
void l_vrint_kodgr(class l_vrint_k_data *data);
void  l_vrint_k_e_knopka(GtkWidget *widget,class l_vrint_k_data *data);

extern SQL_baza	bd;
extern char *name_system;


int l_vrint_k(class iceb_u_str *dataz,class iceb_u_str *datan,GtkWidget *wpredok)
{
char strsql[300];
class l_vrint_k_data data;
data.dataz.new_plus(dataz->ravno());
data.datan.new_plus(datan->ravno());
    
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
//g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_widget_destroyed),&data.window);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_vrint_k_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label;
sprintf(strsql,"%s %s",name_system,gettext("Копировать записи по выбранной дате на другую дату"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

label=gtk_label_new(gettext("Копировать записи по выбранной дате на другую дату"));


GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER+1];
for(int i=0 ; i < KOLENTER+1; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
//gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 1);
for(int i=0 ; i < KOLENTER+1; i++)
 gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE, 1);
// gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

//gtk_container_add (GTK_CONTAINER (vbox), hboxknop);
gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 1);


data.knopka_enter[E_DATAZ]=gtk_button_new_with_label(gettext("Дата с которой нужно копировать"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAZ]), data.knopka_enter[E_DATAZ], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAZ],"clicked",G_CALLBACK(l_vrint_k_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAZ],iceb_u_inttochar(E_DATAZ));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAZ],gettext("Выбор даты"));


data.entry[E_DATAZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAZ]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAZ]), data.entry[E_DATAZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAZ], "activate",G_CALLBACK(l_vrint_k_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAZ]),data.dataz.ravno());
gtk_widget_set_name(data.entry[E_DATAZ],iceb_u_inttochar(E_DATAZ));

data.knopka_enter[E_DATAN]=gtk_button_new_with_label(gettext("Дата на которую нужно копировать"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(l_vrint_k_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты"));


data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(l_vrint_k_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));




sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_vrint_k_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_vrint_k_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

if(data.dataz.getdlinna() <= 1)
  gtk_widget_grab_focus(data.entry[0]);
else
  gtk_widget_grab_focus(data.entry[1]);
gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 {
  datan->new_plus(data.datan.ravno());
  dataz->new_plus(data.dataz.ravno());
 }
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_vrint_k_e_knopka(GtkWidget *widget,class l_vrint_k_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case E_DATAN:
    iceb_calendar(&data->datan,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->datan.ravno());


    return;  

  case E_DATAZ:
    iceb_calendar(&data->dataz,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAZ]),data->dataz.ravno());


    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_vrint_k_key_press(GtkWidget *widget,GdkEventKey *event,class l_vrint_k_data *data)
{

//printf("l_vrint_k_key_press\n");
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
void  l_vrint_k_knopka(GtkWidget *widget,class l_vrint_k_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_vrint_k_knopka knop=%d\n",knop);

switch ((gint)knop)
 {
  case FK2:
    data->read_rek();
    if(data->dataz.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно ввели дату с которой нужно копировать !"),data->window);
      return;
     }
    if(data->datan.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно ввели дату на которую нужно копировать !"),data->window);
      return;
     }
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_vrint_k_vvod(GtkWidget *widget,class l_vrint_k_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_vrint_k_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAZ:
    data->dataz.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATAN:
    data->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
