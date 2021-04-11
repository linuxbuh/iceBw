/*$Id: rasspdokuw_m.c,v 1.16 2013/09/26 09:46:54 sasa Exp $*/
/*12.07.2015	22.12.2005	Белых А.И.	rasspdokuw_m.c
Меню для получения отчёта - список документов
*/
#include "buhg_g.h"
#include "rasspdokuw.h"

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
  E_KODOP,
  E_KONTR,
  E_PODR,
  E_GRUPK,
  KOLENTER  
 };

class rasspdokuw_data
 {
  public:
  rasspdokuw_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет
  GtkWidget *radiobutton[2];
  GtkWidget *radiobutton1[3];
  GtkWidget *radiobutton2[3];

  rasspdokuw_data() //Конструктор
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


void       rasspdokuw_radio0(GtkWidget *,class rasspdokuw_data *);
void       rasspdokuw_radio1(GtkWidget *,class rasspdokuw_data *);

void  rasspdokuw_radio01(GtkWidget *widget,class rasspdokuw_data *data);
void  rasspdokuw_radio11(GtkWidget *widget,class rasspdokuw_data *data);
void  rasspdokuw_radio21(GtkWidget *widget,class rasspdokuw_data *data);

void  rasspdokuw_radio02(GtkWidget *widget,class rasspdokuw_data *data);
void  rasspdokuw_radio12(GtkWidget *widget,class rasspdokuw_data *data);
void  rasspdokuw_radio22(GtkWidget *widget,class rasspdokuw_data *data);

gboolean   rasspdokuw_v_key_press(GtkWidget *widget,GdkEventKey *event,class rasspdokuw_data *data);
void    rasspdokuw_v_vvod(GtkWidget *widget,class rasspdokuw_data *data);
void  rasspdokuw_v_knopka(GtkWidget *widget,class rasspdokuw_data *data);
void  rasspdokuw_v_e_knopka(GtkWidget *widget,class rasspdokuw_data *data);


extern SQL_baza bd;

int   rasspdokuw_m(class rasspdokuw_rr *rek_ras)
{
rasspdokuw_data data;

data.rk=rek_ras;

if(data.rk->datan.getdlinna() == 0)
  data.rk->clear_data();
  
char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать список документов"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rasspdokuw_v_key_press),&data);

class iceb_u_str repl;
repl.plus(gettext("Учёт услуг"));
repl.ps_plus(gettext("Распечатать список документов"));
GtkWidget *label=gtk_label_new(repl.ravno());

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
gtk_container_add (GTK_CONTAINER (vbox), separator1);

//Вставляем радиокнопки сортировки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Сортировка по датам"));
if(data.rk->metka_sort == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton[0], "clicked",G_CALLBACK(rasspdokuw_radio0),&data);
//gtk_widget_set_name(data.radiobutton0,iceb_u_inttochar("0"));

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

data.radiobutton[1]=gtk_radio_button_new_with_label(group,gettext("Сортировка по номерам документов"));
if(data.rk->metka_sort == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[1]),TRUE); //Устанавливем активной кнопку
g_signal_connect(data.radiobutton[1], "clicked",G_CALLBACK(rasspdokuw_radio1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[1], TRUE, TRUE, 0);

GtkWidget *separator2=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator2);

//Вставляем радиокнопки видов документов

data.radiobutton1[0]=gtk_radio_button_new_with_label(NULL,gettext("Только подтверждённые документы"));
if(data.rk->metka_dok == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton1[0], "clicked",G_CALLBACK(rasspdokuw_radio01),&data);
//gtk_widget_set_name(data.radiobutton10,iceb_u_inttochar("0"));

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton1[0]));

data.radiobutton1[1]=gtk_radio_button_new_with_label(group,gettext("Только неподтверждённые документы"));
if(data.rk->metka_dok == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[1]),TRUE); //Устанавливем активной кнопку
g_signal_connect(data.radiobutton1[1], "clicked",G_CALLBACK(rasspdokuw_radio11),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[1], TRUE, TRUE, 0);

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton1[1]));

data.radiobutton1[2]=gtk_radio_button_new_with_label(group,gettext("Все документы"));
if(data.rk->metka_dok == 2)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[2]),TRUE); //Устанавливем активной кнопку
g_signal_connect(data.radiobutton1[2], "clicked",G_CALLBACK(rasspdokuw_radio21),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[2], TRUE, TRUE, 0);

GtkWidget *separator3=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator3);

//Вставляем радиокнопки приходов-расходов

data.radiobutton2[0]=gtk_radio_button_new_with_label(NULL,gettext("Все документы"));
if(data.rk->metka_pr == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton2[0], "clicked",G_CALLBACK(rasspdokuw_radio02),&data);
//gtk_widget_set_name(data.radiobutton20,iceb_u_inttochar("0"));

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton2[0]));

data.radiobutton2[1]=gtk_radio_button_new_with_label(group,gettext("Только приходные документы"));
if(data.rk->metka_pr == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[1]),TRUE); //Устанавливем активной кнопку
g_signal_connect(data.radiobutton2[1], "clicked",G_CALLBACK(rasspdokuw_radio12),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton2[1]));

