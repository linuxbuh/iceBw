/*$Id: opss_uw_m.c,v 1.17 2013/09/26 09:46:52 sasa Exp $*/
/*12.07.2015	24.12.2005	Белых А.И.	opss_uw_m.c
Меню для ввода реквизитов получения отчёта по счетам списания услуг
*/
#include "buhg_g.h"
#include "opss_uw.h"
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
  E_PODR,
  E_KODGR,
  E_KODZAP,
  E_SHET_SP,
  E_SHET_UH,
  E_KODOP,
  E_KONTR,
  KOLENTER  
 };

class opss_uw_data
 {
  public:
  opss_uw_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  opss_uw_data() //Конструктор
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



gboolean   opss_uw_v_key_press(GtkWidget *widget,GdkEventKey *event,class opss_uw_data *data);
void    opss_uw_v_vvod(GtkWidget *widget,class opss_uw_data *data);
void  opss_uw_v_knopka(GtkWidget *widget,class opss_uw_data *data);
void  opss_uw_v_e_knopka(GtkWidget *widget,class opss_uw_data *data);

extern SQL_baza bd;

int   opss_uw_m(class opss_uw_rr *rek_ras)
{
opss_uw_data data;

data.rk=rek_ras;

//if(data.rk->datan.getdlinna() == 0)
//  data.rk->clear_data();
  
char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.rk->metka == 1)
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать движение услуг по счетам получения"));

if(data.rk->metka == 2)
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать движение услуг по счетам списания"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(opss_uw_v_key_press),&data);

GtkWidget *label=NULL;

if(data.rk->metka == 1)
  label=gtk_label_new(gettext("Распечатать движение услуг по счетам получения"));

if(data.rk->metka == 2)
  label=gtk_label_new(gettext("Распечатать движение услуг по счетам списания"));

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


sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(opss_uw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(opss_uw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(opss_uw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(opss_uw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));


sprintf(strsql,"%s (,,)",gettext("Склад"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(opss_uw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор склада"));

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(opss_uw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));

sprintf(strsql,"%s (,,)",gettext("Код группы услуги"));
data.knopka_enter[E_KODGR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.knopka_enter[E_KODGR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODGR],"clicked",G_CALLBACK(opss_uw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODGR],iceb_u_inttochar(E_KODGR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODGR],gettext("Выбор группы"));

data.entry[E_KODGR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.entry[E_KODGR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODGR], "activate",G_CALLBACK(opss_uw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODGR]),data.rk->kodgr.ravno());
gtk_widget_set_name(data.entry[E_KODGR],iceb_u_inttochar(E_KODGR));

sprintf(strsql,"%s (,,)",gettext("Код услуги"));
data.knopka_enter[E_KODZAP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODZAP]), data.knopka_enter[E_KODZAP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODZAP],"clicked",G_CALLBACK(opss_uw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODZAP],iceb_u_inttochar(E_KODZAP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODZAP],gettext("Выбор материалла"));

data.entry[E_KODZAP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODZAP]), data.entry[E_KODZAP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODZAP], "activate",G_CALLBACK(opss_uw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODZAP]),data.rk->kodzap.ravno());
gtk_widget_set_name(data.entry[E_KODZAP],iceb_u_inttochar(E_KODZAP));

sprintf(strsql,"%s (,,)",gettext("Счёт учёта"));
data.knopka_enter[E_SHET_UH]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_UH]), data.knopka_enter[E_SHET_UH], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET_UH],"clicked",G_CALLBACK(opss_uw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET_UH],iceb_u_inttochar(E_SHET_UH));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET_UH],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET_UH] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_UH]), data.entry[E_SHET_UH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET_UH], "activate",G_CALLBACK(opss_uw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_UH]),data.rk->shet_uh.ravno());
gtk_widget_set_name(data.entry[E_SHET_UH],iceb_u_inttochar(E_SHET_UH));

sprintf(strsql,"%s (,,)",gettext("Счёт списания"));
data.knopka_enter[E_SHET_SP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_SP]), data.knopka_enter[E_SHET_SP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET_SP],"clicked",G_CALLBACK(opss_uw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET_SP],iceb_u_inttochar(E_SHET_SP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET_SP],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET_SP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_SP]), data.entry[E_SHET_SP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET_SP], "activate",G_CALLBACK(opss_uw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_SP]),data.rk->shet_sp.ravno());
gtk_widget_set_name(data.entry[E_SHET_SP],iceb_u_inttochar(E_SHET_SP));

sprintf(strsql,"%s (,,)",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODOP],"clicked",G_CALLBACK(opss_uw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODOP],iceb_u_inttochar(E_KODOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODOP],gettext("Выбор операции"));

data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(opss_uw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(opss_uw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR],gettext("Выбор контрагента"));

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(opss_uw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk->kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));




sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(opss_uw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(opss_uw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(opss_uw_v_knopka),&data);
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
void  opss_uw_v_e_knopka(GtkWidget *widget,class opss_uw_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
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



  case E_PODR:
    iceb_vibrek(0,"Sklad",&data->rk->podr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
    return;

  case E_KODOP:
    //iceb_vibrek(0,"Rashod",&data->rk->kodop,data->window);
    if(data->rk->metka == 1)
     if(l_uslopp(1,&kod,&naim,data->window) == 0)
      data->rk->kodop.z_plus(kod.ravno());

    if(data->rk->metka == 2)
     if(l_uslopr(1,&kod,&naim,data->window) == 0)
      data->rk->kodop.z_plus(kod.ravno());
     
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk->kodop.ravno());
    return;

  case E_KONTR:
    iceb_vibrek(0,"Kontragent",&data->rk->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk->kontr.ravno());
    return;

  case E_SHET_UH:
    iceb_vibrek(0,"Plansh",&data->rk->shet_uh,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_UH]),data->rk->shet_uh.ravno());
    return;

  case E_SHET_SP:
    iceb_vibrek(0,"Plansh",&data->rk->shet_sp,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_SP]),data->rk->shet_sp.ravno());
    return;


  case E_KODGR:
//    iceb_vibrek(0,"Grup",&data->rk->kodgr,data->window);
    if(l_uslgrup(1,&kod,&naim,data->window) == 0)
     data->rk->kodgr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGR]),data->rk->kodgr.ravno());
    return;

  case E_KODZAP:
    if(l_uslugi(1,&kod,&naim,data->window) == 0)
      data->rk->kodzap.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODZAP]),data->rk->kodzap.ravno());
    return;
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   opss_uw_v_key_press(GtkWidget *widget,GdkEventKey *event,class opss_uw_data *data)
{

//printf("opss_uw_v_key_press\n");
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
void  opss_uw_v_knopka(GtkWidget *widget,class opss_uw_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
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

void    opss_uw_v_vvod(GtkWidget *widget,class opss_uw_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("opss_uw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PODR:
    data->rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_KODGR:
    data->rk->kodgr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODZAP:
    data->rk->kodzap.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET_UH:
    data->rk->shet_uh.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET_SP:
    data->rk->shet_sp.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODOP:
    data->rk->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KONTR:
    data->rk->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
