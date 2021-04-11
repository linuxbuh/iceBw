/*$Id: l_vrint_v.c,v 1.16 2013/08/13 06:10:25 sasa Exp $*/
/*18.01.2007	01.03.2004	Белых А.И.	l_vrint_v.c
Ввод и коррктировка стоимости входа в подразделение
*/
#include <time.h>
#include <unistd.h>
#include "i_rest.h"

enum
{
 FK2,
// FK4,
 FK10,
 KOL_F_KL
};

enum
{
 E_DATA,
 E_VREM,
 E_CENA,
 E_CENA_FOR_WOMEN,
 E_GRUPA,
 E_KOMENT,
 KOLENTER
};

class l_vrint_rek
 {
  public:
   iceb_u_str data;
   iceb_u_str vrem;
   iceb_u_str cena;
   iceb_u_str cena_f_w;
   iceb_u_str grup;
   iceb_u_str koment;
  
   l_vrint_rek()
   {
    clear_data();
   }
  void clear_data()
   {
    data.new_plus("");
    vrem.new_plus("");
    cena.new_plus("");
    cena_f_w.new_plus("");
    grup.new_plus("");
    koment.new_plus("");
   }
 };

class l_vrint_v_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *entry[KOLENTER];
  GtkWidget *opt;
  short     kl_shift; //0-отжата 1-нажата  

  int       voz;
       
  class l_vrint_rek vvod;

  iceb_u_str data_vrem;
  int podr;
      
  l_vrint_v_data()
   {
    voz=0;
    kl_shift=0;
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


gboolean   l_vrint_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_vrint_v_data *data);
void  l_vrint_v_knopka(GtkWidget *widget,class l_vrint_v_data *data);
void    l_vrint_v_vvod(GtkWidget *widget,class l_vrint_v_data *data);
int l_vrint_zap(class l_vrint_v_data *data);
void l_vrint_kodgr(class l_vrint_v_data *data);
void  l_vrint_v_e_knopka(GtkWidget *widget,class l_vrint_v_data *data);

extern SQL_baza	bd;
extern char *name_system;


int l_vrint_v(const char *data_vrem,int podr,GtkWidget *wpredok)
{
char strsql[300];
class l_vrint_v_data data;
iceb_u_str kikz;

printf("l_vrint_v=%s\n",data_vrem);
    
data.kl_shift=0;
data.vvod.clear_data();
data.data_vrem.new_plus(data_vrem);
data.podr=podr;

if(data.data_vrem.getdlinna() > 1 )
 {
  SQL_str row;
  SQLCURSOR cur;
  char strsql[300];
  sprintf(strsql,"select * from Restvi where dv='%s' and kp=%d",data.data_vrem.ravno(),data.podr);
  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(strsql);
    repl.ps_plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  short d,m,g;
  iceb_u_polen(row[0],strsql,sizeof(strsql),1,' ');
  iceb_u_rsdat(&d,&m,&g,strsql,2);
  
  sprintf(strsql,"%d.%d.%d",d,m,g);
  data.vvod.data.new_plus(strsql);
  
  iceb_u_polen(row[0],strsql,sizeof(strsql),2,' ');
  data.vvod.vrem.new_plus(strsql);
  
  data.vvod.cena.new_plus(row[2]);
  data.vvod.grup.new_plus(row[3]);
  data.vvod.koment.new_plus(row[4]);

  kikz.plus(iceb_kikz(row[5],row[6],wpredok));   
  data.vvod.cena_f_w.new_plus(row[7]);
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
//g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_widget_destroyed),&data.window);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_vrint_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label;
if(data.data_vrem.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));

  repl.ps_plus(kikz.ravno());
  
  label=gtk_label_new(repl.ravno());
 }
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER+1];
for(int i=0 ; i < KOLENTER+1; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0 ; i < KOLENTER+1; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("д.м.г"));
//label=gtk_label_new(strsql);
//gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), label, FALSE, FALSE, 0);
data.knopka_enter[E_DATA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.knopka_enter[E_DATA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATA],"clicked",G_CALLBACK(l_vrint_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA],iceb_u_inttochar(E_DATA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA],gettext("Выбор даты"));

data.entry[E_DATA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA]),11);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATA], "activate",G_CALLBACK(l_vrint_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.vvod.data.ravno());
gtk_widget_set_name(data.entry[E_DATA],iceb_u_inttochar(E_DATA));


sprintf(strsql,"%s (%s)",gettext("Время"),gettext("ч.м.с"));

label=gtk_label_new(strsql);
data.entry[E_VREM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VREM]),9);
gtk_box_pack_start (GTK_BOX (hbox[E_VREM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_VREM]), data.entry[E_VREM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_VREM], "activate",G_CALLBACK(l_vrint_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREM]),data.vvod.vrem.ravno());
gtk_widget_set_name(data.entry[E_VREM],iceb_u_inttochar(E_VREM));

