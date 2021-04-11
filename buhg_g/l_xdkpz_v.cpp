/*$Id: l_xdkpz_v.c,v 1.16 2013/08/25 08:26:44 sasa Exp $*/
/*10.07.2015	06.05.2006	Белых А.И.	xdkpz_v.c
Ввод и корректировка подтверждающих записей для платёжных документов
*/
#include "buhg_g.h"
#include "dok4w.h"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAP,
  E_SUMAP,
  KOLENTER  
 };

class l_xdkpz_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

    
  iceb_u_str data_pod; //Дата подтверждения корректируемой записи
  iceb_u_str suma_pod;   //Сумма в корректируемой записи

  iceb_u_str data_pod_n; //Дата подтверждения корректируемой записи начальная
  iceb_u_str suma_pod_n;   //Сумма в корректируемой записи          начальная
    
  class iceb_u_str tablica;
  l_xdkpz_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
 };

gboolean   l_xdkpz_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_xdkpz_v_data *data);
void    l_xdkpz_v_v_vvod(GtkWidget *widget,class l_xdkpz_v_data *data);
void  l_xdkpz_v_v_knopka(GtkWidget *widget,class l_xdkpz_v_data *data);
void xdkpz_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int xdkpz_zap(class l_xdkpz_v_data *data);
int xdkpz_pk(char *kod,GtkWidget *wpredok);

void  xdkpz_v_e_knopka(GtkWidget *widget,class l_xdkpz_v_data *data);

extern SQL_baza bd;
extern class REC rec;

int l_xdkpz_v(const char *tablica,const char *data_pod,double suma_pod,
GtkWidget *wpredok)
{
class l_xdkpz_v_data data;
char strsql[512];
iceb_u_str kikz;

data.data_pod.new_plus(data_pod);
data.suma_pod.new_plus(suma_pod);
data.data_pod_n.new_plus(data_pod);
data.suma_pod_n.new_plus(suma_pod);
data.tablica.plus(tablica);



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.data_pod.getdlinna() > 1)
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
else
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_xdkpz_v_v_key_press),&data);

iceb_u_str zagolov;
if(data.data_pod.getdlinna() <= 1)
 zagolov.new_plus(gettext("Ввод новой записи"));
else
 {
  zagolov.new_plus(gettext("Корректировка записи"));
  zagolov.ps_plus(kikz.ravno());
 }

GtkWidget *label=gtk_label_new(zagolov.ravno());

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
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);



sprintf(strsql,"%s",gettext("Сумма"));
label=gtk_label_new(strsql);
data.entry[E_SUMAP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMAP]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMAP]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMAP]), data.entry[E_SUMAP],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMAP], "activate",G_CALLBACK(l_xdkpz_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMAP]),data.suma_pod.ravno());
gtk_widget_set_name(data.entry[E_SUMAP],iceb_u_inttochar(E_SUMAP));

sprintf(strsql,"%s",gettext("Дата подтверждения"));
data.knopka_enter[E_DATAP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), data.knopka_enter[E_DATAP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAP],"clicked",G_CALLBACK(xdkpz_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAP],iceb_u_inttochar(E_DATAP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAP],gettext("Выбор единицы измерения"));

data.entry[E_DATAP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAP]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), data.entry[E_DATAP],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAP], "activate",G_CALLBACK(l_xdkpz_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAP]),data.data_pod.ravno());
gtk_widget_set_name(data.entry[E_DATAP],iceb_u_inttochar(E_DATAP));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_xdkpz_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_xdkpz_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  xdkpz_v_e_knopka(GtkWidget *widget,class l_xdkpz_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATAP:
    if(iceb_calendar(&data->data_pod,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAP]),data->data_pod.ravno());
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_xdkpz_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_xdkpz_v_data *data)
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
void  l_xdkpz_v_v_knopka(GtkWidget *widget,class l_xdkpz_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(xdkpz_zap(data) != 0)
      return;

    gtk_widget_destroy(data->window);

    data->voz=0;
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_xdkpz_v_v_vvod(GtkWidget *widget,class l_xdkpz_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_DATAP:
    data->data_pod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SUMAP:
    data->suma_pod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int xdkpz_zap(class l_xdkpz_v_data *data)
{
short dv,mv,gv;

//printf("xdkpz_v_zap vido=%d prov=%d\n",data->rk.vido,data->rk.prov);
if(data->suma_pod.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Не введена сума !"),data->window);
  return(1);
 }
if(iceb_u_rsdat(&dv,&mv,&gv,data->data_pod.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
  return(1);
 }

if(data->suma_pod.ravno_atof() > rec.sumd)
 {
  iceb_menu_soob(gettext("Не правильно введена сумма !"),data->window);
  return(1);
 }


time_t   vrem;
time(&vrem);

int i=0;
if(iceb_u_SRAV(data->tablica.ravno(),"Pltp",0) == 0)
 i=0;  
if(iceb_u_SRAV(data->tablica.ravno(),"Pltt",0) == 0)
 i=1;  

char strsql[512];

if(data->data_pod_n.getdlinna() <= 1)
 sprintf(strsql,"insert into Pzpd \
values (%d,'%04d-%02d-%02d','%04d-%02d-%02d','%s',%.2f,%d,%ld)",
   i,rec.gd,rec.md,rec.dd,gv,mv,dv,rec.nomdk.ravno(),data->suma_pod.ravno_atof(),iceb_getuid(data->window),vrem);   
else
 sprintf(strsql,"update Pzpd \
set \
datp='%04d-%02d-%02d',\
suma=%.2f,\
ktoz=%d,\
vrem=%ld \
where tp=%d and datd='%d-%02d-%02d' and datp='%s' and \
nomd='%s' and suma=%.2f",gv,mv,dv,data->suma_pod.ravno_atof(),iceb_getuid(data->window),vrem,
  i,rec.gd,rec.md,rec.dd,data->data_pod_n.ravno_sqldata(),rec.nomdk.ravno(),data->suma_pod_n.ravno_atof());

iceb_sql_zapis(strsql,1,0,data->window);

return(0);

}
