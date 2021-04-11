/*$Id: zar_spisr_m.c,v 1.16 2013/08/25 08:27:01 sasa Exp $*/
/*13.07.2015	21.11.2006	Белых А.И.	zar_spisr_m.c
Меню для расчёта свода отработанного времени по категориям
*/
#include "buhg_g.h"
#include "zar_spisr.h"
enum
 {
  E_PODR,
  E_TABNOM,
  E_KOD_ZVAN,
  E_KOD_KATEG,
  E_SHETU,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class zar_spisr_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton1[2];
  GtkWidget *radiobutton2[3];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class zar_spisr_rek *rk;
  
  zar_spisr_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODR])));
    rk->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM])));
    rk->kod_zvan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_ZVAN])));
    rk->kod_kateg.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_KATEG])));
    rk->shetu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHETU])));
    
   }
  
  void clear_rek()
   {

    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton1[0]),TRUE); //Устанавливем активной кнопку
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton2[0]),TRUE); //Устанавливем активной кнопку

   }
 };


gboolean   zar_spisr_m_key_press(GtkWidget *widget,GdkEventKey *event,class zar_spisr_m_data *data);
void  zar_spisr_m_knopka(GtkWidget *widget,class zar_spisr_m_data *data);
void    zar_spisr_m_vvod(GtkWidget *widget,class zar_spisr_m_data *data);


void  zar_spisr_m_e_knopka(GtkWidget *widget,class zar_spisr_m_data *data);

void  spisr_radio1_0(GtkWidget *widget,class zar_spisr_m_data*);
void  spisr_radio1_1(GtkWidget *widget,class zar_spisr_m_data*);

void  spisr_radio2_0(GtkWidget *widget,class zar_spisr_m_data*);
void  spisr_radio2_1(GtkWidget *widget,class zar_spisr_m_data*);
void  spisr_radio2_2(GtkWidget *widget,class zar_spisr_m_data*);

extern SQL_baza  bd;

int zar_spisr_m(class zar_spisr_rek *rek,GtkWidget *wpredok)
{

class zar_spisr_m_data data;
char strsql[512];
iceb_u_str kikz;
data.rk=rek;
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;


sprintf(strsql,"%s\n%s",gettext("Распечатать список работников"),
gettext("Если код подразделения не ввести, то получите список по всем подразделениям"));

label=gtk_label_new(strsql);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать список работников"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_spisr_m_key_press),&data);

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
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
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

data.radiobutton2[0]=gtk_radio_button_new_with_label(NULL,gettext("Список всех работающих"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton2[0], "clicked",G_CALLBACK(spisr_radio2_0),&data);
//gtk_widget_set_name(data.radiobutton0,iceb_u_inttochar("0"));

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton2[0]));

data.radiobutton2[1]=gtk_radio_button_new_with_label(group,gettext("Список всех уволенных"));
g_signal_connect(data.radiobutton2[1], "clicked",G_CALLBACK(spisr_radio2_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton2[1]));

data.radiobutton2[2]=gtk_radio_button_new_with_label(group,gettext("Список всех совместителей"));
g_signal_connect(data.radiobutton2[2], "clicked",G_CALLBACK(spisr_radio2_2),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[2], TRUE, TRUE, 0);


gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk->metka_ras]),TRUE); //Устанавливем активной кнопку

gtk_container_add (GTK_CONTAINER (vbox), separator3);

GtkWidget *separator4=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator5=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator4);

GSList *group2;

data.radiobutton1[0]=gtk_radio_button_new_with_label(NULL,gettext("Сортировка по табельным номерам"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton1[0], "clicked",G_CALLBACK(spisr_radio1_0),&data);
//gtk_widget_set_name(data.radiobutton0,iceb_u_inttochar("0"));

group2=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton1[0]));

data.radiobutton1[1]=gtk_radio_button_new_with_label(group2,gettext("Сортировка по фамилиям"));
g_signal_connect(data.radiobutton1[1], "clicked",G_CALLBACK(spisr_radio1_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[1], TRUE, TRUE, 0);


gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[data.rk->metka_sort]),TRUE); //Устанавливем активной кнопку

gtk_container_add (GTK_CONTAINER (vbox), separator5);





gtk_container_add (GTK_CONTAINER (vbox), hboxknop);



sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(zar_spisr_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор подразделения"));

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(zar_spisr_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));


sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_TABNOM],"clicked",G_CALLBACK(zar_spisr_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_TABNOM],iceb_u_inttochar(E_TABNOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"));

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABNOM], "activate",G_CALLBACK(zar_spisr_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.rk->podr.ravno());
gtk_widget_set_name(data.entry[E_TABNOM],iceb_u_inttochar(E_TABNOM));


sprintf(strsql,"%s (,,)",gettext("Код звания"));
data.knopka_enter[E_KOD_ZVAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_ZVAN]), data.knopka_enter[E_KOD_ZVAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_ZVAN],"clicked",G_CALLBACK(zar_spisr_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_ZVAN],iceb_u_inttochar(E_KOD_ZVAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_ZVAN],gettext("Выбор звания"));

data.entry[E_KOD_ZVAN] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_ZVAN]), data.entry[E_KOD_ZVAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_ZVAN], "activate",G_CALLBACK(zar_spisr_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_ZVAN]),data.rk->kod_zvan.ravno());
gtk_widget_set_name(data.entry[E_KOD_ZVAN],iceb_u_inttochar(E_KOD_ZVAN));

sprintf(strsql,"%s (,,)",gettext("Код категории"));
data.knopka_enter[E_KOD_KATEG]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KATEG]), data.knopka_enter[E_KOD_KATEG], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_KATEG],"clicked",G_CALLBACK(zar_spisr_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_KATEG],iceb_u_inttochar(E_KOD_KATEG));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_KATEG],gettext("Выбор категории"));

data.entry[E_KOD_KATEG] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KATEG]), data.entry[E_KOD_KATEG], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_KATEG], "activate",G_CALLBACK(zar_spisr_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_KATEG]),data.rk->kod_kateg.ravno());
gtk_widget_set_name(data.entry[E_KOD_KATEG],iceb_u_inttochar(E_KOD_KATEG));


sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHETU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHETU],"clicked",G_CALLBACK(zar_spisr_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETU],iceb_u_inttochar(E_SHETU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETU],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHETU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHETU], "activate",G_CALLBACK(zar_spisr_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.rk->shetu.ravno());
gtk_widget_set_name(data.entry[E_SHETU],iceb_u_inttochar(E_SHETU));





sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(zar_spisr_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(zar_spisr_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(zar_spisr_m_knopka),&data);
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

void  spisr_radio1_0(GtkWidget *widget,class zar_spisr_m_data *data)
{
//printf("1-1\n");
data->rk->metka_sort=0;
}
void  spisr_radio1_1(GtkWidget *widget,class zar_spisr_m_data *data)
{
//printf("1-2\n");
data->rk->metka_sort=1;
}

void  spisr_radio2_0(GtkWidget *widget,class zar_spisr_m_data *data)
{
//printf("2-1\n");
data->rk->metka_ras=0;
}
void  spisr_radio2_1(GtkWidget *widget,class zar_spisr_m_data *data)
{
//printf("2-2\n");
data->rk->metka_ras=1;
}
void  spisr_radio2_2(GtkWidget *widget,class zar_spisr_m_data *data)
{
//printf("2-3\n");
data->rk->metka_ras=2;
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  zar_spisr_m_e_knopka(GtkWidget *widget,class zar_spisr_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  
  case E_PODR:
    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     data->rk->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
    return;

  case E_KOD_KATEG:
    if(l_zarkateg(1,&kod,&naim,data->window) == 0)
     data->rk->kod_kateg.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_KATEG]),data->rk->kod_kateg.ravno());
    return;

  case E_KOD_ZVAN:
    if(l_zarzvan(1,&kod,&naim,data->window) == 0)
     data->rk->kod_zvan.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_ZVAN]),data->rk->kod_zvan.ravno());
    return;

  case E_SHETU:
    if(iceb_l_plansh(1,&kod,&naim,data->window) == 0)
     data->rk->shetu.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->rk->shetu.ravno());
    return;

  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->rk->tabnom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->rk->tabnom.ravno());
    return;
  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_spisr_m_key_press(GtkWidget *widget,GdkEventKey *event,class zar_spisr_m_data *data)
{
//char  bros[512];

//printf("zar_spisr_m_key_press\n");
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
void  zar_spisr_m_knopka(GtkWidget *widget,class zar_spisr_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("zar_spisr_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:


    data->read_rek();
    printf("metka_ras=%d metka_sort=%d\n",data->rk->metka_ras,data->rk->metka_sort);
    data->voz=0;
    gtk_widget_destroy(data->window);
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

void    zar_spisr_m_vvod(GtkWidget *widget,class zar_spisr_m_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("zar_spisr_m_vvod enter=%d\n",enter);

switch (enter)
 {
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
