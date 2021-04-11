/*$Id: rpod_p.c,v 1.9 2013/08/13 06:10:25 sasa Exp $*/
/*29.04.2004	31.12.2003	Белых А.И.	rpod_p.c
Меню для ввода реквизитов поиска 
*/
#include "i_rest.h"
#include "rpod.h"


void rpod_p_clear(class rpod_data *data);
void    rpod_p_vvod(GtkWidget *widget,class rpod_data *data);
void  rpod_p_knopka(GtkWidget *widget,class rpod_data *data);
gboolean   rpod_p_key_press(GtkWidget *widget,GdkEventKey *event,class rpod_data *data);

void rpod_p(class rpod_data *data)
{
char strsql[300];

if(data->pwindow != NULL)
 return;

data->kl_shift=0;

data->pwindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data->pwindow),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data->pwindow),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data->pwindow),strsql);
g_signal_connect(data->pwindow,"destroy",G_CALLBACK(gtk_widget_destroyed),&data->pwindow);
g_signal_connect_after(data->pwindow,"key_press_event",G_CALLBACK(rpod_p_key_press),data);

//Удерживать окно над породившем его окном всегда
gtk_window_set_transient_for(GTK_WINDOW(data->pwindow),GTK_WINDOW(data->window));
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data->pwindow),TRUE);




GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox2),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data->pwindow), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Код подразделения"));
data->entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data->entry[E_KOD]),10);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data->entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data->entry[E_KOD], "activate",G_CALLBACK(rpod_p_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->poisk.kod.ravno());
gtk_widget_set_name(data->entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Наименование подразделения"));
data->entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data->entry[E_NAIM]),80);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data->entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data->entry[E_NAIM], "activate",G_CALLBACK(rpod_p_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_NAIM]),data->poisk.naim.ravno());
gtk_widget_set_name(data->entry[E_NAIM],iceb_u_inttochar(E_NAIM));






sprintf(strsql,"F2 %s",gettext("Поиск"));
data->knopka_pv[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data->knopka_pv[PFK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data->knopka_pv[PFK2],"clicked",G_CALLBACK(rpod_p_knopka),data);
gtk_widget_set_name(data->knopka_pv[PFK2],iceb_u_inttochar(PFK2));

sprintf(strsql,"F4 %s",gettext("Очистить"));
data->knopka_pv[PFK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data->knopka_pv[PFK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data->knopka_pv[PFK4],"clicked",G_CALLBACK(rpod_p_knopka),data);
gtk_widget_set_name(data->knopka_pv[PFK4],iceb_u_inttochar(PFK4));


sprintf(strsql,"F10 %s",gettext("Выход"));
data->knopka_pv[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data->knopka_pv[PFK10],gettext("Поиск не выполнять"));
g_signal_connect(data->knopka_pv[PFK10],"clicked",G_CALLBACK(rpod_p_knopka),data);
gtk_widget_set_name(data->knopka_pv[PFK10],iceb_u_inttochar(PFK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data->knopka_pv[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data->knopka_pv[PFK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data->knopka_pv[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data->entry[0]);

gtk_widget_show_all (data->pwindow);


}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rpod_p_key_press(GtkWidget *widget,GdkEventKey *event,class rpod_data *data)
{
//char  bros[300];

//printf("vrpod_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka_pv[PFK2],"clicked");
    return(FALSE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka_pv[PFK4],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka_pv[PFK10],"clicked");
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
void  rpod_p_knopka(GtkWidget *widget,class rpod_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case PFK2:
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(data->entry[i],"activate");
    gtk_widget_destroy(data->pwindow);
    data->pwindow=NULL;
    data->metka_poi=1;

    rpod_create_list (data);
    return;  

  case PFK4:
    rpod_p_clear(data);
    return;  


  case PFK10:
    gtk_widget_destroy(data->pwindow);
    data->pwindow=NULL;
    data->metka_poi=0;

    rpod_create_list (data);
    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    rpod_p_vvod(GtkWidget *widget,class rpod_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("rpod_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->poisk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NAIM:
    data->poisk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Очистка меню от введенной информации*/
/**************************************/
void rpod_p_clear(class rpod_data *data)
{

data->poisk.clear_zero();
for(int i=0; i< KOLENTER; i++)
 gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");

gtk_widget_grab_focus(data->entry[0]);

}
