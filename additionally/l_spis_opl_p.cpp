/*$Id: l_spis_opl_p.c,v 1.17 2013/08/13 06:10:24 sasa Exp $*/
/*09.01.2007	27.01.2006	Белых А.И.	l_spis_opl_p.c
Реквизиты для просмотра списка оплат
*/
#include <unistd.h>
#include "i_rest.h"
#include "l_spis_opl.h"

enum
{
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
};

enum
 {
  E_DATAN,
  E_VREMN,
  E_DATAK,
  E_VREMK,

  E_KASSA,
  E_SUMA,
  E_DATAD,
  E_NOMDOK,
  E_KODKL,
  E_KOMENT,
  E_PODR,
  KOLENTER  
 };

class  spis_opl_p_data
 {
  public:
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopkap[KOL_PFK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton[2];
  GtkWidget *radiobutton2[2];
  GtkWidget *radiobutton3[2];
  
  class l_spis_opl_rek *rk; //Реквизиты поиска документов
  short kl_shift;
  int voz; //0-искать записи 1-нет
    
 };
 
void       spis_opl_p_radio0(GtkWidget *,class spis_opl_p_data *);
void       spis_opl_p_radio1(GtkWidget *,class spis_opl_p_data *);

void       spis_opl_p_radio2_0(GtkWidget *,class spis_opl_p_data *);
void       spis_opl_p_radio2_1(GtkWidget *,class spis_opl_p_data *);

void       spis_opl_p_radio3_0(GtkWidget *,class spis_opl_p_data *);
void       spis_opl_p_radio3_1(GtkWidget *,class spis_opl_p_data *);

gboolean   spis_opl_p_key_press(GtkWidget *widget,GdkEventKey *event,class spis_opl_p_data *data);
void  spis_opl_p_knopka(GtkWidget *widget,class spis_opl_p_data *data);
void  spis_opl_v_e_knopka(GtkWidget *widget,class spis_opl_p_data *data);
void    spis_opl_p_vvod(GtkWidget *widget,class spis_opl_p_data *data);

//extern SQL_baza	bd;
extern char *name_system;
extern class iceb_get_dev_data config_dev;


int l_spis_opl_p(class l_spis_opl_rek *datap,GtkWidget *wpredok)
{
printf("l_spis_opl_p\n");

class  spis_opl_p_data data;
data.rk=datap;

char strsql[300];

    
data.kl_shift=0;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));


gtk_window_set_title(GTK_WINDOW(data.window),strsql);
//g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_widget_destroyed),&data.window);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(spis_opl_p_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label=gtk_label_new(gettext("Поиск записей внесения средств и погашения счетов"));


GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (vbox), label);

GtkWidget *hbox[KOLENTER];

for(int i=0 ; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator2=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator3=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator4=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0 ; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), separator1);

//Вставляем радиокнопки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Оплату наличными - показывать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton[0], "clicked",G_CALLBACK(spis_opl_p_radio0),&data);
//gtk_widget_set_name(data.radiobutton0,iceb_u_inttochar("0"));

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton[0]));


data.radiobutton[1]=gtk_radio_button_new_with_label(group,gettext("Оплату наличными - не показывать"));
g_signal_connect(data.radiobutton[1], "clicked",G_CALLBACK(spis_opl_p_radio1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[1], TRUE, TRUE, 0);


gtk_container_add (GTK_CONTAINER (vbox), separator2);

data.radiobutton2[0]=gtk_radio_button_new_with_label(NULL,gettext("Безналичную оплату - показывать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton2[0], "clicked",G_CALLBACK(spis_opl_p_radio2_0),&data);
//gtk_widget_set_name(data.radiobutton20,iceb_u_inttochar("0"));

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton2[0]));


data.radiobutton2[1]=gtk_radio_button_new_with_label(group,gettext("Безналичную оплату - не показывать"));
g_signal_connect(data.radiobutton2[1], "clicked",G_CALLBACK(spis_opl_p_radio2_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);

gtk_container_add (GTK_CONTAINER (vbox), separator3);

data.radiobutton3[0]=gtk_radio_button_new_with_label(NULL,gettext("Списание - показывать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton3[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton3[0], "clicked",G_CALLBACK(spis_opl_p_radio3_0),&data);
//gtk_widget_set_name(data.radiobutton30,iceb_u_inttochar("0"));

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton3[0]));


data.radiobutton3[1]=gtk_radio_button_new_with_label(group,gettext("Спиание - не показывать"));
g_signal_connect(data.radiobutton3[1], "clicked",G_CALLBACK(spis_opl_p_radio3_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton3[1], TRUE, TRUE, 0);

if(data.rk->metka_nal == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[0]),TRUE); //Устанавливем активной кнопку
if(data.rk->metka_nal == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[1]),TRUE); //Устанавливем активной кнопку

if(data.rk->metka_bnal == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[0]),TRUE); //Устанавливем активной кнопку
if(data.rk->metka_bnal == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[1]),TRUE); //Устанавливем активной кнопку

