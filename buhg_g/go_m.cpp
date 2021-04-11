/*$Id: go_m.c,v 1.38 2013/09/26 09:46:48 sasa Exp $*/
/*10.07.2015	18.03.2004	Белых А.И.	go_m.c
Расчёт журналов ордеров
*/
#include "buhg_g.h"
#include "go.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SHET,
  E_DATAN,
  E_DATAK,
//  E_KODGR,
  E_KOD_VAL,
  KOLENTER  
 };

class go_data
 {
  public:
  go_rr  *rek_r;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет
  
  go_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rek_r->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rek_r->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rek_r->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
//    rek_r->kodgr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODGR])));
    rek_r->kod_val.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_VAL])));
    

//    for(int i=0; i < KOLENTER; i++)
//      g_signal_emit_by_name(entry[i],"activate");
    rek_r->metka_r=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rek_r->clear_data();
   }
 };

int   go_m_mkz(class go_rr *rek_ras,GtkWidget *wpredok);


gboolean   go_v_key_press(GtkWidget *widget,GdkEventKey *event,class go_data *data);
void    go_v_vvod(GtkWidget *widget,class go_data *data);
void  go_v_knopka(GtkWidget *widget,class go_data *data);
void  go_v_e_knopka(GtkWidget *widget,class go_data *data);

extern SQL_baza bd;

int   go_m(class go_rr *rek_ras)
{
class go_data data;

data.rek_r=rek_ras;

if(data.rek_r->datan.getdlinna() == 0)
  data.rek_r->clear_data();
  
char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Журнал ордер"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(go_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Расчёт журнала-ордера по счёту"));

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxradio = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxradio),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

class iceb_u_spisok spmenu;
spmenu.plus(gettext("Расчёт по субсчетам"));
spmenu.plus(gettext("Расчёт по счетам"));
iceb_pm_vibor(&spmenu,&data.opt,data.rek_r->metka_r);
gtk_box_pack_start (GTK_BOX (hboxradio),data.opt, TRUE, TRUE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxradio);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


data.knopka_enter[E_SHET]=gtk_button_new_with_label(gettext("Счёт"));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(go_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(go_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rek_r->shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(go_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"));


data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(go_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rek_r->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(go_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(go_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rek_r->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));

/*****************
sprintf(strsql,"%s (,,)",gettext("Код группы контрагента"));
data.knopka_enter[E_KODGR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.knopka_enter[E_KODGR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODGR],"clicked",G_CALLBACK(go_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODGR],iceb_u_inttochar(E_KODGR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODGR],gettext("Выбор группы контрагента"));

data.entry[E_KODGR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.entry[E_KODGR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODGR], "activate",G_CALLBACK(go_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODGR]),data.rek_r->kodgr.ravno());
gtk_widget_set_name(data.entry[E_KODGR],iceb_u_inttochar(E_KODGR));
*******************/


data.knopka_enter[E_KOD_VAL]=gtk_button_new_with_label(gettext("Код валюты"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VAL]), data.knopka_enter[E_KOD_VAL], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_VAL],"clicked",G_CALLBACK(go_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_VAL],iceb_u_inttochar(E_KOD_VAL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_VAL],gettext("Выбор кода валюты"));

data.entry[E_KOD_VAL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VAL]), data.entry[E_KOD_VAL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_VAL], "activate",G_CALLBACK(go_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_VAL]),data.rek_r->kod_val.ravno());
gtk_widget_set_name(data.entry[E_KOD_VAL],iceb_u_inttochar(E_KOD_VAL));



sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(go_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(go_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(go_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0 && data.rek_r->metka_r == 1) 
 {
  data.voz=go_m_mkz(rek_ras,NULL);
 }
return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  go_v_e_knopka(GtkWidget *widget,class go_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("go_v_e_knopka knop=%d\n",knop);
class iceb_u_str naim("");
switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rek_r->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rek_r->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rek_r->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rek_r->datak.ravno());
      
    return;  
   
  case E_SHET:

    iceb_vibrek(1,"Plansh",&data->rek_r->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rek_r->shet.ravno());

    return;  
/*****************
  case E_KODGR:
    iceb_vibrek(0,"Gkont",&data->rek_r->kodgr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGR]),data->rek_r->kodgr.ravno());
    return;
*****************/
  case E_KOD_VAL:
    if(l_sval(1,&data->rek_r->kod_val,&naim,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VAL]),data->rek_r->kod_val.ravno());
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   go_v_key_press(GtkWidget *widget,GdkEventKey *event,class go_data *data)
{

//printf("go_v_key_press\n");
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
void  go_v_knopka(GtkWidget *widget,class go_data *data)
{
short dn,mn,gn;
short dk,mk,gk;
char  strsql[1024];
SQLCURSOR cur;
SQL_str   row;
iceb_u_str kod;
iceb_u_str naikod;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(iceb_rsdatp(&dn,&mn,&gn,data->rek_r->datan.ravno(),&dk,&mk,&gk,data->rek_r->datak.ravno(),data->window) != 0)
      return;

    if(data->rek_r->shet.getdlinna() <= 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не введён счёт !"));
      iceb_menu_soob(&repl,data->window);
      return;
     }
    if(data->rek_r->kod_val.getdlinna() > 1)
     {
      sprintf(strsql,"select kod from Glksval where kod='%s'",data->rek_r->kod_val.ravno());

      if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
       {
        sprintf(strsql,"%s %s!",gettext("В справочкике валют не найден код валюты"),data->rek_r->kod_val.ravno());
        iceb_menu_soob(strsql,data->window);
        return;
       }
     }             

    sprintf(strsql,"select nais,vids,saldo,stat from Plansh where ns='%s'",data->rek_r->shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      sprintf(strsql,gettext("Нет счета %s в плане счетов !"),data->rek_r->shet.ravno());
      iceb_menu_soob(strsql,data->window);
      return;
      
     }

    data->rek_r->naimshet.new_plus(row[0]);            
    data->rek_r->vds=atoi(row[1]);
    data->rek_r->saldo=atoi(row[2]);
    data->rek_r->stat=atoi(row[3]);
        
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

void    go_v_vvod(GtkWidget *widget,class go_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("go_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rek_r->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rek_r->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET:
    data->rek_r->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
/*************
  case E_KODGR:
    data->rek_r->kodgr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
**************/
 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**********************************/
/*ввод максимального количества знаков*/
/****************************************/

int   go_m_mkz(class go_rr *rek_ras,GtkWidget *wpredok)
{

class iceb_u_str titl(gettext("Введите максимальное количество знаков в номере счёта"));
class iceb_u_str mkz("");

if(iceb_menu_vvod1(&titl,&mkz,8,"",wpredok) != 0)
 return(1);

rek_ras->kolih_znak=mkz.ravno_atoi();
if(rek_ras->kolih_znak <= 0)
 {
  iceb_menu_soob(gettext("Количество знаков не может равнятся нолю!"),wpredok);
  return(1);
 }

return(0);
}

