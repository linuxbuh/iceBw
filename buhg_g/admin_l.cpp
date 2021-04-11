/*$Id: admin_l.c,v 1.16 2013/08/25 08:26:31 sasa Exp $*/
/*24.05.2018	05.02.2009	Белых А.И.	admin_l.c
Ввод логина и пароля для доступа к базе
*/
#include "buhg_g.h"
enum
 {
  E_LOGIN,
  E_PAROL,
  E_HOST,
//  E_KZKB,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK10,
  KOL_PFK
 };

class admin_l_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str parol;
  class iceb_u_str login;  
  class iceb_u_str host;  
  class iceb_u_str kzkb;
  
  admin_l_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    parol.plus("");
    login.plus("root");
    host.plus("localhost");
    kzkb.plus("utf8");
  
   }

  void read_rek()
   {
    parol.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PAROL])));
    login.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_LOGIN])));
    host.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_HOST])));
//    kzkb.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KZKB])));
    
   }
 };

gboolean   admin_l_key_press(GtkWidget *widget,GdkEventKey *event,class admin_l_data *data);
void  admin_l_knopka(GtkWidget *widget,class admin_l_data *data);
void    admin_l_vvod(GtkWidget *widget,class admin_l_data *data);

extern SQL_baza  bd;

int admin_l(class iceb_u_str *parol,class iceb_u_str *login,class iceb_u_str *host,class iceb_u_str *kzkb)
{
class admin_l_data data;
char strsql[512];
if(kzkb->getdlinna() > 1)
 data.kzkb.new_plus(kzkb->ravno());

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",NAME_SYSTEM,gettext("Администрирование баз данных"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(admin_l_key_press),&data);


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
//gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Введите логин"));
data.entry[E_LOGIN] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), data.entry[E_LOGIN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_LOGIN], "activate",G_CALLBACK(admin_l_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_LOGIN]),data.login.ravno());
gtk_widget_set_name(data.entry[E_LOGIN],iceb_u_inttochar(E_LOGIN));

label=gtk_label_new(gettext("Введите пароль для доступа к базе данных"));
data.entry[E_PAROL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), data.entry[E_PAROL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PAROL], "activate",G_CALLBACK(admin_l_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PAROL]),data.parol.ravno());
gtk_widget_set_name(data.entry[E_PAROL],iceb_u_inttochar(E_PAROL));
gtk_entry_set_visibility(GTK_ENTRY(data.entry[E_PAROL]),FALSE); //невидимый ввод

label=gtk_label_new(gettext("Хост на базу"));
data.entry[E_HOST] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HOST]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_HOST]), data.entry[E_HOST], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_HOST], "activate",G_CALLBACK(admin_l_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HOST]),data.host.ravno());
gtk_widget_set_name(data.entry[E_HOST],iceb_u_inttochar(E_HOST));
/******************
label=gtk_label_new(gettext("Кодировка запросов к базе"));
data.entry[E_KZKB] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KZKB]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KZKB]), data.entry[E_KZKB], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KZKB], "activate",G_CALLBACK(admin_l_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KZKB]),data.kzkb.ravno());
gtk_widget_set_name(data.entry[E_KZKB],iceb_u_inttochar(E_KZKB));
*******************/


sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK2],gettext("Продолжить работу с программой"));
g_signal_connect(data.knopka[PFK2],"clicked",G_CALLBACK(admin_l_knopka),&data);
gtk_widget_set_name(data.knopka[PFK2],iceb_u_inttochar(PFK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK10],gettext("Завершить работу с программой"));
g_signal_connect(data.knopka[PFK10],"clicked",G_CALLBACK(admin_l_knopka),&data);
gtk_widget_set_name(data.knopka[PFK10],iceb_u_inttochar(PFK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  parol->new_plus(data.parol.ravno());
  login->new_plus(data.login.ravno());
  host->new_plus(data.host.ravno());
  kzkb->new_plus(data.kzkb.ravno());

  
 }
return(data.voz);

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_l_key_press(GtkWidget *widget,GdkEventKey *event,class admin_l_data *data)
{
//char  bros[512];

//printf("admin_l_key_press\n");
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
void  admin_l_knopka(GtkWidget *widget,class admin_l_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("admin_l_knopka knop=%d\n",knop);

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
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    admin_l_vvod(GtkWidget *widget,class admin_l_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("admin_l_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_LOGIN:
    data->login.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_PAROL:
    data->parol.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_HOST:
    data->host.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
