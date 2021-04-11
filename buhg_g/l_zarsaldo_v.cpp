/*$Id: l_zarsaldo_v.c,v 1.16 2013/08/25 08:26:46 sasa Exp $*/
/*03.03.2016	26.06.2006	Белых А.И.	l_zarsaldo_v.c
Ввод и корректировка категорий работников
*/
#include "buhg_g.h"
enum
 {
  E_GOD,
  E_TABN,
  E_SALDO,
  E_SALDO_B,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class l_zarsaldo_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  
  //Уникальные реквизиты корректируемой записи
  short godk; 
  int tabnk;
  
  //Реквизиты введённые в меню
  class iceb_u_str tabn;
  class iceb_u_str god;
  class iceb_u_str saldo;
  class iceb_u_str saldo_b;
  
    
  l_zarsaldo_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      g_signal_emit_by_name(entry[i],"activate");
     }
   }
  void clear_rek()
   {
    tabn.new_plus("");
    god.new_plus("");
    saldo.new_plus("");
    saldo_b.new_plus("");
   }
 };


gboolean   l_zarsaldo_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsaldo_v_data *data);
void  l_zarsaldo_v_knopka(GtkWidget *widget,class l_zarsaldo_v_data *data);
void    l_zarsaldo_v_vvod(GtkWidget *widget,class l_zarsaldo_v_data *data);
int l_zarsaldo_pk(int god,int tabn,GtkWidget *wpredok);
int l_zarsaldo_zap(class l_zarsaldo_v_data *data);


extern SQL_baza  bd;

int l_zarsaldo_v(short *godk,int *tabnk,GtkWidget *wpredok)
{

class l_zarsaldo_v_data data;
char strsql[512];
class iceb_u_str kikz;
int gor=0,ver=0;
data.godk=*godk;
data.tabnk=*tabnk;
data.name_window.plus(__FUNCTION__);
static short god_zap;

if(*godk == 0)
 if(god_zap > 0)
  data.god.new_plus(god_zap);

if(*godk != 0)
 {
  data.god.new_plus(*godk);
  data.tabn.new_plus(*tabnk);
  
  sprintf(strsql,"select * from Zsal where god=%d and tabn=%d",*godk,*tabnk);
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.saldo.new_plus(row[2]);
  data.saldo_b.new_plus(row[5]);
  kikz.plus(iceb_kikz(row[3],row[4],wpredok));
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(*godk == 0)
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zarsaldo_v_key_press),&data);

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


label=gtk_label_new(gettext("Год"));
data.entry[E_GOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GOD]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GOD], "activate",G_CALLBACK(l_zarsaldo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.god.ravno());
gtk_widget_set_name(data.entry[E_GOD],iceb_u_inttochar(E_GOD));

label=gtk_label_new(gettext("Табельный номер"));
data.entry[E_TABN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_TABN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_TABN]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_TABN]), data.entry[E_TABN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABN], "activate",G_CALLBACK(l_zarsaldo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABN]),data.tabn.ravno());
gtk_widget_set_name(data.entry[E_TABN],iceb_u_inttochar(E_TABN));

label=gtk_label_new(gettext("Общее сальдо"));
data.entry[E_SALDO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SALDO]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_SALDO]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SALDO]), data.entry[E_SALDO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SALDO], "activate",G_CALLBACK(l_zarsaldo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SALDO]),data.saldo.ravno());
gtk_widget_set_name(data.entry[E_SALDO],iceb_u_inttochar(E_SALDO));

label=gtk_label_new(gettext("Сальдо бюджетное"));
data.entry[E_SALDO_B] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SALDO_B]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_SALDO_B]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SALDO_B]), data.entry[E_SALDO_B], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SALDO_B], "activate",G_CALLBACK(l_zarsaldo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SALDO_B]),data.saldo_b.ravno());
gtk_widget_set_name(data.entry[E_SALDO_B],iceb_u_inttochar(E_SALDO_B));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[PFK2],"clicked",G_CALLBACK(l_zarsaldo_v_knopka),&data);
gtk_widget_set_name(data.knopka[PFK2],iceb_u_inttochar(PFK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[PFK10],"clicked",G_CALLBACK(l_zarsaldo_v_knopka),&data);
gtk_widget_set_name(data.knopka[PFK10],iceb_u_inttochar(PFK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);
if(data.god.ravno_atoi() != 0)
 gtk_widget_grab_focus(data.entry[1]);
else
 gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  god_zap=*godk=data.god.ravno_atoi();
  *tabnk=data.tabn.ravno_atoi();
 }
 
return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarsaldo_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsaldo_v_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[PFK2],"clicked");

    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[PFK10],"clicked");

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
void  l_zarsaldo_v_knopka(GtkWidget *widget,class l_zarsaldo_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case PFK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    if(l_zarsaldo_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  


  case PFK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_zarsaldo_v_vvod(GtkWidget *widget,class l_zarsaldo_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_GOD:
    data->god.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_TABN:
    data->tabn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_SALDO:
    data->saldo.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_SALDO_B:
    data->saldo_b.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int l_zarsaldo_zap(class l_zarsaldo_v_data *data)
{
char strsql[1024];


if(data->god.ravno_atoi() == 0)
 {
  iceb_menu_soob(gettext("Не введён год !"),data->window);
  return(1);
 }

if(data->tabn.ravno_atoi() == 0)
 {
  iceb_menu_soob(gettext("Не введён табельный номер !"),data->window);
  return(1);
 }
//Проверяем табельный номер
sprintf(strsql,"select tabn from Kartb where tabn=%d",data->tabn.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),data->tabn.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(iceb_pbpds(1,data->god.ravno_atoi(),data->window) != 0)
 return(1);

if(data->godk != 0)
 if(data->godk != data->god.ravno_atoi())
  if(iceb_pbpds(1,data->godk,data->window) != 0)
   return(1);
  

if(data->godk != data->god.ravno_atoi() || data->tabnk != data->tabn.ravno_atoi())
  if(l_zarsaldo_pk(data->god.ravno_atoi(),data->tabn.ravno_atoi(),data->window) != 0)
     return(1);


time_t   vrem;
time(&vrem);

if(data->godk == 0)
 {

  sprintf(strsql,"insert into Zsal \
values (%d,%d,%.2f,%d,%ld,%.2f)",
   data->god.ravno_atoi(),
   data->tabn.ravno_atoi(),
   data->saldo.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->saldo_b.ravno_atof());

 }
else
 {
  
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Zsal \
set \
god=%d,\
tabn=%d,\
saldo=%.2f,\
ktoz=%d,\
vrem=%ld,\
saldob=%.2f \
where god=%d and tabn=%d",
   data->god.ravno_atoi(),
   data->tabn.ravno_atoi(),
   data->saldo.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->saldo_b.ravno_atof(),
   data->godk,
   data->tabnk);
   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_zarsaldo_pk(int god,int tabn,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select god from Zsal where god=%d and tabn=%d",god,tabn);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}
