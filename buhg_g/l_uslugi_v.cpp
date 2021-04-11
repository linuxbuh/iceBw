/*$Id: l_uslugi_v.c,v 1.32 2013/10/04 07:27:16 sasa Exp $*/
/*09.10.2019	18.07.2005	Белых А.И.	l_uslugi_v.c
Ввод и корректировка записей в список 
*/
#include "buhg_g.h"
#include "l_uslugi.h"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOD,
  E_GRUPA,
  E_NAIM,
  E_EI,
  E_SHET,
  E_CENA,
  E_ARTIKUL,
  E_KU,
//  E_NDS,
  KOLENTER  
 };

class l_uslugi_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *radiobutton[2];
  GtkWidget *label_naigr;
  GtkWidget *label_naiei;
  GtkWidget *label_naishet;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class uslugi_rek rk;
  
  iceb_u_str kodk; //Код записи которую корректируют
  static int kod_usl_pred;  
  l_uslugi_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton[0])) == TRUE)
     {
      rk.metka_cen=0;
     }
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton[1])) == TRUE)
     {
      rk.metka_cen=1;
     }
   }
 };

gboolean   l_uslugi_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uslugi_v_data *data);
void    l_uslugi_v_v_vvod(GtkWidget *widget,class l_uslugi_v_data *data);
void  l_uslugi_v_v_knopka(GtkWidget *widget,class l_uslugi_v_data *data);
void uslugi_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok);
int uslugi_zap(class l_uslugi_v_data *data);
int uslugi_pk(const char *kod,GtkWidget *wpredok);
void  uslugi_v_e_knopka(GtkWidget *widget,class l_uslugi_v_data *data);


extern SQL_baza bd;
//extern double	okrcn;  /*Округление цены*/
int l_uslugi_v_data::kod_usl_pred;

int l_uslugi_v(iceb_u_str *kodzap, //Код записи для корректировки; возвращаем код введённой записи
GtkWidget *wpredok)
{
int gor=0,ver=0;
class l_uslugi_v_data data;
data.kodk.new_plus(kodzap->ravno());
data.name_window.plus(__FUNCTION__);

char strsql[512];
iceb_u_str kikz;

if(kodzap->getdlinna() > 1)
 {
  //читаем корректируемую запись
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Uslugi where kodus=%s",kodzap->ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.rk.kod.new_plus(row[0]);
  data.rk.grupa.new_plus(row[1]);
  
  data.rk.naim.new_plus(row[2]);
  data.rk.ei.new_plus(row[3]);
  data.rk.shet.new_plus(row[4]);
  data.rk.cena.new_plus(row[5]);
  if(row[6] != NULL)
    data.rk.metka_cen=atoi(row[6]);
  else
    data.rk.metka_cen=0;  
  kikz.plus(iceb_kikz(row[7],row[8],wpredok));
  data.rk.artikul.new_plus(row[9]);
  data.rk.ku.new_plus(row[10]);
 }
else
 {
  sprintf(strsql,"%d",nomkmw(data.kod_usl_pred,wpredok));
  data.rk.kod.new_plus(strsql);
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uslugi_v_v_key_press),&data);

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

//Вставляем радиокнопки

data.radiobutton[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Цена без НДС"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[0], TRUE, TRUE, 0);

data.radiobutton[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton[0]),gettext("Цена с НДС"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[1], TRUE, TRUE, 0);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[data.rk.metka_cen]),TRUE); //Устанавливем активной кнопку

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

/******************
sprintf(strsql,"%s",gettext("Код услуги"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
*****************/

data.knopka_enter[E_KOD]=gtk_button_new_with_label(gettext("Код услуги"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.knopka_enter[E_KOD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD],"clicked",G_CALLBACK(uslugi_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD],iceb_u_inttochar(E_KOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD],gettext("Получение нового кода услуги"));

data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));


