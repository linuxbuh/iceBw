/*$Id: iceb_l_sklad_v.c,v 1.25 2013/08/25 08:27:06 sasa Exp $*/
/*03.12.2014	05.05.2004	Белых А.И.	iceb_l_sklad_v.c
Ввод и корректировка склада
*/
#include "iceb_libbuh.h"
#include "iceb_l_sklad.h"

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

enum
 {
  E_KOD,
  E_NAIM,
  E_FIO,
  E_DOLG,
  E_NP,
  KOLENTER  
 };

class iceb_l_sklad_v
 {
  public:
    
  GtkWidget *radiobutton2[2];
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class  sklad_rek rk;
  
  class iceb_u_str sklad_i;
  
  iceb_l_sklad_v() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    rk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM])));
    rk.fmol.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FIO])));
    rk.dolg.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DOLG])));
    rk.np.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NP])));
/**************
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
**************/
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[0])) == TRUE)
     rk.metka_isp_skl=0;
    else
     rk.metka_isp_skl=1;
   }
 };

gboolean   sklad_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_sklad_v *data);
void  sklad_v_knopka(GtkWidget *widget,class iceb_l_sklad_v *data);
void    sklad_v_vvod(GtkWidget *widget,class iceb_l_sklad_v *data);
int sklad_pk(const char *god,GtkWidget*);
int sklad_zap(class iceb_l_sklad_v *data);
void sklad_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);


int iceb_l_sklad_v(class iceb_u_str *sklad_k,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str kikz;
class iceb_l_sklad_v data;
int gor=0,ver=0;

data.sklad_i.new_plus(sklad_k->ravno());
data.name_window.plus(__FUNCTION__);
    
data.rk.clear_zero();

if(data.sklad_i.getdlinna() > 1)
 {
  data.rk.kod.new_plus(data.sklad_i.ravno());
  sprintf(strsql,"select * from Sklad where kod=%s",data.sklad_i.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(sql_readkey(&bd,strsql,&row,&cur) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.fmol.new_plus(row[2]);
  data.rk.dolg.new_plus(row[3]);
  data.rk.naim.new_plus(row[1]);
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  data.rk.metka_isp_skl=atoi(row[6]);  
  data.rk.np.new_plus(row[7]);
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Sklad",0,wpredok));
//  printf("%s\n",strsql);
  data.rk.kod.new_plus(strsql);
 }
  
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(data.sklad_i.getdlinna() <= 1)
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
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),&data.window);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(sklad_v_key_press),&data);

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
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 2);
// gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator3=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_box_pack_start (GTK_BOX (vbox),separator1, TRUE, TRUE, 2);

//Вставляем радиокнопки

data.radiobutton2[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Используется"));
data.radiobutton2[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[0]),gettext("Неиспользуется"));

gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[1], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk.metka_isp_skl]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox),separator3, TRUE, TRUE, 2);

  gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 2);
//gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(sklad_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),124);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(sklad_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

label=gtk_label_new(gettext("Фамилия материально-ответственного"));
data.entry[E_FIO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FIO]),255);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO], "activate",G_CALLBACK(sklad_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rk.fmol.ravno());
gtk_widget_set_name(data.entry[E_FIO],iceb_u_inttochar(E_FIO));

label=gtk_label_new(gettext("Должность материально-ответственного"));
data.entry[E_DOLG] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DOLG]),124);
gtk_box_pack_start (GTK_BOX (hbox[E_DOLG]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DOLG]), data.entry[E_DOLG], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DOLG], "activate",G_CALLBACK(sklad_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOLG]),data.rk.dolg.ravno());
gtk_widget_set_name(data.entry[E_DOLG],iceb_u_inttochar(E_DOLG));

label=gtk_label_new(gettext("Населённый пункт"));
data.entry[E_NP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NP]),255);
gtk_box_pack_start (GTK_BOX (hbox[E_NP]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NP]), data.entry[E_NP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NP], "activate",G_CALLBACK(sklad_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NP]),data.rk.np.ravno());
gtk_widget_set_name(data.entry[E_NP],iceb_u_inttochar(E_NP));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(sklad_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(sklad_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 sklad_k->new_plus(data.rk.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
 
return(data.voz);

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   sklad_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_sklad_v *data)
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
void  sklad_v_knopka(GtkWidget *widget,class iceb_l_sklad_v *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    if(sklad_zap(data) == 0)
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

void    sklad_v_vvod(GtkWidget *widget,class iceb_l_sklad_v *data)
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
  case E_FIO:
    data->rk.fmol.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DOLG:
    data->rk.dolg.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int sklad_zap(class iceb_l_sklad_v *data)
{
char strsql[2048];
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

short metkakor=0;

if(data->sklad_i.getdlinna() <= 1)
 {

  if(sklad_pk(data->rk.kod.ravno(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Sklad \
values ('%s','%s','%s','%s',%d,%ld,%d,'%s')",
   data->rk.kod.ravno_filtr(),
   data->rk.naim.ravno_filtr(),
   data->rk.fmol.ravno_filtr(),
   data->rk.dolg.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.metka_isp_skl,
   data->rk.np.ravno_filtr());
 }
else
 {
  
  if(iceb_u_SRAV(data->sklad_i.ravno(),data->rk.kod.ravno(),0) != 0)
   {
    if(sklad_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Sklad \
set \
kod='%s',\
naik='%s',\
fmol='%s',\
dolg='%s',\
ktoz=%d,\
vrem=%ld,\
mi=%d,\
np='%s' \
where kod='%s'",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   data->rk.fmol.ravno_filtr(),
   data->rk.dolg.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.metka_isp_skl,
   data->rk.np.ravno_filtr(),
   data->sklad_i.ravno());

   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 sklad_kkvt(data->sklad_i.ravno(),data->rk.kod.ravno(),data->window);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int sklad_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Sklad where kod=%s",kod);

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

void sklad_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Kart set sklad=%s where sklad=%s",
nkod,skod);
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

sprintf(strsql,"update Zkart set sklad=%s where sklad=%s",
nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Dokummat set sklad=%s where sklad=%s",
nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Dokummat1 set sklad=%s where sklad=%s",
nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);


sprintf(strsql,"update Dokummat2 set sklad=%s where sklad=%s",
nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Dokummat3 set sklad=%s where sklad=%s",
nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Uplouot set skl=%s where skl=%s",
nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

}
