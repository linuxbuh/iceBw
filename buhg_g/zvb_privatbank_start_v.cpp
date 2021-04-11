/*$Id: zvb_privatbank_start_v.c,v 1.10 2013/08/25 08:27:02 sasa Exp $*/
/*13.07.2015	18.10.2014	Белых А.И.	zvb_privatbank_v.c
Ввод необходимых реквизитов для перечисления на картсчета
*/
#include "buhg_g.h"
enum
 {
  E_BRANCH,
  E_ZPKOD,
  E_RLKOD,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class zvb_privatbank_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str branch;
  class iceb_u_str zpkod;
  class iceb_u_str rlkod;    

  zvb_privatbank_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();    
   }

  void read_rek()
   {
    branch.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_BRANCH])));
    zpkod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ZPKOD])));
    rlkod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_RLKOD])));
    
   }
  void clear()
   {
    branch.new_plus("");
    zpkod.new_plus("");
   }
 };


gboolean   zvb_privatbank_v_key_press(GtkWidget *widget,GdkEventKey *event,class zvb_privatbank_v_data *data);
void  zvb_privatbank_v_knopka(GtkWidget *widget,class zvb_privatbank_v_data *data);
void    zvb_privatbank_v_vvod(GtkWidget *widget,class zvb_privatbank_v_data *data);
int zvb_privatbank_zap(class zvb_privatbank_v_data *data);

void  zvb_privatbank_v_e_knopka(GtkWidget *widget,class zvb_privatbank_v_data *data);


extern SQL_baza  bd;

int zvb_privatbank_v(class iceb_u_str *branch,class iceb_u_str *zpkod,class iceb_u_str *rlkod,GtkWidget *wpredok)
{

class zvb_privatbank_v_data data;
char strsql[512];
iceb_u_str kikz;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",gettext("Перечисление на картсчета"),gettext("Альфабанк"));
label=gtk_label_new(strsql);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Перечисление на картсчета"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zvb_privatbank_v_key_press),&data);

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


label=gtk_label_new(gettext("Карточный бранч"));
gtk_box_pack_start (GTK_BOX (hbox[E_BRANCH]), label, FALSE, FALSE, 0);


data.entry[E_BRANCH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_BRANCH]),3);
gtk_box_pack_start (GTK_BOX (hbox[E_BRANCH]), data.entry[E_BRANCH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_BRANCH], "activate",G_CALLBACK(zvb_privatbank_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_BRANCH]),data.branch.ravno());
gtk_widget_set_name(data.entry[E_BRANCH],iceb_u_inttochar(E_BRANCH));

label=gtk_label_new(gettext("Код зарплатного проэкта"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZPKOD]), label, FALSE, FALSE, 0);

data.entry[E_ZPKOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ZPKOD]),3);
gtk_box_pack_start (GTK_BOX (hbox[E_ZPKOD]), data.entry[E_ZPKOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ZPKOD], "activate",G_CALLBACK(zvb_privatbank_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZPKOD]),data.zpkod.ravno());
gtk_widget_set_name(data.entry[E_ZPKOD],iceb_u_inttochar(E_ZPKOD));

label=gtk_label_new(gettext("Назначение зачисления"));
gtk_box_pack_start (GTK_BOX (hbox[E_RLKOD]), label, FALSE, FALSE, 0);

data.entry[E_RLKOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_RLKOD]),30);
gtk_box_pack_start (GTK_BOX (hbox[E_RLKOD]), data.entry[E_RLKOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_RLKOD], "activate",G_CALLBACK(zvb_privatbank_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_RLKOD]),data.zpkod.ravno());
gtk_widget_set_name(data.entry[E_RLKOD],iceb_u_inttochar(E_RLKOD));



sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(zvb_privatbank_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(zvb_privatbank_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
if(data.voz == 0)
 {
  branch->new_plus(data.branch.ravno());
  zpkod->new_plus(data.zpkod.ravno());  
  rlkod->new_plus(data.rlkod.ravno());
 }

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/

void  zvb_privatbank_v_e_knopka(GtkWidget *widget,class zvb_privatbank_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zvb_privatbank_v_key_press(GtkWidget *widget,GdkEventKey *event,class zvb_privatbank_v_data *data)
{
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

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zvb_privatbank_v_knopka(GtkWidget *widget,class zvb_privatbank_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(zvb_privatbank_zap(data) == 0)
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

void zvb_privatbank_v_vvod(GtkWidget *widget,class zvb_privatbank_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_BRANCH:
    data->branch.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_ZPKOD:
    data->zpkod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_RLKOD:
    data->rlkod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int zvb_privatbank_zap(class zvb_privatbank_v_data *data)
{


return(0);

}