data.radiobutton2[2]=gtk_radio_button_new_with_label(group,gettext("Только расходные документы"));
if(data.rk->metka_pr == 2)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[2]),TRUE); //Устанавливем активной кнопку
g_signal_connect(data.radiobutton2[2], "clicked",G_CALLBACK(rasspdokuw_radio22),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[2], TRUE, TRUE, 0);

GtkWidget *separator4=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator4);


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(rasspdokuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(rasspdokuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(rasspdokuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(rasspdokuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));


sprintf(strsql,"%s (,,)",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODOP],"clicked",G_CALLBACK(rasspdokuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODOP],iceb_u_inttochar(E_KODOP));

data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(rasspdokuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(rasspdokuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(rasspdokuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk->kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));

sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(rasspdokuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(rasspdokuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));

sprintf(strsql,"%s (,,)",gettext("Код группы контрагента"));
data.knopka_enter[E_GRUPK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPK]), data.knopka_enter[E_GRUPK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_GRUPK],"clicked",G_CALLBACK(rasspdokuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_GRUPK],iceb_u_inttochar(E_GRUPK));

data.entry[E_GRUPK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPK]), data.entry[E_GRUPK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GRUPK], "activate",G_CALLBACK(rasspdokuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPK]),data.rk->grupk.ravno());
gtk_widget_set_name(data.entry[E_GRUPK],iceb_u_inttochar(E_GRUPK));




sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(rasspdokuw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

/*********************
sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Просмотр нужных реквизитов"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(rasspdokuw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);
**********************/

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(rasspdokuw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(rasspdokuw_v_knopka),&data);
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
void  rasspdokuw_v_e_knopka(GtkWidget *widget,class rasspdokuw_data *data)
{
class iceb_u_str kod;
class iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rk->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rk->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
      
    return;  

  case E_KODOP:
    vib_kodop_usl(&data->rk->kodop,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk->kodop.ravno());
    return;  

  case E_KONTR:
    iceb_vibrek(0,"Kontragent",&data->rk->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk->kontr.ravno());
    return;

  case E_PODR:
    if(l_uslpodr(1,&kod,&naim,data->window) == 0)
      data->rk->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
    return;
  case E_GRUPK:
    if(iceb_l_gkontr(1,&kod,&naim,data->window) == 0)
      data->rk->grupk.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPK]),data->rk->grupk.ravno());
    return;
 }
}
/******************************/
/*Обработчик нажатия радиокнопок сортировки*/
/**********************************/

void  rasspdokuw_radio0(GtkWidget *widget,class rasspdokuw_data *data)
{
//g_print("rasspdokuw_radio0\n");

if(data->rk->metka_sort == 0)
  return;
data->rk->metka_sort=0;

}
void  rasspdokuw_radio1(GtkWidget *widget,class rasspdokuw_data *data)
{
//g_print("rasspdokuw_radio1\n");
if(data->rk->metka_sort == 1)
  return;
data->rk->metka_sort=1;
}
/******************************/
/*Обработчик нажатия радиокнопок видов документов подтверждённых-неподтверждённых*/
/**********************************/

void  rasspdokuw_radio01(GtkWidget *widget,class rasspdokuw_data *data)
{
//g_print("rasspdokuw_radio0\n");

if(data->rk->metka_dok == 0)
  return;
data->rk->metka_dok=0;

}
void  rasspdokuw_radio11(GtkWidget *widget,class rasspdokuw_data *data)
{
//g_print("rasspdokuw_radio1\n");
if(data->rk->metka_dok == 1)
  return;
data->rk->metka_dok=1;
}
void  rasspdokuw_radio21(GtkWidget *widget,class rasspdokuw_data *data)
{
//g_print("rasspdokuw_radio1\n");
if(data->rk->metka_dok == 2)
  return;
data->rk->metka_dok=2;
}

/******************************/
/*Обработчик нажатия радиокнопок приходов/расходов*/
/**********************************/

void  rasspdokuw_radio02(GtkWidget *widget,class rasspdokuw_data *data)
{
//g_print("rasspdokuw_radio0\n");

if(data->rk->metka_pr == 0)
  return;
data->rk->metka_pr=0;

}
void  rasspdokuw_radio12(GtkWidget *widget,class rasspdokuw_data *data)
{
//g_print("rasspdokuw_radio1\n");
if(data->rk->metka_pr == 1)
  return;
data->rk->metka_pr=1;
}
void  rasspdokuw_radio22(GtkWidget *widget,class rasspdokuw_data *data)
{
//g_print("rasspdokuw_radio1\n");
if(data->rk->metka_pr == 2)
  return;
data->rk->metka_pr=2;
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rasspdokuw_v_key_press(GtkWidget *widget,GdkEventKey *event,class rasspdokuw_data *data)
{

//printf("rasspdokuw_v_key_press\n");
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
void  rasspdokuw_v_knopka(GtkWidget *widget,class rasspdokuw_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
//    if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
    if(iceb_rsdatp_str(&data->rk->datan,&data->rk->datak,data->window) != 0)
      return;

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

void    rasspdokuw_v_vvod(GtkWidget *widget,class rasspdokuw_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("rasspdokuw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODOP:
    data->rk->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KONTR:
    data->rk->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PODR:
    data->rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_GRUPK:
    data->rk->grupk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
