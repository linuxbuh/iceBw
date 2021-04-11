/*$Id: l_uplavt0_v.c,v 1.3 2013/08/25 08:26:41 sasa Exp $*/
/*01.03.2016	06.03.2008	Белых А.И.	l_uplavt0_v.c
Ввод и корректировка автомобилей
*/
#include "buhg_g.h"
#include "l_uplavt0.h"
enum
 {
  E_KOD,
  E_NAIM,
  E_GOS_NOMER,
  E_KOD_VOD,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uplavt0_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_kod_vod;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uplavt0_rek rk;
  class iceb_u_str kod_avt;  
  
  l_uplavt0_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    rk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM])));
    rk.gos_nomer.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GOS_NOMER])));
    rk.kod_vod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_VOD])));
   }
 };


gboolean   l_uplavt0_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplavt0_v_data *data);
void  l_uplavt0_v_knopka(GtkWidget *widget,class l_uplavt0_v_data *data);
void    l_uplavt0_v_vvod(GtkWidget *widget,class l_uplavt0_v_data *data);
int l_uplavt0_pk(const char *god,GtkWidget*);
int l_uplavt0_zap(class l_uplavt0_v_data *data);
void l_uplavt0_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);

void  l_uplavt0_v_e_knopka(GtkWidget *widget,l_uplavt0_v_data *data);

extern SQL_baza  bd;

int l_uplavt0_v(class iceb_u_str *kod_avt,GtkWidget *wpredok)
{
class iceb_u_str naim_kod_vod("");
class l_uplavt0_v_data data;
char strsql[512];
class iceb_u_str kikz;
int gor=0,ver=0;
data.kod_avt.new_plus(kod_avt->ravno());
data.name_window.plus(__FUNCTION__);

data.rk.clear_data();

if(data.kod_avt.getdlinna() >  1)
 {
  data.rk.kod.new_plus(data.kod_avt.ravno());
  sprintf(strsql,"select nomz,kzv,naik,ktoz,vrem from Uplavt where kod=%d",data.kod_avt.ravno_atoi());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.rk.gos_nomer.new_plus(row[0]);
  if(row[1][0] != '0')
    data.rk.kod_vod.new_plus(row[1]);
  data.rk.naim.new_plus(row[2]);
  kikz.plus(iceb_kikz(row[3],row[4],wpredok));
  /*узнаём фамилию водителя*/
  sprintf(strsql,"select naik from Uplouot where kod=%d",data.rk.kod_vod.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_kod_vod.new_plus(row[0]);    
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Uplavt",0,wpredok));
  data.rk.kod.new_plus(strsql);
 } 
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(data.kod_avt.getdlinna() <= 1)
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uplavt0_v_key_press),&data);

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


label=gtk_label_new(gettext("Код"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE,1);

data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_uplavt0_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Наименование"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE,1);

data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_uplavt0_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

label=gtk_label_new(gettext("Государственный номерной знак автотранспортного средства"));
gtk_box_pack_start (GTK_BOX (hbox[E_GOS_NOMER]), label, FALSE, FALSE,1);

data.entry[E_GOS_NOMER] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GOS_NOMER]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_GOS_NOMER]), data.entry[E_GOS_NOMER], TRUE, TRUE,1);
g_signal_connect(data.entry[E_GOS_NOMER], "activate",G_CALLBACK(l_uplavt0_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOS_NOMER]),data.rk.gos_nomer.ravno());
gtk_widget_set_name(data.entry[E_GOS_NOMER],iceb_u_inttochar(E_GOS_NOMER));


sprintf(strsql,"%s",gettext("Код водителя"));
data.knopka_enter[E_KOD_VOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.knopka_enter[E_KOD_VOD], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_VOD],"clicked",G_CALLBACK(l_uplavt0_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_VOD],iceb_u_inttochar(E_KOD_VOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_VOD],gettext("Выбор водителя"));

data.entry[E_KOD_VOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_VOD]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.entry[E_KOD_VOD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_VOD], "activate",G_CALLBACK(l_uplavt0_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_VOD]),data.rk.kod_vod.ravno());
gtk_widget_set_name(data.entry[E_KOD_VOD],iceb_u_inttochar(E_KOD_VOD));

data.label_kod_vod=gtk_label_new(naim_kod_vod.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.label_kod_vod, TRUE, TRUE,1);



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_uplavt0_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_uplavt0_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_avt->new_plus(data.rk.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uplavt0_v_e_knopka(GtkWidget *widget,l_uplavt0_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {

  case E_KOD_VOD:

    if(l_uplout(1,&kod,&naim,1,data->window) == 0)
     data->rk.kod_vod.new_plus(kod.ravno());
    
    /*Читаем наименование склада*/
    sprintf(strsql,"select naik from Uplouot where kod=%d",data->rk.kod_vod.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
      naim.new_plus(row[0]);
    gtk_label_set_text(GTK_LABEL(data->label_kod_vod),naim.ravno(20));
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VOD]),data->rk.kod_vod.ravno());

    return;  



 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplavt0_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplavt0_v_data *data)
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
void  l_uplavt0_v_knopka(GtkWidget *widget,class l_uplavt0_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    if(l_uplavt0_zap(data) == 0)
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

void    l_uplavt0_v_vvod(GtkWidget *widget,class l_uplavt0_v_data *data)
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

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_uplavt0_zap(class l_uplavt0_v_data *data)
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
  iceb_menu_soob(gettext("Код должен быть цифровым и неравным нолю!"),data->window);
  return(1);
 }

if(data->rk.naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }

if(data->rk.kod_vod.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Uplouot where kod=%d and mt=1",data->rk.kod_vod.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {

    sprintf(strsql,"%s %s !",gettext("Не найден код водителя"),
    data->rk.kod_vod.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 
 }

short metkakor=0;

if(iceb_u_SRAV(data->kod_avt.ravno(),data->rk.kod.ravno(),0) != 0)
  if(l_uplavt0_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);

if(data->kod_avt.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Uplavt (kod,nomz,kzv,naik,ktoz,vrem) \
values (%d,'%s',%d,'%s',%d,%ld)",
   data->rk.kod.ravno_atoi(),
   data->rk.gos_nomer.ravno_filtr(),
   data->rk.kod_vod.ravno_atoi(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_avt.ravno(),data->rk.kod.ravno(),0) != 0)
   {

    if(iceb_menu_danet(gettext("Корректировать код ? Вы уверены ?"),2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Uplavt \
set \
kod=%d,\
nomz='%s',\
kzv=%d,\
naik='%s',\
ktoz=%d,\
vrem=%ld \
where kod=%d",
   data->rk.kod.ravno_atoi(),
   data->rk.gos_nomer.ravno_filtr(),
   data->rk.kod_vod.ravno_atoi(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->kod_avt.ravno_atoi());
   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 l_uplavt0_kkvt(data->kod_avt.ravno(),data->rk.kod.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_uplavt0_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Uplavt where kod=%d",atoi(kod));

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void l_uplavt0_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Upldok set ka='%s' where ka=%s",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
   return;
  }
 else
  {
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
   return;
  }
 }
sprintf(strsql,"update Uplsal set ka='%s' where ka=%s",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

}
