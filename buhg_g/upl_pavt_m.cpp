/*$Id: upl_pavt_m.c,v 1.12 2013/09/26 09:46:58 sasa Exp $*/
/*31.03.2008	31.03.2008	Белых А.И.	upl_pavt_m.c
Меню для ввода реквизитов
*/

#include "buhg_g.h"
#include "upl_pavt.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAN,
  E_DATAK,
  E_KOD_VOD,
  E_KOD_AVT,
  E_KOD_POD,
  KOLENTER
 };

class upl_pavt_m_data
 {
  public:
  class upl_pavt_data *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  upl_pavt_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rk->kod_vod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_VOD])));
    rk->kod_avt.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_AVT])));
    rk->kod_pod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_POD])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rk->clear();
   }
 };


gboolean   upl_pavt_v_key_press(GtkWidget *widget,GdkEventKey *event,class upl_pavt_m_data *data);
void    upl_pavt_v_vvod(GtkWidget *widget,class upl_pavt_m_data *data);
void  upl_pavt_v_knopka(GtkWidget *widget,class upl_pavt_m_data *data);
void  upl_pavt_v_e_knopka(GtkWidget *widget,class upl_pavt_m_data *data);

int upl_pavt_m_provr(class upl_pavt_m_data *data);

extern SQL_baza bd;

int upl_pavt_m(class upl_pavt_data *rek_ras)
{
char strsql[512];


class upl_pavt_m_data data;

data.rk=rek_ras;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка оборотной ведомости движения топлива по автомобилям"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(upl_pavt_v_key_press),&data);

GtkWidget *label=NULL;
label=gtk_label_new(gettext("Распечатка оборотной ведомости движения топлива по автомобилям"));

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE,1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE,1);

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE,1);


sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(upl_pavt_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(upl_pavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));


sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(upl_pavt_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(upl_pavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));


sprintf(strsql,"%s (,,)",gettext("Код водителя"));
data.knopka_enter[E_KOD_VOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.knopka_enter[E_KOD_VOD], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_VOD],"clicked",G_CALLBACK(upl_pavt_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_VOD],iceb_u_inttochar(E_KOD_VOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_VOD],gettext("Выбор водителя"));

data.entry[E_KOD_VOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.entry[E_KOD_VOD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_VOD], "activate",G_CALLBACK(upl_pavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_VOD]),data.rk->kod_vod.ravno());
gtk_widget_set_name(data.entry[E_KOD_VOD],iceb_u_inttochar(E_KOD_VOD));


sprintf(strsql,"%s (,,)",gettext("Код автомобиля"));
data.knopka_enter[E_KOD_AVT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.knopka_enter[E_KOD_AVT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_AVT],"clicked",G_CALLBACK(upl_pavt_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_AVT],iceb_u_inttochar(E_KOD_AVT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_AVT],gettext("Выбор автомобиля"));

data.entry[E_KOD_AVT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.entry[E_KOD_AVT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_AVT], "activate",G_CALLBACK(upl_pavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_AVT]),data.rk->kod_avt.ravno());
gtk_widget_set_name(data.entry[E_KOD_AVT],iceb_u_inttochar(E_KOD_AVT));



sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_KOD_POD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POD]), data.knopka_enter[E_KOD_POD], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_POD],"clicked",G_CALLBACK(upl_pavt_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_POD],iceb_u_inttochar(E_KOD_POD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_POD],gettext("Выбор подразделения"));

data.entry[E_KOD_POD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POD]), data.entry[E_KOD_POD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_POD], "activate",G_CALLBACK(upl_pavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_POD]),data.rk->kod_pod.ravno());
gtk_widget_set_name(data.entry[E_KOD_POD],iceb_u_inttochar(E_KOD_POD));





sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(upl_pavt_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(upl_pavt_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(upl_pavt_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  upl_pavt_v_e_knopka(GtkWidget *widget,class upl_pavt_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
/*g_print("upl_pavt_v_e_knopka knop=%d\n",knop);*/

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rk->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rk->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
      
    return;  

  case E_KOD_VOD:

    if(l_uplout(1,&kod,&naim,1,data->window) == 0)
     data->rk->kod_vod.z_plus(kod.ravno());

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VOD]),data->rk->kod_vod.ravno());
      
    return;  

  case E_KOD_AVT:

    if(l_uplavt0(1,&kod,&naim,data->window) == 0)
     data->rk->kod_avt.z_plus(kod.ravno());

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_AVT]),data->rk->kod_avt.ravno());
      
    return;  

  case E_KOD_POD:

    if(l_uplpod(1,&kod,&naim,data->window) == 0)
     data->rk->kod_pod.z_plus(kod.ravno());

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_POD]),data->rk->kod_pod.ravno());
      
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   upl_pavt_v_key_press(GtkWidget *widget,GdkEventKey *event,class upl_pavt_m_data *data)
{

//printf("upl_pavt_v_key_press\n");
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
void  upl_pavt_v_knopka(GtkWidget *widget,class upl_pavt_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(upl_pavt_m_provr(data) != 0)
     return;
    
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    upl_pavt_v_vvod(GtkWidget *widget,class upl_pavt_m_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("upl_pavt_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************************/
/*Проверка реквизитов*/
/*******************************/
int upl_pavt_m_provr(class upl_pavt_m_data *data)
{

if(iceb_rsdatp(data->rk->datan.ravno(),data->rk->datak.ravno(),data->window) != 0)
 return(1);

return(0);
}


