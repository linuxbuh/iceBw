/*$Id: zar_ar_v.c,v 1.22 2013/08/25 08:27:00 sasa Exp $*/
/*13.07.2015	19.09.2006	Белых А.И.	zar_ar_v.c
Ввод реквизитов для расчёта зарплаты по всем карточкам
*/
#include "buhg_g.h"
#include "zar_ar.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAR,
  E_PODR,
  E_TABNOM,
  E_NAH_ONLY,
  E_UDER_ONLY,
  KOLENTER  
 };

class zar_ar_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton2[2];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class zar_ar_rek *rk;
  
  zar_ar_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datar.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAR])));
    rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODR])));
    rk->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM])));
    rk->nah_only.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAH_ONLY])));
    rk->uder_only.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_UDER_ONLY])));

//    for(int i=0; i < KOLENTER; i++)
//      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_rek()
   {
    for(int ii=0; ii < KOLENTER; ii++)
     gtk_entry_set_text(GTK_ENTRY(entry[ii]),"");
    rk->clear_rek();
   }
 };

gboolean   zar_ar_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class zar_ar_v_data *data);
void    zar_ar_v_v_vvod(GtkWidget *widget,class zar_ar_v_data *data);
void  zar_ar_v_v_knopka(GtkWidget *widget,class zar_ar_v_data *data);

void  zar_ar_v_e_knopka(GtkWidget *widget,class zar_ar_v_data *data);
void  zar_ar_v_radio2_0(GtkWidget *widget,class zar_ar_v_data *data);
void  zar_ar_v_radio2_1(GtkWidget *widget,class zar_ar_v_data *data);

extern SQL_baza bd;
extern double	okrcn;  /*Округление цены*/
extern short mmm,ggg;

int zar_ar_v(class zar_ar_rek *rk,GtkWidget *wpredok)
{
class zar_ar_v_data data;
char strsql[512];
data.rk=rk;
if(data.rk->datar.getdlinna() <= 1)
 {
  data.rk->datar.new_plus(mmm);
  data.rk->datar.plus(".");
  data.rk->datar.plus(ggg);
  
 }
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Автоматизированный расчёт по всем карточкам"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_ar_v_v_key_press),&data);


GtkWidget *label=NULL;
if(data.rk->metka_r == 2)
 label=gtk_label_new(gettext("Расчёт только удержаний по всем карточкам"));
if(data.rk->metka_r == 1)
 label=gtk_label_new(gettext("Расчёт только начислений по всем карточкам"));
if(data.rk->metka_r == 3)
 label=gtk_label_new(gettext("Расчёт только начислений и удержаний по всем карточкам"));
if(data.rk->metka_r == 4)
 label=gtk_label_new(gettext("Расчёт только начислений на фонд оплаты труда"));
if(data.rk->metka_r == 5)
 label=gtk_label_new(gettext("Расчёт только проводок"));

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

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator3=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator1);
//Вставляем радиокнопки
GSList *group;

data.radiobutton2[0]=gtk_radio_button_new_with_label(NULL,gettext("Без просмотра протокола хода расчёта"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton2[0], "clicked",G_CALLBACK(zar_ar_v_radio2_0),&data);
if(data.rk->metka_ff == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[0]),TRUE); //Устанавливем активной кнопку
//gtk_widget_set_name(data.radiobutton0,iceb_u_inttochar("0"));

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton2[0]));

data.radiobutton2[1]=gtk_radio_button_new_with_label(group,gettext("Просмотр протокола хода расчёта"));
g_signal_connect(data.radiobutton2[1], "clicked",G_CALLBACK(zar_ar_v_radio2_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);
if(data.rk->metka_ff == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[1]),TRUE); //Устанавливем активной кнопку

gtk_container_add (GTK_CONTAINER (vbox), separator3);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s %s",gettext("Дата расчёта зарплаты"),gettext("(м.г)"));
data.knopka_enter[E_DATAR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAR]), data.knopka_enter[E_DATAR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAR],"clicked",G_CALLBACK(zar_ar_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAR],iceb_u_inttochar(E_DATAR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAR],gettext("Выбор даты"));

data.entry[E_DATAR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAR]),7);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAR]), data.entry[E_DATAR],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAR], "activate",G_CALLBACK(zar_ar_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAR]),data.rk->datar.ravno());
gtk_widget_set_name(data.entry[E_DATAR],iceb_u_inttochar(E_DATAR));

