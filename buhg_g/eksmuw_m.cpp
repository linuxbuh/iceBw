/*$Id: eksmuw_m.c,v 1.3 2013/09/26 09:46:47 sasa Exp $*/
/*10.07.2015	18.06.2013	Белых А.И.	eksmuw_m.c
Меню для получения отчёта 
*/
#include "buhg_g.h"
#include "eksmuw.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAOST,
  E_KODSKL,
  E_SHETUH,
  KOLENTER  
 };

class eksmuw_data
 {
  public:
  eksmuw_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  eksmuw_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->dataost.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAOST])));
    rk->kodskl.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODSKL])));
    rk->shetuh.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHETUH])));
   }

  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }

 };




gboolean   eksmuw_v_key_press(GtkWidget *widget,GdkEventKey *event,class eksmuw_data *data);
void    eksmuw_v_vvod(GtkWidget *widget,class eksmuw_data *data);
void  eksmuw_v_knopka(GtkWidget *widget,class eksmuw_data *data);
void  eksmuw_v_e_knopka(GtkWidget *widget,class eksmuw_data *data);


extern SQL_baza bd;

int eksmuw_m(class eksmuw_rr *rek_ras,GtkWidget *wpredok)
{
eksmuw_data data;

data.rk=rek_ras;
if(data.rk->dataost.getdlinna() <= 1)
 data.rk->dataost.plus_tek_dat();
char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Экспорт"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(eksmuw_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

class iceb_u_str repl;
repl.plus(gettext("Материальный учёт"));

repl.ps_plus(gettext("Экспорт остатков по материалам"));
GtkWidget *label=gtk_label_new(repl.ravno());

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, FALSE, FALSE, 0);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, FALSE, FALSE, 0);


sprintf(strsql,"%s",gettext("Дата остатка"));
data.knopka_enter[E_DATAOST]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAOST]), data.knopka_enter[E_DATAOST], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAOST],"clicked",G_CALLBACK(eksmuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAOST],iceb_u_inttochar(E_DATAOST));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAOST],gettext("Выбор даты"));

data.entry[E_DATAOST] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAOST]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAOST]), data.entry[E_DATAOST], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAOST], "activate",G_CALLBACK(eksmuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAOST]),data.rk->dataost.ravno());
gtk_widget_set_name(data.entry[E_DATAOST],iceb_u_inttochar(E_DATAOST));


sprintf(strsql,"%s (,,)",gettext("Код склада"));
data.knopka_enter[E_KODSKL]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODSKL]), data.knopka_enter[E_KODSKL], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODSKL],"clicked",G_CALLBACK(eksmuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODSKL],iceb_u_inttochar(E_KODSKL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODSKL],gettext("Выбор склада"));

data.entry[E_KODSKL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODSKL]), data.entry[E_KODSKL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODSKL], "activate",G_CALLBACK(eksmuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODSKL]),data.rk->kodskl.ravno());
gtk_widget_set_name(data.entry[E_KODSKL],iceb_u_inttochar(E_KODSKL));


sprintf(strsql,"%s (,,)",gettext("Счёт учёта"));
data.knopka_enter[E_SHETUH]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETUH]), data.knopka_enter[E_SHETUH], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHETUH],"clicked",G_CALLBACK(eksmuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETUH],iceb_u_inttochar(E_SHETUH));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETUH],gettext("Выбор счёта"));

data.entry[E_SHETUH] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETUH]), data.entry[E_SHETUH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHETUH], "activate",G_CALLBACK(eksmuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETUH]),data.rk->shetuh.ravno());
gtk_widget_set_name(data.entry[E_SHETUH],iceb_u_inttochar(E_SHETUH));




sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(eksmuw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(eksmuw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(eksmuw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  eksmuw_v_e_knopka(GtkWidget *widget,class eksmuw_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATAOST:

    if(iceb_calendar(&data->rk->dataost,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAOST]),data->rk->dataost.ravno());
      
    return;  


  case E_KODSKL:
    if(iceb_l_sklad(1,&kod,&naim,data->window) == 0)
     data->rk->kodskl.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODSKL]),data->rk->kodskl.ravno());
    return;  



  case E_SHETUH:
    iceb_vibrek(0,"Plansh",&data->rk->shetuh,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETUH]),data->rk->shetuh.ravno());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   eksmuw_v_key_press(GtkWidget *widget,GdkEventKey *event,class eksmuw_data *data)
{

//printf("eksmuw_v_key_press\n");
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
void  eksmuw_v_knopka(GtkWidget *widget,class eksmuw_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->rk->dataost.prov_dat() != 0)
      return;

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

void    eksmuw_v_vvod(GtkWidget *widget,class eksmuw_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("eksmuw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAOST:
    data->rk->dataost.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODSKL:
    data->rk->kodskl.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_SHETUH:
    data->rk->shetuh.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
