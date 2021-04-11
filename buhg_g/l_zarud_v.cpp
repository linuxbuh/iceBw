/*$Id: l_zarud_v.c,v 1.23 2013/09/26 09:46:51 sasa Exp $*/
/*03.03.2016	23.06.2006	Белых А.И.	l_zarud_v.c
Ввод и корректировка удержаний
*/
#include <ctype.h>
#include "buhg_g.h"
#include "l_zarud.h"

#define KOLIH_VVOD 3

enum
 {
  E_KOD,
  E_NAIM,
  E_SHET,
  E_SHETB,
  E_PROV,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_zarud_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt_prov;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_zarud_rek rk;
  class iceb_u_str kod_uder;  
  
  l_zarud_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    rk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM])));
    rk.shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    rk.shetb.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHETB])));

    rk.prov=gtk_combo_box_get_active (GTK_COMBO_BOX(opt_prov));

   }
 };


gboolean   l_zarud_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarud_v_data *data);
void  l_zarud_v_knopka(GtkWidget *widget,class l_zarud_v_data *data);
void    l_zarud_v_vvod(GtkWidget *widget,class l_zarud_v_data *data);
int l_zarud_pk(const char *god,GtkWidget*);
int l_zarud_zap(class l_zarud_v_data *data);
void l_zarud_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);

void  l_zarud_v_e_knopka(GtkWidget *widget,class l_zarud_v_data *data);

extern SQL_baza  bd;

int l_zarud_v(class iceb_u_str *kod_uder,GtkWidget *wpredok)
{

class l_zarud_v_data data;
char strsql[512];
class iceb_u_str kikz;
int gor=0,ver=0;
data.kod_uder.new_plus(kod_uder->ravno());
data.name_window.plus(__FUNCTION__);

data.rk.clear_data();

if(data.kod_uder.getdlinna() >  1)
 {
  data.rk.kod.new_plus(data.kod_uder.ravno());
  sprintf(strsql,"select * from Uder where kod=%s",data.kod_uder.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.naim.new_plus(row[1]);
  data.rk.shet.new_plus(row[2]);
  data.rk.prov=atoi(row[5]);
  data.rk.shetb.new_plus(row[6]);
  
  kikz.plus(iceb_kikz(row[2],row[3],wpredok));
  
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Uder",0,wpredok));
  data.rk.kod.new_plus(strsql);
 } 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(data.kod_uder.getdlinna() <= 1)
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zarud_v_key_press),&data);

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
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_zarud_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),60);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_zarud_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

sprintf(strsql,"%s",gettext("Счёт хоз-ный"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(l_zarud_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(l_zarud_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk.shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

sprintf(strsql,"%s",gettext("Счёт бюд-ный"));
data.knopka_enter[E_SHETB]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETB]), data.knopka_enter[E_SHETB], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHETB],"clicked",G_CALLBACK(l_zarud_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETB],iceb_u_inttochar(E_SHETB));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETB],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHETB] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHETB]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETB]), data.entry[E_SHETB], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHETB], "activate",G_CALLBACK(l_zarud_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETB]),data.rk.shetb.ravno());
gtk_widget_set_name(data.entry[E_SHETB],iceb_u_inttochar(E_SHETB));


label=gtk_label_new(gettext("Проводки"));
gtk_box_pack_start (GTK_BOX (hbox[E_PROV]), label, FALSE, FALSE, 0);

class iceb_u_spisok spmenu;
spmenu.new_plus(gettext("Нужно делать"));
spmenu.plus(gettext("Не нужно делать"));
iceb_pm_vibor(&spmenu,&data.opt_prov,data.rk.prov);
gtk_box_pack_start (GTK_BOX (hbox[E_PROV]), data.opt_prov, TRUE, TRUE, 0);



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_zarud_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_zarud_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_uder->new_plus(data.rk.kod.ravno());
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_zarud_v_e_knopka(GtkWidget *widget,class l_zarud_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
   
  case E_SHET:

    iceb_vibrek(1,"Plansh",&data->rk.shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk.shet.ravno());

    return;  

  case E_SHETB:

    iceb_vibrek(1,"Plansh",&data->rk.shetb,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETB]),data->rk.shetb.ravno());

    return;  



 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarud_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarud_v_data *data)
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
void  l_zarud_v_knopka(GtkWidget *widget,class l_zarud_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    if(l_zarud_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  


  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_zarud_v_vvod(GtkWidget *widget,class l_zarud_v_data *data)
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

  case E_SHET:
    data->rk.shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLIH_VVOD)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_zarud_zap(class l_zarud_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);

if(data->rk.kod.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }
if(data->rk.kod.ravno_atoi() == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым и не равным нолю!"),data->window);
  return(1);
 }

if(data->rk.naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }

if(data->rk.shet.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён счёт хоз-ный !"),data->window);
  return(1);
 }

OPSHET rek_shet;
if(iceb_prsh1(data->rk.shet.ravno(),&rek_shet,data->window) != 0)
   return(1);

if(data->rk.shet.getdlinna() > 1)
 {
  if(iceb_prsh1(data->rk.shet.ravno(),&rek_shet,data->window) != 0)
   return(1);
 }


if(iceb_u_SRAV(data->kod_uder.ravno(),data->rk.kod.ravno(),0) != 0)
  if(l_zarud_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);
if(isdigit(data->rk.kod.ravno()[0]) == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым !"),data->window);
  return(1);
 }
short metkakor=0;

if(data->kod_uder.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Uder \
values (%d,'%s','%s',%d,%ld,%d,'%s')",
   data->rk.kod.ravno_atoi(),
   data->rk.naim.ravno_filtr(),
   data->rk.shet.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.prov,
   data->rk.shetb.ravno_filtr());
   
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_uder.ravno(),data->rk.kod.ravno(),0) != 0)
   {

    if(iceb_menu_danet(gettext("Корректировать код ? Вы уверены ?"),2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Uder \
set \
kod=%d,\
naik='%s',\
shet='%s',\
shetb='%s',\
ktoz=%d,\
vrem=%ld,\
prov=%d \
where kod=%d",
   data->rk.kod.ravno_atoi(),
   data->rk.naim.ravno_filtr(),
   data->rk.shet.ravno_filtr(),
   data->rk.shetb.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.prov,
   data->kod_uder.ravno_atoi());
   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(metkakor == 1)
 l_zarud_kkvt(data->kod_uder.ravno(),data->rk.kod.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_zarud_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Uder where kod=%s",kod);

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

void l_zarud_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Zarp set knah=%s where knah=%s and prn='2'",nkod,skod);

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
sprintf(strsql,"update Zarn1 set knah=%s where knah=%s and prn=2",nkod,skod);
iceb_sql_zapis(strsql,0,0,wpredok);

}
