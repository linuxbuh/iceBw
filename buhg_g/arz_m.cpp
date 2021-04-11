/*$Id: arz_m.c,v 1.16 2013/08/25 08:26:31 sasa Exp $*/
/*13.11.2017	08.11.2006	Белых А.И.	arz_m.c
Меню для автоматической розноски зарплаты
*/
#include "buhg_g.h"
#include "arz.h"
enum
 {
  E_DATA,
  E_TABNOM,
  E_DATA_PER,
  E_KOD_NAH,
  E_PODR,
  E_KOD_KAT,
  E_KOD_GRUP_POD,  
  E_PROC_VIP,
  E_MAX_SUM,
  E_KOMENT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class arz_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton2[2];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class arz_rek *rk;
  
  arz_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->data.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA])));
    rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODR])));
    rk->kod_nah.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_NAH])));
    rk->kod_kat.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_KAT])));
    rk->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM])));
    rk->proc_vip.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PROC_VIP])));
    rk->kod_grup_pod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_GRUP_POD])));
    rk->data_per.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_PER])));
    rk->max_sum.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MAX_SUM])));
    rk->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
    
   }
  
  void clear_rek()
   {

    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
    gtk_entry_set_text(GTK_ENTRY(entry[E_PROC_VIP]),rk->proc_vip.ravno());
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton2[rk->metka_saldo]),TRUE); //Устанавливем активной кнопку

   }
 };


gboolean   arz_m_key_press(GtkWidget *widget,GdkEventKey *event,class arz_m_data *data);
void  arz_m_knopka(GtkWidget *widget,class arz_m_data *data);
void    arz_m_vvod(GtkWidget *widget,class arz_m_data *data);

void  arz_radio2_0(GtkWidget *widget,class arz_m_data *data);
void  arz_radio2_1(GtkWidget *widget,class arz_m_data *data);

void  arz_m_e_knopka(GtkWidget *widget,class arz_m_data *data);

extern SQL_baza  bd;

int arz_m(class arz_rek *rek,GtkWidget *wpredok)
{

class arz_m_data data;
char strsql[512];
iceb_u_str kikz;
data.rk=rek;
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Автоматическая разноска зарплаты"));
label=gtk_label_new(gettext("Автоматическая разноска зарплаты"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(arz_m_key_press),&data);

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
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator3=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator1);
//Вставляем радиокнопки
GSList *group;

data.radiobutton2[0]=gtk_radio_button_new_with_label(NULL,gettext("С учётом сальдо"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton2[0], "clicked",G_CALLBACK(arz_radio2_0),&data);

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton2[0]));

data.radiobutton2[1]=gtk_radio_button_new_with_label(group,gettext("Без учёта сальдо"));
g_signal_connect(data.radiobutton2[1], "clicked",G_CALLBACK(arz_radio2_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk->metka_saldo]),TRUE); //Устанавливем активной кнопку

gtk_container_add (GTK_CONTAINER (vbox), separator3);


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);



sprintf(strsql,"%s %s",gettext("Дата"),gettext("(м.г)"));
data.knopka_enter[E_DATA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.knopka_enter[E_DATA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATA],"clicked",G_CALLBACK(arz_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA],iceb_u_inttochar(E_DATA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA],gettext("Выбор даты"));

data.entry[E_DATA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA]),7);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_DATA], "activate",G_CALLBACK(arz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.rk->data.ravno());
gtk_widget_set_name(data.entry[E_DATA],iceb_u_inttochar(E_DATA));


sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_TABNOM],"clicked",G_CALLBACK(arz_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_TABNOM],iceb_u_inttochar(E_TABNOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"));

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABNOM], "activate",G_CALLBACK(arz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.rk->tabnom.ravno());
gtk_widget_set_name(data.entry[E_TABNOM],iceb_u_inttochar(E_TABNOM));


sprintf(strsql,"%s (%s)",gettext("Дата выплаты зарплаты"),gettext("д.м.г"));
data.knopka_enter[E_DATA_PER]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PER]), data.knopka_enter[E_DATA_PER], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATA_PER],"clicked",G_CALLBACK(arz_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_PER],iceb_u_inttochar(E_DATA_PER));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_PER],gettext("Выбор даты"));

data.entry[E_DATA_PER] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_PER]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PER]), data.entry[E_DATA_PER], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_DATA_PER], "activate",G_CALLBACK(arz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_PER]),data.rk->data_per.ravno());
gtk_widget_set_name(data.entry[E_DATA_PER],iceb_u_inttochar(E_DATA_PER));


sprintf(strsql,"%s (,,)",gettext("Код начисления"));
data.knopka_enter[E_KOD_NAH]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NAH]), data.knopka_enter[E_KOD_NAH], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_NAH],"clicked",G_CALLBACK(arz_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_NAH],iceb_u_inttochar(E_KOD_NAH));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_NAH],gettext("Выбор кода начисления"));

data.entry[E_KOD_NAH] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NAH]), data.entry[E_KOD_NAH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_NAH], "activate",G_CALLBACK(arz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_NAH]),data.rk->kod_nah.ravno());
gtk_widget_set_name(data.entry[E_KOD_NAH],iceb_u_inttochar(E_KOD_NAH));


sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(arz_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор подразделения"));

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(arz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));


sprintf(strsql,"%s (,,)",gettext("Код группы подразделения"));
data.knopka_enter[E_KOD_GRUP_POD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GRUP_POD]), data.knopka_enter[E_KOD_GRUP_POD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_GRUP_POD],"clicked",G_CALLBACK(arz_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_GRUP_POD],iceb_u_inttochar(E_KOD_GRUP_POD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_GRUP_POD],gettext("Выбор группы"));

data.entry[E_KOD_GRUP_POD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GRUP_POD]), data.entry[E_KOD_GRUP_POD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_GRUP_POD], "activate",G_CALLBACK(arz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_GRUP_POD]),data.rk->kod_grup_pod.ravno());
gtk_widget_set_name(data.entry[E_KOD_GRUP_POD],iceb_u_inttochar(E_KOD_GRUP_POD));


sprintf(strsql,"%s (,,)",gettext("Код категории"));
data.knopka_enter[E_KOD_KAT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KAT]), data.knopka_enter[E_KOD_KAT], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_KAT],"clicked",G_CALLBACK(arz_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_KAT],iceb_u_inttochar(E_KOD_KAT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_KAT],gettext("Выбор категории"));

data.entry[E_KOD_KAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KAT]), data.entry[E_KOD_KAT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_KAT], "activate",G_CALLBACK(arz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_KAT]),data.rk->kod_kat.ravno());
gtk_widget_set_name(data.entry[E_KOD_KAT],iceb_u_inttochar(E_KOD_KAT));


label=gtk_label_new(gettext("Процент выплаты"));
gtk_box_pack_start (GTK_BOX (hbox[E_PROC_VIP]), label, FALSE, FALSE, 0);

data.entry[E_PROC_VIP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PROC_VIP]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC_VIP]), data.entry[E_PROC_VIP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PROC_VIP], "activate",G_CALLBACK(arz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROC_VIP]),data.rk->proc_vip.ravno());
gtk_widget_set_name(data.entry[E_PROC_VIP],iceb_u_inttochar(E_PROC_VIP));

label=gtk_label_new(gettext("Максимальная сумма выплати"));
gtk_box_pack_start (GTK_BOX (hbox[E_MAX_SUM]), label, FALSE, FALSE, 0);

data.entry[E_MAX_SUM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MAX_SUM]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_MAX_SUM]), data.entry[E_MAX_SUM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_MAX_SUM], "activate",G_CALLBACK(arz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MAX_SUM]),data.rk->max_sum.ravno());
gtk_widget_set_name(data.entry[E_MAX_SUM],iceb_u_inttochar(E_MAX_SUM));


label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),35);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(arz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk->koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));


sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(arz_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(arz_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(arz_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/**************************/

void  arz_radio2_0(GtkWidget *widget,class arz_m_data *data)
{
//g_print("arz_radio0\n");

data->rk->metka_saldo=0;
//printf("prov=0\n");

}
void  arz_radio2_1(GtkWidget *widget,class arz_m_data *data)
{
//g_print("arz_radio1\n");
data->rk->metka_saldo=1;
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  arz_m_e_knopka(GtkWidget *widget,class arz_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATA:
    iceb_calendar1(&data->rk->data,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA]),data->rk->data.ravno());
    return;
  case E_DATA_PER:

    if(iceb_calendar(&data->rk->data_per,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_PER]),data->rk->data_per.ravno());
      
    return;  

   
  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->rk->tabnom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->rk->tabnom.ravno());
    return;
  
  case E_KOD_NAH:
    if(l_zarnah(1,&kod,&naim,data->window) == 0)
     data->rk->kod_nah.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_NAH]),data->rk->kod_nah.ravno());
    return;
  
  case E_PODR:
    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     data->rk->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
    return;
  
  case E_KOD_GRUP_POD:
    if(l_gruppod(1,&kod,&naim,data->window) == 0)
     data->rk->kod_grup_pod.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_GRUP_POD]),data->rk->kod_grup_pod.ravno());
    return;

  case E_KOD_KAT:
    if(l_zarkateg(1,&kod,&naim,data->window) == 0)
     data->rk->kod_kat.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_KAT]),data->rk->kod_kat.ravno());
    return;
    
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   arz_m_key_press(GtkWidget *widget,GdkEventKey *event,class arz_m_data *data)
{
//char  bros[512];

//printf("arz_m_key_press\n");
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
void  arz_m_knopka(GtkWidget *widget,class arz_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("arz_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(data->rk->data.prov_dat1() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата !"),data->window);
      return;
     }
    if(data->rk->data_per.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата выплаты!"),data->window);
      return;
     }
    data->voz=0;
    gtk_widget_destroy(data->window);
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

void    arz_m_vvod(GtkWidget *widget,class arz_m_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("arz_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATA:
    data->rk->data.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_TABNOM:
    data->rk->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATA_PER:
    data->rk->data_per.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_PODR:
    data->rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOD_NAH:
    data->rk->kod_nah.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOD_GRUP_POD:
    data->rk->kod_grup_pod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_PROC_VIP:
    data->rk->proc_vip.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_MAX_SUM:
    data->rk->max_sum.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOD_KAT:
    data->rk->kod_kat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
