/*$Id: iceb_l_opmur_v.c,v 1.27 2013/08/25 08:27:06 sasa Exp $*/
/*16.11.2016	05.05.2004	Белых А.И.	iceb_l_opmur_v.c
Ввод и корректировка операций расхода для "Материального учёта"
*/
#include "iceb_libbuh.h"
#include "iceb_l_opmur.h"

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
  E_SHETA,
  E_KONTR,
  KOLENTER  
 };

class iceb_l_opmur_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *menuvid;
  GtkWidget *menuprov;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class opmur_rek rk;
  class iceb_u_str kod_op;  
  
  iceb_l_opmur_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    rk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM])));
    rk.sheta.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHETA])));
    rk.kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR])));
    rk.vidop=gtk_combo_box_get_active (GTK_COMBO_BOX(menuvid));
    rk.prov=gtk_combo_box_get_active (GTK_COMBO_BOX(menuprov));
   }
 };

gboolean   opmur_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_opmur_v_data *data);
void  opmur_v_knopka(GtkWidget *widget,class iceb_l_opmur_v_data *data);
void    opmur_v_vvod(GtkWidget *widget,class iceb_l_opmur_v_data *data);
int opmur_pk(const char *god,GtkWidget*);
int opmur_zap(class iceb_l_opmur_v_data *data);
void  iceb_l_opmur_v_e_knopka(GtkWidget *widget,class iceb_l_opmur_v_data *data);

GtkWidget *opmur_vidop_v(int nomerstr);
GtkWidget *opmur_vidop_p(int nomerstr);
void            opmur_get_pm0(GtkWidget*,int*);
void            opmur_get_pm1(GtkWidget*,int*);
void            opmur_get_pm2(GtkWidget*,int*);
void opmur_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok);

extern SQL_baza bd;

int iceb_l_opmur_v(class iceb_u_str *kod_op,GtkWidget *wpredok)
{
class iceb_l_opmur_v_data data;
char strsql[512];
SQL_str row;
int gor=0,ver=0;
class SQLCURSOR cur;
data.kod_op.new_plus(kod_op->ravno());
data.rk.clear_zero();
data.name_window.plus(__FUNCTION__);

if(data.kod_op.getdlinna() > 1)
 {
  data.rk.kod.new_plus(data.kod_op.ravno());
  
  sprintf(strsql,"select * from Rashod where kod='%s'",data.kod_op.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.rk.naim.new_plus(row[1]);
  data.rk.vidop=atoi(row[4]);
  data.rk.sheta.new_plus(row[5]);
  data.rk.prov=atoi(row[6]);
  data.rk.kontr.new_plus(row[7]);
 }
else
 data.rk.kontr.new_plus("00");
  
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

if(data.kod_op.getdlinna() <= 1)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
else
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(opmur_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

if(data.kod_op.getdlinna() <= 1)
  sprintf(strsql,"%s",gettext("Ввод новой записи"));
else
 {
  sprintf(strsql,"%s\n%s:%s %s:%s",
  gettext("Корректировка записи"),
  gettext("Записал"),
  iceb_kszap(row[2],wpredok),
  gettext("Дата и время записи"),
  iceb_u_vremzap(row[3]));
 } 

GtkWidget *label=gtk_label_new(strsql);

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
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox), hbox[i], FALSE, FALSE, 0);




GtkWidget *hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox4),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox5),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox5);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(opmur_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(opmur_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

sprintf(strsql,"%s (,,)",gettext("Счета"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETA]), label, FALSE, FALSE, 0);

data.entry[E_SHETA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHETA]),99);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETA]), data.entry[E_SHETA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHETA], "activate",G_CALLBACK(opmur_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETA]),data.rk.sheta.ravno());
gtk_widget_set_name(data.entry[E_SHETA],iceb_u_inttochar(E_SHETA));

data.knopka_enter[E_KONTR]=gtk_button_new_with_label(gettext("Код контрагента"));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(iceb_l_opmur_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR],gettext("Выбор контрагента реквизиты которого будут указаны в расходных документах"));

data.entry[E_KONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KONTR]),16);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(opmur_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk.kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));

label=gtk_label_new(gettext("Вид операции"));
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
data.menuvid = opmur_vidop_v(data.rk.vidop);
gtk_box_pack_start (GTK_BOX (hbox4), data.menuvid, FALSE, FALSE, 0);

