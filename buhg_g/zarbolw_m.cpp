/*$Id: zarbolw_m.c,v 1.19 2013/08/25 08:26:45 sasa Exp $*/
/*13.07.2015	02.12.2014	Белых А.И.	zarbolw_m.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"

enum
 {
  E_MES_GOD,
  E_KOD_BOL,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class zarbolw_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str mes_god;
  class iceb_u_str kod_bol;  
  
  zarbolw_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    mes_god.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MES_GOD])));
    kod_bol.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_BOL])));
   }

  void clear_data()
   {
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void zarbolw_m_clear(class zarbolw_m_data *data);
void    zarbolw_m_vvod(GtkWidget *widget,class zarbolw_m_data *data);
void  zarbolw_m_knopka(GtkWidget *widget,class zarbolw_m_data *data);
gboolean   zarbolw_m_key_press(GtkWidget *widget,GdkEventKey *event,class zarbolw_m_data *data);
void  zarbolw_v_e_knopka(GtkWidget *widget,zarbolw_m_data *data);


int zarbolw_m(class iceb_u_str *mes_god,class iceb_u_str *kod_bol,GtkWidget *wpredok)
{
class zarbolw_m_data data;
char strsql[512];
data.mes_god.new_plus(mes_god->ravno());
data.kod_bol.new_plus(kod_bol->ravno());

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт удержаний с больничного"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zarbolw_m_key_press),&data);
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

GtkWidget *label=gtk_label_new(gettext("Расчёт удержаний с больничного"));
gtk_box_pack_start (GTK_BOX (vbox),label, FALSE, FALSE,1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 0);


sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("м.г"));
data.knopka_enter[E_MES_GOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_MES_GOD]), data.knopka_enter[E_MES_GOD], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_MES_GOD],"clicked",G_CALLBACK(zarbolw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_MES_GOD],iceb_u_inttochar(E_MES_GOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_MES_GOD],gettext("Выбор даты"));

data.entry[E_MES_GOD] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MES_GOD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_MES_GOD]), data.entry[E_MES_GOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_MES_GOD], "activate",G_CALLBACK(zarbolw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MES_GOD]),data.mes_god.ravno());
gtk_widget_set_name(data.entry[E_MES_GOD],iceb_u_inttochar(E_MES_GOD));/******************/


data.knopka_enter[E_KOD_BOL]=gtk_button_new_with_label(gettext("Код больничного"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_BOL]), data.knopka_enter[E_KOD_BOL], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_BOL],"clicked",G_CALLBACK(zarbolw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_BOL],iceb_u_inttochar(E_KOD_BOL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_BOL],gettext("Выбор кода больничного"));

data.entry[E_KOD_BOL] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_BOL]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_BOL]), data.entry[E_KOD_BOL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_BOL], "activate",G_CALLBACK(zarbolw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_BOL]),data.kod_bol.ravno());

gtk_widget_set_name(data.entry[E_KOD_BOL],iceb_u_inttochar(E_KOD_BOL));/******************/


sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(zarbolw_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(zarbolw_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(zarbolw_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
if(data.voz == 0)
 {
  mes_god->new_plus(data.mes_god.ravno());
  kod_bol->new_plus(data.kod_bol.ravno());
 }
return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  zarbolw_v_e_knopka(GtkWidget *widget,zarbolw_m_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_MES_GOD:

    iceb_calendar1(&data->mes_god,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_MES_GOD]),data->mes_god.ravno());
      
    return;  

  case E_KOD_BOL:

    if(l_zarnah(1,&kod,&naim,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_BOL]),kod.ravno());
      
    return;  
   

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zarbolw_m_key_press(GtkWidget *widget,GdkEventKey *event,class zarbolw_m_data *data)
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
void  zarbolw_m_knopka(GtkWidget *widget,class zarbolw_m_data *data)
{
//char bros[512];

//int knop=atoi(gtk_widget_get_name(widget));
int knop=atoi(gtk_widget_get_name(widget));
//g_print("zarbolw_m_knopka knop=%d\n",knop);
switch (knop)
 {
  case FK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    zarbolw_m_vvod(GtkWidget *widget,class zarbolw_m_data *data)
{
//int enter=atoi(gtk_widget_get_name(widget));
//printf("%s-%s\n",__FUNCTION__,gtk_widget_get_name(widget));
int enter=atoi(gtk_widget_get_name(widget));
switch (enter)
 {
  case E_MES_GOD:
    data->mes_god.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_MES_GOD])));
    break;
  case E_KOD_BOL:
    data->kod_bol.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_KOD_BOL])));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
