/*$Id: uosvamotbuw_m.c,v 1.14 2013/09/26 09:46:57 sasa Exp $*/
/*23.11.2016	28.12.2007	Белых А.И.	uosvamotbuw_m.c
Меню для ввода реквизитов
*/

#include "buhg_g.h"
#include "uosvamotbuw.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_MESN,
  E_MESK,
  E_GOD,
  E_GRUPA,
  E_PODR,
  E_MAT_OT,
  E_SHETU,
  E_HZT,
  E_INNOM,
  E_HAU,
  KOLENTER  
 };

class uosvamotbuw_m_data
 {
  public:
  class uosvamotbuw_data *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt1;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет
  GtkWidget *radiobutton[2];

  uosvamotbuw_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->mesn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MESN])));
    rk->mesk.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MESK])));
    rk->god.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GOD])));
    rk->grupa.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GRUPA])));
    rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODR])));
    rk->mat_ot.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MAT_OT])));
    rk->shetu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHETU])));
    rk->hzt.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_HZT])));
    rk->innom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_INNOM])));
    rk->hau.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_HAU])));

    rk->sost_ob=gtk_combo_box_get_active (GTK_COMBO_BOX(opt1));

/***********
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
***************/
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rk->clear();
   }
 };


gboolean   uosvamotbuw_v_key_press(GtkWidget *widget,GdkEventKey *event,class uosvamotbuw_m_data *data);
void    uosvamotbuw_v_vvod(GtkWidget *widget,class uosvamotbuw_m_data *data);
void  uosvamotbuw_v_knopka(GtkWidget *widget,class uosvamotbuw_m_data *data);
void  uosvamotbuw_v_e_knopka(GtkWidget *widget,class uosvamotbuw_m_data *data);

int uosvamotbuw_m_provr(class uosvamotbuw_m_data *data);

extern SQL_baza bd;

int uosvamotbuw_m(class uosvamotbuw_data *rek_ras)
{
char strsql[512];


class uosvamotbuw_m_data data;

data.rk=rek_ras;

if(data.rk->god.getdlinna() <= 1)
 {
  short d=0,m=0,g=0;
  iceb_u_poltekdat(&d,&m,&g);
  data.rk->god.new_plus(g);
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка ведомости аморт-отчислений для бух. учёта"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uosvamotbuw_v_key_press),&data);

GtkWidget *label=NULL;
label=gtk_label_new(gettext("Распечатка ведомости аморт-отчислений для бух. учёта"));

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxopt1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxopt1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE,1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE,1);

gtk_box_pack_start (GTK_BOX (vbox), hboxopt1, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE,1);


sprintf(strsql,"%s (м)",gettext("Месяц начала"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_MESN]), label, FALSE, FALSE,1);

data.entry[E_MESN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MESN]),2);
gtk_box_pack_start (GTK_BOX (hbox[E_MESN]), data.entry[E_MESN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_MESN], "activate",G_CALLBACK(uosvamotbuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MESN]),data.rk->mesn.ravno());
gtk_widget_set_name(data.entry[E_MESN],iceb_u_inttochar(E_MESN));


sprintf(strsql,"%s (м)",gettext("Месяц конца"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_MESK]), label, FALSE, FALSE,1);

data.entry[E_MESK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MESK]),2);
gtk_box_pack_start (GTK_BOX (hbox[E_MESK]), data.entry[E_MESK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_MESK], "activate",G_CALLBACK(uosvamotbuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MESK]),data.rk->mesk.ravno());
gtk_widget_set_name(data.entry[E_MESK],iceb_u_inttochar(E_MESK));

sprintf(strsql,"%s",gettext("Год"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE,1);

data.entry[E_GOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GOD]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_GOD], "activate",G_CALLBACK(uosvamotbuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.rk->god.ravno());
gtk_widget_set_name(data.entry[E_GOD],iceb_u_inttochar(E_GOD));



