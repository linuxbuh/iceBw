/*$Id: l_uslopr_v.c,v 1.23 2013/10/04 07:27:16 sasa Exp $*/
/*02.03.2016	18.07.2005	Белых А.И.	l_uslopr_v.c
Ввод и корректировка записей 
*/
#include "buhg_g.h"
#include "l_uslop.h"

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
  E_KONTR,
  KOLENTER  
 };

class l_uslopr_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton1[2];
  GtkWidget *radiobutton2[2];
  GtkWidget *label_naishet;
  GtkWidget *label_naikontr;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class uslop_rek rk;
  
  iceb_u_str kodk; //Код записи которую корректируют
  
  l_uslopr_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    rk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM])));
    rk.shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    rk.kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR])));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[0])) == TRUE)
     {
      rk.vido=0;
     }
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[1])) == TRUE)
     {
      rk.vido=1;
     }

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[0])) == TRUE)
     {
      rk.prov=0;
     }
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[1])) == TRUE)
     {
      rk.prov=1;
     }

   }
 };

gboolean   l_uslopr_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uslopr_v_data *data);
void    l_uslopr_v_v_vvod(GtkWidget *widget,class l_uslopr_v_data *data);
void  l_uslopr_v_v_knopka(GtkWidget *widget,class l_uslopr_v_data *data);
void uslopr_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok);
int uslopr_zap(class l_uslopr_v_data *data);
int uslopr_pk(const char *kod,GtkWidget *wpredok);

void  uslopr_v_e_knopka(GtkWidget *widget,class l_uslopr_v_data *data);

extern SQL_baza bd;

int l_uslopr_v(iceb_u_str *kodzap, //Код записи для корректировки; возвращается введённый код
GtkWidget *wpredok)
{
class l_uslopr_v_data data;
int gor=0,ver=0;
data.kodk.new_plus(kodzap->ravno());
data.name_window.plus(__FUNCTION__);
class iceb_u_str naim_shet("");
char strsql[512];
class iceb_u_str kikz;
class iceb_u_str naim_kontr("");
SQL_str row;
SQLCURSOR cur;

if(kodzap->getdlinna() > 1)
 {
  //читаем корректируемую запись
  sprintf(strsql,"select * from Usloper2 where kod='%s'",kodzap->ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.rk.kod.new_plus(row[0]);
  data.rk.naim.new_plus(row[1]);
  kikz.plus(iceb_kikz(row[2],row[3],wpredok));
  data.rk.vido=atoi(row[4]);
  data.rk.prov=atoi(row[5]);
  data.rk.shet.new_plus(row[6]);
  data.rk.kontr.new_plus(row[7]);
  if(data.rk.shet.getdlinna() > 1)
   {
    //Узнаём наименование счёта
    sprintf(strsql,"select nais from Plansh where ns='%s'",data.rk.shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_shet.new_plus(row[0]);
   }
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data.rk.kontr.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    naim_kontr.new_plus(row[0]);
  
 }
else
 data.rk.kontr.new_plus("00");
 
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data.rk.kontr.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  naim_kontr.new_plus(row[0]);

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uslopr_v_v_key_press),&data);

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
GtkWidget *separator2=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator3=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator1);
//Вставляем радиокнопки
data.radiobutton1[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Внешняя"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[0], TRUE, TRUE, 0);

data.radiobutton1[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton1[0]),gettext("Внутренняя"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[1], TRUE, TRUE, 0);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[data.rk.vido]),TRUE); //Устанавливем активной кнопку

/*************************/
gtk_container_add (GTK_CONTAINER (vbox), separator2);

data.radiobutton2[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Проводки нужно делать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);

