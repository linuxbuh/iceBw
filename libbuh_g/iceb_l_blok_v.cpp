/*$Id: iceb_l_blok_v.c,v 1.9 2013/08/25 08:27:05 sasa Exp $*/
/*27.02.2016	06.09.2010	Белых А.И.	iceb_l_blok_v.c
Ввод и корректировка блокировок
*/
#include "iceb_libbuh.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATA,
  E_LOGIN,
  E_SHET,
  KOLENTER  
 };

extern int iceb_kod_podsystem;

class iceb_l_blok_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str datablok;
  class iceb_u_str login;
  class iceb_u_str shet;

  short godk,mesk;
    
  iceb_l_blok_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    datablok.plus("");
    login.plus("");
    shet.plus("");  
   }

  void read_rek()
   {
    datablok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA])));
    login.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_LOGIN])));
    if(iceb_kod_podsystem == ICEB_PS_GK)
      shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
   }
 };


gboolean   iceb_l_blok_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_blok_v_data *data);
void  iceb_l_blok_v_knopka(GtkWidget *widget,class iceb_l_blok_v_data *data);
void    iceb_l_blok_v_vvod(GtkWidget *widget,class iceb_l_blok_v_data *data);
int iceb_l_blok_zap(class iceb_l_blok_v_data *data);
void  iceb_l_blok_v_e_knopka(GtkWidget *widget,class iceb_l_blok_v_data *data);


int iceb_l_blok_v(short *mesk,short *godk,GtkWidget *wpredok)
{

class iceb_l_blok_v_data data;
char strsql[512];
class iceb_u_str kikz;

data.mesk=*mesk;
data.godk=*godk;

if(data.godk != 0 )
 {
  sprintf(strsql,"select * from Blok where kod=%d and god=%d and mes=%d",
  iceb_kod_podsystem,data.godk,data.mesk);
//  printf("%s-%s\n",__FUNCTION__,strsql);
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  if(data.mesk == 0)
   data.datablok.new_plus("*");
  else
   data.datablok.new_plus(data.mesk);
  data.datablok.plus(".");
  data.datablok.plus(data.godk);
  data.login.new_plus(row[3]);
  data.shet.new_plus(row[4]);
  kikz.plus(iceb_kikz(row[5],row[6],wpredok));
  
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.godk == 0)
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_l_blok_v_key_press),&data);

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
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE, 0);

/********
sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("м.г"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), label, FALSE, FALSE, 0);
***********/
sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("м.г"));
data.knopka_enter[E_DATA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.knopka_enter[E_DATA], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATA],"clicked",G_CALLBACK(iceb_l_blok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA],iceb_u_inttochar(E_DATA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA],gettext("Выбор даты"));

data.entry[E_DATA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATA], "activate",G_CALLBACK(iceb_l_blok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.datablok.ravno());
gtk_widget_set_name(data.entry[E_DATA],iceb_u_inttochar(E_DATA));

sprintf(strsql,"%s (,,)",gettext("Логины операторов для котoрых разблокировано"));
label=gtk_label_new(strsql);
data.entry[E_LOGIN] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), data.entry[E_LOGIN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_LOGIN], "activate",G_CALLBACK(iceb_l_blok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_LOGIN]),data.login.ravno());
gtk_widget_set_name(data.entry[E_LOGIN],iceb_u_inttochar(E_LOGIN));

if(iceb_kod_podsystem == ICEB_PS_GK)
 {
  sprintf(strsql,"%s (,,)",gettext("Заблокированные счета"));
  label=gtk_label_new(strsql);
  data.entry[E_SHET] = gtk_entry_new();
  gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
  g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(iceb_l_blok_v_vvod),&data);
  gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno());
  gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));
 }
 

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(iceb_l_blok_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(iceb_l_blok_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  iceb_u_polen(data.datablok.ravno(),mesk,1,'.');
  iceb_u_polen(data.datablok.ravno(),godk,2,'.');
  
 }
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  iceb_l_blok_v_e_knopka(GtkWidget *widget,class iceb_l_blok_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATA:

    if(iceb_calendar1(&data->datablok,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA]),data->datablok.ravno());
      
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_l_blok_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_blok_v_data *data)
{
//char  bros[512];

//printf("ei_v_key_press\n");
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
void  iceb_l_blok_v_knopka(GtkWidget *widget,class iceb_l_blok_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("ei_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    if(iceb_l_blok_zap(data) == 0)
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

void    iceb_l_blok_v_vvod(GtkWidget *widget,class iceb_l_blok_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("ei_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATA:
    data->datablok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_LOGIN:
    data->login.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int iceb_l_blok_zap(class iceb_l_blok_v_data *data)
{
char strsql[1024];
short mesi=0,godi=0;

data->read_rek();

if(iceb_l_blok_prov(data->window) != 0)
  return(1);
if(data->datablok.ravno()[0] != '*')
 {
 
  if(iceb_u_rsdat1(&mesi,&godi,data->datablok.ravno()) != 0)
   {
    iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
    return(1);
   }     
 }
else  
  iceb_u_rsdat1(&mesi,&godi,data->datablok.ravno());
 
if(godi == 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
  return(1);
 }     


 /*проверяем может такая запись уже есть*/
if(data->godk == 0 || (mesi != data->mesk || godi != data->godk))
 {
  sprintf(strsql,"select kod from Blok where kod=%d and god=%d and mes=%d",
  iceb_kod_podsystem,mesi,godi);
  if(iceb_sql_readkey(strsql,data->window) == 1)
   {
    iceb_menu_soob(gettext("Такая запись уже есть!"),data->window);
    return(1);
   }
 }      

short metka_bg=0;
if(data->datablok.ravno()[0] == '*')
  {
  if(iceb_menu_danet(gettext("Заблокировать весь год? Вы уверены?"),2,data->window) == 2)
    return(1);
   metka_bg=1;
  }

 if(data->godk != 0) /*корректировка записи*/
  {
   

   sprintf(strsql,"update Blok set god=%d,mes=%d,log='%s',shet='%s',ktoz=%d,vrem=%ld\
  where kod=%d and god=%d and mes=%d",
   godi,mesi,data->login.ravno(),data->shet.ravno(),iceb_getuid(data->window),time(NULL),
   iceb_kod_podsystem,data->godk,data->mesk);
          
  
  }
 else  /*новая запись*/
  {
   sprintf(strsql,"insert into Blok values(%d,%d,%d,'%s','%s',%d,%ld)",
   iceb_kod_podsystem,godi,mesi,data->login.ravno(),data->shet.ravno(),iceb_getuid(data->window),time(NULL));
  }

 if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
  return(1);

 if(metka_bg == 1)
  {
   sprintf(strsql,"delete from Blok where kod=%d and god=%d and mes != 0",iceb_kod_podsystem,godi);
   iceb_sql_zapis(strsql,1,0,data->window);
  }


return(0);

}
