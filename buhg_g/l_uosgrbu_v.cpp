/*$Id: l_uosgrbu_v.c,v 1.19 2013/08/25 08:26:40 sasa Exp $*/
/*01.03.2016	18.10.2007	Белых А.И.	l_uosgrbu_v.c
Ввод и корректировка групп для бухгалтерского учёта для учёта основных средств
*/
#include <ctype.h>
#include "buhg_g.h"
#include "l_uosgrbu.h"
enum
 {
  E_KOD,
  E_NAIM,
  E_PROC,
  E_AR,
  KOLENTER  
 };

enum
 {
  FK2,
  FK10,
  KOL_FK
 };

class l_uosgrbu_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt1;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uosgrbu_rek rk;
  class iceb_u_str kod_podr;  
  
  l_uosgrbu_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    rk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM])));
    rk.kof.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PROC])));
    rk.metka_ar=gtk_combo_box_get_active (GTK_COMBO_BOX(opt1));

   }
 };


gboolean   l_uosgrbu_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosgrbu_v_data *data);
void  l_uosgrbu_v_knopka(GtkWidget *widget,class l_uosgrbu_v_data *data);
void    l_uosgrbu_v_vvod(GtkWidget *widget,class l_uosgrbu_v_data *data);
int l_uosgrbu_pk(const char *god,GtkWidget*);
int l_uosgrbu_zap(class l_uosgrbu_v_data *data);
void l_uosgrbu_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);


extern SQL_baza  bd;

int l_uosgrbu_v(class iceb_u_str *kod_podr,GtkWidget *wpredok)
{

class l_uosgrbu_v_data data;
char strsql[512];
iceb_u_str kikz;
data.kod_podr.new_plus(kod_podr->ravno());

data.rk.clear_data();

if(data.kod_podr.getdlinna() >  1)
 {
  data.rk.kod.new_plus(data.kod_podr.ravno());
  sprintf(strsql,"select * from Uosgrup1 where kod='%s'",data.kod_podr.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.naim.new_plus(row[1]);
  data.rk.kof.new_plus(row[2]);
  kikz.plus(iceb_kikz(row[3],row[4],wpredok));
  data.rk.metka_ar=atoi(row[5]);
 }
else
 {
//  sprintf(strsql,"%d",iceb_get_new_kod("Uosgrup1",1,wpredok));
//  data.rk.kod.new_plus(strsql);
 } 

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_podr.getdlinna() <= 1)
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uosgrbu_v_key_press),&data);

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


label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_uosgrbu_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),60);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_uosgrbu_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

label=gtk_label_new(gettext("Годовой процент амортизации"));
data.entry[E_PROC] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PROC]),60);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC]), data.entry[E_PROC], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PROC], "activate",G_CALLBACK(l_uosgrbu_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROC]),data.rk.kof.ravno());
gtk_widget_set_name(data.entry[E_PROC],iceb_u_inttochar(E_PROC));

class iceb_u_spisok menu_vib;

menu_vib.plus(gettext("От начальной балансовой стоимости"));
menu_vib.plus(gettext("От остаточной балансовой стоимости"));

iceb_pm_vibor(&menu_vib,&data.opt1,data.rk.metka_ar);
gtk_box_pack_start (GTK_BOX (hbox[E_AR]), data.opt1, FALSE, FALSE, 0);


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_uosgrbu_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_uosgrbu_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_podr->new_plus(data.rk.kod.ravno());
return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosgrbu_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosgrbu_v_data *data)
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
void  l_uosgrbu_v_knopka(GtkWidget *widget,class l_uosgrbu_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_uosgrbu_zap(data) == 0)
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

void    l_uosgrbu_v_vvod(GtkWidget *widget,class l_uosgrbu_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KOD:
    data->rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NAIM:
    data->rk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_PROC:
    data->rk.kof.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int l_uosgrbu_zap(class l_uosgrbu_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);


if(data->rk.kod.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }

if(data->rk.naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }


if(iceb_u_SRAV(data->kod_podr.ravno(),data->rk.kod.ravno(),0) != 0)
  if(l_uosgrbu_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);

if(isdigit(data->rk.kod.ravno()[0]) == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым"),data->window);
  return(1);
 }

 
short metkakor=0;

if(data->kod_podr.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Uosgrup1 \
values ('%s','%s',%.10g,%d,%ld,%d)",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   data->rk.kof.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->rk.metka_ar);
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_podr.ravno(),data->rk.kod.ravno(),0) != 0)
   {

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }

  memset(strsql,'\0',sizeof(strsql));

  sprintf(strsql,"update Uosgrup1 \
set \
kod='%s',\
naik='%s',\
kof=%.10g,\
ktoz=%d,\
vrem=%ld,\
ar=%d \
where kod='%s'",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   data->rk.kof.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->rk.metka_ar,
   data->kod_podr.ravno());
   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


if(metkakor == 1)
 l_uosgrbu_kkvt(data->kod_podr.ravno(),data->rk.kod.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_uosgrbu_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Uosgrup1 where kod='%s'",kod);

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

void l_uosgrbu_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Uosdok set podr=%s where podr=%s",nkod,skod);
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
sprintf(strsql,"update Uosdok1 set podr=%s where podr=%s",nkod,skod);
iceb_sql_zapis(strsql,0,0,wpredok);

sprintf(strsql,"update Uosamor set podr=%s where podr=%s",nkod,skod);
iceb_sql_zapis(strsql,0,0,wpredok);

sprintf(strsql,"update Uosamor1 set podr=%s where podr=%s",nkod,skod);
iceb_sql_zapis(strsql,0,0,wpredok);

}




