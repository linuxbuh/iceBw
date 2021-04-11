/*$Id: l_grkl_v.c,v 1.14 2013/08/13 06:10:23 sasa Exp $*/
/*16.12.2004	05.05.2004	Белых А.И.	l_grkl_v.c
Ввод и корректировка сальдо по счетам
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "i_rest.h"
#include "l_grkl.h"
enum
{
 FK2,
 FK4,
 FK10,
 KOL_F_KL
};

enum
 {
  E_KOD,
  E_NAIM,
  KOLENTER  
 };

class grkl_v_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *entry[KOLENTER];
  short     kl_shift; //0-отжата 1-нажата  
  int       voz;
       
  class grkl_rek vvod;
  iceb_u_str kodgr;
  
  grkl_v_data()
   {
    voz=0;
    kl_shift=0;
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


gboolean   grkl_v_key_press(GtkWidget *widget,GdkEventKey *event,class grkl_v_data *data);
void  grkl_v_knopka(GtkWidget *widget,class grkl_v_data *data);
void    grkl_v_vvod(GtkWidget *widget,class grkl_v_data *data);
int grkl_pk(const char *god,GtkWidget*);
int grkl_zap(class grkl_v_data *data);
void grkl_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);

extern uid_t  kod_operatora;

int l_grkl_v(iceb_u_str *kodgr,GtkWidget *wpredok) 
{
char strsql[300];
class grkl_v_data data;
iceb_u_str kikz;
printf("l_grkl_v\n");

data.vvod.clear_zero();
data.kodgr.new_plus(kodgr->ravno());

if(kodgr->getdlinna() > 1)
 {
  data.vvod.kod.new_plus(kodgr->ravno());
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Grupklient where kod='%s'",kodgr->ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) < 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.vvod.naim.new_plus(row[1]);
  kikz.plus(iceb_kikz(row[2],row[3],wpredok));   
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Grupklient",1,wpredok));

  data.vvod.kod.new_plus(strsql);

 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;
if(kodgr->getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));

  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
  
  iceb_u_str repl;
  
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  
  label=gtk_label_new(repl.ravno());

 }
gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(grkl_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox2),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Код группы"));
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),10);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(grkl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.vvod.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Наименование группы"));
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),80);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(grkl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.vvod.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(grkl_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(grkl_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(grkl_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

if(kodgr->getdlinna() > 1)
  gtk_widget_grab_focus(data.entry[0]);
else
  gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 kodgr->new_plus(data.vvod.kod.ravno());

return(data.voz);

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   grkl_v_key_press(GtkWidget *widget,GdkEventKey *event,class grkl_v_data *data)
{
//char  bros[300];

//printf("grkl_v_key_press\n");
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
void  grkl_v_knopka(GtkWidget *widget,class grkl_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek();
    if(grkl_zap(data) == 0)
     {
      gtk_widget_destroy(data->window);
      data->window=NULL;
      data->voz=0;
     }
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

void    grkl_v_vvod(GtkWidget *widget,class grkl_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("grkl_v_vvod enter=%d\n",enter);

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
int grkl_zap(class grkl_v_data *data)
{
char strsql[1000];
//SQL_str row;
time_t   vrem;
time(&vrem);

printf("grkl_v_zap\n");

if(data->vvod.kod.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введен код !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

if(data->vvod.naim.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введено наименование !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

short metkakor=0;

if(data->kodgr.getdlinna() <= 1)
 {

  if(grkl_pk(data->vvod.kod.ravno(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Grupklient \
values (%d,'%s',%d,%ld)",
   atoi(data->vvod.kod.ravno()),
   data->vvod.naim.ravno_filtr(),
   kod_operatora,vrem);
 }
else
 {
//  printf("metkazapisi=%d\n",data->metkazapisi);
  
  if(iceb_u_SRAV(data->kodgr.ravno(),data->vvod.kod.ravno(),0) != 0)
   {
    if(grkl_pk(data->vvod.kod.ravno(),data->window) != 0)
     return(1);

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Grupklient \
set \
kod=%d,\
naik='%s',\
ktoz=%d,\
vrem=%ld \
where kod=%d",
   atoi(data->vvod.kod.ravno()),
   data->vvod.naim.ravno_filtr(),
   kod_operatora,vrem,
   atoi(data->kodgr.ravno()));
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 grkl_kkvt(data->kodgr.ravno(),data->vvod.kod.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int grkl_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[300];

sprintf(strsql,"select kod from Grupklient where kod='%s'",kod);
//printf("grkl_pk-%s\n",strsql);

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

void grkl_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok)
{

char strsql[300];

sprintf(strsql,"update Material set kodgr=%s where kodgr=%s",kodn,kods);

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
}
