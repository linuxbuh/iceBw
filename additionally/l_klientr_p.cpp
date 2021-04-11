/*$Id: l_klientr_p.c,v 1.13 2013/08/13 06:10:23 sasa Exp $*/
/*14.03.2007	01.03.2004	Белых А.И.	l_klientr_p.c
Ввод и коррктировка реквизитов поиска
*/
#include <unistd.h>
#include "i_rest.h"
#include "klient.h"

enum
{
 FK2,
 FK4,
 FK10,
 KOL_F_KL
};

enum
{
 E_KOD,
 E_FIO,
 E_ADRES,
 E_TELEFON,
 E_PROCENT,
 E_KODGR,
 E_KOMENT,
 KOLENTER
};

class klientr_p_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *entry[KOLENTER];
  GtkWidget *opt;
  short     kl_shift; //0-отжата 1-нажата  

  int       voz;
       
  class klient_rek_data *poi;
  iceb_u_str kodkl;
  iceb_u_spisok menu_kodgr;
  int           vib_kodgr;
      
  klientr_p_data()
   {
    vib_kodgr=0;
    voz=0;
    kl_shift=0;
   }
  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
  
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    poi->clear_data();
   }
 };


gboolean   klientr_p_key_press(GtkWidget *widget,GdkEventKey *event,class klientr_p_data *data);
void  klientr_p_knopka(GtkWidget *widget,class klientr_p_data *data);
void    klientr_p_vvod(GtkWidget *widget,class klientr_p_data *data);
void  l_klientr_p_v_e_knopka(GtkWidget *widget,class klientr_p_data *data);

extern SQL_baza	bd;
extern char *name_system;


int l_klientr_p(class klient_rek_data *datap,GtkWidget *wpredok)
{
char strsql[300];

class klientr_p_data data;
data.poi=datap;
printf("l_klientr_p\n");
    
data.kl_shift=0;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));


gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
//g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_widget_destroyed),&data.window);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(klientr_p_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0 ; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0 ; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkWidget *label=NULL;

sprintf(strsql,"%s",gettext("Код"));
data.knopka_enter[E_KOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.knopka_enter[E_KOD], FALSE, FALSE, 1);
g_signal_connect(data.knopka_enter[E_KOD],"clicked",G_CALLBACK(l_klientr_p_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD],iceb_u_inttochar(E_KOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD],gettext("Введение кода клиента"));

data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(klientr_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.poi->kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Фамилия клиента"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);

data.entry[E_FIO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FIO]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO], "activate",G_CALLBACK(klientr_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.poi->fio.ravno());
gtk_widget_set_name(data.entry[E_FIO],iceb_u_inttochar(E_FIO));

label=gtk_label_new(gettext("Адрес"));
data.entry[E_ADRES] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ADRES]),99);
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES]), data.entry[E_ADRES], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ADRES], "activate",G_CALLBACK(klientr_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRES]),data.poi->adres.ravno());
gtk_widget_set_name(data.entry[E_ADRES],iceb_u_inttochar(E_ADRES));


label=gtk_label_new(gettext("Телефон"));
data.entry[E_TELEFON] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_TELEFON]),39);
gtk_box_pack_start (GTK_BOX (hbox[E_TELEFON]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_TELEFON]), data.entry[E_TELEFON], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TELEFON], "activate",G_CALLBACK(klientr_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TELEFON]),data.poi->telef.ravno());
gtk_widget_set_name(data.entry[E_TELEFON],iceb_u_inttochar(E_TELEFON));

label=gtk_label_new(gettext("Процент скидки"));
data.entry[E_PROCENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PROCENT]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_PROCENT]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_PROCENT]), data.entry[E_PROCENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PROCENT], "activate",G_CALLBACK(klientr_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROCENT]),data.poi->pr_sk.ravno());
gtk_widget_set_name(data.entry[E_PROCENT],iceb_u_inttochar(E_PROCENT));


sprintf(strsql,"%s (,,)",gettext("Группа"));
data.knopka_enter[E_KODGR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.knopka_enter[E_KODGR], FALSE, FALSE, 1);
g_signal_connect(data.knopka_enter[E_KODGR],"clicked",G_CALLBACK(l_klientr_p_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODGR],iceb_u_inttochar(E_KODGR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODGR],gettext("Выбор группы"));

data.entry[E_KODGR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.entry[E_KODGR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODGR], "activate",G_CALLBACK(klientr_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODGR]),data.poi->kodgr.ravno());
gtk_widget_set_name(data.entry[E_KODGR],iceb_u_inttochar(E_KODGR));

label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(klientr_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.poi->koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));



sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(klientr_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(klientr_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без поиска нужных записей"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(klientr_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

//printf("l_klientr_p end\n");

return(data.voz);



}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_klientr_p_v_e_knopka(GtkWidget *widget,class klientr_p_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("go_v_e_knopka knop=%d\n",knop);

switch ((gint)knop)
 {
  case E_KOD:
   iceb_mous_klav(gettext("Введите код клиента"),&data->poi->kod,20,1,0,1,data->window);
   gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->poi->kod.ravno());
   return; 
 
  case E_KODGR:
   if(l_grkl(1,&kod,&naim,data->window) == 0)
    data->poi->kodgr.z_plus(kod.ravno());  
   gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGR]),data->poi->kodgr.ravno());
   return; 
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   klientr_p_key_press(GtkWidget *widget,GdkEventKey *event,class klientr_p_data *data)
{
//char  bros[300];

//printf("klientr_p_key_press\n");
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
    return(TRUE);
//    return(FALSE);

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
void  klientr_p_knopka(GtkWidget *widget,class klientr_p_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
naim.plus("");
kod.plus("");

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek();
    
    gtk_widget_destroy(data->window);
    data->voz=1;     
    
    return;  

  case FK4:
    data->clear_rek();
    return;


  case FK10:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    klientr_p_vvod(GtkWidget *widget,class klientr_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("klientr_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->poi->kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_FIO:
    data->poi->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOMENT:
    data->poi->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_ADRES:
    data->poi->adres.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_TELEFON:
    data->poi->telef.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PROCENT:
    data->poi->pr_sk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODGR:
    data->poi->kodgr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