label=gtk_label_new(gettext("Стоимость входа"));
data.entry[E_CENA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENA]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_CENA], "activate",G_CALLBACK(l_vrint_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.vvod.cena.ravno());
gtk_widget_set_name(data.entry[E_CENA],iceb_u_inttochar(E_CENA));

label=gtk_label_new(gettext("Стоимость входа для женщин"));
data.entry[E_CENA_FOR_WOMEN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENA_FOR_WOMEN]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA_FOR_WOMEN]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA_FOR_WOMEN]), data.entry[E_CENA_FOR_WOMEN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_CENA_FOR_WOMEN], "activate",G_CALLBACK(l_vrint_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA_FOR_WOMEN]),data.vvod.cena_f_w.ravno());
gtk_widget_set_name(data.entry[E_CENA_FOR_WOMEN],iceb_u_inttochar(E_CENA_FOR_WOMEN));



sprintf(strsql,"%s (,,)",gettext("Группы клиентов с бесплатным входом"));
data.knopka_enter[E_GRUPA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.knopka_enter[E_GRUPA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_GRUPA],"clicked",G_CALLBACK(l_vrint_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_GRUPA],iceb_u_inttochar(E_GRUPA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_GRUPA],gettext("Выбор даты"));


data.entry[E_GRUPA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.entry[E_GRUPA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GRUPA], "activate",G_CALLBACK(l_vrint_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPA]),data.vvod.grup.ravno());
gtk_widget_set_name(data.entry[E_GRUPA],iceb_u_inttochar(E_GRUPA));

sprintf(strsql,"%s",gettext("Коментарий"));

label=gtk_label_new(strsql);
data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_vrint_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.vvod.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_vrint_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_vrint_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
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
void  l_vrint_v_e_knopka(GtkWidget *widget,class l_vrint_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");
int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case E_DATA:

    if(iceb_calendar(&data->vvod.data,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA]),data->vvod.data.ravno());

    return;  

  case E_GRUPA:

    if(l_grkl(1,&kod,&naim,data->window) == 0)
     { 
      data->vvod.grup.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),data->vvod.grup.ravno());
     }
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_vrint_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_vrint_v_data *data)
{

//printf("l_vrint_v_key_press\n");
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
void  l_vrint_v_knopka(GtkWidget *widget,class l_vrint_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek();
    if(l_vrint_zap(data) == 0)
     {
      gtk_widget_destroy(data->window);
      data->window=NULL;
     }
    return;  


  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_vrint_v_vvod(GtkWidget *widget,class l_vrint_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_vrint_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATA:
    data->vvod.data.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_VREM:
    data->vvod.vrem.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_CENA:
    data->vvod.cena.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_CENA_FOR_WOMEN:
    data->vvod.cena_f_w.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GRUPA:
    data->vvod.grup.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOMENT:
    data->vvod.koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int l_vrint_zap(class l_vrint_v_data *data)
{
char strsql[1000];
short d,m,g;
short has,min,sek;
printf("l_vrint_zap\n");

if(iceb_u_rsdat(&d,&m,&g,data->vvod.data.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
  return(1);
 }
if(iceb_u_rstime(&has,&min,&sek,data->vvod.vrem.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введено время !"),data->window);
  return(1);
 }

char data_vrem[50];
sprintf(data_vrem,"%d-%d-%d %d:%d:%d",g,m,d,has,min,sek);
printf("%s\n",data_vrem);

time_t vrem;
time(&vrem);
if(data->data_vrem.getdlinna() <= 1) //Ввод новой записи
 {
  sprintf(strsql,"replace into Restvi \
values ('%s',%d,%.2f,'%s','%s',%d,%ld,%.2f)",
  data_vrem,
  data->podr,
  data->vvod.cena.ravno_atof(),
  data->vvod.grup.ravno_filtr(),
  data->vvod.koment.ravno_filtr(),
  iceb_getuid(data->window),vrem,
  data->vvod.cena_f_w.ravno_atof());
 }
else
 {
  sprintf(strsql,"update Restvi set \
dv='%s',\
cena=%.2f,\
grup='%s',\
koment='%s',\
ktoi=%d,\
vrem=%ld,\
cdg=%.2f \
where dv='%s'",
  data_vrem,
  data->vvod.cena.ravno_atof(),
  data->vvod.grup.ravno_filtr(),
  data->vvod.koment.ravno_filtr(),
  iceb_getuid(data->window),vrem,
  data->vvod.cena_f_w.ravno_atof(),
  data->data_vrem.ravno());

 }

printf("l_vrint_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

return(0);

}