sprintf(strsql,"%s (,,)",gettext("Код группы"));
data.knopka_enter[E_GRUPA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.knopka_enter[E_GRUPA], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_GRUPA],"clicked",G_CALLBACK(uosvamotbuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_GRUPA],iceb_u_inttochar(E_GRUPA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_GRUPA],gettext("Выбор группы"));

data.entry[E_GRUPA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.entry[E_GRUPA], TRUE, TRUE,1);
g_signal_connect(data.entry[E_GRUPA], "activate",G_CALLBACK(uosvamotbuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPA]),data.rk->grupa.ravno());
gtk_widget_set_name(data.entry[E_GRUPA],iceb_u_inttochar(E_GRUPA));


sprintf(strsql,"%s (,,)",gettext("Код мат.ответственного"));
data.knopka_enter[E_MAT_OT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.knopka_enter[E_MAT_OT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_MAT_OT],"clicked",G_CALLBACK(uosvamotbuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_MAT_OT],iceb_u_inttochar(E_MAT_OT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_MAT_OT],gettext("Выбор материально-ответственного"));

data.entry[E_MAT_OT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.entry[E_MAT_OT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_MAT_OT], "activate",G_CALLBACK(uosvamotbuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MAT_OT]),data.rk->mat_ot.ravno());
gtk_widget_set_name(data.entry[E_MAT_OT],iceb_u_inttochar(E_MAT_OT));


sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(uosvamotbuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор подразделения"));

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(uosvamotbuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));


sprintf(strsql,"%s (,,)",gettext("Счёт учёта"));
data.knopka_enter[E_SHETU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHETU],"clicked",G_CALLBACK(uosvamotbuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETU],iceb_u_inttochar(E_SHETU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETU],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHETU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SHETU], "activate",G_CALLBACK(uosvamotbuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.rk->shetu.ravno());
gtk_widget_set_name(data.entry[E_SHETU],iceb_u_inttochar(E_SHETU));


sprintf(strsql,"%s (,,)",gettext("Шифр затрат аморт-отчислений"));
data.knopka_enter[E_HZT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_HZT]), data.knopka_enter[E_HZT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_HZT],"clicked",G_CALLBACK(uosvamotbuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_HZT],iceb_u_inttochar(E_HZT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_HZT],gettext("Выбор шифра аналитического учёта"));

data.entry[E_HZT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HZT]), data.entry[E_HZT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_HZT], "activate",G_CALLBACK(uosvamotbuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HZT]),data.rk->hzt.ravno());
gtk_widget_set_name(data.entry[E_HZT],iceb_u_inttochar(E_HZT));


sprintf(strsql,"%s (,,)",gettext("Инвентарный номер"));
data.knopka_enter[E_INNOM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_INNOM]), data.knopka_enter[E_INNOM], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_INNOM],"clicked",G_CALLBACK(uosvamotbuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_INNOM],iceb_u_inttochar(E_INNOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_INNOM],gettext("Выбор инвентарного номера"));

data.entry[E_INNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_INNOM]), data.entry[E_INNOM], TRUE, TRUE,1);
g_signal_connect(data.entry[E_INNOM], "activate",G_CALLBACK(uosvamotbuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INNOM]),data.rk->innom.ravno());
gtk_widget_set_name(data.entry[E_INNOM],iceb_u_inttochar(E_INNOM));

sprintf(strsql,"%s (,,)",gettext("Шифр аналитического учёта"));
data.knopka_enter[E_HAU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_HAU]), data.knopka_enter[E_HAU], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_HAU],"clicked",G_CALLBACK(uosvamotbuw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_HAU],iceb_u_inttochar(E_HAU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_HAU],gettext("Выбор инвентарного номера"));

data.entry[E_HAU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HAU]), data.entry[E_HAU], TRUE, TRUE,1);
g_signal_connect(data.entry[E_HAU], "activate",G_CALLBACK(uosvamotbuw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HAU]),data.rk->hau.ravno());
gtk_widget_set_name(data.entry[E_HAU],iceb_u_inttochar(E_HAU));

