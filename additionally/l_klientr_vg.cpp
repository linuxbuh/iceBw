/*$Id: l_klientr_vg.c,v 1.13 2013/08/13 06:10:23 sasa Exp $*/
/*16.08.2006	16.08.2006	Белых А.И.	l_klientr_vg.c
Ввод клиентов в группу
*/
#include "i_rest.h"
#include <unistd.h>
#include "l_klientr.h"
enum
{
 GFK2,
 GFK10,
 G_KOL_F_KL
};

enum
{
 E_KODGR,
 E_KODKL,
 G_KOLENTER
};

class l_klientr_vg_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[G_KOL_F_KL];
  GtkWidget *knopka_enter[G_KOLENTER];
  GtkWidget *entry[G_KOLENTER];
  GtkWidget *opt;
  short     kl_shift; //0-отжата 1-нажата  

  class  klientr_data *data_sp_kl;
       
  iceb_u_str kodkl;
  iceb_u_str kodgr;
  guint     timer;
      
  l_klientr_vg_data()
   {
    kl_shift=0;
    timer=0;
    kodkl.plus("");
    kodgr.plus("");
   }
  void read_rek()
   {
    for(int i=0; i < G_KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
  
  void clear_rek()
   {
    for(int i=0; i < G_KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };

void    l_klientr_vg_vvod(GtkWidget *widget,class l_klientr_vg_data *data);
void  l_klientr_vg_knopka(GtkWidget *widget,class l_klientr_vg_data *data);
gboolean   l_klientr_vg_key_press(GtkWidget *widget,GdkEventKey *event,class l_klientr_vg_data *data);
void  l_klientr_vg_e_knopka(GtkWidget *widget,class l_klientr_vg_data *data);

int l_klient_vg_zap(class l_klientr_vg_data *data);

extern SQL_baza	bd;
extern char *name_system;


void l_klientr_vg(class  klientr_data *data_sp_kl)
{
char strsql[500];
class l_klientr_vg_data data;
data.data_sp_kl=data_sp_kl;





data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
//g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_widget_destroyed),&data.window);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_klientr_vg_key_press),&data);


if(data_sp_kl->window != NULL)
 {
//gdk_window_set_cursor(gtk_widget_get_window(data_sp_kl->window),gdk_cursor_new(ICEB_CURSOR_GDITE));
  gdk_window_set_cursor(gtk_widget_get_window(data_sp_kl->window),gdk_cursor_new_for_display(gtk_widget_get_display(data_sp_kl->window),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(data_sp_kl->window));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label;
sprintf(strsql,"%s",gettext("Ввод клиентов в группу"));

label=gtk_label_new(strsql);

sprintf(strsql,"%s %s",name_system,gettext("Ввод клиентов в группу"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[G_KOLENTER+1];
for(int i=0 ; i < G_KOLENTER+1; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0 ; i < G_KOLENTER+1; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Код группы"));
data.knopka_enter[E_KODGR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.knopka_enter[E_KODGR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODGR],"clicked",G_CALLBACK(l_klientr_vg_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODGR],iceb_u_inttochar(E_KODGR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODGR],gettext("Выбор группы"));

data.entry[E_KODGR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.entry[E_KODGR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODGR], "activate",G_CALLBACK(l_klientr_vg_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODGR]),data.kodgr.ravno());
gtk_widget_set_name(data.entry[E_KODGR],iceb_u_inttochar(E_KODGR));

label=gtk_label_new(gettext("Код клиента"));
data.entry[E_KODKL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODKL]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.entry[E_KODKL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODKL], "activate",G_CALLBACK(l_klientr_vg_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKL]),data.kodkl.ravno());
gtk_widget_set_name(data.entry[E_KODKL],iceb_u_inttochar(E_KODKL));

#ifdef READ_CARD
extern tdcon           td_server; 
if(td_server >= 0)
 {
  class iceb_read_card_enter_data card;
  card.window=data.window;
  card.entry=data.entry[E_KODKL];
  data.timer=card.timer=gtk_timeout_add(500,(GtkFunction)iceb_read_card_enter,&card);
 }
#endif



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[GFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[GFK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[GFK2],"clicked",G_CALLBACK(l_klientr_vg_knopka),&data);
gtk_widget_set_name(data.knopka[GFK2],iceb_u_inttochar(GFK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[GFK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[GFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[GFK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[GFK10],"clicked",G_CALLBACK(l_klientr_vg_knopka),&data);
gtk_widget_set_name(data.knopka[GFK10],iceb_u_inttochar(GFK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[GFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data_sp_kl->window != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(data_sp_kl->window),gdk_cursor_new_for_display(gtk_widget_get_display(data_sp_kl->window),ICEB_CURSOR));
//  gdk_window_set_cursor(gtk_widget_get_window(data_sp_kl->window),gdk_cursor_new(ICEB_CURSOR));

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_klientr_vg_e_knopka(GtkWidget *widget,class l_klientr_vg_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case E_KODGR:
    if(l_grkl(1,&kod,&naim,data->window) == 0)
     {
      data->kodgr.new_plus(kod.ravno());
      data->kodgr.plus(" ");
      data->kodgr.plus(naim.ravno());
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGR]),data->kodgr.ravno());
    return;  

 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_klientr_vg_key_press(GtkWidget *widget,GdkEventKey *event,class l_klientr_vg_data *data)
{

//printf("l_klientr_vg_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[GFK2],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[GFK10],"clicked");
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
void  l_klientr_vg_knopka(GtkWidget *widget,class l_klientr_vg_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case GFK2:
    data->read_rek();

    if(l_klient_vg_zap(data) != 0)
     {
      data->kodkl.new_plus("");
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKL]),data->kodkl.ravno());
     }
    else
     {
      klientr_create_list(data->data_sp_kl);
//    gdk_window_set_cursor(gtk_widget_get_window(data->data_sp_kl->window),gdk_cursor_new(ICEB_CURSOR_GDITE));
      gdk_window_set_cursor(gtk_widget_get_window(data->data_sp_kl->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->data_sp_kl->window),ICEB_CURSOR_GDITE));
      gtk_widget_grab_focus(data->entry[1]);
     }
    return;  


  case GFK10:
    if(data->timer != 0)
        g_source_remove(data->timer);
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_klientr_vg_vvod(GtkWidget *widget,class l_klientr_vg_data *data)
{
char strsql[500];
SQL_str row;
class SQLCURSOR cur;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_klientr_vg_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KODGR:
    data->kodgr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select kod,naik from Grupklient where kod='%s'",data->kodgr.ravno_dpp());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      data->kodgr.new_plus(row[0]); //читает много раз - чтобы наименование всё время не добавлялось
      data->kodgr.plus(" ");
      data->kodgr.plus(row[1]);
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGR]),data->kodgr.ravno());

    break;
  case E_KODKL:
    data->kodkl.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }

enter+=1;
if(enter >= G_KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}

/***********************************/
/*Присвоение группы клиенту и если нет такого клиента ввод новой записи клиента с назначенной группой*/
/************************************/

int l_klient_vg_zap(class l_klientr_vg_data *data)
{
char strsql[500];
//Проверяем есть ли такая группа
sprintf(strsql,"select kod from Grupklient where kod='%s'",data->kodgr.ravno_dpp());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код группы"),data->kodgr.ravno_dpp());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
time_t vrem;
time(&vrem);

//Проверяем есть ли такой клиент
sprintf(strsql,"select fio from Taxiklient where kod='%s'",data->kodkl.ravno());
if(iceb_sql_readkey(strsql,data->window) > 0)
 {
  //клиент есть в базе
  sprintf(strsql,"update Taxiklient set grup='%s',ktoi=%d,vrem=%ld where kod='%s'",
  data->kodgr.ravno_dpp(),iceb_getuid(data->window),vrem,
  data->kodkl.ravno());
 }
else
 {
  
  sprintf(strsql,"insert into Taxiklient (kod,grup,ktoi,vrem) values ('%s','%s',%d,%ld)",
  data->kodkl.ravno(),data->kodgr.ravno_dpp(),iceb_getuid(data->window),vrem);
 }

return(iceb_sql_zapis(strsql,1,0,data->window));

}


