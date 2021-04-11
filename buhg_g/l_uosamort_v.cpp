/*$Id: l_uosamort_v.c,v 1.14 2013/08/25 08:26:40 sasa Exp $*/
/*01.03.2016	11.12.2007	Белых А.И.	l_uosamort_v.c
Ввод и коррертировка амортизационных отчислений для налогового учёта
*/
#include "buhg_g.h"

enum
 {
  E_DATA,
  E_SUMA,
  KOLENTER  
 };

enum
 {
  FK2,
  FK10,
  KOL_FK
 };

class l_uosamort_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  int innom;
  int podr;
  int kodmo;
  class iceb_u_str hzt;
  class iceb_u_str hna;
  short mes;
  short god;
    
  class iceb_u_str mesgod;
  class iceb_u_str sao; /*Cумма амортотчисления*/
  
  l_uosamort_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    sao.plus("");
    mesgod.plus("");
   }

  void read_rek()
   {
    mesgod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA])));
    sao.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA])));
   }
 };


gboolean   l_uosamort_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosamort_v_data *data);
void  l_uosamort_v_knopka(GtkWidget *widget,class l_uosamort_v_data *data);
void    l_uosamort_v_vvod(GtkWidget *widget,class l_uosamort_v_data *data);
int l_uosamort_pk(char *god,GtkWidget*);
int l_uosamort_zap(class l_uosamort_v_data *data);
void l_uosamort_kkvt(char *kods,char *kodn,GtkWidget *wpredok);

void  l_uosamort_v_e_knopka(GtkWidget *widget,class l_uosamort_v_data *data);

extern SQL_baza  bd;

int l_uosamort_v(int innom,int podr,int kodmo,const char *mes_god,const char *hzt,const char *hna,GtkWidget *wpredok)
{

class l_uosamort_v_data data;
char strsql[512];
class iceb_u_str kikz;

data.innom=innom;
short d=0;
iceb_u_rsdat(&d,&data.mes,&data.god,mes_god,1);
if(d != 0)
 {
  sprintf(strsql,"%02d.%04d",data.mes,data.god);
  data.mesgod.new_plus(strsql);
 }
data.hzt.new_plus(hzt);
data.hna.new_plus(hna);
data.podr=podr;
data.kodmo=kodmo;
if(data.mes != 0)
 {
//  sprintf(strsql,"select suma,ktoz,vrem from Uosamor where innom=%d and god=%d and mes=%d",innom,data.god,data.mes);
  sprintf(strsql,"select suma,ktoz,vrem from Uosamor where innom=%d and da='%04d-%02d-01'",innom,data.god,data.mes);
  printf("%s-%s\n",__FUNCTION__,strsql);
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  
  sprintf(strsql,"%.2f",atof(row[0]));
  data.sao.new_plus(strsql);
  kikz.plus(iceb_kikz(row[1],row[2],wpredok));
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.mes == 0)
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uosamort_v_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("м.г"));
data.knopka_enter[E_DATA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.knopka_enter[E_DATA], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATA],"clicked",G_CALLBACK(l_uosamort_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA],iceb_u_inttochar(E_DATA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA],gettext("Выбор даты"));

data.entry[E_DATA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATA], "activate",G_CALLBACK(l_uosamort_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.mesgod.ravno());
gtk_widget_set_name(data.entry[E_DATA],iceb_u_inttochar(E_DATA));

label=gtk_label_new(gettext("Сумма"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE,1);

data.entry[E_SUMA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA]),60);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SUMA], "activate",G_CALLBACK(l_uosamort_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.sao.ravno());
gtk_widget_set_name(data.entry[E_SUMA],iceb_u_inttochar(E_SUMA));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_uosamort_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_uosamort_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

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
void  l_uosamort_v_e_knopka(GtkWidget *widget,class l_uosamort_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATA:

    if(iceb_calendar1(&data->mesgod,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA]),data->mesgod.ravno());
      
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosamort_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosamort_v_data *data)
{

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
void  l_uosamort_v_knopka(GtkWidget *widget,class l_uosamort_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_uosamort_zap(data) == 0)
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

void    l_uosamort_v_vvod(GtkWidget *widget,class l_uosamort_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_DATA:
    data->mesgod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_SUMA:
    data->sao.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/*****************************/
/*Запись*/
/***************************/
int l_uosamort_zap(class l_uosamort_v_data *data)
{
char strsql[512];

if(data->mesgod.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена дата!"),data->window); 
  return(1);
 }

short m1,g1;

if(iceb_u_rsdat1(&m1,&g1,data->mesgod.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window); 
  return(1);
 }



if(iceb_pbpds(m1,g1,data->window) != 0)
  return(1);

if(data->mes != 0)
 {
  if(iceb_pbpds(data->mes,data->god,data->window) != 0)
    return(1);
 }

if(data->sao.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена сумма аморт-отчислений!"),data->window);
  return(1);
 }


if(data->mes == 0)
 sprintf(strsql,"insert into Uosamor \
values (%d,'%04d-%02d-01',%d,'%s','%s',%.2f,%d,%ld,%d)",
 data->innom,g1,m1,data->podr,data->hzt.ravno(),data->hna.ravno(),data->sao.ravno_atof(),iceb_getuid(data->window),time(NULL),data->kodmo);
else
 sprintf(strsql,"update Uosamor \
set \
da='%04d-%02d-01',\
podr=%d,\
kmo=%d,\
hzt='%s',\
hna='%s',\
suma=%.2f,\
ktoz=%d,\
vrem=%ld \
where innom=%d and da='%04d-%02d-01'",
  g1,m1,data->podr,data->kodmo,data->hzt.ravno(),data->hna.ravno(),data->sao.ravno_atof(),iceb_getuid(data->window),time(NULL),data->innom,data->god,data->mes);
/**************
 sprintf(strsql,"update Uosamor \
set \
god=%d,\
mes=%d,\
podr=%d,\
kmo=%d,\
hzt='%s',\
hna='%s',\
suma=%.2f,\
ktoz=%d,\
vrem=%ld \
where innom=%d and god=%d and mes=%d",
  g1,m1,data->podr,data->kodmo,data->hzt.ravno(),data->hna.ravno(),data->sao.ravno_atof(),iceb_getuid(data->window),time(NULL),data->innom,data->god,data->mes);
****************/

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 return(1);


return(0);
}
