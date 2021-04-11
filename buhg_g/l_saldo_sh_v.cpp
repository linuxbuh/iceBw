/*$Id: l_saldo_sh_v.c,v 1.26 2014/08/31 06:19:20 sasa Exp $*/
/*11.07.2015	31.12.2003	Белых А.И.	l_saldo_sh_v.c
Ввод и корректировка сальдо по счетам
*/
#include "buhg_g.h"

enum
 {
  E_GOD,
  E_SHET,
  E_DEBET,
  E_KREDIT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK10,
  KOL_FK
 };

class saldo_shv_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  int god_k;
  class iceb_u_str shet_k;
  
  iceb_u_str god;
  iceb_u_str shet;
  iceb_u_str debet;
  iceb_u_str kredit;

  void clear()      
   {
    god.new_plus("");  
    shet.new_plus("");  
    debet.new_plus("");  
    kredit.new_plus("");  
   }

  saldo_shv_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();
   }

  void read_rek()
   {
    god.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GOD])));
    shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    debet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DEBET])));
    kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KREDIT])));
   }
 };



gboolean   saldo_sh_v_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_shv_data *data);
void  saldo_sh_v_knopka(GtkWidget *widget,class saldo_shv_data *data);
void    saldo_sh_v_vvod(GtkWidget *widget,class saldo_shv_data *data);
int saldo_sh_pk(const char *god,const char *shet,GtkWidget*);
int saldo_sh_zap(class saldo_shv_data *data);

void  l_saldo_sh_v_e_knopka(GtkWidget *widget,class saldo_shv_data *data);


int l_saldo_sh_v(class iceb_u_str *god_k,class iceb_u_str *shet_k,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str kikz;

class saldo_shv_data data;

data.god_k=god_k->ravno_atoi();
data.shet_k.new_plus(shet_k->ravno());
    

if(data.god_k != 0)
 {
  SQL_str row;

  SQLCURSOR cur;
  sprintf(strsql,"select * from Saldo where kkk=0 and gs=%d and ns='%s'",
  data.god_k,data.shet_k.ravno_filtr());
  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.god.new_plus(row[1]);
  data.shet.new_plus(row[2]);
  data.debet.new_plus(row[4]);
  data.kredit.new_plus(row[5]);
  kikz.plus(iceb_kikz(row[6],row[7],wpredok));
  
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


if(data.god_k == 0)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
else
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

GtkWidget *label=NULL;

if(data.god_k == 0)
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


g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(saldo_sh_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL,1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0 ; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0 ; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

label=gtk_label_new(gettext("Год"));
data.entry[E_GOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GOD]),5);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_GOD], "activate",G_CALLBACK(saldo_sh_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.god.ravno());
gtk_widget_set_name(data.entry[E_GOD],iceb_u_inttochar(E_GOD));


//label=gtk_label_new(gettext("Счёт"));
//gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), label, FALSE, FALSE,1);

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(l_saldo_sh_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(saldo_sh_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

label=gtk_label_new(gettext("Дебет"));
data.entry[E_DEBET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DEBET]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_DEBET]), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox[E_DEBET]), data.entry[E_DEBET], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DEBET], "activate",G_CALLBACK(saldo_sh_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEBET]),data.debet.ravno());
gtk_widget_set_name(data.entry[E_DEBET],iceb_u_inttochar(E_DEBET));


label=gtk_label_new(gettext("Кредит"));
data.entry[E_KREDIT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KREDIT]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KREDIT]), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox[E_KREDIT]), data.entry[E_KREDIT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KREDIT], "activate",G_CALLBACK(saldo_sh_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KREDIT]),data.kredit.ravno());
gtk_widget_set_name(data.entry[E_KREDIT],iceb_u_inttochar(E_KREDIT));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(saldo_sh_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(saldo_sh_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  god_k->new_plus(data.god.ravno());
  shet_k->new_plus(data.shet.ravno());
 }
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_saldo_sh_v_e_knopka(GtkWidget *widget,class saldo_shv_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_SHET:

    iceb_vibrek(1,"Plansh",&data->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno());

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   saldo_sh_v_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_shv_data *data)
{
//char  bros[512];

//printf("saldo_sh_v_key_press\n");
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
    data->kl_shift=1;
    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldo_sh_v_knopka(GtkWidget *widget,class saldo_shv_data *data)
{
//char bros[512];

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(saldo_sh_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
       
     }
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

void    saldo_sh_v_vvod(GtkWidget *widget,class saldo_shv_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("saldo_sh_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_GOD:
    data->god.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_SHET:
    data->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DEBET:
    data->debet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KREDIT:
    data->kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int saldo_sh_zap(class saldo_shv_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);


if(data->god.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введён год !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

   
if(data->shet.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введён счёт !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

if(iceb_pbpds(1,data->god.ravno_atoi(),data->window) != 0)
 return(1);

if(iceb_pbsh(1,data->god.ravno_atoi(),data->shet.ravno(),data->shet_k.ravno(),"",data->window) != 0)
 return(1);

if(data->god_k != 0)
  if(data->god_k != data->god.ravno_atoi())
   if(iceb_pbpds(1,data->god_k,data->window) != 0)
    return(1);

OPSHET rekshet;

if(iceb_prsh1(data->shet.ravno(),&rekshet,data->window) != 0)
 return(1);


if(iceb_u_atof(data->debet.ravno()) == 0. && iceb_u_atof(data->kredit.ravno()) == 0.)
 {
  iceb_u_str sp;
  sp.plus(gettext("Дебет и кредит равны нолю !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }


if(data->god_k == 0)
 {

  if(saldo_sh_pk(data->god.ravno(),data->shet.ravno(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Saldo \
values (%d,%d,'%s','%s',%.2f,%.2f,%d,%ld)",
   0,
   atoi(data->god.ravno_filtr()),
   data->shet.ravno_filtr(),
   "",
   iceb_u_atof(data->debet.ravno()),
   iceb_u_atof(data->kredit.ravno()),
   iceb_getuid(data->window),vrem);
 }
else
 {

  if(data->god_k != data->god.ravno_atoi() ||
  iceb_u_SRAV(data->shet.ravno(),data->shet_k.ravno(),0) != 0 )
   {
    if(saldo_sh_pk(data->god.ravno(),data->shet.ravno(),data->window) != 0)
     return(1);
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Saldo \
set \
gs=%d,\
ns='%s',\
deb=%.2f,\
kre=%.2f,\
ktoi=%d,\
vrem=%ld \
where kkk=%d and gs=%d and ns='%s'",
   atoi(data->god.ravno()),
   data->shet.ravno_filtr(),
   iceb_u_atof(data->debet.ravno()),
   iceb_u_atof(data->kredit.ravno()),
   iceb_getuid(data->window),vrem,
   0,
   data->god_k,
   data->shet_k.ravno_filtr());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);



return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int saldo_sh_pk(const char *god,const char *shet,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kkk from Saldo where kkk=0 and gs=%d and ns='%s'",
atoi(god),shet);
//printf("saldo_sh_pk-%s\n",strsql);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