sprintf(strsql,"%s",gettext("Код группы"));
data.knopka_enter[E_GRUPA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.knopka_enter[E_GRUPA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_GRUPA],"clicked",G_CALLBACK(uslugi_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_GRUPA],iceb_u_inttochar(E_GRUPA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_GRUPA],gettext("Выбор группы"));

data.entry[E_GRUPA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.entry[E_GRUPA],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GRUPA], "activate",G_CALLBACK(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPA]),data.rk.grupa.ravno());
gtk_widget_set_name(data.entry[E_GRUPA],iceb_u_inttochar(E_GRUPA));

data.label_naigr=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.label_naigr,TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Наименование услуги"));
label=gtk_label_new(strsql);
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),79);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

sprintf(strsql,"%s",gettext("Единица измерения"));
data.knopka_enter[E_EI]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_EI],"clicked",G_CALLBACK(uslugi_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_EI],iceb_u_inttochar(E_EI));
gtk_widget_set_tooltip_text(data.knopka_enter[E_EI],gettext("Выбор единицы измерения"));

data.entry[E_EI] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_EI], "activate",G_CALLBACK(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.rk.ei.ravno());
gtk_widget_set_name(data.entry[E_EI],iceb_u_inttochar(E_EI));

data.label_naiei=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.label_naiei,TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Цена"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, FALSE, FALSE, 0);

data.entry[E_CENA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_CENA], "activate",G_CALLBACK(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.rk.cena.ravno());
gtk_widget_set_name(data.entry[E_CENA],iceb_u_inttochar(E_CENA));

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(uslugi_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор единицы измерения"));

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk.shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

data.label_naishet=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.label_naishet,TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Артикул"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_ARTIKUL]), label, FALSE, FALSE, 0);

data.entry[E_ARTIKUL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_ARTIKUL]), data.entry[E_ARTIKUL],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ARTIKUL], "activate",G_CALLBACK(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ARTIKUL]),data.rk.artikul.ravno());
gtk_widget_set_name(data.entry[E_ARTIKUL],iceb_u_inttochar(E_ARTIKUL));

sprintf(strsql,"%s",gettext("Код ГКПУ")); /*Державний класіфікатор продукції і полуг*/
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KU]), label, FALSE, FALSE, 0);

data.entry[E_KU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KU]), data.entry[E_KU],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KU], "activate",G_CALLBACK(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KU]),data.rk.ku.ravno());
gtk_widget_set_name(data.entry[E_KU],iceb_u_inttochar(E_KU));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_uslugi_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_uslugi_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

if(kodzap->getdlinna() <= 1)
  gtk_widget_grab_focus(data.entry[1]);
else
  gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 kodzap->new_plus(data.rk.kod.ravno());
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uslugi_v_e_knopka(GtkWidget *widget,class l_uslugi_v_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_KOD:
    data->rk.kod.new_plus(nomkmw(data->kod_usl_pred,data->window));
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->rk.kod.ravno());
    return;

  case E_EI:
    if(iceb_vibrek(1,"Edizmer",&data->rk.ei,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naiei),naim.ravno(20));
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->rk.ei.ravno());
    return;

  case E_SHET:
    if(iceb_vibrek(1,"Plansh",&data->rk.shet,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naishet),naim.ravno(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk.shet.ravno());
    return;

  case E_GRUPA:
    if(l_uslgrup(1,&kod,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naigr),naim.ravno(20));
      data->rk.grupa.new_plus(kod.ravno());
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),data->rk.grupa.ravno());
    return;
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uslugi_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uslugi_v_data *data)
{

//printf("l_uslugi_v_v_key_press\n");
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
void  l_uslugi_v_v_knopka(GtkWidget *widget,class l_uslugi_v_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    iceb_sizww(data->name_window.ravno(),data->window);
    if(uslugi_zap(data) != 0)
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

void    l_uslugi_v_v_vvod(GtkWidget *widget,class l_uslugi_v_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_uslugi_v_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GRUPA:
    data->rk.grupa.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select naik from Uslgrup where kod=%d",data->rk.grupa.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naigr),strsql);
     }    
    break;

  case E_EI:
    data->rk.ei.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select naik from Edizmer where kod='%s'",data->rk.ei.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naiei),strsql);
     }    
    break;

  case E_CENA:
    data->rk.cena.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET:
    data->rk.shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select nais from Plansh where ns='%s'",data->rk.shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naishet),strsql);
     }    
    break;

  case E_NAIM:
    data->rk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_ARTIKUL:
    data->rk.artikul.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KU:
    data->rk.ku.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int uslugi_zap(class l_uslugi_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);
