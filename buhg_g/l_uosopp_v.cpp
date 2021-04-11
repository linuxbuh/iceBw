/*$Id: l_uosopp_v.c,v 1.25 2013/10/04 07:27:16 sasa Exp $*/
/*01.03.2016	28.12.2005	Белых А.И.	l_uosopp_v.c
Ввод и корректировка записей кодов операций на приход
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
  KOLENTER  
 };

class l_uosopp_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton1[3];
  GtkWidget *radiobutton2[2];
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class uslop_rek rk;
  
  iceb_u_str kodk; //Код записи которую корректируют
  
  l_uosopp_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[0])) == TRUE)
     {
      rk.vido=0;
     }
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[1])) == TRUE)
     {
      rk.vido=1;
     }
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[2])) == TRUE)
     {
      rk.vido=2;
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

gboolean   l_uosopp_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosopp_v_data *data);
void    l_uosopp_v_v_vvod(GtkWidget *widget,class l_uosopp_v_data *data);
void  l_uosopp_v_v_knopka(GtkWidget *widget,class l_uosopp_v_data *data);
void uosopp_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok);
int uosopp_zap(class l_uosopp_v_data *data);
int uosopp_pk(const char *kod,GtkWidget *wpredok);


extern SQL_baza bd;

int l_uosopp_v(iceb_u_str *kodzap, //Код записи для корректировки ; возвращается код только что введённой записи
GtkWidget *wpredok)
{
l_uosopp_v_data data;
data.kodk.new_plus(kodzap->ravno());
char strsql[512];
int gor=0,ver=0;
class iceb_u_str kikz;
data.name_window.plus(__FUNCTION__);

if(kodzap->getdlinna() > 1)
 {
  //читаем корректируемую запись
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Uospri where kod='%s'",kodzap->ravno());
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uosopp_v_v_key_press),&data);

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
//  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator2=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator3=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

gtk_box_pack_start (GTK_BOX (vbox),separator1, TRUE, TRUE, 1);
//Вставляем радиокнопки
/************************
GSList *group;

data.radiobutton1[0]=gtk_radio_button_new_with_label(NULL,gettext("Внешняя"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton1[0], "clicked",G_CALLBACK(uosopp_radio1_0),&data);

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton1[0]));

data.radiobutton1[1]=gtk_radio_button_new_with_label(group,gettext("Внутренняя"));
g_signal_connect(data.radiobutton1[1], "clicked",G_CALLBACK(uosopp_radio1_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[1], TRUE, TRUE, 0);

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton1[1]));

data.radiobutton1[2]=gtk_radio_button_new_with_label(group,gettext("Смена стоимости"));
g_signal_connect(data.radiobutton1[2], "clicked",G_CALLBACK(uosopp_radio1_2),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[2], TRUE, TRUE, 0);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[data.rk.vido]),TRUE); //Устанавливем активной кнопку
************************/
data.radiobutton1[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Внешняя"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[0], TRUE, TRUE, 0);

data.radiobutton1[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton1[0]),gettext("Внутренняя"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[1], TRUE, TRUE, 0);

data.radiobutton1[2]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton1[1]),gettext("Смена стоимости"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[2], TRUE, TRUE, 0);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[data.rk.vido]),TRUE); //Устанавливем активной кнопку



/*************************/

gtk_box_pack_start (GTK_BOX (vbox),separator2, TRUE, TRUE, 1);

/***********************************
data.radiobutton2[0]=gtk_radio_button_new_with_label(NULL,gettext("Проводки нужно делать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton2[0], "clicked",G_CALLBACK(uosopp_radio2_0),&data);

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton2[0]));

data.radiobutton2[1]=gtk_radio_button_new_with_label(group,gettext("Проводки не нужно делать"));
g_signal_connect(data.radiobutton2[1], "clicked",G_CALLBACK(uosopp_radio2_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk.prov]),TRUE); //Устанавливем активной кнопку
*************************/
data.radiobutton2[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Проводки нужно делать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);

data.radiobutton2[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[0]),gettext("Проводки не нужно делать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk.prov]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox),separator3, TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 1);


sprintf(strsql,"%s",gettext("Код операции"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);

data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_uosopp_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

sprintf(strsql,"%s",gettext("Наименование операции"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label,FALSE, FALSE, 0);

data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_uosopp_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));




sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_uosopp_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_uosopp_v_v_knopka),&data);
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

/**************************/

void  uosopp_radio2_0(GtkWidget *widget,class l_uosopp_v_data *data)
{
//g_print("uosopp_radio0\n");

data->rk.prov=0;
//printf("prov=0\n");

}
void  uosopp_radio2_1(GtkWidget *widget,class l_uosopp_v_data *data)
{
//g_print("uosopp_radio1\n");
data->rk.prov=1;
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosopp_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosopp_v_data *data)
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
void  l_uosopp_v_v_knopka(GtkWidget *widget,class l_uosopp_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek(); //Читаем реквизиты меню
    if(uosopp_zap(data) != 0)
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

void    l_uosopp_v_v_vvod(GtkWidget *widget,class l_uosopp_v_data *data)
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
int uosopp_zap(class l_uosopp_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);


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

if(iceb_u_SRAV(data->kodk.ravno(),data->rk.kod.ravno(),0) != 0)
 if(uosopp_pk(data->rk.kod.ravno(),data->window) != 0)
  return(1);
  
short metkakor=0;

if(data->kodk.getdlinna() <= 1)
 {

  
  sprintf(strsql,"insert into Uospri \
values ('%s','%s',%d,%ld,%d,%d)",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.vido,
   data->rk.prov);
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
  sprintf(strsql,"update Uospri \
set \
kod='%s',\
naik='%s',\
ktoz=%d,\
vrem=%ld,\
vido=%d,\
prov=%d \
where kod='%s'",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.vido,
   data->rk.prov,
   data->kodk.ravno());
   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(data->kodk.getdlinna() <= 1)
 if(data->rk.vido > 0) /*внутреннее перемещение */
  {
  sprintf(strsql,"insert into Uosras \
values ('%s','%s',%d,%ld,%d,%d)",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.vido,
   data->rk.prov);

   iceb_sql_zapis(strsql,0,0,data->window);
  }

if(metkakor == 1)
 uosopp_kkvt(data->kodk.ravno(),data->rk.kod.ravno(),data->window);


return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void uosopp_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];
sprintf(strsql,"update Uosdok set kodop='%s' where kodop='%s' and tipz=1",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
  return;
 }

sprintf(strsql,"update Prov set oper='%s' where oper='%s' and tz=1 and kto='%s'",nkod,skod,ICEB_MP_UOS);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Reenn1 set ko='%s' where ko='%s' and mi=3",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int uosopp_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Uospri where kod='%s'",kod);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}