label=gtk_label_new(gettext("Проводки"));
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE, 0);
data.menuprov = opmur_vidop_p(data.rk.prov);
gtk_box_pack_start (GTK_BOX (hbox5), data.menuprov, FALSE, FALSE, 0);



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(opmur_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(opmur_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 kod_op->new_plus(data.rk.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  iceb_l_opmur_v_e_knopka(GtkWidget *widget,class iceb_l_opmur_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_KONTR:

    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),kod.ravno());
      
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   opmur_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_opmur_v_data *data)
{
//char  bros[512];

//printf("opmur_v_key_press\n");
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
void  opmur_v_knopka(GtkWidget *widget,class iceb_l_opmur_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    if(opmur_zap(data) == 0)
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

void    opmur_v_vvod(GtkWidget *widget,class iceb_l_opmur_v_data *data)
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

  case E_SHETA:
    data->rk.sheta.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int opmur_zap(class iceb_l_opmur_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);

data->read_rek(); 


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
  iceb_menu_soob(gettext("Не введено код контрагента!"),data->window);
  return(1);
 }
/*проверяем код контрагента*/
sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",data->rk.kontr.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код контрагента"),data->rk.kontr.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
if(iceb_u_SRAV(data->kod_op.ravno(),data->rk.kod.ravno(),0) != 0)
 if(opmur_pk(data->rk.kod.ravno(),data->window) != 0)
  return(1);

short metkakor=0;

if(data->kod_op.getdlinna() <= 1)
 {

  
  sprintf(strsql,"insert into Rashod \
values ('%s','%s',%d,%ld,%d,'%s',%d,'%s')",
   data->rk.kod.ravno_filtr(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.vidop,
   data->rk.sheta.ravno_filtr(),
   data->rk.prov,
   data->rk.kontr.ravno_filtr());
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_op.ravno(),data->rk.kod.ravno(),0) != 0)
   {
    if(iceb_menu_danet(gettext("Корректировать код ? Вы уверены ?"),2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Rashod \
set \
kod='%s',\
naik='%s',\
ktoz=%d,\
vrem=%ld,\
vido=%d,\
sheta='%s',\
prov=%d,\
kk='%s' \
where kod='%s'",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.vidop,
   data->rk.sheta.ravno_filtr(),
   data->rk.prov,
   data->rk.kontr.ravno_filtr(),
   data->kod_op.ravno());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(data->kod_op.getdlinna() <= 1 && data->rk.vidop > 0)
 { 
  sprintf(strsql,"insert into Prihod \
values ('%s','%s',%d,%ld,%d,%d)",
   data->rk.kod.ravno_filtr(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.vidop,
   1);
  iceb_sql_zapis(strsql,0,0,data->window);
 }
gtk_widget_hide(data->window);

if(metkakor == 1)
 opmur_kkvt(data->kod_op.ravno(),data->rk.kod.ravno(),data->window);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int opmur_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Rashod where kod='%s'",kod);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}

/*********************************************/
/**********************************************/

/*****************************/
/*Меню выбора вида операции     */
/*****************************/

GtkWidget *opmur_vidop_v(int nomerstr)
{

GtkWidget *opt;
class iceb_u_spisok strmenu;
strmenu.plus(gettext("Внешняя"));
strmenu.plus(gettext("Внутренняя"));
strmenu.plus(gettext("Изменения учётных данных"));

iceb_pm_vibor(&strmenu,&opt,nomerstr);

return(opt);
}

/*****************************/
/*Меню выбора проводок     */
/*****************************/

GtkWidget *opmur_vidop_p(int nomerstr)
{

GtkWidget *opt;
class iceb_u_spisok strmenu;
strmenu.plus(gettext("Нужно делать"));
strmenu.plus(gettext("Не нужно делать"));

iceb_pm_vibor(&strmenu,&opt,nomerstr);
return(opt);
}
/**********************/
/*Читаем              */
/**********************/

void   opmur_get_pm0(GtkWidget *widget,int *data)
{
*data=0;
printf("opmur_get_pm0-%d\n",*data);
}
void   opmur_get_pm1(GtkWidget *widget,int *data)
{
*data=1;
printf("opmur_get_pm1-%d\n",*data);
}
void   opmur_get_pm2(GtkWidget *widget,int *data)
{
*data=2;
printf("opmur_get_pm2-%d\n",*data);
}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void opmur_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Dokummat set kodop='%s' where kodop='%s' and tip=2",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
  return;
 }

sprintf(strsql,"update Prov set oper='%s' where oper='%s' and tz=2 and kto='%s'",nkod,skod,ICEB_MP_MATU);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Reenn set ko='%s' where ko='%s' and mi=1",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

}

