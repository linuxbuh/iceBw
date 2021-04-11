/*$Id: l_zarsdo_p.c,v 1.19 2013/08/25 08:26:45 sasa Exp $*/
/*16.04.2017	30.05.2016	Белых А.И.	l_zarsdo_p.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "l_zarsdo.h"
enum
 {
  E_TABNOM,
  E_DATAZ,
  E_SHET,
  E_KOD_NAH,
  E_KOMENT,
  E_FIO,
  E_PODR,
  E_SPOSOBR,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_zarsdo_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_zarsdo_rek *rk;
  
  
  l_zarsdo_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM])));
    rk->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FIO])));
    rk->dataz.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAZ])));
    rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    rk->kod_nah.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_NAH])));
    rk->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
    rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODR])));

    rk->metka_sr=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));
   }

  void clear_data()
   {
    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void l_zarsdo_p_clear(class l_zarsdo_p_data *data);
void    l_zarsdo_p_vvod(GtkWidget *widget,class l_zarsdo_p_data *data);
void  l_zarsdo_p_knopka(GtkWidget *widget,class l_zarsdo_p_data *data);
gboolean   l_zarsdo_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsdo_p_data *data);
void  l_zarsdo_p_e_knopka(GtkWidget *widget,class l_zarsdo_p_data *data);


int l_zarsdo_p(class l_zarsdo_rek *rek_poi,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class l_zarsdo_p_data data;
char strsql[512];
data.rk=rek_poi;
data.name_window.plus(__FUNCTION__);


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zarsdo_p_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
 gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 0);


sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_TABNOM],"clicked",G_CALLBACK(l_zarsdo_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_TABNOM],iceb_u_inttochar(E_TABNOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_TABNOM],gettext("Поиск в списке работников"));

data.entry[E_TABNOM] = gtk_entry_new();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_TABNOM]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABNOM], "activate",G_CALLBACK(l_zarsdo_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.rk->tabnom.ravno());
gtk_widget_set_name(data.entry[E_TABNOM],iceb_u_inttochar(E_TABNOM));/******************/

sprintf(strsql,"%s (,,)",gettext("Фамилия"));
GtkWidget *label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);

data.entry[E_FIO] = gtk_entry_new();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FIO]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO], "activate",G_CALLBACK(l_zarsdo_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rk->fio.ravno());
gtk_widget_set_name(data.entry[E_FIO],iceb_u_inttochar(E_FIO));/******************/

sprintf(strsql,"%s (,,)",gettext("Код начисления"));
data.knopka_enter[E_KOD_NAH]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NAH]), data.knopka_enter[E_KOD_NAH], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_NAH],"clicked",G_CALLBACK(l_zarsdo_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_NAH],iceb_u_inttochar(E_KOD_NAH));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_NAH],gettext("Поиск в справочнике начислений"));

data.entry[E_KOD_NAH] = gtk_entry_new();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_NAH]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NAH]), data.entry[E_KOD_NAH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_NAH], "activate",G_CALLBACK(l_zarsdo_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_NAH]),data.rk->kod_nah.ravno());
gtk_widget_set_name(data.entry[E_KOD_NAH],iceb_u_inttochar(E_KOD_NAH));/******************/


sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(l_zarsdo_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(l_zarsdo_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));/******************/


sprintf(strsql,"%s (,,)",gettext("Коментарий"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_zarsdo_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk->koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));/******************/


sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(l_zarsdo_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор подразделения в справочнике"));

data.entry[E_PODR] = gtk_entry_new();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PODR]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(l_zarsdo_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));/******************/


sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("м.г"));
data.knopka_enter[E_DATAZ]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAZ]), data.knopka_enter[E_DATAZ], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAZ],"clicked",G_CALLBACK(l_zarsdo_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAZ],iceb_u_inttochar(E_DATAZ));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAZ],gettext("Выбор даты"));

data.entry[E_DATAZ] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAZ]),7);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAZ]), data.entry[E_DATAZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAZ], "activate",G_CALLBACK(l_zarsdo_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAZ]),data.rk->dataz.ravno());
gtk_widget_set_name(data.entry[E_DATAZ],iceb_u_inttochar(E_DATAZ));/******************/

label=gtk_label_new(gettext("Способ расчёта"));
gtk_box_pack_start (GTK_BOX (hbox[E_SPOSOBR]), label, FALSE, FALSE, 0);

class l_zarsdo_menu_mr spmenu;
spmenu.strsr.plus(gettext("Все способы расчёта"));

iceb_pm_vibor(&spmenu.strsr,&data.opt,spmenu.strsr.kolih()-1);
gtk_box_pack_start (GTK_BOX (hbox[E_SPOSOBR]),data.opt, TRUE, TRUE,1);





sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_zarsdo_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_zarsdo_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_zarsdo_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
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
void  l_zarsdo_p_e_knopka(GtkWidget *widget,class l_zarsdo_p_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_TABNOM:

     if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
        data->rk->tabnom.z_plus(kod.ravno());      
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->rk->tabnom.ravno());
      
    return;  

  case E_DATAZ:

    if(iceb_calendar1(&data->rk->dataz,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAZ]),data->rk->dataz.ravno());
      
    return;  


   
  case E_SHET:

    iceb_vibrek(0,"Plansh",&data->rk->shet,&naim,data->window);
      
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno());

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

 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarsdo_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsdo_p_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(FALSE);

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
void  l_zarsdo_p_knopka(GtkWidget *widget,class l_zarsdo_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    data->voz=0;
    data->rk->metka_poi=1;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->rk->metka_poi=0;
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_zarsdo_p_vvod(GtkWidget *widget,class l_zarsdo_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
switch (enter)
 {
  case E_TABNOM:
    data->rk->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_TABNOM])));
    break;
  case E_FIO:
    data->rk->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_FIO])));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
