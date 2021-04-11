/*$Id: l_uplavt1_v.c,v 1.3 2013/08/25 08:26:41 sasa Exp $*/
/*10.07.2015	20.04.2012	Белых А.И.	l_uplavt1_v.c
Ввод и корректировка норм списания топлива
*/
#include "buhg_g.h"
enum
 {
  E_KODNST,
  E_NST,
  E_EI,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uplavt1_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_naim_nst;
  GtkWidget *label_naim_ei;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str kod;
  class iceb_u_str nst;
  class iceb_u_str ei;
  
  class iceb_u_str kod_nst;  
  int kod_avt;  

  
  l_uplavt1_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    kod_avt=0;
    clear_data();
   }

  void clear_data()
   {
    kod.new_plus("");
    nst.new_plus("");
    ei.new_plus("");
   }

  void read_rek()
   {
    kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODNST])));
    nst.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NST])));
    ei.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_EI])));
   }

  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_label_set_text(GTK_LABEL(label_naim_nst),"");
    gtk_label_set_text(GTK_LABEL(label_naim_ei),"");
    
   }
 };


gboolean   l_uplavt1_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplavt1_v_data *data);
void  l_uplavt1_v_knopka(GtkWidget *widget,class l_uplavt1_v_data *data);
void    l_uplavt1_v_vvod(GtkWidget *widget,class l_uplavt1_v_data *data);
int l_uplavt1_pk(int kod_avt,const char *god,GtkWidget*);
int l_uplavt1_zap(class l_uplavt1_v_data *data);
void  l_uplavt1_v_e_knopka(GtkWidget *widget,class l_uplavt1_v_data *data);


extern SQL_baza  bd;

int l_uplavt1_v(int kod_avt,class iceb_u_str *kod_nst,GtkWidget *wpredok)
{
class iceb_u_str naim_ei;
class iceb_u_str naim_nst;

class l_uplavt1_v_data data;
char strsql[512];
iceb_u_str kikz;
data.kod_nst.new_plus(kod_nst->ravno());
data.kod_avt=kod_avt;
data.kod.new_plus(kod_nst->ravno());

if(data.kod_nst.getdlinna() >  1)
 {
  sprintf(strsql,"select * from Uplavt1 where kod=%d and knst='%s'",data.kod_avt,data.kod_nst.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }


  data.nst.new_plus(row[2]);
  data.ei.new_plus(row[3]);
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  

  sprintf(strsql,"select naik from Uplnst where kod='%s'",data.kod.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_nst.new_plus(row[0]);
  sprintf(strsql,"select naik from Edizmer where kod='%s'",data.ei.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_ei.new_plus(row[0]);
   

 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_nst.getdlinna() <= 1)
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uplavt1_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

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
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);



data.knopka_enter[E_KODNST]=gtk_button_new_with_label(gettext("Код нормы списания"));
gtk_box_pack_start (GTK_BOX (hbox[E_KODNST]), data.knopka_enter[E_KODNST], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KODNST],"clicked",G_CALLBACK(l_uplavt1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODNST],iceb_u_inttochar(E_KODNST));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODNST],gettext("Выбор нормы списания топлива"));

data.entry[E_KODNST] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODNST]),16);
gtk_box_pack_start (GTK_BOX (hbox[E_KODNST]), data.entry[E_KODNST], FALSE,FALSE, 0);
g_signal_connect(data.entry[E_KODNST], "activate",G_CALLBACK(l_uplavt1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODNST]),data.kod.ravno());
gtk_widget_set_name(data.entry[E_KODNST],iceb_u_inttochar(E_KODNST));


data.label_naim_nst=gtk_label_new(naim_nst.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KODNST]), data.label_naim_nst, TRUE, TRUE, 0);



label=gtk_label_new(gettext("Норма списания топлива"));
gtk_box_pack_start (GTK_BOX (hbox[E_NST]), label, FALSE, FALSE, 0);

