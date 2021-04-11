/*$Id: l_spec_v.c,v 1.23 2013/09/26 09:46:50 sasa Exp $*/
/*12.11.2008	11.05.2005	Белых А.И.	l_spec_v.c
Ввод записи в изделие
*/
#include "buhg_g.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOD,
  E_KOLIH,
  E_KOMENT,
  E_EI,
  KOLENTER  
 };

class l_spec_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_naim;
  short kl_shift;
  short voz;      //0-ввели запись 1 нет

  short metka_zap; //0-материал 1-услуга  
  iceb_u_str kod_izdel; //код изделия
  iceb_u_str kod_zap;   //Код редактируемой записи в изделии
  iceb_u_str kolih;
  iceb_u_str ei;
  iceb_u_str koment;

  iceb_u_str naim; //Наименование записывается во время проверки кода записи на наличие в справочнике
  
  iceb_u_str kod_zap_p; //первоначальный код записи для корректировки
  
  l_spec_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    metka_zap=0;
    kod_izdel.plus("");
    kod_zap.plus("");
    kolih.plus("");
    ei.plus("");
    koment.plus("");
    naim.plus("");
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };

gboolean   l_spec_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_spec_v_data *data);
void    l_spec_v_v_vvod(GtkWidget *widget,class l_spec_v_data *data);
void  l_spec_v_v_knopka(GtkWidget *widget,class l_spec_v_data *data);
void   l_spec_v_rekviz(class l_spec_v_data *data);
void  l_spec_v_v_e_knopka(GtkWidget *widget,class l_spec_v_data *data);

int l_spec_zap(class l_spec_v_data *data);
int spec1_prov_kod(class l_spec_v_data *data);

extern SQL_baza bd;

int   l_spec_v(const char *kod_izdel,
iceb_u_str *kod_zap, //код записи для корректировки, возвращаем код введённой записи
int metka_zap,//0-материал 1-услуга
GtkWidget *wpredok)
{
printf("l_spec_v_m\n");

class l_spec_v_data data;
char strsql[512];
data.kod_izdel.new_plus(kod_izdel);
data.kod_zap.new_plus(kod_zap->ravno());
data.kod_zap_p.new_plus(kod_zap->ravno());
data.metka_zap=metka_zap;
iceb_u_str kikz;


if(data.kod_zap.getdlinna() > 1) //Корректировка записи
 {
  SQL_str row;
  SQLCURSOR cur;
  
  sprintf(strsql,"select * from Specif where kodi=%d and kodd=%d",
  data.kod_izdel.ravno_atoi(),data.kod_zap.ravno_atoi());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    return(1);
   }

  kikz.plus(iceb_kikz(row[4],row[5],wpredok));

  data.kod_zap.new_plus(row[1]);
  sprintf(strsql,"%.10g",atof(row[2]));
  data.kolih.new_plus(strsql);
  data.koment.new_plus(row[3]);
  data.ei.new_plus(row[7]);
  data.metka_zap=atoi(row[6]);  
 }




data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
if(data.kod_zap.getdlinna() <= 1)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод записи"));
else
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_spec_v_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label;
if(data.kod_zap.getdlinna() <= 1)
  label=gtk_label_new(gettext("Ввод записи"));
else
 {
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno());
 }
data.label_naim=gtk_label_new("");

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
gtk_container_add (GTK_CONTAINER (vbox),label);
gtk_container_add (GTK_CONTAINER (vbox),data.label_naim);
for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);



gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


if(data.metka_zap == 0)
 data.knopka_enter[E_KOD]=gtk_button_new_with_label(gettext("Код материалла"));
if(data.metka_zap == 1)
 data.knopka_enter[E_KOD]=gtk_button_new_with_label(gettext("Код услуги"));

gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.knopka_enter[E_KOD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD],"clicked",G_CALLBACK(l_spec_v_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD],iceb_u_inttochar(E_KOD));
if(data.metka_zap == 0)
  gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD],gettext("Выбор материалла"));
if(data.metka_zap == 1)
  gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD],gettext("Выбор услуги"));

data.entry[E_KOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_spec_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.kod_zap.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

sprintf(strsql,"%s",gettext("Количество"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]),label, FALSE, FALSE, 0);

data.entry[E_KOLIH] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLIH], "activate",G_CALLBACK(l_spec_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_widget_set_name(data.entry[E_KOLIH],iceb_u_inttochar(E_KOLIH));

sprintf(strsql,"%s",gettext("Коментарий"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]),label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_spec_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));

data.knopka_enter[E_EI]=gtk_button_new_with_label(gettext("Единица измерения"));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_EI],"clicked",G_CALLBACK(l_spec_v_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_EI],iceb_u_inttochar(E_EI));
gtk_widget_set_tooltip_text(data.knopka_enter[E_EI],gettext("Выбор единицы измерения"));

data.entry[E_EI] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_EI], "activate",G_CALLBACK(l_spec_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei.ravno());
gtk_widget_set_name(data.entry[E_EI],iceb_u_inttochar(E_EI));


