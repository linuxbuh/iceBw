/*$Id: l_tippl_v.c,v 1.19 2013/10/04 07:27:16 sasa Exp $*/
/*01.03.2015	12.05.2006	Белых А.И.	l_tipplp_v.c
Ввод и корректировка записей кодов операций прихода для "Учёта кассовых ордеров"
*/
#include "buhg_g.h"
#include "l_tippl.h"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOD,
  E_NAIM,
  KOLENTER  
 };

class l_tipplp_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton1[2];
  GtkWidget *radiobutton2[2];
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class tippl_rek rk;
  
  iceb_u_str kodk; //Код записи которую корректируют
  char tablica[64];
     
  l_tipplp_v_data() //Конструктор
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

gboolean   l_tipplp_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_tipplp_v_data *data);
void    l_tipplp_v_v_vvod(GtkWidget *widget,class l_tipplp_v_data *data);
void  l_tipplp_v_v_knopka(GtkWidget *widget,class l_tipplp_v_data *data);
void tipplp_kkvt(const char *skod,const char *nkod,const char*,GtkWidget *wpredok);
int tipplp_zap(class l_tipplp_v_data *data);
int tipplp_pk(const char *kod,const char*,GtkWidget *wpredok);

void  tipplp_v_e_knopka(GtkWidget *widget,class l_tipplp_v_data *data);

extern SQL_baza bd;
extern double	okrcn;  /*Округление цены*/

int l_tippl_v(char *tablica,iceb_u_str *kodzap, //Код записи для корректировки ; возвращается код только что введённой записи
GtkWidget *wpredok)
{
class l_tipplp_v_data data;
int gor=0,ver=0;
data.kodk.new_plus(kodzap->ravno());
data.name_window.plus(__FUNCTION__);
strcpy(data.tablica,tablica);

iceb_u_str naim_shet;
iceb_u_str naim_shetk;
char strsql[512];
iceb_u_str kikz;

if(kodzap->getdlinna() > 1)
 {
  //читаем корректируемую запись
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from %s where kod='%s'",data.tablica,kodzap->ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.rk.kod.new_plus(row[0]);
  data.rk.naim.new_plus(row[1]);
  kikz.plus(iceb_kikz(row[2],row[3],wpredok));

 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

if(kodzap->getdlinna() > 1)
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_tipplp_v_v_key_press),&data);

iceb_u_str zagolov;
if(kodzap->getdlinna() <= 1)
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


sprintf(strsql,"%s",gettext("Код"));
label=gtk_label_new(strsql);
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_tipplp_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

sprintf(strsql,"%s",gettext("Наименование"));
label=gtk_label_new(strsql);
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_tipplp_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_tipplp_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_tipplp_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 kodzap->new_plus(data.rk.kod.ravno());
return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  tipplp_v_e_knopka(GtkWidget *widget,class l_tipplp_v_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_tipplp_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_tipplp_v_data *data)
{

//printf("l_tipplp_v_v_key_press\n");
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
void  l_tipplp_v_v_knopka(GtkWidget *widget,class l_tipplp_v_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek(); //Читаем реквизиты меню
    if(tipplp_zap(data) != 0)
      return;

    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_tipplp_v_v_vvod(GtkWidget *widget,class l_tipplp_v_data *data)
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

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int tipplp_zap(class l_tipplp_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);

if(data->rk.kod.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }

if(data->rk.naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }

if(iceb_u_SRAV(data->kodk.ravno(),data->rk.kod.ravno(),0) != 0)
 if(tipplp_pk(data->rk.kod.ravno(),data->tablica,data->window) != 0)
  return(1);
  
short metkakor=0;

if(data->kodk.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into %s \
values ('%s','%s',%d,%ld)",
   data->tablica,
   data->rk.kod.ravno_filtr(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kodk.ravno(),data->rk.kod.ravno(),0) != 0)
   {
    if(iceb_menu_danet(gettext("Корректировать код ? Вы уверены ?"),2,data->window) == 2)
      return(1);
    metkakor=1;
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update %s \
set \
kod='%s',\
naik='%s',\
ktoz=%d,\
vrem=%ld \
where kod='%s'",
   data->tablica,
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->kodk.ravno());
   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 tipplp_kkvt(data->kodk.ravno(),data->rk.kod.ravno(),data->tablica,data->window);


return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void tipplp_kkvt(const char *skod,const char *nkod,const char *tablica,GtkWidget *wpredok)
{
char strsql[512];

if(iceb_u_SRAV(tablica,"Ktnp",0) == 0)
  sprintf(strsql,"update Tpltp set nomd='%s' where nomd='%s'",nkod,skod);
if(iceb_u_SRAV(tablica,"Ktnt",0) == 0)
  sprintf(strsql,"update Tpltt set nomd='%s' where nomd='%s'",nkod,skod);

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
  return;


}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int tipplp_pk(const char *kod,const char *tablica,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from %s where kod='%s'",tablica,kod);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}