SQL_str row;
class SQLCURSOR cur;

data->read_rek(); //Читаем реквизиты меню

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
if(data->rk.grupa.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели группу!"),data->window);
  return(1);
 }

if(data->rk.ei.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Edizmer where kod='%s'",data->rk.ei.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 0)
   {
    iceb_menu_soob(gettext("Нет такой единицы измерения в справочнике!"),data->window);
    return(1);
   }
 }
if(data->rk.grupa.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Uslgrup where kod=%d",data->rk.grupa.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 0)
   {
    iceb_menu_soob(gettext("Нет такой группы в справочнике!"),data->window);
    return(1);
   }
 }

if(data->rk.shet.getdlinna() > 1)
 if(iceb_prsh1(data->rk.shet.ravno(),data->window) != 0)
  return(1);

short metkakor=0;

if(data->kodk.getdlinna() <= 1)
 {

  if(uslugi_pk(data->rk.kod.ravno(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Uslugi \
values (%d,%s,'%s','%s','%s',%.7g,%d,%d,%ld,'%s','%s')",
   data->rk.kod.ravno_atoi(),
   data->rk.grupa.ravno(),
   data->rk.naim.ravno_filtr(),
   data->rk.ei.ravno_filtr(),
   data->rk.shet.ravno_filtr(),
   data->rk.cena.ravno_atof(),
   data->rk.metka_cen,
   iceb_getuid(data->window),vrem,
   data->rk.artikul.ravno_filtr(),
   data->rk.ku.ravno_filtr());
 }
else
 {
  
  if(iceb_u_SRAV(data->kodk.ravno(),data->rk.kod.ravno(),0) != 0)
   {
    if(uslugi_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Uslugi \
set \
kodus=%d,\
kodgr=%s,\
naius='%s',\
ei='%s',\
shetu='%s',\
cena=%.7g,\
nds=%d,\
ktoz=%d,\
vrem=%ld,\
art='%s',\
ku='%s' \
where kodus=%s",
   data->rk.kod.ravno_atoi(),
   data->rk.grupa.ravno(),
   data->rk.naim.ravno_filtr(),
   data->rk.ei.ravno_filtr(),
   data->rk.shet.ravno_filtr(),
   data->rk.cena.ravno_atof(),
   data->rk.metka_cen,
   iceb_getuid(data->window),vrem,
   data->rk.artikul.ravno_filtr(),
   data->rk.ku.ravno_filtr(),
   data->kodk.ravno());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(data->kodk.getdlinna() <= 1)
 data->kod_usl_pred=data->rk.kod.ravno_atoi();
 
gtk_widget_hide(data->window);

if(metkakor == 1)
 uslugi_kkvt(data->kodk.ravno(),data->rk.kod.ravno(),data->window);


return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void uslugi_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Uslugi set kodus=%s where kodus=%s",nkod,skod);
if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
  return;

sprintf(strsql,"update Usldokum1 set kodzap=%s where kodzap=%s and metka=1",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Usldokum2 set kodzap=%s where kodzap=%s and metka=1",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);


sprintf(strsql,"update Roznica set kodm=%s where kodm=%s and metka=2",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Specif set kodd=%s where kodd=%s and kz=1",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);


}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int uslugi_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kodus from Uslugi where kodus=%s",kod);
//printf("ei_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
