/*$Id: lzaktaxi_p.c,v 1.14 2013/08/13 06:10:25 sasa Exp $*/
/*03.08.2013	13.03.2005	Белых А.И.	lzaktaxi_p.c
Меню для для ввода реквизитов поиска
*/
#include "taxi.h"
#include "l_zaktaxi.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAN,
  E_DATAK,
  E_TELEFON,
  E_KODKLIENTA,
  E_FIO,
  E_KOLPAS,
  E_ADRES_POD,
  E_ADRES_PROEZDA,
  E_KOMENT,
  E_KODVOD,
  E_KODZAV,
  E_KODOP,
  E_SUMA,
  KOLENTER  
 };

class lzaktaxi_p_data
 {
  public:
  class l_zaktaxi_rek  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет
  
  lzaktaxi_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
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


void    lzaktaxi_p_v_vvod(GtkWidget *widget,class lzaktaxi_p_data *data);
void  lzaktaxi_p_v_knopka(GtkWidget *widget,class lzaktaxi_p_data *data);
void   lzaktaxi_p_rekviz(class lzaktaxi_p_data *data);
void  lzaktaxi_p_v_e_knopka(GtkWidget *widget,class lzaktaxi_p_data *data);
gboolean   lzaktaxi_p_v_key_press(GtkWidget *widget,GdkEventKey *event,class lzaktaxi_p_data *data);

extern char *name_system;
extern SQL_baza bd;

int lzaktaxi_p(class l_zaktaxi_rek *rek_poi,GtkWidget *wpredok)
{
lzaktaxi_p_data data;
data.rk=rek_poi;
if(data.rk->datan.getdlinna() == 0)
 data.rk->clear_data();

char strsql[300];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

sprintf(strsql,"%s %s",name_system,"Ввод реквизитов поиска");
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(lzaktaxi_p_v_key_press),&data);

GtkWidget *label=gtk_label_new("Ввод реквизитов поиска");

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


data.knopka_enter[E_DATAN]=gtk_button_new_with_label("Дата начала");
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(lzaktaxi_p_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],"Выбор даты в календаре");

data.entry[E_DATAN] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

data.knopka_enter[E_DATAK]=gtk_button_new_with_label("Дата конца");
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(lzaktaxi_p_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],"Выбор даты в календаре");

data.entry[E_DATAK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));

sprintf(strsql,"%s","Телефон");
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_TELEFON]), label, FALSE, FALSE, 0);

data.entry[E_TELEFON] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TELEFON]), data.entry[E_TELEFON], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TELEFON], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TELEFON]),data.rk->telef.ravno());
gtk_widget_set_name(data.entry[E_TELEFON],iceb_u_inttochar(E_TELEFON));

sprintf(strsql,"%s (,,)","Код клиента");
data.knopka_enter[E_KODKLIENTA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKLIENTA]), data.knopka_enter[E_KODKLIENTA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODKLIENTA],"clicked",G_CALLBACK(lzaktaxi_p_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODKLIENTA],iceb_u_inttochar(E_KODKLIENTA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODKLIENTA],"Выбор кода в списке клиентов");

data.entry[E_KODKLIENTA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODKLIENTA]), data.entry[E_KODKLIENTA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODKLIENTA], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKLIENTA]),data.rk->kodk.ravno());
gtk_widget_set_name(data.entry[E_KODKLIENTA],iceb_u_inttochar(E_KODKLIENTA));

label=gtk_label_new("Фамилия");
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);

data.entry[E_FIO] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rk->fio.ravno());
gtk_widget_set_name(data.entry[E_FIO],iceb_u_inttochar(E_FIO));

label=gtk_label_new("Количество пассажиров");
gtk_box_pack_start (GTK_BOX (hbox[E_KOLPAS]), label, FALSE, FALSE, 0);

data.entry[E_KOLPAS] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOLPAS]), data.entry[E_KOLPAS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLPAS], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLPAS]),data.rk->kolp.ravno());
gtk_widget_set_name(data.entry[E_KOLPAS],iceb_u_inttochar(E_KOLPAS));

label=gtk_label_new("Адрес подачи");
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES_POD]), label, FALSE, FALSE, 0);

data.entry[E_ADRES_POD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES_POD]), data.entry[E_ADRES_POD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ADRES_POD], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRES_POD]),data.rk->adreso.ravno());
gtk_widget_set_name(data.entry[E_ADRES_POD],iceb_u_inttochar(E_ADRES_POD));

