/*$Id: vsvks.c,v 1.28 2013/08/13 06:10:26 sasa Exp $*/
/*08.10.2007	23.12.2004	Белых А.И.	vsvks.c
Ввод суммы на счёт клиента (кассовый регистратор)
*/
#include <math.h>
#include "i_rest.h"

enum
{
  FK2,
  FK3,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SUMA,
  E_KOMENT,
  E_VIDNAL,
  KOLENTER  
 };

class vsvks_vs_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_suma;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  class iceb_u_str suma;
  struct KASSA *kasr;
  short metka;  //0-внесение денег в кассу 1-выдача денег из кассы
  class iceb_u_str koment;
  double znak_sum; //Либо 1 либо -1
  int vidnal; //0-c ПДВ 1-без ПДВ
  class iceb_u_str kodkl;
  double saldo_kon;
  
  vsvks_vs_data() //Конструктор
   {
    saldo_kon=0.;
    vidnal=0;  
    kl_shift=0;
    voz=1;
    suma.new_plus("");
    kodkl.new_plus("");        
    znak_sum=1;
    koment.plus("");
   }

  void read_rek()
   {
    suma.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA])));
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER-1; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    suma.new_plus("");
    gtk_widget_grab_focus(entry[0]);
   
   }
 };


gboolean   vsvks_vs_v_key_press(GtkWidget *widget,GdkEventKey *event,class vsvks_vs_data *data);
void    vsvks_vs_v_vvod(GtkWidget *widget,class vsvks_vs_data *data);
void  vsvks_vs_v_knopka(GtkWidget *widget,class vsvks_vs_data *data);
void  vsvks_v_e_knopka(GtkWidget *widget,class vsvks_vs_data *data);

GtkWidget *vsvks_vidnal(int *nomerstr);

extern char *name_system;
extern short metka_skreen; //0-сенсорный экран не используется 1-используется
extern int   nomer_kas;  //номер кассы
extern uid_t           kod_operatora;

int vsvks(int metka, //0-внесение денег в кассу 1-выдача из кассы денег
const char *kod_kl,
double saldo_r,
struct KASSA *kasr)
{
class vsvks_vs_data data;

if(kod_kl[0] == '\0')
 {
 if(iceb_mous_klav(gettext("Введите код клиента"),&data.kodkl,20,0,0,1,NULL) != 0)
   return(1);
 }
else
 data.kodkl.new_plus(kod_kl);
   
class iceb_gdite_data gdite;
iceb_gdite(&gdite,1,NULL);

//Проверяем код клиента
char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select fio from Taxiklient where kod='%s'",data.kodkl.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,NULL) != 1)
 {
  iceb_menu_soob(gettext("Не найдено код клиента !"),NULL);
  return(1);
 }

iceb_refresh();

//double saldo_pered_1neopl=0.;
//double saldo_po_kas=0.;
data.saldo_kon=saldo_r;
  
data.kasr=kasr;
data.metka=metka;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(metka_skreen == 1)
  gtk_window_set_default_size(GTK_WINDOW(data.window),-1,400);
iceb_u_str repl;
if(metka == 1)
 {
  repl.plus(gettext("Выдача из кассы клиенту"));
  sprintf(strsql,"%s %s",name_system,
  gettext("Выдача из кассы клиенту"));

  data.znak_sum=-1;
 }
if(metka == 0)
 { 
  repl.plus(gettext("Внесение в кассу клиентом"));
  sprintf(strsql,"%s %s",name_system,gettext("Внесение в кассу клиентом"));
  data.znak_sum=1;
 }

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vsvks_vs_v_key_press),&data);

GtkWidget *label=NULL;


