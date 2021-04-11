/*$Id: l_zarkh_v.c,v 1.12 2013/08/25 08:26:46 sasa Exp $*/
/*12.07.2015	11.12.2009	Белых А.И.	l_zarkh_v.c
Ввод и корректировка
*/
#include <ctype.h>
#include "buhg_g.h"
enum
 {
  E_KODBANKA,
  E_KARTSHET,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_zarkh_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str kodbankas;
  int tabnom;
  
  class iceb_u_str kodbanka;
  class iceb_u_str kartshet;
    
  l_zarkh_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
   }

  void read_rek()
   {
    kodbanka.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODBANKA])));
    kartshet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KARTSHET])));
   }
  void clear_rek()
   {
    kodbanka.new_plus("");
    kartshet.new_plus("");
   }

 };


gboolean   l_zarkh_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarkh_v_data *data);
void  l_zarkh_v_knopka(GtkWidget *widget,class l_zarkh_v_data *data);
void    l_zarkh_v_vvod(GtkWidget *widget,class l_zarkh_v_data *data);
int l_zarkh_zap(class l_zarkh_v_data *data);
void  l_zarkh_v_e_knopka(GtkWidget *widget,class l_zarkh_v_data *data);


extern SQL_baza  bd;

int l_zarkh_v(int tabnom,const char *kodb,GtkWidget *wpredok)
{
class l_zarkh_v_data data;
char strsql[512];
iceb_u_str kikz;

data.kodbankas.new_plus(kodb);
data.tabnom=tabnom;

if(data.kodbankas.getdlinna() >  1)
 {
  sprintf(strsql,"select * from Zarkh where tn=%d and kb=%d",tabnom,atoi(kodb));
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.kodbanka.new_plus(row[1]);
  data.kartshet.new_plus(row[2]);
  kikz.plus(iceb_kikz(row[3],row[4],wpredok));
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kodbankas.getdlinna() <=  1)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zarkh_v_key_press),&data);

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


//label=gtk_label_new(gettext("Код банка"));
//gtk_box_pack_start (GTK_BOX (hbox[E_KODBANKA]), label, FALSE, FALSE, 0);
sprintf(strsql,"%s",gettext("Код банка"));
data.knopka_enter[E_KODBANKA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODBANKA]), data.knopka_enter[E_KODBANKA], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KODBANKA],"clicked",G_CALLBACK(l_zarkh_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODBANKA],iceb_u_inttochar(E_KODBANKA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODBANKA],gettext("Выбор банка"));

data.entry[E_KODBANKA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODBANKA]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KODBANKA]), data.entry[E_KODBANKA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODBANKA], "activate",G_CALLBACK(l_zarkh_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODBANKA]),data.kodbanka.ravno());
gtk_widget_set_name(data.entry[E_KODBANKA],iceb_u_inttochar(E_KODBANKA));

label=gtk_label_new(gettext("Номер картрахунку"));
gtk_box_pack_start (GTK_BOX (hbox[E_KARTSHET]), label, FALSE, FALSE, 0);

data.entry[E_KARTSHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KARTSHET]),60);
gtk_box_pack_start (GTK_BOX (hbox[E_KARTSHET]), data.entry[E_KARTSHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KARTSHET], "activate",G_CALLBACK(l_zarkh_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KARTSHET]),data.kartshet.ravno());
gtk_widget_set_name(data.entry[E_KARTSHET],iceb_u_inttochar(E_KARTSHET));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_zarkh_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_zarkh_v_knopka),&data);
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
void  l_zarkh_v_e_knopka(GtkWidget *widget,class l_zarkh_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_zarsb_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_KODBANKA:
   if(l_zarsb(1,&kod,&naim,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODBANKA]),kod.ravno());
      
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarkh_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarkh_v_data *data)
{
//char  bros[512];

//printf("l_zarkh_v_key_press\n");
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
void  l_zarkh_v_knopka(GtkWidget *widget,class l_zarkh_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_zarkh_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_zarkh_zap(data) == 0)
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

void    l_zarkh_v_vvod(GtkWidget *widget,class l_zarkh_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_zarkh_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KODBANKA:
    data->kodbanka.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KARTSHET:
    data->kartshet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int l_zarkh_zap(class l_zarkh_v_data *data)
{
char strsql[1024];
//SQL_str row;
time_t   vrem;
time(&vrem);


if(data->kodbanka.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }

if(data->kartshet.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }



if(isdigit(data->kodbanka.ravno()[0]) == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым !"),data->window);
  return(1);
 }


sprintf(strsql,"select kod from Zarsb where kod=%d",data->kodbanka.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  iceb_menu_soob(gettext("Нет такого кода банка!"),data->window);
  return(1);
 }



if(data->kodbankas.getdlinna() <= 1) /*новая запись*/
 {
  sprintf(strsql,"select kb from Zarkh where tn=%d and kb=%d",data->tabnom,data->kodbanka.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) > 0)
   {
    sprintf(strsql,gettext("Код банка %d уже есть!"),data->kodbanka.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
  sprintf(strsql,"insert into Zarkh values(%d,%d,'%s',%d,%ld)",
  data->tabnom,data->kodbanka.ravno_atoi(),data->kartshet.ravno(),iceb_getuid(data->window),time(NULL));
 }
else
 {
  sprintf(strsql,"update Zarkh set \
kb=%d,\
nks='%s',\
ktoz=%d,\
vrem=%ld \
where tn=%d and kb=%s",
  data->kodbanka.ravno_atoi(),
  data->kartshet.ravno(),
  iceb_getuid(data->window),
  time(NULL),
  data->tabnom,data->kodbankas.ravno());
       
 }


if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