data.entry[E_NST] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NST]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NST]), data.entry[E_NST], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NST], "activate",G_CALLBACK(l_uplavt1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NST]),data.nst.ravno());
gtk_widget_set_name(data.entry[E_NST],iceb_u_inttochar(E_NST));

//label=gtk_label_new(gettext("Единица измерения"));
//gtk_box_pack_start (GTK_BOX (hbox[E_EI]), label, FALSE, FALSE, 0);
data.knopka_enter[E_EI]=gtk_button_new_with_label(gettext("Единица измерения"));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_EI],"clicked",G_CALLBACK(l_uplavt1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_EI],iceb_u_inttochar(E_EI));
gtk_widget_set_tooltip_text(data.knopka_enter[E_EI],gettext("Выбор единицы измерения"));

data.entry[E_EI] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_EI]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], FALSE,FALSE, 0);
g_signal_connect(data.entry[E_EI], "activate",G_CALLBACK(l_uplavt1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei.ravno());
gtk_widget_set_name(data.entry[E_EI],iceb_u_inttochar(E_EI));

data.label_naim_ei=gtk_label_new(naim_ei.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.label_naim_ei, TRUE, TRUE, 0);


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_uplavt1_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_uplavt1_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_uplavt1_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_nst->new_plus(data.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uplavt1_v_e_knopka(GtkWidget *widget,class l_uplavt1_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_KODNST:

     if(l_uplnst(1,&kod,&naim,data->window) == 0)
      {
       data->kod.new_plus(kod.ravno());

       gtk_label_set_text(GTK_LABEL(data->label_naim_nst),naim.ravno(20));
       
      }
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODNST]),data->kod.ravno());
      
    return;  

  case E_EI:

     if(iceb_l_ei(1,&kod,&naim,data->window) == 0)
      {
       data->ei.new_plus(kod.ravno());

       gtk_label_set_text(GTK_LABEL(data->label_naim_ei),naim.ravno(20));
       
      }
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->ei.ravno());
      
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplavt1_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplavt1_v_data *data)
{
//char  bros[512];

//printf("l_uplavt1_v_key_press\n");
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
  //  printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_uplavt1_v_knopka(GtkWidget *widget,class l_uplavt1_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_uplavt1_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_uplavt1_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  

  case FK4:
    data->clear_rek();
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

void    l_uplavt1_v_vvod(GtkWidget *widget,class l_uplavt1_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_uplavt1_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KODNST:
    data->kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NST:
    data->nst.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int l_uplavt1_zap(class l_uplavt1_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);


if(data->kod.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }


if(data->nst.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена норма списания топлива !"),data->window);
  return(1);
 }

if(data->ei.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена единица измерения!"),data->window);
  return(1);
 }
/*проверяем единицу измерения*/
sprintf(strsql,"select kod from Edizmer where kod='%s'",data->ei.ravno_filtr());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена единица измерения"),data->ei.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(iceb_u_SRAV(data->kod_nst.ravno(),data->kod.ravno(),0) != 0)
  if(l_uplavt1_pk(data->kod_avt,data->kod.ravno(),data->window) != 0)
     return(1);


if(data->kod_nst.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Uplavt1 \
values (%d,'%s',%.3f,'%s',%d,%ld)",
   data->kod_avt,
   data->kod.ravno_filtr(),
   data->nst.ravno_atof(),
   data->ei.ravno_filtr(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_nst.ravno(),data->kod.ravno(),0) != 0)
   {
/******
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
***************/
    iceb_menu_soob(gettext("Корректировка кода невозможна!"),data->window);
    return(1);
      
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Uplavt1 \
set \
nst=%.3f,\
ei='%s',\
ktoz=%d,\
vrem=%ld \
where kod=%d and knst='%s'",
   data->nst.ravno_atof(),
   data->ei.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->kod_avt,
   data->kod_nst.ravno());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_uplavt1_pk(int kod_avt,const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Uplavt1 where kod=%d and knst='%s'",kod_avt,kod);
//printf("l_uplavt1_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}
