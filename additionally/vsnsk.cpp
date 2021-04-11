/*$Id: vsnsk.c,v 1.10 2013/08/13 06:10:26 sasa Exp $*/
/*09.02.2006	09.02.2006	Белых А.И.	vsnsk.c
Ввод суммы на счёт клиента (безналичный)
*/
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
  E_SUMA,
  E_KOMENT,
  KOLENTER  
 };

class vsnsk_vs_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_suma;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  iceb_u_str suma;
  iceb_u_str koment;
  iceb_u_str kodkl;
  
  vsnsk_vs_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    suma.new_plus("");
    kodkl.new_plus("");        
    koment.plus("");
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER-1; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    suma.new_plus("");
    gtk_widget_grab_focus(entry[0]);
   
   }
 };


gboolean   vsnsk_vs_v_key_press(GtkWidget *widget,GdkEventKey *event,class vsnsk_vs_data *data);
void    vsnsk_vs_v_vvod(GtkWidget *widget,class vsnsk_vs_data *data);
void  vsnsk_vs_v_knopka(GtkWidget *widget,class vsnsk_vs_data *data);
void  vsnsk_v_e_knopka(GtkWidget *widget,class vsnsk_vs_data *data);


extern char *name_system;
extern short metka_skreen; //0-сенсорный экран не используется 1-используется
extern int   nomer_kas;  //номер кассы
extern uid_t           kod_operatora;

int vsnsk()
{
class vsnsk_vs_data data;


if(iceb_mous_klav(gettext("Введите код клиента"),&data.kodkl,20,0,0,1,NULL) != 0)
 return(1);
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

iceb_u_str repl;
repl.plus(gettext("Ввод сумы на счёт клиента"));
sprintf(strsql,"%s %s",name_system,gettext("Ввод сумы на счёт клиента"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vsnsk_vs_v_key_press),&data);

GtkWidget *label=NULL;


repl.ps_plus(data.kodkl.ravno());
repl.plus(" ");
repl.plus(row[0]);
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

data.knopka_suma=gtk_button_new_with_label(gettext("Сумма"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.knopka_suma, FALSE, FALSE, 0);
g_signal_connect(data.knopka_suma,"clicked",G_CALLBACK(vsnsk_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_suma,iceb_u_inttochar(E_SUMA));


data.entry[E_SUMA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA], "activate",G_CALLBACK(vsnsk_vs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno());
gtk_widget_set_name(data.entry[E_SUMA],iceb_u_inttochar(E_SUMA));

data.knopka_suma=gtk_button_new_with_label(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.knopka_suma, FALSE, FALSE, 0);
g_signal_connect(data.knopka_suma,"clicked",G_CALLBACK(vsnsk_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_suma,iceb_u_inttochar(E_KOMENT));


data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(vsnsk_vs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));



sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод суммы в кассовый регистратор"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vsnsk_vs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(vsnsk_vs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vsnsk_vs_v_knopka),&data);
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

gboolean   vsnsk_vs_v_key_press(GtkWidget *widget,GdkEventKey *event,class vsnsk_vs_data *data)
{

//printf("vsnsk_vs_v_key_press\n");
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
void  vsnsk_vs_v_knopka(GtkWidget *widget,class vsnsk_vs_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
//    printf("%s %d\n",data->suma.ravno(),data->vidnal);     

    zap_v_kas(nomer_kas,data->kodkl.ravno(),data->suma.ravno_atof(),1,"",0,0,0,0,kod_operatora,data->koment.ravno());

    
    gtk_widget_destroy(data->window);
    data->window=NULL;
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

void    vsnsk_vs_v_vvod(GtkWidget *widget,class vsnsk_vs_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//printf("vsnsk_vs_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SUMA:
    data->suma.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOMENT:
    data->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }
enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);

}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vsnsk_v_e_knopka(GtkWidget *widget,class vsnsk_vs_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case E_SUMA:

    g_signal_emit_by_name(data->entry[E_SUMA],"activate");

    iceb_mous_klav(gettext("Сумма"),&data->suma,20,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA]),data->suma.ravno());

    return;  

  case E_KOMENT:

    g_signal_emit_by_name(data->entry[E_KOMENT],"activate");

    iceb_mous_klav(gettext("Введите коментарий"),&data->koment,20,1,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOMENT]),data->koment.ravno());

    return;  


 }
}


