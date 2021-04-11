/*$Id: admin_dostup.c,v 1.18 2013/08/29 05:56:31 sasa Exp $*/
/*27.12.2016	05.02.2009	Белых А.И.	admin_dostup.c
Организация доступа к базе данных
*/
#define DVERSIQ "27.12.2016"
#include        "buhg_g.h"


enum
 {
  E_IMABAZ,
  E_LOGIN,
  E_PAROL,
  E_HOSTPOL,
  KOLENTER  
 };

enum
 {
  FK1,
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class admin_dostup_r
 {
  public:
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_link;
  GtkWidget *opt;
  class iceb_u_str parolpol;
  class iceb_u_str hostpol;
  class iceb_u_str login;
  class iceb_u_str imabaz;
  class iceb_u_str admin_host;
  
  int       dostup; //0-запись чтение 1-только чтение
  int fk4;

  admin_dostup_r()
   {
    clear_data();    
   }  

  void read_rek()
   {
    parolpol.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PAROL])));
    hostpol.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_HOSTPOL])));
    login.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_LOGIN])));
    imabaz.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_IMABAZ])));
    dostup=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));

   }

  void clear_data()
   {
    parolpol.new_plus("");
    hostpol.new_plus("");
    login.new_plus("");
    imabaz.new_plus("");
    dostup=0;
    fk4=0;
   }
   
 };


gboolean   dortupw_key_press(GtkWidget *,GdkEventKey *,class admin_dostup_r*);
void dortupw_knop(GtkWidget *,class admin_dostup_r *);
void dostupb(class admin_dostup_r *);
void dostup_v_vvod(GtkWidget *widget,class admin_dostup_r *data);
void admin_newuser(const char *imabazr,const char *user,GtkWidget *wpredok);
void admin_dostup_e_knopka(GtkWidget *widget,class admin_dostup_r *data);

extern SQL_baza	bd;

int admin_dostup(const char *admin_host)
{
char bros[2048];
class admin_dostup_r data;

data.hostpol.new_plus("localhost");
data.admin_host.new_plus(admin_host);


data.window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,gettext("Доступ к базе"));
gtk_window_set_title(GTK_WINDOW(data.window),bros);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(dortupw_key_press),&data);

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

sprintf(bros,"%s\n%s %s %s %s %s",
gettext("Установка доступа к базе данных"),
gettext("Версия"),
VERSION,
gettext("от"),
DVERSIQ,
gettext("г."));

GtkWidget *label=gtk_label_new(bros);

GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hbox_tipd=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
gtk_box_set_homogeneous (GTK_BOX(hbox_tipd),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxkn=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
gtk_box_set_homogeneous (GTK_BOX(hboxkn),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window),vbox);

gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);

gtk_box_pack_start((GtkBox*)vbox,hbox_tipd,FALSE,FALSE,0);
gtk_box_pack_start((GtkBox*)vbox,hboxkn,FALSE,FALSE,0);



data.knopka_enter[E_IMABAZ]=gtk_button_new_with_label(gettext("Имя базы данных"));
gtk_box_pack_start (GTK_BOX (hbox[E_IMABAZ]), data.knopka_enter[E_IMABAZ], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_IMABAZ],"clicked",G_CALLBACK(admin_dostup_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_IMABAZ],iceb_u_inttochar(E_IMABAZ));
gtk_widget_set_tooltip_text(data.knopka_enter[E_IMABAZ],gettext("Выбор базы"));

data.entry[E_IMABAZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_IMABAZ]),32);
gtk_box_pack_start (GTK_BOX (hbox[E_IMABAZ]), data.entry[E_IMABAZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_IMABAZ], "activate",G_CALLBACK(dostup_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMABAZ]),data.imabaz.ravno());
gtk_widget_set_name(data.entry[E_IMABAZ],iceb_u_inttochar(E_IMABAZ));


label=gtk_label_new(gettext("Логин оператора"));
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), label, FALSE, FALSE, 0);

data.entry[E_LOGIN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_LOGIN]),32);
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), data.entry[E_LOGIN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_LOGIN], "activate",G_CALLBACK(dostup_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_LOGIN]),data.login.ravno());
gtk_widget_set_name(data.entry[E_LOGIN],iceb_u_inttochar(E_LOGIN));


label=gtk_label_new(gettext("Пароль оператора для работы с базой"));
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), label, FALSE, FALSE, 0);

data.entry[E_PAROL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PAROL]),32);
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), data.entry[E_PAROL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PAROL], "activate",G_CALLBACK(dostup_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PAROL]),data.parolpol.ravno());
gtk_widget_set_name(data.entry[E_PAROL],iceb_u_inttochar(E_PAROL));


