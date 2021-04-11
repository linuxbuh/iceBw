/*$Id: zvb_hreshatik_start_v.c,v 1.10 2013/08/25 08:27:02 sasa Exp $*/
/*18.10.2014	16.10.2009	Белых А.И.	zvb_hreshatik_v.c
Ввод необходимых реквизитов для перечисления на картсчета
*/
#include "buhg_g.h"
#include "zvb_hreshatik.h"
enum
 {
  E_DATPP,
  E_NOMERPP,
  E_MESZP,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class zvb_hreshatik_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class zvb_hreshatik_rek *rk;
  
  zvb_hreshatik_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datpp.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATPP])));
    rk->nomerpp.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMERPP])));
    rk->meszp.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MESZP])));
   }
 };


gboolean   zvb_hreshatik_v_key_press(GtkWidget *widget,GdkEventKey *event,class zvb_hreshatik_v_data *data);
void  zvb_hreshatik_v_knopka(GtkWidget *widget,class zvb_hreshatik_v_data *data);
void    zvb_hreshatik_v_vvod(GtkWidget *widget,class zvb_hreshatik_v_data *data);
int zvb_hreshatik_zap(class zvb_hreshatik_v_data *data);

void  zvb_hreshatik_v_e_knopka(GtkWidget *widget,class zvb_hreshatik_v_data *data);


extern SQL_baza  bd;

int zvb_hreshatik_start_v(class zvb_hreshatik_rek *rek,GtkWidget *wpredok)
{

class zvb_hreshatik_v_data data;
char strsql[512];
iceb_u_str kikz;

data.rk=rek;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",gettext("Перечисление на картсчета"),gettext("Банк Крещатик"));
label=gtk_label_new(strsql);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Перечисление на картсчета"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zvb_hreshatik_v_key_press),&data);

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
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


//label=gtk_label_new(gettext("Дата платёжного поручения"));
//gtk_box_pack_start (GTK_BOX (hbox[E_DATPP]), label, FALSE, FALSE, 0);

sprintf(strsql,"%s (%s)",gettext("Дата платёжного поручения"),gettext("д.м.г"));
data.knopka_enter[E_DATPP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATPP]), data.knopka_enter[E_DATPP], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATPP],"clicked",G_CALLBACK(zvb_hreshatik_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATPP],iceb_u_inttochar(E_DATPP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATPP],gettext("Выбор даты"));

data.entry[E_DATPP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATPP]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATPP]), data.entry[E_DATPP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATPP], "activate",G_CALLBACK(zvb_hreshatik_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATPP]),data.rk->datpp.ravno());
gtk_widget_set_name(data.entry[E_DATPP],iceb_u_inttochar(E_DATPP));

label=gtk_label_new(gettext("Номер платёжного поручения"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERPP]), label, FALSE, FALSE, 0);

data.entry[E_NOMERPP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMERPP]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERPP]), data.entry[E_NOMERPP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMERPP], "activate",G_CALLBACK(zvb_hreshatik_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMERPP]),data.rk->nomerpp.ravno());
gtk_widget_set_name(data.entry[E_NOMERPP],iceb_u_inttochar(E_NOMERPP));

sprintf(strsql,"%s (%s)",gettext("Зароботная плата за"),gettext("м.г"));

label=gtk_label_new(strsql);

gtk_box_pack_start (GTK_BOX (hbox[E_MESZP]), label, FALSE, FALSE, 0);

data.entry[E_MESZP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MESZP]),7);
gtk_box_pack_start (GTK_BOX (hbox[E_MESZP]), data.entry[E_MESZP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_MESZP],"activate",G_CALLBACK(zvb_hreshatik_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MESZP]),data.rk->meszp.ravno());
gtk_widget_set_name(data.entry[E_MESZP],iceb_u_inttochar(E_MESZP));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(zvb_hreshatik_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(zvb_hreshatik_v_knopka),&data);
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

void  zvb_hreshatik_v_e_knopka(GtkWidget *widget,class zvb_hreshatik_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATPP:

    if(iceb_calendar(&data->rk->datpp,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATPP]),data->rk->datpp.ravno());
      
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zvb_hreshatik_v_key_press(GtkWidget *widget,GdkEventKey *event,class zvb_hreshatik_v_data *data)
{
//char  bros[512];

//printf("zvb_hreshatik_v_key_press\n");
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
void  zvb_hreshatik_v_knopka(GtkWidget *widget,class zvb_hreshatik_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("zvb_hreshatik_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(zvb_hreshatik_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
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

void zvb_hreshatik_v_vvod(GtkWidget *widget,class zvb_hreshatik_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("zvb_hreshatik_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATPP:
    data->rk->datpp.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NOMERPP:
    data->rk->nomerpp.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int zvb_hreshatik_zap(class zvb_hreshatik_v_data *data)
{

if(data->rk->datpp.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
  return(1);
 }

if(data->rk->meszp.prov_dat1() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата зарплаты!"),data->window);
  return(1);
 }

return(0);

}