if(data.rk->metka_spis == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton3[0]),TRUE); //Устанавливем активной кнопку
if(data.rk->metka_spis == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton3[1]),TRUE); //Устанавливем активной кнопку

gtk_container_add (GTK_CONTAINER (vbox), separator4);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


data.knopka_enter[E_DATAN]=gtk_button_new_with_label(gettext("Дата начала"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(spis_opl_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));


data.knopka_enter[E_VREMN]=gtk_button_new_with_label(gettext("Время начала"));
gtk_box_pack_start (GTK_BOX (hbox[E_VREMN]), data.knopka_enter[E_VREMN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_VREMN],"clicked",G_CALLBACK(spis_opl_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_VREMN],iceb_u_inttochar(E_VREMN));

data.entry[E_VREMN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VREMN]),8);
gtk_box_pack_start (GTK_BOX (hbox[E_VREMN]), data.entry[E_VREMN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_VREMN], "activate",G_CALLBACK(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREMN]),data.rk->vremn.ravno());
gtk_widget_set_name(data.entry[E_VREMN],iceb_u_inttochar(E_VREMN));


data.knopka_enter[E_DATAK]=gtk_button_new_with_label(gettext("Дата конца"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(spis_opl_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));


data.knopka_enter[E_VREMK]=gtk_button_new_with_label(gettext("Время конца"));
gtk_box_pack_start (GTK_BOX (hbox[E_VREMK]), data.knopka_enter[E_VREMK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_VREMK],"clicked",G_CALLBACK(spis_opl_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_VREMK],iceb_u_inttochar(E_VREMK));

data.entry[E_VREMK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VREMK]),8);
gtk_box_pack_start (GTK_BOX (hbox[E_VREMK]), data.entry[E_VREMK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_VREMK], "activate",G_CALLBACK(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREMK]),data.rk->vremk.ravno());
gtk_widget_set_name(data.entry[E_VREMK],iceb_u_inttochar(E_VREMK));

sprintf(strsql,"%s (,,)",gettext("Номер документа"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NOMDOK],"clicked",G_CALLBACK(spis_opl_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));

data.knopka_enter[E_DATAD]=gtk_button_new_with_label(gettext("Дата документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.knopka_enter[E_DATAD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAD],"clicked",G_CALLBACK(spis_opl_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAD],iceb_u_inttochar(E_DATAD));

data.entry[E_DATAD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.entry[E_DATAD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAD], "activate",G_CALLBACK(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAD]),data.rk->datad.ravno());
gtk_widget_set_name(data.entry[E_DATAD],iceb_u_inttochar(E_DATAD));

sprintf(strsql,"%s (,,)",gettext("Код клиента"));
data.knopka_enter[E_KODKL]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.knopka_enter[E_KODKL], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODKL],"clicked",G_CALLBACK(spis_opl_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODKL],iceb_u_inttochar(E_KODKL));

data.entry[E_KODKL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODKL]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.entry[E_KODKL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODKL], "activate",G_CALLBACK(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKL]),data.rk->kodkl.ravno());
gtk_widget_set_name(data.entry[E_KODKL],iceb_u_inttochar(E_KODKL));


data.knopka_enter[E_KOMENT]=gtk_button_new_with_label(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.knopka_enter[E_KOMENT], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOMENT],"clicked",G_CALLBACK(spis_opl_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOMENT],iceb_u_inttochar(E_KOMENT));