label=gtk_label_new("Адрес проезда");
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES_PROEZDA]), label, FALSE, FALSE, 0);

data.entry[E_ADRES_PROEZDA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES_PROEZDA]), data.entry[E_ADRES_PROEZDA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ADRES_PROEZDA], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRES_PROEZDA]),data.rk->adresk.ravno());
gtk_widget_set_name(data.entry[E_ADRES_PROEZDA],iceb_u_inttochar(E_ADRES_PROEZDA));

label=gtk_label_new("Коментарий");
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk->koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));

sprintf(strsql,"%s (,,)","Код водителя");
data.knopka_enter[E_KODVOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODVOD]), data.knopka_enter[E_KODVOD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODVOD],"clicked",G_CALLBACK(lzaktaxi_p_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODVOD],iceb_u_inttochar(E_KODVOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODVOD],"Выбор кода в списке водителей");

data.entry[E_KODVOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODVOD]), data.entry[E_KODVOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODVOD], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODVOD]),data.rk->kv.ravno());
gtk_widget_set_name(data.entry[E_KODVOD],iceb_u_inttochar(E_KODVOD));

sprintf(strsql,"%s (,,)","Код завершения");
data.knopka_enter[E_KODZAV]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODZAV]), data.knopka_enter[E_KODZAV], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODZAV],"clicked",G_CALLBACK(lzaktaxi_p_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODZAV],iceb_u_inttochar(E_KODZAV));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODZAV],"Выбор кода в списке кодов завершения");

data.entry[E_KODZAV] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODZAV]), data.entry[E_KODZAV], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODZAV], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODZAV]),data.rk->kodzav.ravno());
gtk_widget_set_name(data.entry[E_KODZAV],iceb_u_inttochar(E_KODZAV));

sprintf(strsql,"%s (,,)","Код оператора");
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), label, FALSE, FALSE, 0);

data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kod_operatora.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));

label=gtk_label_new("Сумма");
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);

data.entry[E_SUMA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA], "activate",G_CALLBACK(lzaktaxi_p_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.rk->suma.ravno());
gtk_widget_set_name(data.entry[E_SUMA],iceb_u_inttochar(E_SUMA));



sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(lzaktaxi_p_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(lzaktaxi_p_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(lzaktaxi_p_v_knopka),&data);
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
void  lzaktaxi_p_v_e_knopka(GtkWidget *widget,class lzaktaxi_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
class iceb_u_str kod("");
class iceb_u_str naim("");

switch ((gint)knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rk->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rk->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
      
    return;  

  case E_KODKLIENTA:
    if(l_taxisk(1,&kod,&naim,data->window) == 0)
     {
      data->rk->kodk.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKLIENTA]),data->rk->kodk.ravno());
     }
    return;  

  case E_KODVOD:
    if(l_vod(1,&kod,&naim,data->window) == 0)
     {
      data->rk->kv.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODVOD]),data->rk->kv.ravno());
     }
    return;  

  case E_KODZAV:
    if(l_taxikz(1,&kod,&naim,data->window) == 0)
     {
      data->rk->kodzav.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODZAV]),data->rk->kodzav.ravno());
     }
    return;  
   
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   lzaktaxi_p_v_key_press(GtkWidget *widget,GdkEventKey *event,class lzaktaxi_p_data *data)
{

//printf("lzaktaxi_p_v_key_press\n");
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
void  lzaktaxi_p_v_knopka(GtkWidget *widget,class lzaktaxi_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->rk->datan.getdlinna() > 1)
     if(data->rk->datan.prov_dat() != 0)
      {
       iceb_menu_soob("Не правильно введена дата начала !",data->window);
       return;
      }
    if(data->rk->datak.getdlinna() > 1)
     if(data->rk->datak.prov_dat() != 0)
      {
        iceb_menu_soob("Не правильно введена дата конца !",data->window);
        return;
      }

    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    lzaktaxi_p_v_vvod(GtkWidget *widget,class lzaktaxi_p_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("lzaktaxi_p_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_TELEFON:
    data->rk->telef.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODKLIENTA:
    data->rk->kodk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_FIO:
    data->rk->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOLPAS:
    data->rk->kolp.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_ADRES_POD:
    data->rk->adreso.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_ADRES_PROEZDA:
    data->rk->adresk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOMENT:
    data->rk->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KODVOD:
    data->rk->kv.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KODZAV:
    data->rk->kodzav.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KODOP:
    data->rk->kod_operatora.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_SUMA:
    data->rk->suma.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
