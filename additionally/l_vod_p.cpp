/*$Id: l_vod_p.c,v 1.5 2013/08/13 06:10:25 sasa Exp $*/
/*30.05.2009	30.05.2009	Белых А.И.	l_vod_p.c
Меню для ввода реквизитов поиска 
*/
#include <stdlib.h>
#include "taxi.h"
#include "l_vod.h"
enum
 {
  E_KODVOD,
  E_FIO,
  E_ADRES,
  E_TELEF,
  E_GOSN,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_vod_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_vod_rek *rk;
  
  
  l_vod_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->kodvod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODVOD])));
    rk->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FIO])));
    rk->adres.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ADRES])));
    rk->telef.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TELEF])));
    rk->gosn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GOSN])));
   }

  void clear_data()
   {
    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void l_vod_p_clear(class l_vod_p_data *data);
void    l_vod_p_vvod(GtkWidget *widget,class l_vod_p_data *data);
void  l_vod_p_knopka(GtkWidget *widget,class l_vod_p_data *data);
gboolean   l_vod_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_vod_p_data *data);

extern char      *name_system;

int l_vod_p(class l_vod_rek *rek_poi,GtkWidget *wpredok)
{
class l_vod_p_data data;
char strsql[300];
data.rk=rek_poi;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_vod_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }



GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
 gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 0);


GtkWidget *label=gtk_label_new(gettext("Код"));
gtk_box_pack_start (GTK_BOX (hbox[E_KODVOD]), label, FALSE, FALSE, 0);

data.entry[E_KODVOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODVOD]), data.entry[E_KODVOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODVOD], "activate",G_CALLBACK(l_vod_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODVOD]),data.rk->kodvod.ravno());
gtk_widget_set_name(data.entry[E_KODVOD],iceb_u_inttochar(E_KODVOD));

label=gtk_label_new(gettext("Фамилия"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);

data.entry[E_FIO] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO], "activate",G_CALLBACK(l_vod_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rk->fio.ravno());
gtk_widget_set_name(data.entry[E_FIO],iceb_u_inttochar(E_FIO));

label=gtk_label_new(gettext("Адрес"));
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES]), label, FALSE, FALSE, 0);

data.entry[E_ADRES] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES]), data.entry[E_ADRES], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ADRES], "activate",G_CALLBACK(l_vod_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRES]),data.rk->adres.ravno());
gtk_widget_set_name(data.entry[E_ADRES],iceb_u_inttochar(E_ADRES));

label=gtk_label_new(gettext("Телефон"));
gtk_box_pack_start (GTK_BOX (hbox[E_TELEF]), label, FALSE, FALSE, 0);

data.entry[E_TELEF] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TELEF]), data.entry[E_TELEF], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TELEF], "activate",G_CALLBACK(l_vod_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TELEF]),data.rk->telef.ravno());
gtk_widget_set_name(data.entry[E_TELEF],iceb_u_inttochar(E_TELEF));


label=gtk_label_new(gettext("Гос. номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_GOSN]), label, FALSE, FALSE, 0);

data.entry[E_GOSN] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GOSN]), data.entry[E_GOSN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GOSN], "activate",G_CALLBACK(l_vod_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOSN]),data.rk->gosn.ravno());
gtk_widget_set_name(data.entry[E_GOSN],iceb_u_inttochar(E_GOSN));


sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_vod_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_vod_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_vod_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_vod_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_vod_p_data *data)
{
//char  bros[300];

//printf("vl_vod_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(FALSE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
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
void  l_vod_p_knopka(GtkWidget *widget,class l_vod_p_data *data)
{
//char bros[300];

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_vod_p_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_vod_p_vvod(GtkWidget *widget,class l_vod_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KODVOD:
    data->rk->kodvod.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_KODVOD])));
    break;
  case E_FIO:
    data->rk->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_FIO])));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
