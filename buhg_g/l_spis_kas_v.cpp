/*$Id: l_spis_kas_v.c,v 1.22 2013/10/04 07:27:16 sasa Exp $*/
/*29.02.2016	18.01.2006	Белых А.И.	l_spis_kas_v.c
Ввод и корректировка записей кодов операций прихода для "Учёта кассовых ордеров"
*/
#include "buhg_g.h"
#include "l_kasop.h"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOD,
  E_NAIM,
  E_SHET,
  E_VAL,
  E_FIO_KASIRA,
  KOLENTER  
 };



class l_spis_kas_v_data
 {
  public:
  //Реквизиты записи
  iceb_u_str kod;
  iceb_u_str naim;
  iceb_u_str shet;
  iceb_u_str fio_kasira;    
  class iceb_u_str kod_val;


  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_naishet;
  GtkWidget *label_naikodval;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  iceb_u_str kodk; //Код записи которую корректируют
  
  l_spis_kas_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    kod.plus("");
    naim.plus("");
    shet.plus("");
    fio_kasira.plus("");
    kod_val.plus("");
   }

  void read_rek()
   {
    kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    naim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM])));
    shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    fio_kasira.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FIO_KASIRA])));
    kod_val.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_VAL])));

   }
 };

gboolean   l_spis_kas_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_spis_kas_v_data *data);
void    l_spis_kas_v_v_vvod(GtkWidget *widget,class l_spis_kas_v_data *data);
void  l_spis_kas_v_v_knopka(GtkWidget *widget,class l_spis_kas_v_data *data);
void spis_kas_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok);
int spis_kas_zap(class l_spis_kas_v_data *data);
int spis_kas_pk(const char *kod,GtkWidget *wpredok);

void  spis_kas_v_e_knopka(GtkWidget *widget,class l_spis_kas_v_data *data);
void  spis_kas_radio2_0(GtkWidget *widget,class l_spis_kas_v_data *data);
void  spis_kas_radio2_1(GtkWidget *widget,class l_spis_kas_v_data *data);

extern SQL_baza bd;
extern double	okrcn;  /*Округление цены*/

int l_spis_kas_v(iceb_u_str *kodzap, //Код записи для корректировки ; возвращается код только что введённой записи
GtkWidget *wpredok)
{
int gor=0,ver=0;
class l_spis_kas_v_data data;
data.kodk.new_plus(kodzap->ravno());
class iceb_u_str naim_shet("");
class iceb_u_str naim_kodval("");
iceb_u_str naim_shetk("");
char strsql[1024];
class iceb_u_str kikz;
data.name_window.plus(__FUNCTION__);

if(kodzap->getdlinna() > 1)
 {
  //читаем корректируемую запись
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Kas where kod=%d",kodzap->ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.kod.new_plus(row[0]);
  data.naim.new_plus(row[1]);
  data.shet.new_plus(row[2]);
  data.fio_kasira.new_plus(row[3]);
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  data.kod_val.new_plus(row[6]);
  
  if(data.shet.getdlinna() > 1)
   {
    //Узнаём наименование счёта
    sprintf(strsql,"select nais from Plansh where ns='%s'",data.shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_shet.new_plus(row[0]);
   }
  if(data.kod_val.getdlinna() > 1)
   {
    //Узнаём наименование валюты
    sprintf(strsql,"select naik from Glksval where kod='%s'",data.kod_val.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_kodval.new_plus(row[0]);
   }
  
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Kas",0,wpredok));
  data.kod.new_plus(strsql);
 } 

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

if(kodzap->getdlinna() > 1)
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
else
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_spis_kas_v_v_key_press),&data);

iceb_u_str zagolov;
if(kodzap->getdlinna() <= 1)
 zagolov.new_plus(gettext("Ввод новой записи"));
else
 {
  zagolov.new_plus(gettext("Корректировка записи"));
  zagolov.ps_plus(kikz.ravno());
 }

GtkWidget *label=gtk_label_new(zagolov.ravno());

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator1);


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Код кассы"));
label=gtk_label_new(strsql);
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_spis_kas_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

sprintf(strsql,"%s",gettext("Наименование кассы"));
label=gtk_label_new(strsql);
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_spis_kas_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(spis_kas_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор единицы измерения"));

data.entry[E_SHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(l_spis_kas_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

data.label_naishet=gtk_label_new(naim_shet.ravno());
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.label_naishet,TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Фамилия кассира"));
label=gtk_label_new(strsql);
data.entry[E_FIO_KASIRA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FIO_KASIRA]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO_KASIRA]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO_KASIRA]), data.entry[E_FIO_KASIRA],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO_KASIRA], "activate",G_CALLBACK(l_spis_kas_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO_KASIRA]),data.fio_kasira.ravno());
gtk_widget_set_name(data.entry[E_FIO_KASIRA],iceb_u_inttochar(E_FIO_KASIRA));


