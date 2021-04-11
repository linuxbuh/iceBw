/*$Id: iceb_l_gkontr_v.c,v 1.25 2013/08/25 08:27:05 sasa Exp $*/
/*28.02.2016	31.12.2003	Белых А.И.	iceb_l_gkontr_v.c
Ввод и корректировка групп контрагентов
*/
#include "iceb_libbuh.h"
#include "iceb_l_gkontr.h"
enum
 {
  E_KOD,
  E_NAIM,
  KOLENTER  
 };
enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class iceb_l_gkontr_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class gkontr_rek rk;
  class iceb_u_str gr_kontr;  
  
  iceb_l_gkontr_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      g_signal_emit_by_name(entry[i],"activate");
     }
   }
 };


void    gkontr_v_rk(GtkWidget *widget,class iceb_l_gkontr_v_data *data);
gboolean   gkontr_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_gkontr_v_data *data);
void  gkontr_v_knopka(GtkWidget *widget,class iceb_l_gkontr_v_data *data);
void    gkontr_v_vvod(GtkWidget *widget,class iceb_l_gkontr_v_data *data);
int gkontr_pk(const char *god,GtkWidget*);
int gkontr_zap(class iceb_l_gkontr_v_data *data);
void gkontr_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);


int iceb_l_gkontr_v(class iceb_u_str *gr_kontr,GtkWidget *wpredok)
{
class iceb_l_gkontr_v_data data;
class iceb_u_str kikz;
int gor=0,ver=0;
char strsql[512];

data.gr_kontr.new_plus(gr_kontr->ravno());
data.rk.clear_zero();
data.name_window.plus(__FUNCTION__);

if(data.gr_kontr.getdlinna() > 1)
 {
  SQL_str row;
  SQLCURSOR cur;
  
  data.rk.kod.new_plus(data.gr_kontr.ravno());

  sprintf(strsql,"select * from Gkont where kod=%d",data.gr_kontr.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.naim.new_plus(row[1]);
  kikz.plus(iceb_kikz(row[2],row[3],wpredok));
  
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Gkont",0,wpredok));

  data.rk.kod.new_plus(strsql);

 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);


if(data.gr_kontr.getdlinna() <= 1)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
else
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));


gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(gkontr_v_key_press),&data);


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
GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Код группы"));
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),4);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(gkontr_v_rk),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Наименование группы"));
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),80);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(gkontr_v_rk),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[PFK2],"clicked",G_CALLBACK(gkontr_v_knopka),&data);
gtk_widget_set_name(data.knopka[PFK2],iceb_u_inttochar(PFK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[PFK10],"clicked",G_CALLBACK(gkontr_v_knopka),&data);
gtk_widget_set_name(data.knopka[PFK10],iceb_u_inttochar(PFK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 gr_kontr->new_plus(data.rk.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   gkontr_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_gkontr_v_data *data)
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
void  gkontr_v_knopka(GtkWidget *widget,class iceb_l_gkontr_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case PFK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    if(gkontr_zap(data) == 0)
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

void    gkontr_v_rk(GtkWidget *widget,class iceb_l_gkontr_v_data *data)
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

enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int gkontr_zap(class iceb_l_gkontr_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);


for(int i=0; i < KOLENTER; i++)
  g_signal_emit_by_name(data->entry[i],"activate");

if(data->rk.kod.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }
if(data->rk.kod.ravno_atoi() == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым и не равным нолю!"),data->window);
  return(1);
 }

if(data->rk.naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }

short metkakor=0;

if(data->gr_kontr.getdlinna() <= 1)
 {

  if(gkontr_pk(data->rk.kod.ravno(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Gkont \
values (%d,'%s',%d,%ld)",
   atoi(data->rk.kod.ravno()),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->gr_kontr.ravno(),data->rk.kod.ravno(),0) != 0)
   {
    if(gkontr_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Gkont \
set \
kod=%d,\
naik='%s',\
ktoz=%d,\
vrem=%ld \
where kod=%d",
   atoi(data->rk.kod.ravno()),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   atoi(data->gr_kontr.ravno()));
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(metkakor == 1)
 gkontr_kkvt(data->gr_kontr.ravno(),data->rk.kod.ravno(),data->window);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int gkontr_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Gkont where kod=%d",atoi(kod));

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void gkontr_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Kontragent set grup='%s' where grup='%s'",kodn,kods);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
   return;
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
 }

}