sprintf(strsql,"%s (,,)",gettext("Подразделение"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(zar_ar_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор счёта в плане счетов"));

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(zar_ar_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));


sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_TABNOM],"clicked",G_CALLBACK(zar_ar_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_TABNOM],iceb_u_inttochar(E_TABNOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_TABNOM],gettext("Выбор счёта в плане счетов"));

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABNOM], "activate",G_CALLBACK(zar_ar_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.rk->tabnom.ravno());
gtk_widget_set_name(data.entry[E_TABNOM],iceb_u_inttochar(E_TABNOM));


sprintf(strsql,"%s (,,)",gettext("Код начисления"));
data.knopka_enter[E_NAH_ONLY]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NAH_ONLY]), data.knopka_enter[E_NAH_ONLY], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NAH_ONLY],"clicked",G_CALLBACK(zar_ar_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NAH_ONLY],iceb_u_inttochar(E_NAH_ONLY));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NAH_ONLY],gettext("Выбор кода начисления"));

data.entry[E_NAH_ONLY] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NAH_ONLY]), data.entry[E_NAH_ONLY],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAH_ONLY], "activate",G_CALLBACK(zar_ar_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAH_ONLY]),data.rk->tabnom.ravno());
gtk_widget_set_name(data.entry[E_NAH_ONLY],iceb_u_inttochar(E_NAH_ONLY));


sprintf(strsql,"%s (,,)",gettext("Код удержания"));
data.knopka_enter[E_UDER_ONLY]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_UDER_ONLY]), data.knopka_enter[E_UDER_ONLY], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_UDER_ONLY],"clicked",G_CALLBACK(zar_ar_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_UDER_ONLY],iceb_u_inttochar(E_UDER_ONLY));
gtk_widget_set_tooltip_text(data.knopka_enter[E_UDER_ONLY],gettext("Выбор кода удержания"));

data.entry[E_UDER_ONLY] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_UDER_ONLY]), data.entry[E_UDER_ONLY],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_UDER_ONLY], "activate",G_CALLBACK(zar_ar_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_UDER_ONLY]),data.rk->tabnom.ravno());
gtk_widget_set_name(data.entry[E_UDER_ONLY],iceb_u_inttochar(E_UDER_ONLY));



sprintf(strsql,"F2 %s",gettext("Выполнить"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Выполнить расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(zar_ar_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(zar_ar_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(zar_ar_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);
}
void  zar_ar_v_radio2_0(GtkWidget *widget,class zar_ar_v_data *data)
{
//g_print("zar_ar_v_radio0\n");

data->rk->metka_ff=0;
//printf("prov=0\n");

}
void  zar_ar_v_radio2_1(GtkWidget *widget,class zar_ar_v_data *data)
{
//g_print("zar_ar_v_radio1\n");
data->rk->metka_ff=1;
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  zar_ar_v_e_knopka(GtkWidget *widget,class zar_ar_v_data *data)
{
iceb_u_str kod(0);
iceb_u_str naim(0);

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATAR:
    iceb_calendar1(&data->rk->datar,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAR]),data->rk->datar.ravno());
    return;

  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->rk->tabnom.z_plus(kod.ravno());
     
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->rk->tabnom.ravno());
    return;

  case E_NAH_ONLY:
    if(l_zarnah(1,&kod,&naim,data->window) == 0)
     data->rk->nah_only.z_plus(kod.ravno());
     
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NAH_ONLY]),data->rk->nah_only.ravno());
    return;

  case E_UDER_ONLY:
    if(l_zarud(1,&kod,&naim,data->window) == 0)
     data->rk->uder_only.z_plus(kod.ravno());
     
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_UDER_ONLY]),data->rk->uder_only.ravno());
    return;

  case E_PODR:
    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     data->rk->podr.z_plus(kod.ravno());
     
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_ar_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class zar_ar_v_data *data)
{

//printf("zar_ar_v_v_key_press\n");
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
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_ar_v_v_knopka(GtkWidget *widget,class zar_ar_v_data *data)
{
short m,g;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("zar_ar_v_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->rk->datar.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата !"),data->window);
      return;
     }        
    if(iceb_u_rsdat1(&m,&g,data->rk->datar.ravno()) != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата !"),data->window);
      return;
     }
    if(iceb_pbpds(m,g,data->window) != 0)
     return;
    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

  case FK4:
    data->clear_rek();
    return;
      
  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    zar_ar_v_v_vvod(GtkWidget *widget,class zar_ar_v_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//g_print("zar_ar_v_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAR:
    data->rk->datar.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PODR:
    data->rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_TABNOM:
    data->rk->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}

