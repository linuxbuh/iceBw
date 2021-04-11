/*$Id: sklkar_m.c,v 1.18 2013/09/26 09:46:55 sasa Exp $*/
/*12.07.2015	25.05.2004	Белых А.И.	sklkar_m.c
Меню задания просмотра карточки материалла
*/
#include "buhg_g.h"

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
  E_SKL,
  E_NKART,
  KOLENTER  
 };

class sklkar_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  static iceb_u_str skl;
  static iceb_u_str n_kar;

  sklkar_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
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
    clear();
   }
  void clear()
   {
    skl.new_plus("");
    n_kar.new_plus("");
   }
 };

iceb_u_str sklkar_m_data::skl;
iceb_u_str sklkar_m_data::n_kar;

void    sklkar_m_v_vvod(GtkWidget *widget,class sklkar_m_data *data);
void  sklkar_m_v_knopka(GtkWidget *widget,class sklkar_m_data *data);
void   sklkar_m_rekviz(class sklkar_m_data *data);
gboolean   sklkar_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class sklkar_m_data *data);

extern SQL_baza bd;

int sklkar_m(iceb_u_str *skl,iceb_u_str *n_kar,GtkWidget *wpredok)
{
sklkar_m_data data;


char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Просмотр карточки материалла"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(sklkar_m_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Просмотр карточки материалла"));

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
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

sprintf(strsql,"%s",gettext("Склад"));
label=gtk_label_new(strsql);
data.entry[E_SKL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SKL]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_SKL]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SKL]), data.entry[E_SKL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SKL], "activate",G_CALLBACK(sklkar_m_v_vvod),&data);
if(data.skl.ravno() != NULL)
 gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKL]),data.skl.ravno());
gtk_widget_set_name(data.entry[E_SKL],iceb_u_inttochar(E_SKL));

label=gtk_label_new(gettext("Номер карточки"));
data.entry[E_NKART] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NKART]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_NKART]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NKART]), data.entry[E_NKART], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NKART], "activate",G_CALLBACK(sklkar_m_v_vvod),&data);
if(data.n_kar.ravno() != NULL)
 gtk_entry_set_text(GTK_ENTRY(data.entry[E_NKART]),data.n_kar.ravno());
gtk_widget_set_name(data.entry[E_NKART],iceb_u_inttochar(E_NKART));


sprintf(strsql,"F2 %s",gettext("Просмотр"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать просмотр"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(sklkar_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Просмотр списка складов"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(sklkar_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(sklkar_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(sklkar_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 {
  skl->new_plus(data.skl.ravno());
  n_kar->new_plus(data.n_kar.ravno());
 }
 
return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   sklkar_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class sklkar_m_data *data)
{

//printf("sklkar_m_v_key_press\n");
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
void  sklkar_m_v_knopka(GtkWidget *widget,class sklkar_m_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

  case FK3:
    iceb_vibrek(1,"Sklad",&data->skl,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKL]),data->skl.ravno());
    return;

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    sklkar_m_v_vvod(GtkWidget *widget,class sklkar_m_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//g_print("sklkar_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SKL:
    data->skl.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NKART:
    data->n_kar.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
