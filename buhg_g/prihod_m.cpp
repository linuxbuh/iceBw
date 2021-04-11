/*$Id: prihod_m.c,v 1.12 2014/02/28 05:21:00 sasa Exp $*/
/*12.07.2015	21.06.2006	Белых А.И.	prihod_m.c
Ввод и корректировка 
*/
#include "buhg_g.h"

enum
 {
  E_KOLIH,
  E_SHET_POL,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class prihod_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  class iceb_u_str kolih;
  class iceb_u_str shet_pol;
  
  
  prihod_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    kolih.plus("");
    shet_pol.plus("");
   }

  void read_rek()
   {
    kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIH])));
    shet_pol.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_POL])));
   }
 };


gboolean   prihod_m_key_press(GtkWidget *widget,GdkEventKey *event,class prihod_m_data *data);
void  prihod_m_knopka(GtkWidget *widget,class prihod_m_data *data);
void    prihod_m_vvod(GtkWidget *widget,class prihod_m_data *data);
int prihod_m_pk(const char *god,GtkWidget*);
int prihod_m_zap(class prihod_m_data *data);
void  prihod_m_e_knopka(GtkWidget *widget,class prihod_m_data *data);


extern SQL_baza  bd;

int prihod_m(class iceb_u_str *kolih,class iceb_u_str *shet_pol,GtkWidget *wpredok)
{

class prihod_m_data data;
char strsql[512];
iceb_u_str kikz;

printf("%s\n",__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
label=gtk_label_new(gettext("Ввод новой записи"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(prihod_m_key_press),&data);

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
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Количество"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);

data.entry[E_KOLIH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLIH]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLIH], "activate",G_CALLBACK(prihod_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_widget_set_name(data.entry[E_KOLIH],iceb_u_inttochar(E_KOLIH));

sprintf(strsql,"%s",gettext("Счёт получения"));
data.knopka_enter[E_SHET_POL]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_POL]), data.knopka_enter[E_SHET_POL], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHET_POL],"clicked",G_CALLBACK(prihod_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET_POL],iceb_u_inttochar(E_SHET_POL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET_POL],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET_POL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET_POL]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_POL]), data.entry[E_SHET_POL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET_POL], "activate",G_CALLBACK(prihod_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_POL]),data.shet_pol.ravno());
gtk_widget_set_name(data.entry[E_SHET_POL],iceb_u_inttochar(E_SHET_POL));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(prihod_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(prihod_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  kolih->new_plus(data.kolih.ravno());
  shet_pol->new_plus(data.shet_pol.ravno());
 }
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  prihod_m_e_knopka(GtkWidget *widget,class prihod_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_SHET_POL:

    iceb_vibrek(1,"Plansh",&data->shet_pol,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_POL]),data->shet_pol.ravno());

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   prihod_m_key_press(GtkWidget *widget,GdkEventKey *event,class prihod_m_data *data)
{
//char  bros[512];

//printf("prihod_m_key_press\n");
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
void  prihod_m_knopka(GtkWidget *widget,class prihod_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("prihod_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(prihod_m_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
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

void    prihod_m_vvod(GtkWidget *widget,class prihod_m_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("prihod_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOLIH:
    data->kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_SHET_POL:
    data->shet_pol.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int prihod_m_zap(class prihod_m_data *data)
{

struct OPSHET rshet;
if(data->shet_pol.getdlinna() > 1)
 if(iceb_prsh1(data->shet_pol.ravno(),&rshet,data->window) != 0)
  return(1);




return(0);

}
