/*$Id: l_uplsal_v.c,v 1.14 2013/08/25 08:26:42 sasa Exp $*/
/*10.07.2015	03.07.2008	Белых А.И.	l_uplsal_v.c
Ввод и корректировка сальдо по топливу
*/
#include "buhg_g.h"
#include "l_uplsal.h"
enum
 {
  E_GOD,
  E_KOD_AVT,
  E_KOD_VOD,
  E_KOD_TOP,
  E_SALDO,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uplsal_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *label_vod;
  GtkWidget *label_avt;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uplsal_rek rk;
  
  short god;
  int kod_vod;
  int kod_avt;
  class iceb_u_str kod_top;
    
  l_uplsal_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.god.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GOD])));
    rk.kod_avt.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_AVT])));
    rk.kod_vod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_VOD])));
    rk.kod_top.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_TOP])));
    rk.saldo.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SALDO])));
   }
 };


gboolean   l_uplsal_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplsal_v_data *data);
void  l_uplsal_v_knopka(GtkWidget *widget,class l_uplsal_v_data *data);
void    l_uplsal_v_vvod(GtkWidget *widget,class l_uplsal_v_data *data);
int l_uplsal_pk(short god,int kod_avt,int kod_vod,const char *kod_top,GtkWidget *wpredok);
int l_uplsal_zap(class l_uplsal_v_data *data);

void  l_uplsal_v_e_knopka(GtkWidget *widget,class l_uplsal_v_data *data);

extern SQL_baza  bd;

int l_uplsal_v(class iceb_u_str *god,class iceb_u_str *kod_avt,class iceb_u_str *kod_vod,class iceb_u_str *kod_top,GtkWidget *wpredok)
{

class l_uplsal_v_data data;
class iceb_u_str naim_vod("");
class iceb_u_str naim_avt("");
char strsql[512];
iceb_u_str kikz;
data.god=god->ravno_atoi();
data.kod_avt=kod_avt->ravno_atoi();
data.kod_vod=kod_vod->ravno_atoi();
data.kod_top.new_plus(kod_top->ravno());


data.rk.clear_data();

if(data.god != 0)
 {
  sprintf(strsql,"select * from Uplsal where god=%d and ka=%d and kv=%d and kmt='%s'",
  god->ravno_atoi(),kod_avt->ravno_atoi(),kod_vod->ravno_atoi(),kod_top->ravno());
  
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.god.new_plus(god->ravno());
  data.rk.kod_avt.plus(kod_avt->ravno());
  data.rk.kod_vod.plus(kod_vod->ravno());
  data.rk.kod_top.plus(kod_top->ravno());
  
  data.rk.saldo.new_plus(row[4]);
  kikz.plus(iceb_kikz(row[5],row[6],wpredok));
  /*читаем наименование автомобиля*/
  sprintf(strsql,"select naik from Uplavt where kod=%d",data.rk.kod_avt.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_avt.new_plus(row[0]);  
  /*читаем наименование водителя*/
  sprintf(strsql,"select naik from Uplouot where kod=%d",data.rk.kod_vod.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_vod.new_plus(row[0]);  
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.god != 0)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uplsal_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL,1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Год"));
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE,1);

data.entry[E_GOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GOD]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_GOD], "activate",G_CALLBACK(l_uplsal_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.rk.god.ravno());
gtk_widget_set_name(data.entry[E_GOD],iceb_u_inttochar(E_GOD));

sprintf(strsql,"%s (,,)",gettext("Код автомобиля"));
data.knopka_enter[E_KOD_AVT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.knopka_enter[E_KOD_AVT], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_AVT],"clicked",G_CALLBACK(l_uplsal_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_AVT],iceb_u_inttochar(E_KOD_AVT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_AVT],gettext("Выбор автомобиля"));

data.entry[E_KOD_AVT] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_AVT]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.entry[E_KOD_AVT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_AVT], "activate",G_CALLBACK(l_uplsal_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_AVT]),data.rk.kod_avt.ravno());
gtk_widget_set_name(data.entry[E_KOD_AVT],iceb_u_inttochar(E_KOD_AVT));

data.label_avt=gtk_label_new(naim_avt.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.label_avt, FALSE, FALSE,1);


sprintf(strsql,"%s (,,)",gettext("Код водителя"));
data.knopka_enter[E_KOD_VOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.knopka_enter[E_KOD_VOD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_VOD],"clicked",G_CALLBACK(l_uplsal_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_VOD],iceb_u_inttochar(E_KOD_VOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_VOD],gettext("Выбор водителя"));

data.entry[E_KOD_VOD] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_VOD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.entry[E_KOD_VOD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_VOD], "activate",G_CALLBACK(l_uplsal_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_VOD]),data.rk.kod_vod.ravno());
gtk_widget_set_name(data.entry[E_KOD_VOD],iceb_u_inttochar(E_KOD_VOD));

data.label_vod=gtk_label_new(naim_vod.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.label_vod, FALSE, FALSE,1);


sprintf(strsql,"%s (,,)",gettext("Код топлива"));
data.knopka_enter[E_KOD_TOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.knopka_enter[E_KOD_TOP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_TOP],"clicked",G_CALLBACK(l_uplsal_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_TOP],iceb_u_inttochar(E_KOD_TOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_TOP],gettext("Выбор топлива"));

data.entry[E_KOD_TOP] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_TOP]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.entry[E_KOD_TOP], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_TOP], "activate",G_CALLBACK(l_uplsal_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_TOP]),data.rk.kod_top.ravno());
gtk_widget_set_name(data.entry[E_KOD_TOP],iceb_u_inttochar(E_KOD_TOP));


