/*$Id: vv_kolih.c,v 1.10 2013/09/26 09:47:12 sasa Exp $*/
/*07.01.2007	07.01.2007	Белых А.И.	vv_kolih.c
Проверка возможности входа-выхода в подразделение.
*/
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "i_rest.h"
enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAN,
  E_VREMN,
  E_PODR,
  KOLENTER  
 };

class vv_kolih_data
 {
  public:
  //Реквизиты записи
  class iceb_u_str datan;
  class iceb_u_str vremn;
  class iceb_u_str podr;


  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_naimpodr;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  
  vv_kolih_data() //Конструктор
   {
    kl_shift=0;
    voz=0;
    clear_rek_zap();
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_rek_zap()
   {
    datan.new_plus("");
    vremn.new_plus("");
    podr.new_plus("");
   }
 };

gboolean   vv_kolih_v_key_press(GtkWidget *widget,GdkEventKey *event,class vv_kolih_data *data);
void    vv_kolih_v_vvod(GtkWidget *widget,class vv_kolih_data *data);
void  vv_kolih_v_knopka(GtkWidget *widget,class vv_kolih_data *data);
void uslopp_kkvt(char *skod,char *nkod,GtkWidget *wpredok);

void  vv_kolih_v_e_knopka(GtkWidget *widget,class vv_kolih_data *data);


extern char *name_system;
extern SQL_baza bd;
extern char	*shrt;    /*Счёта розничной торговли*/
extern double	okrcn;  /*Округление цены*/

int vv_kolih(class iceb_u_str *podr,class iceb_u_str *datan,class iceb_u_str *vremn,GtkWidget *wpredok)
{
vv_kolih_data data;
iceb_u_str naim_podr;
naim_podr.plus("");
char strsql[300];

data.podr.new_plus(podr->ravno());
data.datan.new_plus(datan->ravno());
data.vremn.new_plus(vremn->ravno());

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт количества входов/выходов"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vv_kolih_v_key_press),&data);

iceb_u_str zagolov;
zagolov.new_plus(gettext("Расчёт количества входов/выходов"));

GtkWidget *label=gtk_label_new(zagolov.ravno());

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 1);


sprintf(strsql,"%s (%s)",gettext("Дата начала расчёта"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(vv_kolih_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор подразделения"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(vv_kolih_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

sprintf(strsql,"%s (%s)",gettext("Время начала расчёта"),gettext("ч.м.с"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_VREMN]), label, FALSE, FALSE, 0);

data.entry[E_VREMN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VREMN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_VREMN]), data.entry[E_VREMN],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_VREMN], "activate",G_CALLBACK(vv_kolih_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREMN]),data.vremn.ravno());
gtk_widget_set_name(data.entry[E_VREMN],iceb_u_inttochar(E_VREMN));


sprintf(strsql,"%s",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(vv_kolih_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор подразделения"));

data.entry[E_PODR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PODR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(vv_kolih_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));

data.label_naimpodr=gtk_label_new(naim_podr.ravno());
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.label_naimpodr,TRUE, TRUE, 0);


sprintf(strsql,"F2 %s",gettext("Проверка"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Проверка возможности входа/выхода в подразделение"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vv_kolih_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vv_kolih_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
if(data.voz == 0)
 {
  datan->new_plus(data.datan.ravno());
  vremn->new_plus(data.vremn.ravno());
  podr->new_plus(data.podr.ravno());
 }
return(data.voz);
 
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vv_kolih_v_e_knopka(GtkWidget *widget,class vv_kolih_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("vv_kolih_v_e_knopka knop=%d\n",knop);

switch ((gint)knop)
 {

  case E_DATAN:
    iceb_calendar(&data->datan,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->datan.ravno());
    return;
    
  case E_PODR:
    if(rpod_l(1,&kod,&naim,data->window) == 0)
     {
      data->podr.new_plus(kod.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naimpodr),naim.ravno(20));
     }    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->podr.ravno());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vv_kolih_v_key_press(GtkWidget *widget,GdkEventKey *event,class vv_kolih_data *data)
{

//printf("vv_kolih_v_key_press\n");
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
void  vv_kolih_v_knopka(GtkWidget *widget,class vv_kolih_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("vv_kolih_v_knopka knop=%d\n",knop);

switch ((gint)knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->datan.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не ввели дату !"),data->window);
      return;
     }
    if(data->datan.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно ввели дату !"),data->window);
      return;
     }
    if(data->vremn.prov_time() != 0)
     {
      iceb_menu_soob(gettext("Неправильно ввел время !"),data->window);
      return;
     }

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vv_kolih_v_vvod(GtkWidget *widget,class vv_kolih_data *data)
{
char strsql[300];
SQL_str row;
SQLCURSOR cur;
int enter=atoi(gtk_widget_get_name(widget));
//g_print("vv_kolih_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_VREMN:
    data->vremn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PODR:
    data->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    sprintf(strsql,"select naik from Restpod where kod=%d",data->podr.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.20s",row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naimpodr),strsql);
     }    
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
