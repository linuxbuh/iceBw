/*$Id: l_dover_v.c,v 1.11 2013/08/25 08:26:34 sasa Exp $*/
/*28.02.2016	22.04.2009	Белых А.И.	l_dover_v.c
Ввод и корректировка записи в доверенность
*/
#include "buhg_g.h"

enum
 {
  E_NAIM,
  E_EI,
  E_KOLIH,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_dover_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str naim;
  class iceb_u_str ei;
  class iceb_u_str kolih;
  
  class iceb_u_str datdov;
  class iceb_u_str nomdov;
  int nomerz;    

  l_dover_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();    
   }

  void read_rek()
   {
    naim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM])));
    ei.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_EI])));
    kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIH])));
   }
  void clear()
   {
    naim.new_plus("");
    ei.new_plus("");
    kolih.new_plus("");
   }
 };


gboolean   l_dover_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_dover_v_data *data);
void  l_dover_v_knopka(GtkWidget *widget,class l_dover_v_data *data);
void    l_dover_v_vvod(GtkWidget *widget,class l_dover_v_data *data);
int l_dover_zap(class l_dover_v_data *data);

void  l_dover_e_knopka(GtkWidget *widget,class l_dover_v_data *data);

extern SQL_baza  bd;

int l_dover_v(const char *datadov,const char *nomdov,int nomerz,GtkWidget *wpredok)
{
class l_dover_v_data data;
char strsql[512];
class iceb_u_str kikz;
int gor=0,ver=0;

data.datdov.new_plus(datadov);
data.nomdov.new_plus(nomdov);
data.nomerz=nomerz;
data.name_window.plus(__FUNCTION__);

if(data.nomerz != 0)
 {
  sprintf(strsql,"select zapis,ei,kol,ktoz,vrem from Uddok1 where datd='%s' and nomd='%s' and nz=%d",
  data.datdov.ravno_sqldata(),data.nomdov.ravno(),data.nomerz);
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.naim.new_plus(row[0]);
  data.ei.new_plus(row[1]);
  data.kolih.new_plus(row[2]);
  kikz.plus(iceb_kikz(row[3],row[4],wpredok));
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(data.nomerz == 0)
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_dover_v_key_press),&data);

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



label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),199);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_dover_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));
/*******
label=gtk_label_new(gettext("Единица измерения"));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), label, FALSE, FALSE, 0);
***********/

sprintf(strsql,"%s",gettext("Единица измерения"));
data.knopka_enter[E_EI]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_EI],"clicked",G_CALLBACK(l_dover_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_EI],iceb_u_inttochar(E_EI));
gtk_widget_set_tooltip_text(data.knopka_enter[E_EI],gettext("Выбор единицы измерения"));

data.entry[E_EI] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_EI]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_EI], "activate",G_CALLBACK(l_dover_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei.ravno());
gtk_widget_set_name(data.entry[E_EI],iceb_u_inttochar(E_EI));

label=gtk_label_new(gettext("Количество"));
data.entry[E_KOLIH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLIH]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLIH], "activate",G_CALLBACK(l_dover_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_widget_set_name(data.entry[E_KOLIH],iceb_u_inttochar(E_KOLIH));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_dover_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_dover_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_dover_e_knopka(GtkWidget *widget,class l_dover_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");
int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_zarnah_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_EI:
    if(iceb_l_ei(1,&kod,&naim,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),kod.ravno());
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_dover_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_dover_v_data *data)
{
//char  bros[512];

//printf("l_dover_v_key_press\n");
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
void  l_dover_v_knopka(GtkWidget *widget,class l_dover_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    if(l_dover_zap(data) == 0)
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

void    l_dover_v_vvod(GtkWidget *widget,class l_dover_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_EI:
    data->ei.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOLIH:
    data->kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NAIM:
    data->naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int l_dover_zap(class l_dover_v_data *data)
{
char strsql[1024];



if(data->naim.getdlinna() <= 1 || \
data->ei.getdlinna() <= 1 || \
data->kolih.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введены все обязательные реквизиты!"),data->window);
  return(1);
 } 

class iceb_lock_tables KKK("LOCK TABLES Uddok1 WRITE,icebuser READ");


if(data->nomerz == 0) /*Ввод новой записи*/
 {
  int nomzap=0;
  /*Узнаём свободный номер*/
  for(nomzap=1; nomzap < 10000; nomzap++)
   {
    sprintf(strsql,"select nz from Uddok1 where datd='%s' and nomd='%s' and nz=%d",
    data->datdov.ravno_sqldata(),data->nomdov.ravno(),nomzap);
    if(iceb_sql_readkey(strsql,data->window) == 0)
     break;
   }


  sprintf(strsql,"insert into Uddok1 values('%s','%s',%d,'%s','%s',%10.10g,%d,%ld)",
  data->datdov.ravno_sqldata(),data->nomdov.ravno(),nomzap,
  data->naim.ravno_filtr(),  
  data->ei.ravno_filtr(),  
  data->kolih.ravno_atof(),  
  iceb_getuid(data->window),
  time(NULL));
 }
else /*Корректировка существующей*/
 {
  sprintf(strsql,"update Uddok1 set \
zapis='%s',\
ei='%s',\
kol=%10.10g,\
ktoz=%d,\
vrem=%ld \
where datd='%s' and nomd='%s' and nz=%d",
  data->naim.ravno_filtr(),  
  data->ei.ravno_filtr(),  
  data->kolih.ravno_atof(),  
  iceb_getuid(data->window),
  time(NULL),
  data->datdov.ravno_sqldata(),data->nomdov.ravno(),data->nomerz);

 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