label=gtk_label_new(gettext("Сальдо"));
gtk_box_pack_start (GTK_BOX (hbox[E_SALDO]), label, FALSE, FALSE,1);

data.entry[E_SALDO] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SALDO]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_SALDO]), data.entry[E_SALDO], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SALDO], "activate",G_CALLBACK(l_uplsal_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SALDO]),data.rk.saldo.ravno());
gtk_widget_set_name(data.entry[E_SALDO],iceb_u_inttochar(E_SALDO));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_uplsal_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_uplsal_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  god->new_plus(data.rk.god.ravno());
  kod_avt->new_plus(data.rk.kod_avt.ravno());
  kod_vod->new_plus(data.rk.kod_vod.ravno());
  kod_top->new_plus(data.rk.kod_top.ravno());
 }
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uplsal_v_e_knopka(GtkWidget *widget,class l_uplsal_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_KOD_AVT:
     if(l_uplavt0(1,&kod,&naim,data->window) == 0)
      data->rk.kod_avt.new_plus(kod.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_AVT]),data->rk.kod_avt.ravno());
      
    return;  

  case E_KOD_VOD:
     if(l_uplout(1,&kod,&naim,1,data->window) == 0)
      data->rk.kod_vod.new_plus(kod.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VOD]),data->rk.kod_vod.ravno());
      
    return;  

  case E_KOD_TOP:
     if(l_uplmt(1,&kod,&naim,data->window) == 0)
      data->rk.kod_top.new_plus(kod.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_TOP]),data->rk.kod_top.ravno());
      
    return;  

 }   
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplsal_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplsal_v_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");

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
void  l_uplsal_v_knopka(GtkWidget *widget,class l_uplsal_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_uplsal_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_uplsal_v_vvod(GtkWidget *widget,class l_uplsal_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
switch (enter)
 {

  case E_KOD_AVT:
    data->rk.kod_avt.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    /*читаем наименование автомобиля*/
    sprintf(strsql,"select naik from Uplavt where kod=%d",data->rk.kod_avt.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       gtk_label_set_text(GTK_LABEL(data->label_avt),row[0]);
    else
       gtk_label_set_text(GTK_LABEL(data->label_avt),"");

    break;

  case E_KOD_VOD:
    data->rk.kod_vod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    /*читаем наименование водителя*/
    sprintf(strsql,"select naik from Uplouot where kod=%d",data->rk.kod_vod.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       gtk_label_set_text(GTK_LABEL(data->label_vod),row[0]);
    else
       gtk_label_set_text(GTK_LABEL(data->label_vod),"");
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
int l_uplsal_zap(class l_uplsal_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);


if(data->rk.god.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён год!"),data->window);
  return(1);
 }

short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

if(data->rk.god.ravno_atoi() < gt-100 || data->rk.god.ravno_atoi() > gt+100)
 {
  iceb_menu_soob(gettext("Неправильно ввели год!"),data->window);
  return(1);
 } 

if(iceb_pbpds(1,data->rk.god.ravno_atoi(),data->window) != 0)
 return(1);

if(data->god != 0)
 if(data->god != data->rk.god.ravno_atoi())
  if(iceb_pbpds(1,data->god,data->window) != 0)
   return(1);
 
//Проверяем есть ли такой код водителя

sprintf(strsql,"select kod from Uplouot where kod=%d and mt=1",data->rk.kod_vod.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден код водителя"),data->rk.kod_vod.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

//Проверяем код автомобиля

sprintf(strsql,"select kod from Uplavt where kod=%d ",data->rk.kod_avt.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код автомобиля"),data->rk.kod_avt.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

//Проверяем код топлива

sprintf(strsql,"select kodt from Uplmt where kodt='%s'",data->rk.kod_top.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код топлива"),data->rk.kod_top.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }


if(data->god != data->rk.god.ravno_atoi() || data->kod_avt != data->rk.kod_avt.ravno_atoi() \
 || data->kod_vod != data->rk.kod_vod.ravno_atoi() \
 || iceb_u_SRAV(data->kod_top.ravno(),data->rk.kod_top.ravno(),0) != 0)
  if(l_uplsal_pk(data->rk.god.ravno_atoi(),data->rk.kod_avt.ravno_atoi(),data->rk.kod_vod.ravno_atoi(),data->rk.kod_top.ravno(),data->window) != 0)
     return(1);

if(data->god == 0)
 {

  sprintf(strsql,"insert into Uplsal \
values (%d,%d,%d,'%s',%.3f,%d,%ld)",
   data->rk.god.ravno_atoi(),
   data->rk.kod_avt.ravno_atoi(),
   data->rk.kod_vod.ravno_atoi(),
   data->rk.kod_top.ravno_filtr(),
   data->rk.saldo.ravno_atof(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Uplsal \
set \
god=%d,\
ka=%d,\
kv=%d,\
kmt='%s',\
sal=%.3f,\
ktoz=%d,\
vrem=%ld \
where god=%d and ka=%d and kv=%d and kmt='%s'",
   data->rk.god.ravno_atoi(),
   data->rk.kod_avt.ravno_atoi(),
   data->rk.kod_vod.ravno_atoi(),
   data->rk.kod_top.ravno_filtr(),
   data->rk.saldo.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->god,
   data->kod_avt,
   data->kod_vod,
   data->kod_top.ravno_filtr());
   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_uplsal_pk(short god,int kod_avt,int kod_vod,const char *kod_top,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select god from Uplsal where god=%d and ka=%d and kv=%d and kmt='%s'",god,kod_avt,kod_vod,kod_top);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}
