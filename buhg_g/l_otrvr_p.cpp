/*$Id: l_otrvr_p.c,v 1.13 2013/08/25 08:26:37 sasa Exp $*/
/*11.07.2015	08.09.2006	Белых А.И.	l_otrvr_p.c
Ввод и корректировка вида табеля
*/
#include "buhg_g.h"
#include "l_otrvr.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_TABNOM,
  E_KOD_TAB,
  E_KOMENT,
  E_PODR,
  KOLENTER
 };

class l_otrvr_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton1[2];
  GtkWidget *radiobutton2[2];
  GtkWidget *label_naishet;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_otrvr_rek *poi;
  
  
  l_otrvr_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    poi->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM])));
    poi->kod_tab.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_TAB])));
    poi->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
    poi->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODR])));
   }
  void clear_rek_poi()
   {
    for(int i=0; i<KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    poi->clear_rek();
   }
 };

gboolean   l_otrvr_p_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_otrvr_p_data *data);
void    l_otrvr_p_v_vvod(GtkWidget *widget,class l_otrvr_p_data *data);
void  l_otrvr_p_v_knopka(GtkWidget *widget,class l_otrvr_p_data *data);
void otrvr_t_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int otrvr_t_zap(class l_otrvr_p_data *data);
int otrvr_t_pk(char *kod,GtkWidget *wpredok);
void  otrvr_t_v_e_knopka(GtkWidget *widget,class l_otrvr_p_data *data);


extern SQL_baza bd;



int l_otrvr_p(class l_otrvr_rek *poi,GtkWidget *wpredok)
{
class l_otrvr_p_data data;
char strsql[512];
data.poi=poi;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Реквизиты поиска"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_otrvr_p_v_key_press),&data);

iceb_u_str zagolov;
zagolov.new_plus(gettext("Реквизиты поиска"));

GtkWidget *label=gtk_label_new(zagolov.ravno());

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator1);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator2=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator2);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_TABNOM],"clicked",G_CALLBACK(otrvr_t_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_TABNOM],iceb_u_inttochar(E_TABNOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"));

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABNOM], "activate",G_CALLBACK(l_otrvr_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.poi->tabnom.ravno());
gtk_widget_set_name(data.entry[E_TABNOM],iceb_u_inttochar(E_TABNOM));

sprintf(strsql,"%s (,,)",gettext("Код табеля"));
data.knopka_enter[E_KOD_TAB]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TAB]), data.knopka_enter[E_KOD_TAB], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_TAB],"clicked",G_CALLBACK(otrvr_t_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_TAB],iceb_u_inttochar(E_KOD_TAB));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_TAB],gettext("Выбор кода табеля"));

data.entry[E_KOD_TAB] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TAB]), data.entry[E_KOD_TAB],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_TAB], "activate",G_CALLBACK(l_otrvr_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_TAB]),data.poi->kod_tab.ravno());
gtk_widget_set_name(data.entry[E_KOD_TAB],iceb_u_inttochar(E_KOD_TAB));

sprintf(strsql,"%s (,,)",gettext("Подразделение"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(otrvr_t_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор подразделения"));

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(l_otrvr_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.poi->podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));



sprintf(strsql,"%s",gettext("Коментарий"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),40);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_otrvr_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.poi->koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));



sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_otrvr_p_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_otrvr_p_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_otrvr_p_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

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
void  otrvr_t_v_e_knopka(GtkWidget *widget,class l_otrvr_p_data *data)
{
iceb_u_str kod(0);
iceb_u_str naim(0);

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
      data->poi->tabnom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->poi->tabnom.ravno());
    return;  

  case E_KOD_TAB:
    if(l_zarvidtab(1,&kod,&naim,data->window) == 0)
      data->poi->kod_tab.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_TAB]),data->poi->kod_tab.ravno());
    return;  

  case E_PODR:
    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
      data->poi->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->poi->podr.ravno());
    return;  
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_otrvr_p_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_otrvr_p_data *data)
{

//printf("l_otrvr_p_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

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
void  l_otrvr_p_v_knopka(GtkWidget *widget,class l_otrvr_p_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();

    gtk_widget_destroy(data->window);

    data->voz=0;
    return;  

    
  case FK4:
    data->clear_rek_poi();
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

void    l_otrvr_p_v_vvod(GtkWidget *widget,class l_otrvr_p_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_otrvr_p_v_vvod enter=%d\n",enter);

switch (enter)
 {

  case E_TABNOM:
    data->poi->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOD_TAB:
    data->poi->kod_tab.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PODR:
    data->poi->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_KOMENT:
    data->poi->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