repl.ps_plus(data.kodkl.ravno());
repl.plus(" ");
repl.plus(row[0]);
repl.ps_plus(gettext("Сальдо"));
repl.plus(":");
repl.plus(data.saldo_kon);

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
g_signal_connect(data.knopka_suma,"clicked",G_CALLBACK(vsvks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_suma,iceb_u_inttochar(E_SUMA));


data.entry[E_SUMA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA], "activate",G_CALLBACK(vsvks_vs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno());
gtk_widget_set_name(data.entry[E_SUMA],iceb_u_inttochar(E_SUMA));

data.knopka_suma=gtk_button_new_with_label(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.knopka_suma, FALSE, FALSE, 0);
g_signal_connect(data.knopka_suma,"clicked",G_CALLBACK(vsvks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_suma,iceb_u_inttochar(E_KOMENT));


data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(vsvks_vs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));

label=gtk_label_new(gettext("Нaлог"));
gtk_box_pack_start (GTK_BOX (hbox[E_VIDNAL]), label, FALSE, FALSE, 0);

GtkWidget *menu_vidnal = vsvks_vidnal(&data.vidnal);
gtk_box_pack_start (GTK_BOX (hbox[E_VIDNAL]), menu_vidnal, TRUE, TRUE, 0);


sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Пополнение счёта клиента"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vsvks_vs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Выдача карточки"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Выдача карточки клиенту с вводом на счёт клиента начальной суммы"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(vsvks_vs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(vsvks_vs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vsvks_vs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gdite.close();

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vsvks_vs_v_key_press(GtkWidget *widget,GdkEventKey *event,class vsvks_vs_data *data)
{

//printf("vsvks_vs_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
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
void  vsvks_vs_v_knopka(GtkWidget *widget,class vsvks_vs_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("vsvks_vs_v_knopka knop=%d\n",knop);

switch ((gint)knop)
 {
  case FK3:
     if(fabs(data->saldo_kon) > 0.009)
      {
       iceb_menu_soob(gettext("Карточка имеет не нулевое сальдо !!!"),data->window);
       return;
      }

  case FK2:
    data->read_rek(); //Читаем реквизиты меню
//    printf("%s %d\n",data->suma.ravno(),data->vidnal);     

    if(data->metka == 1) //Выдача денег из кассы
     {
      double suma=data->suma.ravno_atof();
      suma=iceb_u_okrug(suma,0.01);
      if(data->saldo_kon - suma < -0.009)
       {
        char strsql[300];
        sprintf(strsql,"%s\n%f < %f",gettext("Невозможно выдать больше чем остаток на счету !"),
        data->saldo_kon,suma);
        iceb_menu_soob(strsql,data->window);     
        return;
       }  
     }
    if(data->metka < 2)
     if(iceb_mariq_zvs(data->metka,data->suma.ravno_atof(),data->vidnal,data->kasr,data->window) != 0)
      return;

    zap_v_kas(nomer_kas,data->kodkl.ravno(),data->suma.ravno_atof()*data->znak_sum,0,"",0,0,0,0,kod_operatora,data->koment.ravno());

    
    gtk_widget_destroy(data->window);
    data->voz=0;
    if((int)knop == FK3)
     vid_kar_kl(data->kodkl.ravno());
     
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

void    vsvks_vs_v_vvod(GtkWidget *widget,class vsvks_vs_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//printf("vsvks_vs_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SUMA:
    data->suma.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    break;
  case E_KOMENT:
    data->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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

GtkWidget *vsvks_vidnal(int *nomerstr)
{
/******************
size_t nomer=0;

GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();

GtkWidget *item;
item = gtk_menu_item_new_with_label (gettext("С НДС"));
g_signal_connect(item, "activate",G_CALLBACK(iceb_get_menu), nomerstr);
gtk_widget_set_name(item,iceb_u_inttochar(nomer++));
gtk_widget_show (item);

gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Без НДС"));
g_signal_connect(item, "activate",G_CALLBACK(iceb_get_menu), nomerstr);
gtk_widget_set_name(item,iceb_u_inttochar(nomer++));
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);

gtk_option_menu_set_history(GTK_OPTION_MENU(opt),*nomerstr);
*********************/
GtkWidget *opt;
class iceb_u_spisok spmenu;
spmenu.plus(gettext("С НДС"));
spmenu.plus(gettext("Без НДС"));
iceb_pm_vibor(&spmenu,&opt,0);
return(opt);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vsvks_v_e_knopka(GtkWidget *widget,class vsvks_vs_data *data)
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
