/*$Id: l_uosindm_v.c,v 1.14 2013/08/25 08:26:40 sasa Exp $*/
/*10.07.2015	03.03.2009	Белых А.И.	l_uosindm_v.c
Ввод и корректировка 
*/
#include <ctype.h>
#include "buhg_g.h"

class l_uosindm_rek
 {
  public:
   class iceb_u_str innom;
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str ei;
   class iceb_u_str ves;   
   l_uosindm_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    ei.new_plus("");
    ves.new_plus("");
   }
 
 };

enum
 {
  E_KOD,
  E_VES,
  E_EI,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uosindm_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uosindm_rek rk;
  class iceb_u_str kod_gr;  
  int innom;
    
  l_uosindm_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    innom=0;
   }

  void read_rek()
   {
    rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    rk.ves.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_VES])));
    rk.ei.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_EI])));
   }
 };


gboolean   l_uosindm_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosindm_v_data *data);
void  l_uosindm_v_knopka(GtkWidget *widget,class l_uosindm_v_data *data);
void    l_uosindm_v_vvod(GtkWidget *widget,class l_uosindm_v_data *data);
int l_uosindm_pk(int,const char *god,GtkWidget*);
int l_uosindm_zap(class l_uosindm_v_data *data);
void l_uosindm_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);
void  l_uosindm_v_e_knopka(GtkWidget *widget,class l_uosindm_v_data *data);


extern SQL_baza  bd;

int l_uosindm_v(int innom,class iceb_u_str *kod_gr,GtkWidget *wpredok)
{

class l_uosindm_v_data data;
char strsql[512];
iceb_u_str kikz;
data.kod_gr.new_plus(kod_gr->ravno());
data.innom=innom;
data.rk.clear_data();

if(data.kod_gr.getdlinna() >  1)
 {
  data.rk.kod.new_plus(data.kod_gr.ravno());
  sprintf(strsql,"select * from Uosindm where innom=%d and kod=%s",data.innom,data.kod_gr.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.kod.new_plus(row[1]);
  data.rk.ves.new_plus(row[3]);
  data.rk.ei.new_plus(row[2]);
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
    
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_gr.getdlinna() <= 1)
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uosindm_v_key_press),&data);

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
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);



//label=gtk_label_new(gettext("Код"));
//gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
sprintf(strsql,"%s",gettext("Код"));
data.knopka_enter[E_KOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.knopka_enter[E_KOD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD],"clicked",G_CALLBACK(l_uosindm_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD],iceb_u_inttochar(E_KOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD],gettext("Выбор материалла"));

data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_uosindm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Вес"));
data.entry[E_VES] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VES]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_VES]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_VES]), data.entry[E_VES], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_VES], "activate",G_CALLBACK(l_uosindm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VES]),data.rk.ves.ravno());
gtk_widget_set_name(data.entry[E_VES],iceb_u_inttochar(E_VES));



sprintf(strsql,"%s",gettext("Единица измерения"));
data.knopka_enter[E_EI]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_EI],"clicked",G_CALLBACK(l_uosindm_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_EI],iceb_u_inttochar(E_EI));
gtk_widget_set_tooltip_text(data.knopka_enter[E_EI],gettext("Выбор единицы измерения"));

data.entry[E_EI] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_EI]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_EI], "activate",G_CALLBACK(l_uosindm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.rk.ei.ravno());
gtk_widget_set_name(data.entry[E_EI],iceb_u_inttochar(E_EI));





sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_uosindm_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_uosindm_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_gr->new_plus(data.rk.kod.ravno());
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uosindm_v_e_knopka(GtkWidget *widget,class l_uosindm_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
   
  case E_KOD:

    if(l_uosdm(1,&kod,&naim,data->window) == 0)
     data->rk.kod.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->rk.kod.ravno());

    return;  

  case E_EI:

    if(iceb_l_ei(1,&kod,&naim,data->window) == 0)
     data->rk.ei.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->rk.ei.ravno());

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosindm_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosindm_v_data *data)
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
void  l_uosindm_v_knopka(GtkWidget *widget,class l_uosindm_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_uosindm_zap(data) == 0)
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

void    l_uosindm_v_vvod(GtkWidget *widget,class l_uosindm_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KOD:
    data->rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->rk.ei.getdlinna() <= 1)
     {
      char strsql[512];
      SQL_str row;
      class SQLCURSOR cur;
      sprintf(strsql,"select ei from Uosdm where kod=%d",data->rk.kod.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
            gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),row[0]);
       
     }
    break;
  case E_VES:
    data->rk.ves.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int l_uosindm_zap(class l_uosindm_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);


if(data->rk.kod.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }
/*Проверяем код*/
sprintf(strsql,"select kod from Uosdm where kod=%d",data->rk.kod.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден код материалла"),data->rk.kod.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
if(data->rk.ves.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено вес!"),data->window);
  return(1);
 }
if(data->rk.ei.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Edizmer where kod='%s'",data->rk.ei.ravno());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не найдено единицу измерения"),data->rk.ei.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }

if(iceb_u_SRAV(data->kod_gr.ravno(),data->rk.kod.ravno(),0) != 0)
  if(l_uosindm_pk(data->innom,data->rk.kod.ravno(),data->window) != 0)
     return(1);
if(isdigit(data->rk.kod.ravno()[0]) == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым !"),data->window);
  return(1);
 }
short metkakor=0;

if(data->kod_gr.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Uosindm \
values (%d,%d,'%s',%10.10g,%d,%ld)",
   data->innom,
   data->rk.kod.ravno_atoi(),
   data->rk.ei.ravno_filtr(),
   data->rk.ves.ravno_atof(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_gr.ravno(),data->rk.kod.ravno(),0) != 0)
   {

    if(iceb_menu_danet(gettext("Корректировать код ? Вы уверены ?"),2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Uosindm \
set \
kod=%d,\
ei='%s',\
ves=%10.10g,\
ktoz=%d,\
vrem=%ld \
where innom=%d and kod=%d",
   data->rk.kod.ravno_atoi(),
   data->rk.ei.ravno_filtr(),
   data->rk.ves.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->innom,
   data->kod_gr.ravno_atoi());
   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(metkakor == 1)
 l_uosindm_kkvt(data->kod_gr.ravno(),data->rk.kod.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_uosindm_pk(int innom,const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Uosindm where innom=%d kod=%s",innom,kod);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void l_uosindm_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Uosindm set kod=%s where kod=%s",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
   return;
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
 }

}