data.knopka_enter[E_VAL]=gtk_button_new_with_label(gettext("Код валюты"));
gtk_box_pack_start (GTK_BOX (hbox[E_VAL]), data.knopka_enter[E_VAL], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_VAL],"clicked",G_CALLBACK(spis_kas_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_VAL],iceb_u_inttochar(E_VAL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_VAL],gettext("Выбор единицы измерения"));

data.entry[E_VAL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VAL]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_VAL]), data.entry[E_VAL],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_VAL], "activate",G_CALLBACK(l_spis_kas_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VAL]),data.kod_val.ravno());
gtk_widget_set_name(data.entry[E_VAL],iceb_u_inttochar(E_VAL));

data.label_naikodval=gtk_label_new(naim_kodval.ravno());
gtk_box_pack_start (GTK_BOX (hbox[E_VAL]), data.label_naikodval,TRUE, TRUE, 0);



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_spis_kas_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_spis_kas_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 kodzap->new_plus(data.kod.ravno());
return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  spis_kas_v_e_knopka(GtkWidget *widget,class l_spis_kas_v_data *data)
{
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_SHET:
    if(iceb_vibrek(1,"Plansh",&data->shet,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naishet),naim.ravno(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno());
    return;

  case E_VAL:
    if(l_sval(1,&data->kod_val,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naikodval),naim.ravno(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VAL]),data->kod_val.ravno());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_spis_kas_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_spis_kas_v_data *data)
{

//printf("l_spis_kas_v_v_key_press\n");
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
void  l_spis_kas_v_v_knopka(GtkWidget *widget,class l_spis_kas_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek(); //Читаем реквизиты меню
    if(spis_kas_zap(data) != 0)
      return;

    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_spis_kas_v_v_vvod(GtkWidget *widget,class l_spis_kas_v_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KOD:
    data->kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NAIM:
    data->naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET:
    data->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select nais from Plansh where ns='%s'",data->shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naishet),strsql);
     }    
    break;

  case E_FIO_KASIRA:
    data->fio_kasira.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int spis_kas_zap(class l_spis_kas_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);


data->read_rek(); //Читаем реквизиты меню

if(data->kod.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }

if(data->kod.ravno_atoi() == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым и неравным нолю!"),data->window);
  return(1);
 }

if(data->naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }

class OPSHET opsh;

if(data->shet.getdlinna() > 1)
 if(iceb_prsh1(data->shet.ravno(),&opsh,data->window) != 0)
  return(1);

if(data->kod_val.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Glksval where kod='%s'",data->kod_val.ravno());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %s",gettext("В справочкике валют не найден код валюты"),data->kod_val.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   } 

 }
short metkakor=0;

if(data->kodk.getdlinna() <= 1)
 {

  if(spis_kas_pk(data->kod.ravno(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Kas \
values (%d,'%s','%s','%s',%d,%ld,'%s')",
   data->kod.ravno_atoi(),
   data->naim.ravno_filtr(),
   data->shet.ravno(),
   data->fio_kasira.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->kod_val.ravno_filtr());
 }
else
 {
  
  if(iceb_u_SRAV(data->kodk.ravno(),data->kod.ravno(),0) != 0)
   {
//    iceb_menu_soob(gettext("Код операции корректировать нельзя !"),data->window);
//    return(1);
    if(spis_kas_pk(data->kod.ravno(),data->window) != 0)
     return(1);

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Kas \
set \
kod=%d,\
naik='%s',\
shet='%s',\
fio='%s',\
ktoz=%d,\
vrem=%ld, \
kv='%s' \
where kod=%d",
   data->kod.ravno_atoi(),
   data->naim.ravno_filtr(),
   data->shet.ravno_filtr(),
   data->fio_kasira.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->kod_val.ravno_filtr(),
   data->kodk.ravno_atoi());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 spis_kas_kkvt(data->kodk.ravno(),data->kod.ravno(),data->window);


return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void spis_kas_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Kasord set kassa=%d where kassa=%d",atoi(nkod),atoi(skod));
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
sprintf(strsql,"update Kasord1 set kassa=%d where kassa=%d",atoi(nkod),atoi(skod));
iceb_sql_zapis(strsql,0,0,wpredok);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int spis_kas_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Kasop1 where kod='%s'",kod);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}
