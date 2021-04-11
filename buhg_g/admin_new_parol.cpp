/*$Id: admin_new_parol.c,v 1.2 2013/08/29 05:56:31 sasa Exp $*/
/*30.12.2016	21.08.2013	Белых А.И.	admin_new_parol.c
Установка нового пароля для работы с базой
*/
#include "buhg_g.h"

enum
 {
  E_LOGIN,
  E_PAROL1,
  E_PAROL2,
  E_HOST_DOSTUPA,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class admin_new_parol_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str login;
  class iceb_u_str parol1;
  class iceb_u_str parol2;
  class iceb_u_str host_dostupa;

  class iceb_u_str host;    

  admin_new_parol_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    host_dostupa.plus("localhost");
   }

  void read_rek()
   {
    login.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_LOGIN])));
    parol1.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PAROL1])));
    parol2.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PAROL2])));
    host_dostupa.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_HOST_DOSTUPA])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
 };


gboolean   admin_new_parol_key_press(GtkWidget *widget,GdkEventKey *event,class admin_new_parol_data *data);
void  admin_new_parol_knopka(GtkWidget *widget,class admin_new_parol_data *data);
void    admin_new_parol_vvod(GtkWidget *widget,class admin_new_parol_data *data);
int admin_zap(class admin_new_parol_data *data);


extern SQL_baza  bd;

int admin_new_parol(const char *host,GtkWidget *wpredok)
{

class admin_new_parol_data data;
char strsql[1024];
data.host.new_plus(host);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод нового пароля"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

sprintf(strsql,"%s\n%s:%s",gettext("Ввод нового пароля"),gettext("Хост базы с которой работаем"),host);

label=gtk_label_new(strsql);


g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(admin_new_parol_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

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
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Логин оператора"));
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), label, FALSE, FALSE, 0);

data.entry[E_LOGIN] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_LOGIN]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), data.entry[E_LOGIN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_LOGIN], "activate",G_CALLBACK(admin_new_parol_vvod),&data);
gtk_widget_set_name(data.entry[E_LOGIN],iceb_u_inttochar(E_LOGIN));/******************/


label=gtk_label_new(gettext("Пароль"));
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL1]), label, FALSE, FALSE, 0);

data.entry[E_PAROL1] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PAROL1]),60);
gtk_entry_set_visibility(GTK_ENTRY(data.entry[E_PAROL1]),FALSE);
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL1]), data.entry[E_PAROL1], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PAROL1], "activate",G_CALLBACK(admin_new_parol_vvod),&data);
gtk_widget_set_name(data.entry[E_PAROL1],iceb_u_inttochar(E_PAROL1));/******************/

label=gtk_label_new(gettext("Пароль"));
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL2]), label, FALSE, FALSE, 0);

data.entry[E_PAROL2] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PAROL2]),60);
gtk_entry_set_visibility(GTK_ENTRY(data.entry[E_PAROL2]),FALSE);
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL2]), data.entry[E_PAROL2], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PAROL2], "activate",G_CALLBACK(admin_new_parol_vvod),&data);
gtk_widget_set_name(data.entry[E_PAROL2],iceb_u_inttochar(E_PAROL2));/******************/

label=gtk_label_new(gettext("Хост доступа"));
gtk_box_pack_start (GTK_BOX (hbox[E_HOST_DOSTUPA]), label, FALSE, FALSE, 0);

data.entry[E_HOST_DOSTUPA] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_HOST_DOSTUPA]),40);
gtk_box_pack_start (GTK_BOX (hbox[E_HOST_DOSTUPA]), data.entry[E_HOST_DOSTUPA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_HOST_DOSTUPA], "activate",G_CALLBACK(admin_new_parol_vvod),&data);
gtk_widget_set_name(data.entry[E_HOST_DOSTUPA],iceb_u_inttochar(E_HOST_DOSTUPA));/******************/
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HOST_DOSTUPA]),data.host_dostupa.ravno());

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(admin_new_parol_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(admin_new_parol_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(admin_new_parol_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
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

gboolean   admin_new_parol_key_press(GtkWidget *widget,GdkEventKey *event,class admin_new_parol_data *data)
{

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
  //  printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_new_parol_knopka(GtkWidget *widget,class admin_new_parol_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    admin_zap(data);
    return;  

  case FK4:
    data->clear_rek();
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

void    admin_new_parol_vvod(GtkWidget *widget,class admin_new_parol_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//printf("%s-%d\n",__FUNCTION__,enter);

switch (enter)
 {
  case E_LOGIN:
    data->login.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PAROL1:
    data->parol1.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PAROL2:
    data->parol2.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int admin_zap(class admin_new_parol_data *data)
{
char strsql[1024];

if(data->login.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён логин!"),data->window);
  return(1);
 }

if(data->host_dostupa.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён хост доступа!"),data->window);
  return(1);
 }


if(data->parol1.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён пароль!"),data->window);
  return(1);
 }

if(data->parol2.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён пароль!"),data->window);
  return(1);
 }

if(iceb_u_SRAV(data->parol1.ravno(),data->parol2.ravno(),0) != 0)
 {
  iceb_menu_soob(gettext("Введены не одинаковые пароли!"),data->window);
  return(1);
 }

sprintf(strsql,"SET PASSWORD FOR '%s'@'%s' = PASSWORD('%s')",data->login.ravno(),data->host_dostupa.ravno(),data->parol1.ravno());
if(iceb_sql_zapis(strsql,0,0,data->window) !=  0)
  return(1);
else
 iceb_menu_soob(gettext("Новый пароль успешно установлен"),data->window);

iceb_sql_zapis("FLUSH PRIVILEGES",1,1,data->window);

return(0);

}
