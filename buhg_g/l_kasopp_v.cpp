/*$Id: l_kasopp_v.c,v 1.36 2013/10/04 07:27:15 sasa Exp $*/
/*29.02.2016	18.01.2006	Белых А.И.	l_kasopp_v.c
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
  E_SHETK,
  E_KOD_CN,
  KOLENTER  
 };

class l_kasopp_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton2[2];
  GtkWidget *label_naishet;
  GtkWidget *label_naishetk;
  GtkWidget *label_naikodcn;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class kasop_rek rk;
  
  iceb_u_str kodk; //Код записи которую корректируют
  
  l_kasopp_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[0])) == TRUE)
     rk.prov=0;
    else
     rk.prov=1;
   }
 };

gboolean   l_kasopp_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_kasopp_v_data *data);
void    l_kasopp_v_v_vvod(GtkWidget *widget,class l_kasopp_v_data *data);
void  l_kasopp_v_v_knopka(GtkWidget *widget,class l_kasopp_v_data *data);
void kasopp_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok);
int kasopp_zap(class l_kasopp_v_data *data);
int kasopp_pk(const char *kod,GtkWidget *wpredok);

void  kasopp_v_e_knopka(GtkWidget *widget,class l_kasopp_v_data *data);

extern SQL_baza bd;

int l_kasopp_v(iceb_u_str *kodzap, //Код записи для корректировки ; возвращается код только что введённой записи
GtkWidget *wpredok)
{
int gor=0,ver=0;
class l_kasopp_v_data data;
data.kodk.new_plus(kodzap->ravno());

iceb_u_str naim_shet("");
iceb_u_str naim_shetk("");
class iceb_u_str naim_kodcn("");
char strsql[512];
class iceb_u_str kikz;

data.name_window.plus(__FUNCTION__);

if(kodzap->getdlinna() > 1)
 {
  //читаем корректируемую запись
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Kasop1 where kod='%s'",kodzap->ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.rk.kod.new_plus(row[0]);
  data.rk.naim.new_plus(row[1]);
  data.rk.shet.new_plus(row[2]);
  data.rk.shetk.new_plus(row[3]);
  data.rk.prov=atoi(row[4]);
  kikz.plus(iceb_kikz(row[5],row[6],wpredok));
  if(atoi(row[7]) != 0)
    data.rk.kod_cn.new_plus(row[7]);
  
  if(data.rk.shet.getdlinna() > 1)
   {
    //Узнаём наименование счёта
    sprintf(strsql,"select nais from Plansh where ns='%s'",data.rk.shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_shet.new_plus(row[0]);
   }

  if(data.rk.shetk.getdlinna() > 1)
   {
    //Узнаём наименование счёта корреспондента
    sprintf(strsql,"select nais from Plansh where ns='%s'",data.rk.shetk .ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_shetk.new_plus(row[0]);
   }
  if(data.rk.kod_cn.getdlinna() > 1)
   {
    //Узнаём наименование счёта корреспондента
    sprintf(strsql,"select naik from Kascn where kod=%d",data.rk.kod_cn.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_kodcn.new_plus(row[0]);
   }
  
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_kasopp_v_v_key_press),&data);

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
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 1);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator3=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_box_pack_start (GTK_BOX (vbox),separator1, TRUE, TRUE, 2);

//Вставляем радиокнопки

data.radiobutton2[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Проводки не нужно делать"));
data.radiobutton2[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[0]),gettext("Проводки нужно делать"));

gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[1], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk.prov]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox),separator3, TRUE, TRUE, 2);

gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 1);


sprintf(strsql,"%s",gettext("Код операции"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);

data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_kasopp_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

sprintf(strsql,"%s",gettext("Наименование операции"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label,FALSE, FALSE, 0);

data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),249);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_kasopp_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));


sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(kasopp_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(l_kasopp_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk.shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

data.label_naishet=gtk_label_new(naim_shet.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.label_naishet,TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Счёт корреспондент"));
data.knopka_enter[E_SHETK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETK]), data.knopka_enter[E_SHETK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHETK],"clicked",G_CALLBACK(kasopp_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETK],iceb_u_inttochar(E_SHETK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETK],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHETK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHETK]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETK]), data.entry[E_SHETK],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHETK], "activate",G_CALLBACK(l_kasopp_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETK]),data.rk.shetk.ravno());
gtk_widget_set_name(data.entry[E_SHETK],iceb_u_inttochar(E_SHETK));

data.label_naishetk=gtk_label_new(naim_shetk.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_SHETK]), data.label_naishetk,TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Код целевого назначения"));
data.knopka_enter[E_KOD_CN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_CN]), data.knopka_enter[E_KOD_CN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_CN],"clicked",G_CALLBACK(kasopp_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_CN],iceb_u_inttochar(E_KOD_CN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_CN],gettext("Выбор кода целевого назначения"));

data.entry[E_KOD_CN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_CN]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_CN]), data.entry[E_KOD_CN],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_CN], "activate",G_CALLBACK(l_kasopp_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_CN]),data.rk.kod_cn.ravno());
gtk_widget_set_name(data.entry[E_KOD_CN],iceb_u_inttochar(E_KOD_CN));

data.label_naikodcn=gtk_label_new(naim_kodcn.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_CN]), data.label_naikodcn,TRUE, TRUE, 0);


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_kasopp_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_kasopp_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 kodzap->new_plus(data.rk.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  kasopp_v_e_knopka(GtkWidget *widget,class l_kasopp_v_data *data)
{
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_SHET:
    if(iceb_vibrek(1,"Plansh",&data->rk.shet,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naishet),naim.ravno(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk.shet.ravno());
    return;
  case E_SHETK:
    if(iceb_vibrek(1,"Plansh",&data->rk.shetk,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naishetk),naim.ravno(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETK]),data->rk.shetk.ravno());
    return;

  case E_KOD_CN:
    if(l_kaskcn(1,&data->rk.kod_cn,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naikodcn),naim.ravno(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_CN]),data->rk.kod_cn.ravno());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_kasopp_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_kasopp_v_data *data)
{

//printf("l_kasopp_v_v_key_press\n");
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
void  l_kasopp_v_v_knopka(GtkWidget *widget,class l_kasopp_v_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek(); //Читаем реквизиты меню
    if(kasopp_zap(data) != 0)
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

void    l_kasopp_v_v_vvod(GtkWidget *widget,class l_kasopp_v_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
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
      sprintf(strsql,"select nais from Plansh where ns='%s'",data->rk.shet.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
        gtk_label_set_text(GTK_LABEL(data->label_naishet),strsql);
       }
      else
        gtk_label_set_text(GTK_LABEL(data->label_naishet),"");
    break;

  case E_SHETK:
    data->rk.shetk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

      sprintf(strsql,"select nais from Plansh where ns='%s'",data->rk.shetk.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
        gtk_label_set_text(GTK_LABEL(data->label_naishetk),strsql);
       }    
      else
        gtk_label_set_text(GTK_LABEL(data->label_naishetk),"");
    break;

  case E_KOD_CN:
    data->rk.kod_cn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

      sprintf(strsql,"select naik from Kascn where kod=%d",data->rk.kod_cn.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
        gtk_label_set_text(GTK_LABEL(data->label_naikodcn),strsql);
       }    
      else
        gtk_label_set_text(GTK_LABEL(data->label_naikodcn),"");

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
int kasopp_zap(class l_kasopp_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);

printf("kasopp_v_zap prov=%d\n",data->rk.prov);

data->read_rek(); //Читаем реквизиты меню

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

class OPSHET opsh;

if(data->rk.shet.getdlinna() > 1)
 if(iceb_prsh1(data->rk.shet.ravno(),&opsh,data->window) != 0)
  return(1);

if(data->rk.shetk.getdlinna() > 1)
 if(iceb_prsh1(data->rk.shetk.ravno(),&opsh,data->window) != 0)
  return(1);

if(iceb_u_SRAV(data->kodk.ravno(),data->rk.kod.ravno(),0) != 0)
 if(kasopp_pk(data->rk.kod.ravno(),data->window) != 0)
  return(1);
//Проверяем код целевого назначения  
if(data->rk.kod_cn.getdlinna() > 1)
 {
  sprintf(strsql,"select naik from Kascn where kod=%d",data->rk.kod_cn.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) <= 0)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код целевого назначения"),data->rk.kod_cn.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }
 
short metkakor=0;

if(data->kodk.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Kasop1 \
values ('%s','%s','%s','%s',%d,%d,%ld,%d)",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   data->rk.shet.ravno(),
   data->rk.shetk.ravno(),
   data->rk.prov,
   iceb_getuid(data->window),vrem,
   data->rk.kod_cn.ravno_atoi());
 }
else
 {
  
  if(iceb_u_SRAV(data->kodk.ravno(),data->rk.kod.ravno(),0) != 0)
   {
    if(iceb_menu_danet(gettext("Корректировать код ? Вы уверены ?"),2,data->window) == 2)
      return(1);
    metkakor=1;
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Kasop1 \
set \
kod='%s',\
naik='%s',\
shetkas='%s',\
shetkor='%s',\
metkapr=%d,\
ktoz=%d,\
vrem=%ld,\
kcn=%d \
where kod='%s'",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   data->rk.shet.ravno(),
   data->rk.shetk.ravno(),
   data->rk.prov,
   iceb_getuid(data->window),vrem,
   data->rk.kod_cn.ravno_atoi(),
   data->kodk.ravno());
   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 kasopp_kkvt(data->kodk.ravno(),data->rk.kod.ravno(),data->window);


return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void kasopp_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"update Kasord set kodop='%s' where kodop='%s' and tp=1",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
  return;
 }
sprintf(strsql,"update Prov set oper='%s' where oper='%s' and tz=1 and kto='%s'",nkod,skod,ICEB_MP_KASA);
if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int kasopp_pk(const char *kod,GtkWidget *wpredok)
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
