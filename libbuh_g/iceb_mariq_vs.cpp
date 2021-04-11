/*$Id: iceb_mariq_vs.c,v 1.16 2013/09/26 09:47:04 sasa Exp $*/
/*17.01.2008	23.12.2004	Белых А.И.	iceb_mariq_vs.c
Ввод суммы в кассовый регистратор
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
  E_SUMA,
  E_VIDNAL,
  KOLENTER  
 };

class iceb_mariq_vs_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *menu_vidnal;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  class iceb_u_str suma;
  struct KASSA *kasr;
  short metka;  //0-внесение денег в кассу 1-выдача денег из кассы
  int vidnal; //0-c ПДВ 1-без ПДВ
  
  iceb_mariq_vs_data() //Конструктор
   {
    vidnal=0;  
    kl_shift=0;
    voz=1;
    suma.new_plus("");
    
   }

  void read_rek()
   {
//    for(int i=0; i < KOLENTER-1; i++)
//      g_signal_emit_by_name(entry[i],"activate");
    suma.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA])));
    vidnal=gtk_combo_box_get_active (GTK_COMBO_BOX(menu_vidnal));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER-1; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    suma.new_plus("");
    gtk_widget_grab_focus(entry[0]);
   
   }
 };


gboolean   iceb_mariq_vs_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_mariq_vs_data *data);
void    iceb_mariq_vs_v_vvod(GtkWidget *widget,class iceb_mariq_vs_data *data);
void  iceb_mariq_vs_v_knopka(GtkWidget *widget,class iceb_mariq_vs_data *data);

GtkWidget *vidnal(int nomerstr);


int iceb_mariq_vs(int metka, //0-внесение денег в кассу 1-выдача из кассы денег 
struct KASSA *kasr)
{

class iceb_mariq_vs_data data;
data.kasr=kasr;
data.metka=metka;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(metka == 1)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Выдача из кассы клиенту."));
if(metka == 0)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Внесение в кассу клиентом."));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_mariq_vs_v_key_press),&data);

GtkWidget *label=NULL;
if(metka == 1)
 label=gtk_label_new(gettext("Выдача из кассы клиенту."));
if(metka == 0)
 label=gtk_label_new(gettext("Внесение в кассу клиентом."));

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop=gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);



label=gtk_label_new(gettext("Сумма"));
data.entry[E_SUMA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA], "activate",G_CALLBACK(iceb_mariq_vs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno());
gtk_widget_set_name(data.entry[E_SUMA],iceb_u_inttochar(E_SUMA));

label=gtk_label_new(gettext("Нaлог"));
gtk_box_pack_start (GTK_BOX (hbox[E_VIDNAL]), label, FALSE, FALSE, 0);

data.menu_vidnal = vidnal(data.vidnal);
gtk_box_pack_start (GTK_BOX (hbox[E_VIDNAL]), data.menu_vidnal, TRUE, TRUE, 0);


sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод суммы в кассовый регистратор"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(iceb_mariq_vs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации."));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(iceb_mariq_vs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне."));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(iceb_mariq_vs_v_knopka),&data);
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

gboolean   iceb_mariq_vs_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_mariq_vs_data *data)
{

//printf("iceb_mariq_vs_v_key_press\n");
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
void  iceb_mariq_vs_v_knopka(GtkWidget *widget,class iceb_mariq_vs_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
/*g_print("iceb_mariq_vs_v_knopka knop=%d\n",*knop);*/

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
//    printf("%s %d\n",data->suma.ravno(),data->vidnal);     
    if(iceb_mariq_zvs(data->metka,data->suma.ravno_atof(),data->vidnal,data->kasr,data->window) != 0)
     return;

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

void    iceb_mariq_vs_v_vvod(GtkWidget *widget,class iceb_mariq_vs_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//printf("iceb_mariq_vs_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SUMA:
    data->suma.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }
/****************
enter++;
  
if(enter >= KOLENTER)
 enter=0;

gtk_widget_grab_focus(data->entry[enter]);
******************/
}

/*****************************/
/*Меню выбора налога     */
/*****************************/

GtkWidget *vidnal(int nomerstr)
{
GtkWidget *opt;
class iceb_u_spisok strmenu;
strmenu.plus(gettext("С НДС"));
strmenu.plus(gettext("Без НДС"));

iceb_pm_vibor(&strmenu,&opt,nomerstr);

return(opt);
}