data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk->koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));


sprintf(strsql,"%s (,,)",gettext("Подразделение"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(spis_opl_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->koment.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));

sprintf(strsql,"%s (,,)",gettext("Касса"));
data.knopka_enter[E_KASSA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.knopka_enter[E_KASSA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KASSA],"clicked",G_CALLBACK(spis_opl_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KASSA],iceb_u_inttochar(E_KASSA));

data.entry[E_KASSA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.entry[E_KASSA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KASSA], "activate",G_CALLBACK(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KASSA]),data.rk->kassa.ravno());
gtk_widget_set_name(data.entry[E_KASSA],iceb_u_inttochar(E_KASSA));

sprintf(strsql,"%s",gettext("Сумма"));
data.knopka_enter[E_SUMA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.knopka_enter[E_SUMA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SUMA],"clicked",G_CALLBACK(spis_opl_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SUMA],iceb_u_inttochar(E_SUMA));

data.entry[E_SUMA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA], "activate",G_CALLBACK(spis_opl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.rk->suma.ravno());
gtk_widget_set_name(data.entry[E_SUMA],iceb_u_inttochar(E_SUMA));



sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopkap[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopkap[PFK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopkap[PFK2],"clicked",G_CALLBACK(spis_opl_p_knopka),&data);
gtk_widget_set_name(data.knopkap[PFK2],iceb_u_inttochar(PFK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopkap[PFK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopkap[PFK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopkap[PFK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopkap[PFK4],"clicked",G_CALLBACK(spis_opl_p_knopka),&data);
gtk_widget_set_name(data.knopkap[PFK4],iceb_u_inttochar(PFK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopkap[PFK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopkap[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopkap[PFK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopkap[PFK10],"clicked",G_CALLBACK(spis_opl_p_knopka),&data);
gtk_widget_set_name(data.knopkap[PFK10],iceb_u_inttochar(PFK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopkap[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  spis_opl_p_radio0(GtkWidget *widget,class spis_opl_p_data *data)
{
//g_print("spis_opl_p_radio0\n");

data->rk->metka_nal=0;

}
void  spis_opl_p_radio1(GtkWidget *widget,class spis_opl_p_data *data)
{
//g_print("spis_opl_p_radio1\n");
data->rk->metka_nal=1;
}
/*********************/
void  spis_opl_p_radio2_0(GtkWidget *widget,class spis_opl_p_data *data)
{
//g_print("spis_opl_p_radio0\n");

data->rk->metka_bnal=0;

}
void  spis_opl_p_radio2_1(GtkWidget *widget,class spis_opl_p_data *data)
{
//g_print("spis_opl_p_radio1\n");
data->rk->metka_bnal=1;
}
/*********************/
void  spis_opl_p_radio3_0(GtkWidget *widget,class spis_opl_p_data *data)
{
//g_print("spis_opl_p_radio0\n");

data->rk->metka_spis=0;

}
void  spis_opl_p_radio3_1(GtkWidget *widget,class spis_opl_p_data *data)
{
//g_print("spis_opl_p_radio1\n");
data->rk->metka_spis=1;
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  spis_opl_v_e_knopka(GtkWidget *widget,class spis_opl_p_data *data)
{
iceb_u_str naim;
iceb_u_str kod;
kod.plus("");
naim.plus("");

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case E_DATAN:
    //читаем реквизит
    g_signal_emit_by_name(data->entry[E_DATAN],"activate");

    if(config_dev.metka_screen == 0) //сенсорного экрана нет
      iceb_calendar(&data->rk->datan,data->window);
    else
      iceb_mous_klav(gettext("Дата начала"),&data->rk->datan,10,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
    return;  

  case E_VREMN:
    g_signal_emit_by_name(data->entry[E_VREMN],"activate");
    iceb_mous_klav(gettext("Время начала"),&data->rk->vremn,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMN]),data->rk->vremn.ravno());
    return;  

  case E_VREMK:
    g_signal_emit_by_name(data->entry[E_VREMK],"activate");
    iceb_mous_klav(gettext("Время конца"),&data->rk->vremk,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMK]),data->rk->vremk.ravno());
    return;  

  case E_DATAK:
    g_signal_emit_by_name(data->entry[E_DATAK],"activate");

    if(config_dev.metka_screen == 0) //сенсорного экрана нет
      iceb_calendar(&data->rk->datak,data->window);
    else
      iceb_mous_klav(gettext("Дата конца"),&data->rk->datak,10,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());

    return;  

  case E_DATAD:
    g_signal_emit_by_name(data->entry[E_DATAD],"activate");
    if(config_dev.metka_screen == 0) //сенсорного экрана нет
      iceb_calendar(&data->rk->datad,data->window);
    else
      iceb_mous_klav(gettext("Дата документа"),&data->rk->datad,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->rk->datad.ravno());
    return;  

  case E_NOMDOK:
    g_signal_emit_by_name(data->entry[E_NOMDOK],"activate");
    iceb_mous_klav(gettext("Номер документа"),&data->rk->datak,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk->datak.ravno());
    return;  


  case E_KODKL:
//    g_signal_emit_by_name(data->entry[E_KODKL],"activate");
/***********
    if(config_dev.metka_screen == 0) //сенсорного экрана нет
     {
      if(l_klientr(1,&kod,data->window) == 0)
       data->rk->kodkl.z_plus(kod.ravno());
     }
    else
**************/
      iceb_mous_klav(gettext("Код клиента"),&data->rk->kodkl,10,1,0,1,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKL]),data->rk->kodkl.ravno());
    return;  


  case E_KOMENT:
    g_signal_emit_by_name(data->entry[E_KOMENT],"activate");
    iceb_mous_klav(gettext("Коментарий"),&data->rk->koment,100,1,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOMENT]),data->rk->koment.ravno());
    return;  

  case E_PODR:
    g_signal_emit_by_name(data->entry[E_PODR],"activate");
    if(config_dev.metka_screen == 0) //сенсорного экрана нет
     {
      if(rpod_l(1,&kod,&naim,data->window) == 0)
        data->rk->podr.z_plus(kod.ravno());
     }
    else
      iceb_mous_klav(gettext("Подразделение"),&data->rk->podr,100,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
    return;  

  case E_KASSA:
    g_signal_emit_by_name(data->entry[E_KASSA],"activate");
    
    iceb_mous_klav(gettext("Касса"),&data->rk->kassa,100,0,0,0,data->window);
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KASSA]),data->rk->kassa.ravno());
    return;  

  case E_SUMA:
    g_signal_emit_by_name(data->entry[E_SUMA],"activate");
    iceb_mous_klav(gettext("Сумма"),&data->rk->suma,20,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA]),data->rk->suma.ravno());
    return;  
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   spis_opl_p_key_press(GtkWidget *widget,GdkEventKey *event,class spis_opl_p_data *data)
{
//char  bros[300];

//printf("spis_opl_p_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopkap[PFK2],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopkap[PFK4],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopkap[PFK10],"clicked");
    return(TRUE);
//    return(FALSE);

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
void  spis_opl_p_knopka(GtkWidget *widget,class spis_opl_p_data *data)
{
short d,m,g;

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case PFK2:
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(data->entry[i],"activate");

    if(data->rk->datan.getdlinna() > 1)
    if(iceb_u_rsdat(&d,&m,&g,data->rk->datan.ravno(),1) != 0)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не верно введена дата начала !"));
      iceb_menu_soob(&repl,data->window);
      return;
     }

    if(data->rk->datak.getdlinna() > 1)
     if(iceb_u_rsdat(&d,&m,&g,data->rk->datak.ravno(),1) != 0)
      {
       iceb_u_str repl;
       repl.plus(gettext("Не верно введена дата конца !"));
       iceb_menu_soob(&repl,data->window);
       return;
      }

    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case PFK4:
    data->rk->clear_data(); 
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");
    return;

  case PFK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    spis_opl_p_vvod(GtkWidget *widget,class spis_opl_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("spis_opl_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

  case E_VREMN:
    data->rk->vremn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

  case E_VREMK:
    data->rk->vremk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
     
    break;

  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAD:
    data->rk->datad.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_KODKL:
    data->rk->kodkl.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMDOK:
    data->rk->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOMENT:
    data->rk->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PODR:
    data->rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KASSA:
    data->rk->kassa.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