label=gtk_label_new(gettext("Хост c которого оператору будет разрешен доступ к базе"));
gtk_box_pack_start (GTK_BOX (hbox[E_HOSTPOL]), label, FALSE, FALSE, 0);

data.entry[E_HOSTPOL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HOSTPOL]), data.entry[E_HOSTPOL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_HOSTPOL], "activate",G_CALLBACK(dostup_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HOSTPOL]),data.hostpol.ravno());
gtk_widget_set_name(data.entry[E_HOSTPOL],iceb_u_inttochar(E_HOSTPOL));


label=gtk_label_new(gettext("Тип доступа"));
gtk_box_pack_start((GtkBox*)hbox_tipd,label,FALSE,FALSE,0);

data.opt=gtk_combo_box_text_new();
int nom=0;
gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(data.opt),nom++,gettext("Запись и чтение"));
gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT(data.opt),nom++,gettext("Только чтение"));

gtk_combo_box_set_active(GTK_COMBO_BOX(data.opt),0);

gtk_box_pack_start (GTK_BOX (hbox_tipd), data.opt, TRUE, TRUE, 0);




sprintf(bros,"<a href=\"%s/i_admin2.html\" title=\"%s\">F1 %s</a>",ICEB_PUT_NA_HTML,gettext("Просмотр документации"),gettext("Помощь"));
data.label_link=gtk_label_new(bros);
gtk_label_set_use_markup (GTK_LABEL (data.label_link), TRUE);
g_signal_connect(data.label_link, "activate-current-link", G_CALLBACK (iceb_activate_link), NULL);

data.knopka[FK1]=gtk_button_new();
gtk_container_add (GTK_CONTAINER (data.knopka[FK1]), data.label_link);
g_signal_connect(data.knopka[FK1],"clicked",G_CALLBACK(dortupw_knop),&data);
gtk_box_pack_start(GTK_BOX(hboxkn), data.knopka[FK1], TRUE, TRUE, 0);



sprintf(bros,"F2 %s",gettext("Установить доступ"));
data.knopka[FK2]=gtk_button_new_with_label (bros);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start (GTK_BOX (hboxkn), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(dortupw_knop),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Установить разрещение на работу с базой"));

sprintf(bros,"F4 %s",gettext("Проверка"));
data.knopka[FK4]=gtk_button_new_with_label (bros);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start (GTK_BOX (hboxkn), data.knopka[FK4], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(dortupw_knop),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Проверить есть ли уже доступ для этого логина"));

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label (bros);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start (GTK_BOX (hboxkn), data.knopka[FK10], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(dortupw_knop),&data);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));

gtk_widget_show_all(data.window);
gtk_main();

return(0);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void admin_dostup_e_knopka(GtkWidget *widget,class admin_dostup_r *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_IMABAZ:

    if(admin_bases(1,&data->imabaz,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_IMABAZ]),data->imabaz.ravno());
      
    return;  

 }
}
/*************************/
/*Обработчик кнопок      */
/**************************/

void    dortupw_knop(GtkWidget *widget,class admin_dostup_r *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case FK1:
    g_signal_emit_by_name(data->label_link,"activate-current-link");
    return;

  case FK2:
   data->fk4=0;
   dostupb(data);
   return;

  case FK4:
   data->fk4=1;
   dostupb(data);

   return;
   
  case FK10:
    gtk_widget_destroy(data->window);
    return;

 }



}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dortupw_key_press(GtkWidget *widget,GdkEventKey *event,class admin_dostup_r *data)
{

switch(event->keyval)
 {
  case GDK_KEY_F1:
    g_signal_emit_by_name(data->label_link,"activate-current-link");
    return(FALSE);

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(TRUE);

 }
return(TRUE);
}
/*****************************/
/*Установка достува*/
/**************************/