data.radiobutton2[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[0]),gettext("Проводки не нужно делать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk.prov]),TRUE); //Устанавливем активной кнопку

gtk_container_add (GTK_CONTAINER (vbox), separator3);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Код операции"));
label=gtk_label_new(strsql);
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_uslopr_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

sprintf(strsql,"%s",gettext("Наименование операции"));
label=gtk_label_new(strsql);
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_uslopr_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(uslopr_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор единицы измерения"));

data.entry[E_SHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(l_uslopr_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk.shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

data.label_naishet=gtk_label_new(naim_shet.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.label_naishet,TRUE, TRUE, 0);

data.knopka_enter[E_KONTR]=gtk_button_new_with_label(gettext("Код контрагента"));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(uslopr_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR],gettext("Выбор единицы измерения"));

data.entry[E_KONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KONTR]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(l_uslopr_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk.kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));

data.label_naikontr=gtk_label_new(naim_kontr.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.label_naikontr,TRUE, TRUE, 0);



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_uslopr_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_uslopr_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 kodzap->new_plus(data.rk.kod.ravno());
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uslopr_v_e_knopka(GtkWidget *widget,class l_uslopr_v_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_SHET:
    if(iceb_vibrek(1,"Plansh",&data->rk.shet,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naishet),naim.ravno(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk.shet.ravno());
    return;

  case E_KONTR:
    if(iceb_vibrek(1,"Kontragent",&data->rk.kontr,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naikontr),naim.ravno(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk.kontr.ravno());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uslopr_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uslopr_v_data *data)
{

//printf("l_uslopr_v_v_key_press\n");
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
void  l_uslopr_v_v_knopka(GtkWidget *widget,class l_uslopr_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek(); //Читаем реквизиты меню
    if(uslopr_zap(data) != 0)
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

void    l_uslopr_v_v_vvod(GtkWidget *widget,class l_uslopr_v_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

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

  case E_KONTR:
    data->rk.kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->rk.kontr.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naikontr),strsql);
     }    
    else
      gtk_label_set_text(GTK_LABEL(data->label_naikontr),"");
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
int uslopr_zap(class l_uslopr_v_data *data)
{
char strsql[1024];


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
if(data->rk.kontr.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код контрагента!"),data->window);
  return(1);
 }
sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",data->rk.kontr.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s %s!",gettext("Не найден код контрагента"),data->rk.kontr.ravno(),gettext("в списке контрагентов"));
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

class OPSHET opsh;
if(data->rk.shet.getdlinna() > 1)
 if(iceb_prsh1(data->rk.shet.ravno(),&opsh,data->window) != 0)
  return(1);

if(iceb_u_SRAV(data->kodk.ravno(),data->rk.kod.ravno(),0) != 0)
 if(uslopr_pk(data->rk.kod.ravno(),data->window) != 0)
   return(1);
  
short metkakor=0;

if(data->kodk.getdlinna() <= 1)
 {

  
  sprintf(strsql,"insert into Usloper2 \
values ('%s','%s',%d,%ld,%d,%d,'%s','%s')",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),time(NULL),
   data->rk.vido,
   data->rk.prov,
   data->rk.shet.ravno(),
   data->rk.kontr.ravno_filtr());
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
  sprintf(strsql,"update Usloper2 \
set \
kod='%s',\
naik='%s',\
ktoz=%d,\
vrem=%ld,\
vido=%d,\
prov=%d,\
shet='%s',\
kk='%s' \
where kod='%s'",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),time(NULL),
   data->rk.vido,
   data->rk.prov,
   data->rk.shet.ravno(),
   data->rk.kontr.ravno_filtr(),
   data->kodk.ravno());
   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 uslopr_kkvt(data->kodk.ravno(),data->rk.kod.ravno(),data->window);


return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void uslopr_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{
char strsql[512];
sprintf(strsql,"update Usldokum set kodop='%s' where kodop='%s' and tp=2",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
  return;
 }

sprintf(strsql,"update Prov set oper='%s' where oper='%s' and tz=2 and kto='%s'",nkod,skod,ICEB_MP_USLUGI);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Reenn set ko='%s' where ko='%s' and mi=2",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int uslopr_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Usloper2 where kod='%s'",kod);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}
