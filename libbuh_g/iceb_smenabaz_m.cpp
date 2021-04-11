/*$Id: iceb_smenabaz_m.c,v 1.14 2013/09/26 09:47:05 sasa Exp $*/
/*17.01.2008	20.01.2005	Белых А.И.	smenabaz_m.c
Меню для смены базы данных
*/
#include "iceb_libbuh.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_IMABAZ,
  E_HOST,
  KOLENTER  
 };

class iceb_smenabaz_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет
  
  iceb_u_str *imabaz;
  iceb_u_str *host;
  
  iceb_smenabaz_m_data() //Конструктор
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
   }
 };

void    iceb_smenabaz_m_v_vvod(GtkWidget *widget,class iceb_smenabaz_m_data *data);
void  iceb_smenabaz_m_v_knopka(GtkWidget *widget,class iceb_smenabaz_m_data *data);
void   iceb_smenabaz_m_rekviz(class iceb_smenabaz_m_data *data);
gboolean   iceb_smenabaz_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_smenabaz_m_data *data);

extern char *name_system;
extern SQL_baza bd;

int   iceb_smenabaz_m(iceb_u_str *imabaz,iceb_u_str *host)
{
iceb_smenabaz_m_data data;

data.imabaz=imabaz;
data.host=host;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Сменить базу"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_smenabaz_m_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Сменить базу."));

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

sprintf(strsql,"%s",gettext("Имя базы данных"));
label=gtk_label_new(strsql);
data.entry[E_IMABAZ] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_IMABAZ]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_IMABAZ]), data.entry[E_IMABAZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_IMABAZ], "activate",G_CALLBACK(iceb_smenabaz_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMABAZ]),data.imabaz->ravno());
gtk_widget_set_name(data.entry[E_IMABAZ],iceb_u_inttochar(E_IMABAZ));

sprintf(strsql,"%s",gettext("Хост на базу"));
label=gtk_label_new(strsql);
data.entry[E_HOST] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HOST]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_HOST]), data.entry[E_HOST], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_HOST], "activate",G_CALLBACK(iceb_smenabaz_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HOST]),data.host->ravno());
gtk_widget_set_name(data.entry[E_HOST],iceb_u_inttochar(E_HOST));


sprintf(strsql,"F2 %s",gettext("Сменить базу."));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Сменить базу."));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(iceb_smenabaz_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации."));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(iceb_smenabaz_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне."));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(iceb_smenabaz_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_smenabaz_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_smenabaz_m_data *data)
{

//printf("iceb_smenabaz_m_v_key_press\n");
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
void  iceb_smenabaz_m_v_knopka(GtkWidget *widget,class iceb_smenabaz_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
/*g_print("iceb_smenabaz_m_v_knopka knop=%d\n",*knop);*/

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
        
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

void    iceb_smenabaz_m_v_vvod(GtkWidget *widget,class iceb_smenabaz_m_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//g_print("iceb_smenabaz_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_IMABAZ:
    data->imabaz->new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_HOST:
    data->host->new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter++;
  
if(enter >= KOLENTER)
 enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