label=gtk_label_new(gettext("Состояние объекта"));
gtk_box_pack_start (GTK_BOX (hboxopt1), label, FALSE, FALSE, 1);

class iceb_u_spisok bal_st;
bal_st.plus(gettext("Все основные средства"));
bal_st.plus(gettext("Основные средства, амортизация для которых считается"));
bal_st.plus(gettext("Основные средства, амортизация для которых не считается"));

iceb_pm_vibor(&bal_st,&data.opt1,data.rk->sost_ob);
gtk_box_pack_start (GTK_BOX (hboxopt1), data.opt1, TRUE, TRUE,1);





sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(uosvamotbuw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(uosvamotbuw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(uosvamotbuw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uosvamotbuw_v_e_knopka(GtkWidget *widget,class uosvamotbuw_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
/*g_print("uosvamotbuw_v_e_knopka knop=%d\n",knop);*/

switch (knop)
 {

  case E_INNOM:

    if(l_uosin(1,&kod,&naim,data->window) == 0)
      data->rk->innom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_INNOM]),data->rk->innom.ravno());
      
    return;  

  case E_HAU:

    if(l_uoshau(1,&kod,&naim,data->window) == 0)
      data->rk->hau.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_HAU]),data->rk->hau.ravno());
      
    return;  

  case E_GRUPA:

    if(l_uosgrbu(1,&kod,&naim,data->window) == 0)
      data->rk->grupa.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),data->rk->grupa.ravno());
      
    return;  


  case E_MAT_OT:

    if(l_uosmo(1,&kod,&naim,data->window) == 0)
      data->rk->mat_ot.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_MAT_OT]),data->rk->mat_ot.ravno());
      
    return;  

  case E_PODR:

    if(l_uospodr(1,&kod,&naim,data->window) == 0)
      data->rk->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
      
    return;  

  case E_SHETU:

    if(iceb_l_plansh(1,&kod,&naim,data->window) == 0)
      data->rk->shetu.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->rk->shetu.ravno());
      
    return;  

  case E_HZT:

    if(l_uoshz(1,&kod,&naim,data->window) == 0)
      data->rk->hzt.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_HZT]),data->rk->hzt.ravno());
      
    return;  


   

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosvamotbuw_v_key_press(GtkWidget *widget,GdkEventKey *event,class uosvamotbuw_m_data *data)
{

//printf("uosvamotbuw_v_key_press\n");
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
void  uosvamotbuw_v_knopka(GtkWidget *widget,class uosvamotbuw_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(uosvamotbuw_m_provr(data) != 0)
     return;
    
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    uosvamotbuw_v_vvod(GtkWidget *widget,class uosvamotbuw_m_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("uosvamotbuw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_MESN:
    data->rk->mesn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_MESK:
    data->rk->mesk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_GOD:
    data->rk->god.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_INNOM:
    data->rk->innom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GRUPA:
    data->rk->grupa.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_MAT_OT:
    data->rk->mat_ot.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PODR:
    data->rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHETU:
    data->rk->shetu.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_HZT:
    data->rk->hzt.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;



 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************************/
/*Проверка реквизитов*/
/*******************************/
int uosvamotbuw_m_provr(class uosvamotbuw_m_data *data)
{

if(data->rk->mesn.ravno_atoi() < 0 || data->rk->mesn.ravno_atoi() > 12)
 {
  iceb_menu_soob(gettext("Неправильно введён месяц начала!"),data->window);
  return(1);
 }

if(data->rk->mesk.ravno_atoi() < 0 || data->rk->mesk.ravno_atoi() > 12)
 {
  iceb_menu_soob(gettext("Неправильно введён месяц конца!"),data->window);
  return(1);
 }

if(data->rk->mesn.ravno_atoi() > data->rk->mesk.ravno_atoi())
 {
  iceb_menu_soob(gettext("Месяц начала больше месяца конца!"),data->window);
  return(1);
 }

return(0);
}


