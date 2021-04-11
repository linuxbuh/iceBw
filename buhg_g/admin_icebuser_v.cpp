/*$Id: admin_icebuser_v.c,v 1.9 2014/08/31 06:19:18 sasa Exp $*/
/*16.03.2016	16.09.2010	Белых А.И.	admin_icebuser_v.
Корректировка фамилии оператора
*/
#include <pwd.h>
#include "buhg_g.h"

enum
 {
  E_FIO,
  E_KOMENT,
  E_TABNOM,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class admin_icebuser_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *opt;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str fio;
  class iceb_u_str koment;
  class iceb_u_str tabnom;
  
  int metka_r; /*0-разрешена выгрузка дампа базы*/
  
  int user;
  admin_icebuser_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    metka_r=0;
   }

  void read_rek()
   {
    fio.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FIO])));
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
    tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM])));
    metka_r=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
 };


gboolean   admin_icebuser_v_key_press(GtkWidget *widget,GdkEventKey *event,class admin_icebuser_v_data *data);
void  admin_icebuser_v_knopka(GtkWidget *widget,class admin_icebuser_v_data *data);
void    admin_icebuser_v_vvod(GtkWidget *widget,class admin_icebuser_v_data *data);
int admin_icebuser_zap(class admin_icebuser_v_data *data);


extern SQL_baza  bd;

int admin_icebuser_v(int user,GtkWidget *wpredok)
{

class admin_icebuser_v_data data;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str kikz;

data.user=user;
sprintf(strsql,"select fio,kom,vdb,tn from icebuser where un=%d",user);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  class iceb_u_str repl(gettext("Не найдена запись для корректировки !"));
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }
data.fio.new_plus(row[0]);
data.koment.new_plus(row[1]);
data.metka_r=atoi(row[2]);
data.tabnom.new_plus(row[3]);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
iceb_u_str repl;
repl.plus(gettext("Корректировка записи"));
repl.ps_plus(kikz.ravno());
label=gtk_label_new(repl.ravno());

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(admin_icebuser_v_key_press),&data);

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

GtkWidget *hboxmetka = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxmetka),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);

gtk_box_pack_start (GTK_BOX (vbox),hboxmetka, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Фамилия"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);

data.entry[E_FIO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FIO]),254);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO], "activate",G_CALLBACK(admin_icebuser_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.fio.ravno());
gtk_widget_set_name(data.entry[E_FIO],iceb_u_inttochar(E_FIO));

label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),254);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(admin_icebuser_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));

label=gtk_label_new(gettext("Табельный номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), label, FALSE, FALSE, 0);

data.entry[E_TABNOM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_TABNOM]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABNOM], "activate",G_CALLBACK(admin_icebuser_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.tabnom.ravno());
gtk_widget_set_name(data.entry[E_TABNOM],iceb_u_inttochar(E_TABNOM));

label=gtk_label_new(gettext("Выгрузка дампа базы"));
gtk_box_pack_start (GTK_BOX (hboxmetka), label, FALSE, FALSE, 0);

class iceb_u_spisok spmenu;
spmenu.plus(gettext("Разрешено"));
spmenu.plus(gettext("Запрещено"));
iceb_pm_vibor(&spmenu,&data.opt,data.metka_r);
gtk_box_pack_start (GTK_BOX (hboxmetka),data.opt, TRUE, TRUE, 0);

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(admin_icebuser_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(admin_icebuser_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(admin_icebuser_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_icebuser_v_key_press(GtkWidget *widget,GdkEventKey *event,class admin_icebuser_v_data *data)
{
//char  bros[512];

//printf("admin_icebuser_v_key_press\n");
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
void  admin_icebuser_v_knopka(GtkWidget *widget,class admin_icebuser_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("admin_icebuser_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(admin_icebuser_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
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

void    admin_icebuser_v_vvod(GtkWidget *widget,class admin_icebuser_v_data *data)
{
char strsql[1024];

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {

  case E_FIO:
    data->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOMENT:
    data->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_TABNOM:
    data->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    if(data->tabnom.ravno_atoi() != 0)
     {
      sprintf(strsql,"select tabn from Kartb where tabn=%d",data->tabnom.ravno_atoi());
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        sprintf(strsql,"%s! %d",gettext("Нет такого табельного номера"),data->tabnom.ravno_atoi());
        iceb_menu_soob(strsql,data->window);
      
       }
     }
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
int admin_icebuser_zap(class admin_icebuser_v_data *data)
{
char strsql[1024];
struct  passwd  *ktor; /*Кто работает*/
ktor=getpwuid(getuid());

if(data->tabnom.ravno_atoi() != 0)
 {
  sprintf(strsql,"select tabn from Kartb where tabn=%d",data->tabnom.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s! %d",gettext("Нет такого табельного номера"),data->tabnom.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }

sprintf(strsql,"update icebuser set fio='%s',kom='%s',logz='%s',vrem=%ld, vdb=%d,tn=%d where un=%d",
data->fio.ravno_filtr(),data->koment.ravno_filtr(),ktor->pw_name,time(NULL),data->metka_r,data->tabnom.ravno_atoi(),data->user);

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
  return(1);

return(0);

}
