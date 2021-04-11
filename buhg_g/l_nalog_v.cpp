/*$Id: l_nalog_v.c,v 1.9 2013/08/25 08:26:37 sasa Exp $*/
/*11.07.2015	14.10.2010	Белых А.И.	l_nalog_v.c
Ввод и корректировка налога
*/
#include "buhg_g.h"
enum
 {
  E_DNDN,
  E_PROC,
  E_KOMENT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_nalog_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str dndn_z; /*Дата начала действия налога*/
  int nomnal;
  class iceb_u_str dndn;
  class iceb_u_str proc;
  class iceb_u_str koment;

    
  l_nalog_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_data();    
   }

  void read_rek()
   {
    dndn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DNDN])));
    proc.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PROC])));
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    clear_data();    
   }
  void clear_data()
   {
    dndn.new_plus("");
    proc.new_plus("");
    koment.new_plus("");
   }
 };


gboolean   l_nalog_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_nalog_v_data *data);
void  l_nalog_v_knopka(GtkWidget *widget,class l_nalog_v_data *data);
void    l_nalog_v_vvod(GtkWidget *widget,class l_nalog_v_data *data);
int l_nalog_zap(class l_nalog_v_data *data);


extern SQL_baza  bd;

int l_nalog_v(int nomnal,const char *dndn,GtkWidget *wpredok)
{

class l_nalog_v_data data;
char strsql[512];
class iceb_u_str kikz;
data.nomnal=nomnal;
data.dndn_z.new_plus(dndn);
//printf("%s-nomnal=%d dndn=%s\n",__FUNCTION__,nomnal,dndn);
if(data.dndn_z.getdlinna()  >  1)
 {
  sprintf(strsql,"select * from Nalog where vn=%d and dndn='%s'",data.nomnal,data.dndn_z.ravno_sqldata());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.dndn.new_plus(dndn);
  data.proc.new_plus(row[2]);
  data.koment.new_plus(row[3]);
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.dndn_z.getdlinna() <= 1)
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_nalog_v_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Дата"));
gtk_box_pack_start (GTK_BOX (hbox[E_DNDN]), label, FALSE, FALSE, 0);

data.entry[E_DNDN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DNDN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DNDN]), data.entry[E_DNDN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DNDN], "activate",G_CALLBACK(l_nalog_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DNDN]),data.dndn.ravno());
gtk_widget_set_name(data.entry[E_DNDN],iceb_u_inttochar(E_DNDN));

label=gtk_label_new(gettext("Процент"));
gtk_box_pack_start (GTK_BOX (hbox[E_PROC]), label, FALSE, FALSE, 0);

data.entry[E_PROC] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PROC]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC]), data.entry[E_PROC], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PROC], "activate",G_CALLBACK(l_nalog_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROC]),data.proc.ravno());
gtk_widget_set_name(data.entry[E_PROC],iceb_u_inttochar(E_PROC));

label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),254);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_nalog_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_nalog_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_nalog_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_nalog_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_nalog_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_nalog_v_data *data)
{
//char  bros[512];

//printf("l_nalog_v_key_press\n");
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
void  l_nalog_v_knopka(GtkWidget *widget,class l_nalog_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_nalog_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_nalog_zap(data) == 0)
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

void    l_nalog_v_vvod(GtkWidget *widget,class l_nalog_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_nalog_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DNDN:
    data->dndn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_PROC:
    data->proc.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int l_nalog_zap(class l_nalog_v_data *data)
{
char strsql[1024];
if(iceb_u_sravmydat(data->dndn_z.ravno(),data->dndn.ravno()) != 0)
 {
  /*Проверяем может уже есть такая запись*/
  sprintf(strsql,"select vn from Nalog where vn=%d and dndn='%s'",data->nomnal,data->dndn.ravno_sqldata());

  if(iceb_sql_readkey(strsql,data->window) == 1)
   {
    iceb_menu_soob(gettext("Такая запись уже есть!"),data->window);
    return(1);
   }
 }

if(data->dndn_z.getdlinna() <= 1) /*новая запись*/
 {
  sprintf(strsql,"insert into Nalog values (%d,'%s',%.2f,'%s',%d,%ld)",
  data->nomnal,data->dndn.ravno_sqldata(),data->proc.ravno_atof(),data->koment.ravno_filtr(),iceb_getuid(data->window),time(NULL));
 }
else /*Корректировать запись*/
 {
  sprintf(strsql,"update Nalog set \
dndn='%s',\
pr=%.2f,\
kom='%s' \
where vn=%d and dndn='%s'",
  data->dndn.ravno_sqldata(),
  data->proc.ravno_atof(),
  data->koment.ravno_filtr(),
  data->nomnal,
  data->dndn_z.ravno_sqldata()); 
 }

//printf("%s-%s\n",__FUNCTION__,strsql);

iceb_sql_zapis(strsql,1,0,data->window);

return(0);

}
