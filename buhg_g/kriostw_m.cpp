/*$Id: kriostw_m.c,v 1.19 2013/09/26 09:46:49 sasa Exp $*/
/*11.07.2015	06.05.2005	Белых А.И.	kriostw_m.c
Меню для ввода реквизитов расчёта критических остатков по материаллам
*/
#include "buhg_g.h"
#include "kriostw.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SKLAD,
  E_KODGRMAT,
  E_KODMAT,
  E_SHET,
  E_DATAO,
  KOLENTER
 };

class kriostw_data
 {
  public:
  kriostw_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  kriostw_data() //Конструктор
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



gboolean   kriostw_v_key_press(GtkWidget *widget,GdkEventKey *event,class kriostw_data *data);
void    kriostw_v_vvod(GtkWidget *widget,class kriostw_data *data);
void  kriostw_v_knopka(GtkWidget *widget,class kriostw_data *data);
void  kriostw_v_e_knopka(GtkWidget *widget,class kriostw_data *data);

extern SQL_baza bd;

int   kriostw_m(class kriostw_rr *rek_ras)
{
kriostw_data data;

data.rk=rek_ras;

if(data.rk->datao.getdlinna() <= 1)
 {
  short d,m,g;
  iceb_u_poltekdat(&d,&m,&g);
  data.rk->datao.plus(d);
  data.rk->datao.plus(".");
  data.rk->datao.plus(m);
  data.rk->datao.plus(".");
  data.rk->datao.plus(g);
 }

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать материалы имеющие критический остаток"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(kriostw_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Распечатать материалы имеющие критический остаток"));

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


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);



sprintf(strsql,"%s (,,)",gettext("Склад"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SKLAD],"clicked",G_CALLBACK(kriostw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SKLAD],iceb_u_inttochar(E_SKLAD));

data.entry[E_SKLAD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SKLAD], "activate",G_CALLBACK(kriostw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk->sklad.ravno());
gtk_widget_set_name(data.entry[E_SKLAD],iceb_u_inttochar(E_SKLAD));

sprintf(strsql,"%s (,,)",gettext("Код группы материалла"));
data.knopka_enter[E_KODGRMAT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODGRMAT]), data.knopka_enter[E_KODGRMAT], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODGRMAT],"clicked",G_CALLBACK(kriostw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODGRMAT],iceb_u_inttochar(E_KODGRMAT));

data.entry[E_KODGRMAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODGRMAT]), data.entry[E_KODGRMAT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODGRMAT], "activate",G_CALLBACK(kriostw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODGRMAT]),data.rk->kodgrmat.ravno());
gtk_widget_set_name(data.entry[E_KODGRMAT],iceb_u_inttochar(E_KODGRMAT));

sprintf(strsql,"%s (,,)",gettext("Код материалла"));
data.knopka_enter[E_KODMAT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.knopka_enter[E_KODMAT], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODMAT],"clicked",G_CALLBACK(kriostw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODMAT],iceb_u_inttochar(E_KODMAT));

data.entry[E_KODMAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.entry[E_KODMAT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODMAT], "activate",G_CALLBACK(kriostw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODMAT]),data.rk->kodmat.ravno());
gtk_widget_set_name(data.entry[E_KODMAT],iceb_u_inttochar(E_KODMAT));

sprintf(strsql,"%s (,,)",gettext("Счёт учёта"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(kriostw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(kriostw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));


sprintf(strsql,"%s",gettext("Дата остатка"));
data.knopka_enter[E_DATAO]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAO]), data.knopka_enter[E_DATAO], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAO],"clicked",G_CALLBACK(kriostw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAO],iceb_u_inttochar(E_DATAO));

data.entry[E_DATAO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAO]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAO]), data.entry[E_DATAO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAO], "activate",G_CALLBACK(kriostw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAO]),data.rk->datao.ravno());
gtk_widget_set_name(data.entry[E_DATAO],iceb_u_inttochar(E_DATAO));




sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(kriostw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(kriostw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(kriostw_v_knopka),&data);
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
void  kriostw_v_e_knopka(GtkWidget *widget,class kriostw_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATAO:

    if(iceb_calendar(&data->rk->datao,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAO]),data->rk->datao.ravno());
      
    return;  



  case E_SKLAD:
    iceb_vibrek(0,"Sklad",&data->rk->sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno());
    return;

  case E_KODGRMAT:
    iceb_vibrek(0,"Grup",&data->rk->kodgrmat,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGRMAT]),data->rk->kodgrmat.ravno());
    return;

  case E_SHET:
    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno());
    return;

  case E_KODMAT:
     if( l_mater(1,&kod,&naim,0,0,data->window) == 0)
       data->rk->kodmat.z_plus(kod.ravno());
//    iceb_vibrek(0,"Grup",&data->rk->kodmat,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),data->rk->kodmat.ravno());
    return;


 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kriostw_v_key_press(GtkWidget *widget,GdkEventKey *event,class kriostw_data *data)
{

//printf("kriostw_v_key_press\n");
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
void  kriostw_v_knopka(GtkWidget *widget,class kriostw_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->rk->datao.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата остатка !"),data->window);
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

void    kriostw_v_vvod(GtkWidget *widget,class kriostw_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("kriostw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAO:
    data->rk->datao.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SKLAD:
    data->rk->sklad.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODGRMAT:
    data->rk->kodgrmat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODMAT:
    data->rk->kodmat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET:
    data->rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;



 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
