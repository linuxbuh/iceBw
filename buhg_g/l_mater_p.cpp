/*$Id: l_mater_p.c,v 1.20 2013/08/25 08:26:37 sasa Exp $*/
/*29.02.2016	05.05.2004	Белых А.И.	l_mater_p.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "l_mater.h"
enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

enum
 {
  E_KOD,
  E_GRUP,
  E_NAIM,
  E_CENA,
  E_MCENA,
  E_EI,
  E_KRAT,
  E_KODTAR,
  E_FAS,
  E_HTRIX,
  E_ARTIK,
  E_KRIOST,
  KOLENTER  
 };

class  mater_data_poi
 {
  public:
  mater_rek *poisk;

  GtkWidget *window;
  GtkWidget *opt1;
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_pv[KOL_PFK];
  class iceb_u_str name_window;
  short     kl_shift; //0-отжата 1-нажата  

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      if(i == E_MCENA)
        continue;
      g_signal_emit_by_name(entry[i],"activate");
     }
    poisk->mcena=gtk_combo_box_get_active (GTK_COMBO_BOX(opt1));
   } 
 };
 
void mater_p_clear(class mater_data_poi *data);
void    mater_p_vvod(GtkWidget *widget,class mater_data_poi *data);
void  mater_p_knopka(GtkWidget *widget,class mater_data_poi *data);
gboolean   mater_p_key_press(GtkWidget *widget,GdkEventKey *event,class mater_data_poi *data);

void l_mater_p(class  mater_rek *data_poi,GtkWidget *wpredok)
{
char strsql[512];
class  mater_data_poi data;
int gor=0,ver=0;
data.poisk=data_poi;

data.kl_shift=0;
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
//g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_widget_destroyed),&data.window);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(mater_p_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.poisk->kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

sprintf(strsql,"%s (,,)",gettext("Код группы"));
label=gtk_label_new(strsql);
data.entry[E_GRUP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GRUP]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP]), data.entry[E_GRUP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GRUP], "activate",G_CALLBACK(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP]),data.poisk->grupa.ravno());
gtk_widget_set_name(data.entry[E_GRUP],iceb_u_inttochar(E_GRUP));

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.poisk->naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

label=gtk_label_new(gettext("Цена"));
data.entry[E_CENA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENA]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_CENA], "activate",G_CALLBACK(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.poisk->cena.ravno());
gtk_widget_set_name(data.entry[E_CENA],iceb_u_inttochar(E_CENA));


label=gtk_label_new(gettext("Цена введена"));
gtk_box_pack_start (GTK_BOX (hbox[E_MCENA]), label, FALSE, FALSE, 0);

class iceb_u_spisok spmenu;
spmenu.plus("");
spmenu.plus(gettext("С НДС"));
spmenu.plus(gettext("Без НДС"));

iceb_pm_vibor(&spmenu,&data.opt1,data.poisk->mcena);
gtk_box_pack_start (GTK_BOX (hbox[E_MCENA]),data.opt1, FALSE, FALSE, 0);


sprintf(strsql,"%s (,,)",gettext("Единица измерения"));
label=gtk_label_new(strsql);
data.entry[E_EI] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_EI]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_EI], "activate",G_CALLBACK(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.poisk->ei.ravno());
gtk_widget_set_name(data.entry[E_EI],iceb_u_inttochar(E_EI));

label=gtk_label_new(gettext("Кратность"));
data.entry[E_KRAT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KRAT]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KRAT]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KRAT]), data.entry[E_KRAT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KRAT], "activate",G_CALLBACK(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KRAT]),data.poisk->krat.ravno());
gtk_widget_set_name(data.entry[E_KRAT],iceb_u_inttochar(E_KRAT));

label=gtk_label_new(gettext("Код тары"));
data.entry[E_KODTAR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODTAR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTAR]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTAR]), data.entry[E_KODTAR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODTAR], "activate",G_CALLBACK(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODTAR]),data.poisk->kodtar.ravno());
gtk_widget_set_name(data.entry[E_KODTAR],iceb_u_inttochar(E_KODTAR));

label=gtk_label_new(gettext("Фасовка"));
data.entry[E_FAS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FAS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_FAS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FAS]), data.entry[E_FAS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FAS], "activate",G_CALLBACK(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FAS]),data.poisk->fas.ravno());
gtk_widget_set_name(data.entry[E_FAS],iceb_u_inttochar(E_FAS));

label=gtk_label_new(gettext("Штрих код"));
data.entry[E_HTRIX] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_HTRIX]),49);
gtk_box_pack_start (GTK_BOX (hbox[E_HTRIX]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_HTRIX]), data.entry[E_HTRIX], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_HTRIX], "activate",G_CALLBACK(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HTRIX]),data.poisk->htrix.ravno());
gtk_widget_set_name(data.entry[E_HTRIX],iceb_u_inttochar(E_HTRIX));

label=gtk_label_new(gettext("Артикул"));
data.entry[E_ARTIK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ARTIK]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_ARTIK]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_ARTIK]), data.entry[E_ARTIK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ARTIK], "activate",G_CALLBACK(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ARTIK]),data.poisk->artik.ravno());
gtk_widget_set_name(data.entry[E_ARTIK],iceb_u_inttochar(E_ARTIK));

label=gtk_label_new(gettext("Критичный остаток"));
data.entry[E_KRIOST] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KRIOST]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KRIOST]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KRIOST]), data.entry[E_KRIOST], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KRIOST], "activate",G_CALLBACK(mater_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KRIOST]),data.poisk->kriost.ravno());
gtk_widget_set_name(data.entry[E_KRIOST],iceb_u_inttochar(E_KRIOST));


sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka_pv[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka_pv[PFK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka_pv[PFK2],"clicked",G_CALLBACK(mater_p_knopka),&data);
gtk_widget_set_name(data.knopka_pv[PFK2],iceb_u_inttochar(PFK2));

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka_pv[PFK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka_pv[PFK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka_pv[PFK4],"clicked",G_CALLBACK(mater_p_knopka),&data);
gtk_widget_set_name(data.knopka_pv[PFK4],iceb_u_inttochar(PFK4));


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka_pv[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka_pv[PFK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka_pv[PFK10],"clicked",G_CALLBACK(mater_p_knopka),&data);
gtk_widget_set_name(data.knopka_pv[PFK10],iceb_u_inttochar(PFK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka_pv[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka_pv[PFK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka_pv[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   mater_p_key_press(GtkWidget *widget,GdkEventKey *event,class mater_data_poi *data)
{
//char  bros[512];

//printf("vmater_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka_pv[PFK2],"clicked");
    return(FALSE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka_pv[PFK4],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka_pv[PFK10],"clicked");
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
void  mater_p_knopka(GtkWidget *widget,class mater_data_poi *data)
{
//char bros[512];

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case PFK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->poisk->metka_poi=1;
    return;  

  case PFK4:
    data->poisk->clear_zero();
    return;  


  case PFK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->poisk->metka_poi=0;

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    mater_p_vvod(GtkWidget *widget,class mater_data_poi *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KOD:
    data->poisk->kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GRUP:
    data->poisk->grupa.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NAIM:
    data->poisk->naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_CENA:
    data->poisk->cena.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    enter+=1;
    break;

  case E_EI:
    data->poisk->ei.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KRAT:
    data->poisk->krat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODTAR:
    data->poisk->kodtar.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_FAS:
    data->poisk->fas.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_HTRIX:
    data->poisk->htrix.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_ARTIK:
    data->poisk->artik.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KRIOST:
    data->poisk->kriost.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Очистка меню от введенной информации*/
/**************************************/
void mater_p_clear(class mater_data_poi *data)
{

data->poisk->clear_zero();
for(int i=0; i< KOLENTER; i++)
 { 
  if(i == E_MCENA)
    continue;
  gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");
 }

gtk_widget_grab_focus(data->entry[0]);

}
/**********************/
/*Читаем              */
/**********************/

void   mater_get_pm0(GtkWidget *widget,int *data)
{
*data=0;
printf("mater_get_pm0-%d\n",*data);
}
void   mater_get_pm1(GtkWidget *widget,int *data)
{
*data=1;
printf("mater_get_pm1-%d\n",*data);
}
void   mater_get_pm2(GtkWidget *widget,int *data)
{
*data=2;
printf("mater_get_pm2-%d\n",*data);
}
