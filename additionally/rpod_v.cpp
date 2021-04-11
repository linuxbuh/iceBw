/*$Id: rpod_v.c,v 1.15 2013/08/13 06:10:25 sasa Exp $*/
/*12.01.2006	29.04.2004	Белых А.И.	rpod_v.c
Ввод и корректировка сальдо по счетам
*/
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include "i_rest.h"
#include "rpod.h"


gboolean   rpod_v_key_press(GtkWidget *widget,GdkEventKey *event,class rpod_data *data);
void  rpod_v_knopka(GtkWidget *widget,class rpod_data *data);
void    rpod_v_vvod(GtkWidget *widget,class rpod_data *data);
int rpod_pk(const char *god,GtkWidget*);
int rpod_zap(class rpod_data *data);
void rpod_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);

extern uid_t kod_operatora;

void rpod_v(class rpod_data *data)
{
char strsql[300];

//printf("l_rpod_v\n");
if(data->vwindow != NULL)
  return;
    
data->kl_shift=0;
data->vvod.clear_zero();

if(data->metkazapisi == 1)
 {
  data->vvod.kod.new_plus(data->kodv.ravno());
  data->vvod.naim.new_plus(data->naimv.ravno());
  
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Restpod",0,data->window));

  data->vvod.kod.new_plus(strsql);

 }
 
data->vwindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data->vwindow),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data->vwindow),TRUE);
if(data->metkazapisi == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
if(data->metkazapisi == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));


gtk_window_set_title(GTK_WINDOW(data->vwindow),strsql);
g_signal_connect(data->vwindow,"destroy",G_CALLBACK(gtk_widget_destroyed),&data->vwindow);
g_signal_connect_after(data->vwindow,"key_press_event",G_CALLBACK(rpod_v_key_press),data);


//Удерживать окно над породившем его окном всегда
gtk_window_set_transient_for(GTK_WINDOW(data->vwindow),GTK_WINDOW(data->window));
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data->vwindow),TRUE);

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox2),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data->vwindow), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Код подразделения"));
data->entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data->entry[E_KOD]),10);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data->entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data->entry[E_KOD], "activate",G_CALLBACK(rpod_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->vvod.kod.ravno());
gtk_widget_set_name(data->entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Наименование подразделения"));
data->entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data->entry[E_NAIM]),80);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data->entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data->entry[E_NAIM], "activate",G_CALLBACK(rpod_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_NAIM]),data->vvod.naim.ravno());
gtk_widget_set_name(data->entry[E_NAIM],iceb_u_inttochar(E_NAIM));


sprintf(strsql,"F2 %s",gettext("Запись"));
data->knopka_pv[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data->knopka_pv[PFK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data->knopka_pv[PFK2],"clicked",G_CALLBACK(rpod_v_knopka),data);
gtk_widget_set_name(data->knopka_pv[PFK2],iceb_u_inttochar(PFK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data->knopka_pv[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data->knopka_pv[PFK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data->knopka_pv[PFK10],"clicked",G_CALLBACK(rpod_v_knopka),data);
gtk_widget_set_name(data->knopka_pv[PFK10],iceb_u_inttochar(PFK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data->knopka_pv[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data->knopka_pv[PFK10], TRUE, TRUE, 0);

if(data->metkazapisi == 1)
  gtk_widget_grab_focus(data->entry[0]);
else
  gtk_widget_grab_focus(data->entry[1]);

gtk_widget_show_all (data->vwindow);


}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rpod_v_key_press(GtkWidget *widget,GdkEventKey *event,class rpod_data *data)
{
//char  bros[300];

//printf("rpod_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka_pv[PFK2],"clicked");

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
void  rpod_v_knopka(GtkWidget *widget,class rpod_data *data)
{
//char bros[300];

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case PFK2:
    if(rpod_zap(data) == 0)
     {
      gtk_widget_destroy(data->vwindow);
      data->vwindow=NULL;
     }
    return;  


  case PFK10:
    gtk_widget_destroy(data->vwindow);
    data->vwindow=NULL;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    rpod_v_vvod(GtkWidget *widget,class rpod_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("rpod_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->vvod.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NAIM:
    data->vvod.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int rpod_zap(class rpod_data *data)
{
char strsql[1000];
//SQL_str row;
time_t   vrem;
time(&vrem);

printf("rpod_v_zap\n");

for(int i=0; i < KOLENTER; i++)
  g_signal_emit_by_name(data->entry[i],"activate");

if(data->vvod.kod.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введен код !"));
  iceb_menu_soob(&sp,data->vwindow);
  return(1);
 }

if(isdigit(data->vvod.kod.ravno()[0]) == 0)
 {
  iceb_u_str sp;
  sp.plus(gettext("Код должен быть цифровым !"));
  iceb_menu_soob(&sp,data->vwindow);
  return(1);
 }

if(data->vvod.naim.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введено наименование !"));
  iceb_menu_soob(&sp,data->vwindow);
  return(1);
 }

short metkakor=0;

if(data->metkazapisi == 0)
 {

  if(rpod_pk(data->vvod.kod.ravno(),data->vwindow) != 0)
   return(1);
  
  sprintf(strsql,"insert into Restpod \
values (%d,'%s',%d,%ld)",
   atoi(data->vvod.kod.ravno()),
   data->vvod.naim.ravno_filtr(),
   kod_operatora,vrem);
 }
else
 {
  printf("metkazapisi=%d\n",data->metkazapisi);
  
  if(iceb_u_SRAV(data->kodv.ravno(),data->vvod.kod.ravno(),0) != 0)
   {
    if(rpod_pk(data->vvod.kod.ravno(),data->vwindow) != 0)
     return(1);

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->vwindow) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Restpod \
set \
kod=%d,\
naik='%s',\
ktoi=%d,\
vrem=%ld \
where kod=%d",
   atoi(data->vvod.kod.ravno()),
   data->vvod.naim.ravno_filtr(),
   kod_operatora,vrem,
   atoi(data->kodv.ravno()));
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->vwindow) != 0)
 return(1);

gtk_widget_hide(data->vwindow);

if(metkakor == 1)
 rpod_kkvt(data->kodv.ravno(),data->vvod.kod.ravno(),data->vwindow);

rpod_create_list(data);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int rpod_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[300];

sprintf(strsql,"select kod from Restpod where kod=%d",atoi(kod));
//printf("rpod_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void rpod_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok)
{

char strsql[300];

sprintf(strsql,"update Restdok set podr=%s where podr=%s",kodn,kods);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_u_str SOOB;
   SOOB.plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&SOOB,wpredok);
   return;
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
 }
sprintf(strsql,"update Restdok1 set podr=%s where podr=%s",kodn,kods);

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);


}
