/*$Id: vid_kar_kl.c,v 1.14 2013/08/13 06:10:26 sasa Exp $*/
/*18.01.2007	18.01.2007	Белых А.И.	vid_kar_kl.c
Выдача карточки клиенту с установкой пола
*/
#include <unistd.h>
#include "i_rest.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_FIO,
  E_POL,
  KOLENTER  
 };

class vid_kar_kl_vs_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_suma;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  short metka;  //0-внесение денег в кассу 1-выдача денег из кассы
  class iceb_u_str fio;
  int pol; //0-c ПДВ 1-без ПДВ
  class iceb_u_str kodkl;
  
  vid_kar_kl_vs_data() //Конструктор
   {
    pol=0;  
    kl_shift=0;
    voz=1;
    kodkl.new_plus("");        
    fio.plus("");
   }

  void read_rek()
   {
    fio.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FIO])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER-1; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    gtk_widget_grab_focus(entry[0]);
   
   }
 };


gboolean   vid_kar_kl_vs_v_key_press(GtkWidget *widget,GdkEventKey *event,class vid_kar_kl_vs_data *data);
void    vid_kar_kl_vs_v_vvod(GtkWidget *widget,class vid_kar_kl_vs_data *data);
void  vid_kar_kl_vs_v_knopka(GtkWidget *widget,class vid_kar_kl_vs_data *data);
void  vid_kar_kl_v_e_knopka(GtkWidget *widget,class vid_kar_kl_vs_data *data);

GtkWidget *vid_kar_kl_pol(int *nomerstr);
int vid_kar_kl_zap(class vid_kar_kl_vs_data *data);

extern char *name_system;
extern short metka_skreen; //0-сенсорный экран не используется 1-используется
extern int   nomer_kas;  //номер кассы
extern uid_t           kod_operatora;

int vid_kar_kl(const char *kod_kl)
{
class vid_kar_kl_vs_data data;

data.kodkl.new_plus(kod_kl);
   

//Проверяем код клиента
char strsql[300];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select fio from Taxiklient where kod='%s'",data.kodkl.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,NULL) != 1)
 {
  iceb_menu_soob(gettext("Не найдено код клиента !"),NULL);
  return(1);
 }




data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(metka_skreen == 1)
 gtk_window_set_default_size(GTK_WINDOW(data.window),-1,400);
 
class iceb_u_str repl;
repl.plus(gettext("Выдача карточки клиенту"));
repl.ps_plus(gettext("Код карточки"));
repl.plus(":");
repl.plus(data.kodkl.ravno());
sprintf(strsql,"%s %s",name_system,gettext("Выдача карточки клиенту"));


gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vid_kar_kl_vs_v_key_press),&data);

GtkWidget *label=NULL;



label=gtk_label_new(repl.ravno());

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop=gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


data.knopka_suma=gtk_button_new_with_label(gettext("Фамилия"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.knopka_suma, FALSE, FALSE, 0);
g_signal_connect(data.knopka_suma,"clicked",G_CALLBACK(vid_kar_kl_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_suma,iceb_u_inttochar(E_FIO));


data.entry[E_FIO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FIO]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO], "activate",G_CALLBACK(vid_kar_kl_vs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.fio.ravno());
gtk_widget_set_name(data.entry[E_FIO],iceb_u_inttochar(E_FIO));

label=gtk_label_new(gettext("Пол"));
gtk_box_pack_start (GTK_BOX (hbox[E_POL]), label, FALSE, FALSE, 0);

GtkWidget *menu_pol = vid_kar_kl_pol(&data.pol);
gtk_box_pack_start (GTK_BOX (hbox[E_POL]), menu_pol, TRUE, TRUE, 0);


sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Записать введённую в меню информацию"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vid_kar_kl_vs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(vid_kar_kl_vs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vid_kar_kl_vs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);


gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vid_kar_kl_vs_v_key_press(GtkWidget *widget,GdkEventKey *event,class vid_kar_kl_vs_data *data)
{

//printf("vid_kar_kl_vs_v_key_press\n");
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
void  vid_kar_kl_vs_v_knopka(GtkWidget *widget,class vid_kar_kl_vs_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("vid_kar_kl_vs_v_knopka knop=%d\n",knop);

switch ((gint)knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if( vid_kar_kl_zap(data) != 0)
     return;
             
    gtk_widget_destroy(data->window);
    data->voz=0;
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vid_kar_kl_vs_v_vvod(GtkWidget *widget,class vid_kar_kl_vs_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//printf("vid_kar_kl_vs_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_FIO:
    data->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }
/****************
enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
******************/
}

/*****************************/
/*Меню выбора налога     */
/*****************************/

GtkWidget *vid_kar_kl_pol(int *nomerstr)
{
/**********
size_t nomer=0;
GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();

GtkWidget *item;
item = gtk_menu_item_new_with_label (gettext("Мужчина"));
g_signal_connect(item, "activate",G_CALLBACK(iceb_get_menu), nomerstr);
gtk_widget_set_name(item,iceb_u_inttochar(nomer++));
gtk_widget_show (item);

gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Женщина"));
g_signal_connect(item, "activate",G_CALLBACK(iceb_get_menu), nomerstr);
gtk_widget_set_name(item,iceb_u_inttochar(nomer++));
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);

gtk_option_menu_set_history(GTK_OPTION_MENU(opt),*nomerstr);
********************/
GtkWidget *opt;
class iceb_u_spisok spmenu;
spmenu.plus(gettext("Мужчина"));
spmenu.plus(gettext("Женщина"));
iceb_pm_vibor(&spmenu,&opt,0);
return(opt);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vid_kar_kl_v_e_knopka(GtkWidget *widget,class vid_kar_kl_vs_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {

  case E_FIO:

    g_signal_emit_by_name(data->entry[E_FIO],"activate");

    iceb_mous_klav(gettext("Введите коментарий"),&data->fio,20,1,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_FIO]),data->fio.ravno());

    return;  


 }
}
/************************/
/*Запись информации*/
/**********************/

int vid_kar_kl_zap(class vid_kar_kl_vs_data *data)
{
char strsql[300];
/******
sprintf(strsql,"update Taxiklient set \
fio='%s',\
pl=%d \
where kod='%s'",data->fio.ravno(),data->pol,data->kodkl.ravno());
***********/

sprintf(strsql,"update Taxiklient set \
fio='%s',\
adres='',\
telef='',\
ktoi=%d,\
vrem=%ld,\
ps=0.,\
pl=%d \
where kod='%s'",
data->fio.ravno(),      
iceb_getuid(data->window),
time(NULL),
data->pol,
data->kodkl.ravno());



return(iceb_sql_zapis(strsql,1,0,data->window));


}
