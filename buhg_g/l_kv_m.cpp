/*$Id: l_kv_m.c,v 1.1 2013/09/26 09:48:37 sasa Exp $*/
/*05.09.2013	05.09.2013	Белых А.И.	l_kv_m.c
Меню для ввода кода валюты
*/
#include "buhg_g.h"

enum
 {
  E_KOD_VAL,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class l_kv_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str kod_val;  
  l_kv_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    kod_val.plus("");
   }

  void read_rek()
   {
    kod_val.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_VAL])));
   }

  void clear_data()
   {
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void l_kv_m_clear(class l_kv_m_data *data);
void    l_kv_m_vvod(GtkWidget *widget,class l_kv_m_data *data);
void  l_kv_m_knopka(GtkWidget *widget,class l_kv_m_data *data);
void  l_kv_m_e_knopka(GtkWidget *widget,class l_kv_m_data *data);
gboolean   l_kv_m_key_press(GtkWidget *widget,GdkEventKey *event,class l_kv_m_data *data);


int l_kv_m(class iceb_u_str *kod_val,GtkWidget *wpredok)
{
class l_kv_m_data data;
char strsql[512];


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_kv_m_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


//GtkWidget *label=gtk_label_new(gettext("Код"));
//gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VAL]), label, FALSE, FALSE, 0);

data.knopka_enter[E_KOD_VAL]=gtk_button_new_with_label(gettext("Код валюты"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VAL]), data.knopka_enter[E_KOD_VAL], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_VAL],"clicked",G_CALLBACK(l_kv_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_VAL],iceb_u_inttochar(E_KOD_VAL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_VAL],gettext("Выбор кода валюты"));

data.entry[E_KOD_VAL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_VAL]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VAL]), data.entry[E_KOD_VAL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_VAL], "activate",G_CALLBACK(l_kv_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_VAL]),data.kod_val.ravno());
gtk_widget_set_name(data.entry[E_KOD_VAL],iceb_u_inttochar(E_KOD_VAL));



sprintf(strsql,"F2 %s",gettext("Начать"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK2],gettext("Начать работу со справочником курсов валют"));
g_signal_connect(data.knopka[PFK2],"clicked",G_CALLBACK(l_kv_m_knopka),&data);
gtk_widget_set_name(data.knopka[PFK2],iceb_u_inttochar(PFK2));

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[PFK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[PFK4],"clicked",G_CALLBACK(l_kv_m_knopka),&data);
gtk_widget_set_name(data.knopka[PFK4],iceb_u_inttochar(PFK4));


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK10],gettext("Выйти из меню"));
g_signal_connect(data.knopka[PFK10],"clicked",G_CALLBACK(l_kv_m_knopka),&data);
gtk_widget_set_name(data.knopka[PFK10],iceb_u_inttochar(PFK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 kod_val->new_plus(data.kod_val.ravno());
 
return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_kv_m_e_knopka(GtkWidget *widget,class l_kv_m_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
   
  case E_KOD_VAL:
    if(l_sval(1,&kod,&naim,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VAL]),kod.ravno());

    return;  


 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_kv_m_key_press(GtkWidget *widget,GdkEventKey *event,class l_kv_m_data *data)
{
//char  bros[512];

//printf("vl_kv_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[PFK2],"clicked");
    return(FALSE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[PFK4],"clicked");
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
void  l_kv_m_knopka(GtkWidget *widget,class l_kv_m_data *data)
{
//char bros[512];

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_kv_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case PFK4:
    data->clear_data();
    return;  


  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_kv_m_vvod(GtkWidget *widget,class l_kv_m_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_kv_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD_VAL:
    data->kod_val.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }
 
}
