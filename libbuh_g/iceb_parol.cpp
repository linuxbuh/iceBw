/*$Id: iceb_parol.c,v 1.31 2014/05/07 10:58:10 sasa Exp $*/
/*04.10.2017	11.10.2003	Белых А.И.	iceb_parol.c
Если вернули 0- был введён пароль и он правильный
             1- пароль не правильный
*/

#include "iceb_libbuh.h"


int iceb_parol(class iceb_u_str *parol,GtkWidget *wpredok)
{
class iceb_u_spisok text_menu;
return(iceb_parol(2,&text_menu,parol,wpredok));
}
/*************************/
int    iceb_parol(int metkap, //0-проверка по сумме 1-проверка по строке даты 2-не проверять
class iceb_u_spisok *text_menu,
GtkWidget *wpredok) 
{
int kod;
class iceb_u_str parol_voz("");

if((kod=iceb_parol(metkap,text_menu,&parol_voz,wpredok)) == 0)
  return(0);

if(kod == 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Пароль введён неправильно !"));
  iceb_menu_soob(&repl,wpredok);
 }
return(1);

}
/****************************/
int    iceb_parol(int metkap, //0-проверка по сумме 1-проверка по строке даты 2-не проверять
GtkWidget *wpredok) 
{
int kod;
class iceb_u_spisok text_menu;
class iceb_u_str parol("");
if((kod=iceb_parol(metkap,&text_menu,&parol,wpredok)) == 0)
  return(0);

if(kod == 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Пароль введён неправильно !"));
  iceb_menu_soob(&repl,wpredok);
 }
return(1);
}

/**************************************
Ввод и проверка пароля
Если вернули 0- был введён пароль и он правильный
             1- пароль не правильный
            -1- отказались вводить пароль
******************************/



struct iceb_parol_data
 {
  GtkWidget *window;
  GtkWidget *knopka[2];
  GtkWidget *entry;
  char  parol[64];
  int   metkap;  
  int   voz; //0-пароль введён правильно 1-неправильно
 };

void       iceb_parol_knopka(GtkWidget *,struct iceb_parol_data *);
gboolean   iceb_parol_key_press(GtkWidget *,GdkEventKey *,struct iceb_parol_data *);
int        iceb_parol_prov(struct iceb_parol_data *);
void       iceb_parol_get_text(GtkWidget *widget,struct iceb_parol_data *data);
gboolean parol_delete_event(GtkWidget *widget,GdkEvent *event,struct iceb_parol_data *data);


int iceb_parol(int metkap, //0-проверка по сумме 1-проверка по строке даты 2-не проверять
class iceb_u_spisok *text_menu,
class iceb_u_str *parol_voz,
GtkWidget *wpredok) 
{
char strsql[512];
iceb_parol_data data;

memset(&data,'\0',sizeof(data));
data.metkap=metkap;
data.voz=1;
//printf("%s\n",__FUNCTION__);


data.window = gtk_window_new( GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),GTK_WIN_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

gtk_window_set_title(GTK_WINDOW (data.window),iceb_get_namesystem());
g_signal_connect(data.window, "delete_event",G_CALLBACK(parol_delete_event),&data);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_parol_key_press),&data);


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
GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
/*Если текст не задан выводим запрос введите пароль*/
int kolih_text=text_menu->kolih();
if(kolih_text == 0)
  kolih_text=1;
GtkWidget *label[kolih_text];

for(int ii=0; ii < kolih_text; ii++)
  label[ii]=gtk_label_new(text_menu->ravno(ii));
//printf("%s-text_menu=%d\n",__FUNCTION__,text_menu->kolih());
if(text_menu->kolih() == 0) 
 label[0]=gtk_label_new(gettext("Введите пароль"));

for(int ii=0; ii < kolih_text; ii++)
  gtk_box_pack_start (GTK_BOX (vbox), label[ii], TRUE, TRUE, 0);

gtk_box_pack_end(GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

data.entry = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry),63);
g_signal_connect(data.entry, "activate",G_CALLBACK(iceb_parol_get_text),&data);

gtk_box_pack_start (GTK_BOX (vbox), data.entry, TRUE, TRUE, 0);
gtk_entry_set_visibility(GTK_ENTRY(data.entry),FALSE); /*невидимый*/

sprintf(strsql,"F2 %s",gettext("Ввести"));
data.knopka[0] = gtk_button_new_with_label (strsql);
g_signal_connect(data.knopka[0], "clicked",G_CALLBACK(iceb_parol_knopka),&data);
gtk_box_pack_start (GTK_BOX (hbox), data.knopka[0], TRUE, TRUE, 0);
gtk_widget_set_name(data.knopka[0],iceb_u_inttochar(2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[1] = gtk_button_new_with_label (strsql);
g_signal_connect(data.knopka[1], "clicked",G_CALLBACK(iceb_parol_knopka),&data);
gtk_widget_set_name(data.knopka[1],iceb_u_inttochar(10));
gtk_box_pack_start (GTK_BOX (hbox), data.knopka[1], TRUE, TRUE, 0);



gtk_widget_show_all(data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 parol_voz->new_plus(data.parol);

return(data.voz);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_parol_knopka(GtkWidget *widget,struct iceb_parol_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("lvoditel_knopka knop=%s\n",knop);

switch (knop)
 {
  case 2:
    g_signal_emit_by_name(data->entry,"activate");
    return;

  case 10:
    data->voz=-1;
    gtk_widget_destroy(data->window);
    return;
 }
}
 
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_parol_key_press(GtkWidget *widget,GdkEventKey *event,struct iceb_parol_data *data)
{
//printf("lvoditel_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->entry,"activate");
    return(FALSE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    data->voz=-1;
    gtk_widget_destroy(widget);

    return(FALSE);

 }

return(TRUE);
}
/****************************/
/*Проверка пароля           */
/*****************************/

int    iceb_parol_prov(struct iceb_parol_data *data)
{
time_t vrem;
struct  tm      *bf;
time(&vrem);
bf=localtime(&vrem);

if(data->metkap == 0) //Проверка по сумме
 {
  if(atoi(data->parol) != bf->tm_mday + bf->tm_mon+1 + bf->tm_year+1900)
  {
   data->voz=1;
   return(1);  
  }
 }
if(data->metkap == 1) //Проверка по строке даты
 {
  char stroka[112];
  sprintf(stroka,"%d+%d+%d",bf->tm_year+1900,bf->tm_mon+1,bf->tm_mday);
  if(iceb_u_SRAV(data->parol,stroka,0) != 0)
   {
    data->voz=1;
    return(1);
   }
 }
data->voz=0;
return(0);
}
/******************************/
/*Чтение по Enter             */
/*******************************/
void iceb_parol_get_text(GtkWidget *widget,struct iceb_parol_data *data)
{

strcpy(data->parol,gtk_entry_get_text(GTK_ENTRY(widget)));
gtk_editable_select_region(GTK_EDITABLE(widget),0,strlen(data->parol));

iceb_parol_prov(data);
gtk_widget_destroy(data->window);

}
/***************************/
/*Обработчик сигнала delete event*/
/*********************************/
gboolean parol_delete_event(GtkWidget *widget,GdkEvent *event,struct iceb_parol_data *data)
{
data->voz=-1;
gtk_widget_destroy(widget);
return(FALSE);
}
