/*$Id: vvod_tabnom.c,v 1.16 2013/08/25 08:26:59 sasa Exp $*/
/*13.07.2015	20.09.2006	Белых А.И.	vvod_tabnom.c
Ввод табельного номера
*/
#include "buhg_g.h"

enum
{
 FK2,
// FK4,
 FK10,
 KOL_F_KL
};

enum
{
 E_TABNOM,
 KOLENTER
};


class vvod_tabnom_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *opt;
  short     kl_shift; //0-отжата 1-нажата  

  int       voz;
  iceb_u_str tabnom;
       
  vvod_tabnom_data()
   {
    voz=0;
    kl_shift=0;
   }
  void read_rek()
   {
    tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM])));
   }
  
 };


gboolean   vvod_tabnom_key_press(GtkWidget *widget,GdkEventKey *event,class vvod_tabnom_data *data);
void  vvod_tabnom_knopka(GtkWidget *widget,class vvod_tabnom_data *data);
void    vvod_tabnom_vvod(GtkWidget *widget,class vvod_tabnom_data *data);
int l_vrint_zap(class vvod_tabnom_data *data);
void l_vrint_kodgr(class vvod_tabnom_data *data);
void  vvod_tabnom_e_knopka(GtkWidget *widget,class vvod_tabnom_data *data);

extern SQL_baza	bd;


int vvod_tabnom(class iceb_u_str *tabnom,GtkWidget *wpredok)
{
char strsql[512];
class vvod_tabnom_data data;

//printf("vvod_tabnom\n");
    
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/


g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
//g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_widget_destroyed),&data.window);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vvod_tabnom_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label;
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод табельного номера"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

label=gtk_label_new(gettext("Введите табельный номер"));


GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER+1];
for(int i=0 ; i < KOLENTER+1; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0 ; i < KOLENTER+1; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(gettext("Табельный номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_TABNOM],"clicked",G_CALLBACK(vvod_tabnom_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_TABNOM],iceb_u_inttochar(E_TABNOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера."));


data.entry[E_TABNOM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_TABNOM]),11);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABNOM], "activate",G_CALLBACK(vvod_tabnom_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),"");
gtk_widget_set_name(data.entry[E_TABNOM],iceb_u_inttochar(E_TABNOM));




sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации."));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vvod_tabnom_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации."));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vvod_tabnom_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 tabnom->new_plus(data.tabnom.ravno());

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vvod_tabnom_e_knopka(GtkWidget *widget,class vvod_tabnom_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");
int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_TABNOM:

//    g_signal_emit_by_name(data->entry[E_NOMST],"activate");

    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     {
      data->tabnom.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->tabnom.ravno());
     }
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vvod_tabnom_key_press(GtkWidget *widget,GdkEventKey *event,class vvod_tabnom_data *data)
{

//printf("vvod_tabnom_key_press\n");
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
void  vvod_tabnom_knopka(GtkWidget *widget,class vvod_tabnom_data *data)
{
char strsql[512];

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();    

    sprintf(strsql,"select tabn from Kartb where tabn=%d",data->tabnom.ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),data->tabnom.ravno_atoi());
      iceb_menu_soob(strsql,data->window);
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

void    vvod_tabnom_vvod(GtkWidget *widget,class vvod_tabnom_data *data)
{
char strsql[512];

int enter=atoi(gtk_widget_get_name(widget));
//g_print("vvod_tabnom_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_TABNOM:
    data->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->tabnom.getdlinna() <= 1)
     break;
    //Проверяем код подразделения
    sprintf(strsql,"select tabn from Kartb where tabn=%d",data->tabnom.ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),data->tabnom.ravno_atoi());
      iceb_menu_soob(strsql,data->window);
      return;
     }
    data->voz=0;
    gtk_widget_destroy(data->window);
    break;
 }

 
}