void   dostupb(class admin_dostup_r *data)
{
char strsql[512];
data->read_rek();

if(data->imabaz.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели имя базы!"),data->window);
  return;     
 }
if(data->login.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели логин оператора!"),data->window);
  return;     
 }

if(admin_provbaz(data->imabaz.ravno(),data->window) != 0)
 {
  iceb_menu_soob(gettext("Нет базы данных с таким именем!"),data->window);
  return;
 }

if(data->fk4 != 0)
 {
  
  sprintf(strsql,"select User from db where Db='%s' and User='%s'",data->imabaz.ravno(),data->login.ravno());
  if(iceb_sql_readkey(strsql,data->window) > 0) /*Запись уже есть*/
    iceb_menu_soob(gettext("Такой логин для этой базы уже введён!"),data->window);
  else
    iceb_menu_soob(gettext("Такого логина для этой базы ещё нет"),data->window);
  return;  
 }

if(data->parolpol.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели пароль оператора!"),data->window);
  return;     
 }

//SQL_str row;
//class SQLCURSOR cur;
#if 0
################################33
sprintf(strsql,"select User from db where Db='%s' and User='%s' and \
Host='%s'",
data->imabaz.ravno(),data->login.ravno(),data->hostpol.ravno());
if(sql_readkey(&bd,strsql) > 0)
 {
//  printw("Переустановка прав доступа.\n");
  
  if(data->parolpol.ravno()[0] != '\0')
   {
    sprintf(strsql,"SET PASSWORD FOR '%s'@'%s' = PASSWORD('%s')",data->login.ravno(),data->admin_host.ravno(),data->parolpol.ravno());
    iceb_sql_zapis(strsql,1,0,data->window);
   }

  if(data->hostpol.ravno()[0] != '\0')
   {
    if(data->dostup == 0) 
     {
      sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP,LOCK TABLES,CREATE TEMPORARY TABLES ON \
%s.* TO '%s'@'%s'",data->imabaz.ravno(),data->login.ravno(),data->hostpol.ravno());
     }
    if(data->dostup == 1)
      sprintf(strsql,"REVOKE INSERT,DELETE,UPDATE,CREATE,DROP ON \
%s.* FROM %s@%s",data->imabaz.ravno(),data->login.ravno(),data->hostpol.ravno());
   }
  else
   {
    if(data->dostup == 0)
     {
      sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP,LOCK TABLES,CREATE TEMPORARY TABLES ON \
%s.* TO '%s'",data->imabaz.ravno(),data->login.ravno());
     }
    if(data->dostup == 1)
      sprintf(strsql,"REVOKE INSERT,DELETE,UPDATE,CREATE,DROP ON \
%s.* FROM '%s'",data->imabaz.ravno(),data->login.ravno());
   }
 }
else
 {
#########################################
#endif
  if(data->hostpol.ravno()[0] != '\0')
   {
    if(data->dostup == 0)
     {
      
      sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP,LOCK TABLES,CREATE TEMPORARY TABLES ON \
%s.* TO '%s'@'%s' IDENTIFIED BY '%s'",data->imabaz.ravno(),data->login.ravno(),data->hostpol.ravno(),data->parolpol.ravno());
     }
    if(data->dostup == 1)
      sprintf(strsql,"GRANT SELECT ON \
%s.* TO '%s'@'%s' IDENTIFIED BY '%s'",data->imabaz.ravno(),data->login.ravno(),data->hostpol.ravno(),data->parolpol.ravno());
   }
  else
   {
    if(data->dostup == 0)
     {
      sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP,LOCK TABLES,CREATE TEMPORARY TABLES ON \
%s.* TO '%s' IDENTIFIED BY '%s'",data->imabaz.ravno(),data->login.ravno(),data->parolpol.ravno());
     }
    if(data->dostup == 1)
      sprintf(strsql,"GRANT SELECT ON \
%s.* TO '%s' IDENTIFIED BY '%s'",data->imabaz.ravno(),data->login.ravno(),data->parolpol.ravno());
   }
// }

if(iceb_sql_zapis(strsql,1,1,data->window) == 0)
 {
  iceb_menu_soob("Привелегии пользователю установлены",data->window);
  admin_newuser(data->imabaz.ravno(),data->login.ravno(),data->window);
 }

iceb_sql_zapis("FLUSH PRIVILEGES",1,1,data->window);


}

/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    dostup_v_vvod(GtkWidget *widget,class admin_dostup_r *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//g_print("glkni_v_vvod enter=%d\n",enter);

/***************
switch (enter)
 {
  case E_IMABAZ:
    data->imabaz.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case LOGIN:
    strncpy(data->login,gtk_entry_get_text(GTK_ENTRY(widget)),
    sizeof(data->login)-1);
    break;

  case PAROL:
    strncpy(data->parolpol,gtk_entry_get_text(GTK_ENTRY(widget)),
    sizeof(data->parolpol)-1);
    break;

  case HOSTPOL:
    strncpy(data->hostpol,gtk_entry_get_text(GTK_ENTRY(widget)),
    sizeof(data->hostpol)-1);
    break;

 }
******************/
enter+=1;
if(enter >= KOLENTER)
  enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