sprintf(strsql,"F2 %s",gettext("Запис"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запис введённой в меню инфармации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_spec_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_spec_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_spec_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 kod_zap->new_plus(data.kod_zap.ravno());
return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_spec_v_v_e_knopka(GtkWidget *widget,class l_spec_v_data *data)
{
iceb_u_str kod;
iceb_u_str naikod;
kod.plus("");
naikod.plus("");
int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_KOD:
    if(data->metka_zap == 0)  //Материалы
     if(l_mater(1,&kod,&naikod,0,0,data->window) == 0)
      {
       data->kod_zap.new_plus(kod.ravno());
       gtk_label_set_text(GTK_LABEL(data->label_naim),naikod.ravno());
      }
    if(data->metka_zap == 1)  //Услуги
     if(l_uslugi(1,&kod,&naikod,data->window) == 0)
      {
       data->kod_zap.new_plus(kod.ravno());
       gtk_label_set_text(GTK_LABEL(data->label_naim),naikod.ravno());
      }
      
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->kod_zap.ravno());
      
    return;  

  case E_EI:
    iceb_vibrek(1,"Edizmer",&data->ei,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->ei.ravno());
    return;  

 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_spec_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_spec_v_data *data)
{

//printf("l_spec_v_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
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
void  l_spec_v_v_knopka(GtkWidget *widget,class l_spec_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(l_spec_zap(data) != 0)
      return;    
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
//    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_spec_v_v_vvod(GtkWidget *widget,class l_spec_v_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_spec_v_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->kod_zap.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    //Проверяем есть ли запись с таким кодом в справочнике
    spec1_prov_kod(data);
    gtk_label_set_text(GTK_LABEL(data->label_naim),data->naim.ravno());
    break;

  case E_KOLIH:
    data->kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOMENT:
    data->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_EI:
    data->ei.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/***********************************/
/*Запись в базу*/
/***********************************/

int l_spec_zap(class l_spec_v_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

// if(kodiz == atoi(VV.VVOD_SPISOK_return_data(0)))
if(data->kod_izdel.ravno_atoi() == data->kod_zap.ravno_atoi())
 {
  iceb_menu_soob(gettext("Материал не может входить сам в себя !"),data->window);
  return(1);
 }

//Проверяем есть ли запись с таким кодом в справочнике
if(spec1_prov_kod(data) != 0)
 return(1);

if(data->kod_zap.getdlinna() <= 1)
 {
  if(data->metka_zap == 0)
   iceb_menu_soob(gettext("Не введён код материалла !"),data->window);
  if(data->metka_zap == 1)
   iceb_menu_soob(gettext("Не введён код услуги !"),data->window);
  return(1);
 }

if(data->kolih.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Не введено количество !"),data->window);
  return(1);
 }

if(data->ei.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Edizmer where kod='%s'",data->ei.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) < 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найдена единица измерения"),data->ei.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);    
   }
 }

double kolih=data->kolih.ravno_atof();

kolih=iceb_u_okrug(kolih,0.00000000001);

time_t vrem;

time(&vrem);

if(data->kod_zap_p.getdlinna() <= 1)
 {
   //Проверяем может этот материал уже введен
   sprintf(strsql,"select kodd from Specif where kodi=%d and kodd=%d",
   data->kod_izdel.ravno_atoi(),data->kod_zap.ravno_atoi());
   if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
    {
     if(data->metka_zap == 0) 
      iceb_menu_soob(gettext("Этот материал уже введён в изделие !"),data->window);
     if(data->metka_zap == 1) 
      iceb_menu_soob(gettext("Эта услуга уже введена в изделие !"),data->window);
     return(1);
    }

  sprintf(strsql,"insert into Specif \
 values (%d,%d,%.10g,'%s',%d,%ld,%d,'%s')",
  data->kod_izdel.ravno_atoi(),data->kod_zap.ravno_atoi(),kolih,
  data->koment.ravno(),iceb_getuid(data->window),vrem,
  data->metka_zap,data->ei.ravno());
 }
else  
  sprintf(strsql,"update Specif \
set \
kodd=%d,\
kolih=%.10g,\
koment='%s',\
ei='%s',\
ktoz=%d,\
vrem=%ld \
 where kodi=%d and kodd=%d",data->kod_zap.ravno_atoi(),kolih,data->koment.ravno(),\
  data->ei.ravno(),\
  iceb_getuid(data->window),vrem,\
  data->kod_izdel.ravno_atoi(),data->kod_zap_p.ravno_atoi());

//iceb_menu_soob(strsql,data->window);

if(sql_zap(&bd,strsql) != 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
  return(1);
 }


return(0);


}
/********************************/
/*Проверка на наличие записи в справочнике*/
/******************************************/
//Если вернули 0-запись есть 1-нет
int spec1_prov_kod(class l_spec_v_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

if(data->kod_zap.ravno_atoi() == 0)
 return(0);
 
//Проверяем есть ли запись с таким кодом в справочнике
if(data->metka_zap == 0)
 sprintf(strsql,"select naimat from Material where kodm=%d",data->kod_zap.ravno_atoi());
if(data->metka_zap == 1)
 sprintf(strsql,"select naius from Uslugi where kodus=%d",data->kod_zap.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  if(data->metka_zap == 0)
    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),data->kod_zap.ravno());
  if(data->metka_zap == 1)
    sprintf(strsql,"%s %s !",gettext("Не найден код услуги"),data->kod_zap.ravno());
  iceb_menu_soob(strsql,data->window);
  data->naim.new_plus("");
  return(1);
 }
data->naim.new_plus(row[0]);
return(0);
}



