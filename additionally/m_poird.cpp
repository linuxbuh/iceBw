/*$Id: m_poird.c,v 1.10 2013/08/13 06:10:25 sasa Exp $*/
/*05.03.2004	05.03.2004	Белых А.И.	m_poird.c
Поиск документа по номеру документа
Если вернули 0-ищем
             1-нет
*/
#include <stdlib.h>
#include <unistd.h>
#include "i_rest.h"
enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_NOMD,
  E_GOD,
  KOLENTER
 };


class  m_poird_data
 {
  public:
  iceb_u_str nomd;
  iceb_u_str god;

  
  GtkWidget *entry[KOLENTER];
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       voz;  
  //Конструктор
  m_poird_data()
   {
    nomd.plus("");
    god.plus("");
    voz=0;
    kl_shift=0;
    window=NULL;
   }      
 };

gboolean   m_poird_key_press(GtkWidget *widget,GdkEventKey *event,class m_poird_data *data);
void  m_poird_knopka(GtkWidget *widget,class m_poird_data *data);
void    m_poird_vvod(GtkWidget *widget,class m_poird_data *data);
int restdok_pk(char *god,char *shet,GtkWidget*);

extern SQL_baza	bd;
extern char *name_system;


int  m_poird(iceb_u_str *nomd,short *god,GtkWidget *wpredok)
{
char strsql[300];
m_poird_data data;
iceb_u_str nadpis;
time_t vrem;
struct tm *bf;

time(&vrem);
bf=localtime(&vrem);

printf("l_m_poird\n");
    
data.kl_shift=0;
sprintf(strsql,"%d",bf->tm_year+1900);
data.god.new_plus(strsql);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
//g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_widget_destroyed),&data.window);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(m_poird_key_press),&data);

if(wpredok != NULL)
 {
 
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++) 
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
nadpis.new_plus(gettext("Поиск документа по номеру"));

GtkWidget *label=gtk_label_new(nadpis.ravno());

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox),label);
for(int i=0; i < KOLENTER; i++) 
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

short nomer=0;
label=gtk_label_new(gettext("Номер документа"));
data.entry[E_NOMD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMD]),20);
gtk_box_pack_start (GTK_BOX (hbox[nomer]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[nomer++]), data.entry[E_NOMD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMD], "activate",G_CALLBACK(m_poird_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMD]),data.nomd.ravno());
gtk_widget_set_name(data.entry[E_NOMD],iceb_u_inttochar(E_NOMD));

label=gtk_label_new(gettext("Год"));
data.entry[E_GOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GOD]),5);
gtk_box_pack_start (GTK_BOX (hbox[nomer]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[nomer++]), data.entry[E_GOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GOD], "activate",G_CALLBACK(m_poird_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.god.ravno());
gtk_widget_set_name(data.entry[E_GOD],iceb_u_inttochar(E_GOD));



sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(m_poird_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(m_poird_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
nomd->new_plus(data.nomd.ravno());
*god=data.god.ravno_atoi();
return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   m_poird_key_press(GtkWidget *widget,GdkEventKey *event,class m_poird_data *data)
{
//char  bros[300];

printf("m_poird_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
//    printf("Нажата клавиша F10\n");
    return(TRUE);

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
void  m_poird_knopka(GtkWidget *widget,class m_poird_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case FK2:
    for(int i=0; i < KOLENTER ; i++)
      g_signal_emit_by_name(data->entry[i],"activate");
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  


  case FK10:
    printf("Нажата кнопка F10\n");
    data->voz=1;
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    m_poird_vvod(GtkWidget *widget,class m_poird_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("m_poird_vvod enter=%d\n",enter);
switch (enter)
 {
  case E_NOMD:
    data->nomd.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GOD:
    data->god.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
