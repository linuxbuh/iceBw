/*$Id: xdkbanksw_m.c,v 1.3 2013/09/26 09:46:59 sasa Exp $*/
/*13.07.2015	18.07.2013	Белых А.И.	xdkbanksw_m.c
Меню для получения отчёта 
*/
#include "buhg_g.h"
#include "xdkbanksw.h"

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
  E_DATAS,
  E_KODKONTR,
  E_KODGRKONTR,
  E_SHETK,
  E_DATAP,
  KOLENTER  
 };

class xdkbanksw_data
 {
  public:
  xdkbanksw_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt1;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  xdkbanksw_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    rk->datas.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAS])));
    rk->kodkontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODKONTR])));
    rk->kodgrkontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODGRKONTR])));
    rk->shetk.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHETK])));
    rk->datap.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAP])));
    rk->metka=gtk_combo_box_get_active (GTK_COMBO_BOX(opt1));

   }

  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }

 };




gboolean   xdkbanksw_v_key_press(GtkWidget *widget,GdkEventKey *event,class xdkbanksw_data *data);
void    xdkbanksw_v_vvod(GtkWidget *widget,class xdkbanksw_data *data);
void  xdkbanksw_v_knopka(GtkWidget *widget,class xdkbanksw_data *data);
void  xdkbanksw_v_e_knopka(GtkWidget *widget,class xdkbanksw_data *data);


extern SQL_baza bd;

int xdkbanksw_m(class xdkbanksw_rr *rek_ras,GtkWidget *wpredok)
{
xdkbanksw_data data;

data.rk=rek_ras;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Автоматическое выполнение проводок"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(xdkbanksw_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

class iceb_u_str repl;
repl.plus(gettext("Автоматическое выполнение проводок"));

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
GtkWidget *hboxopt1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxopt1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, FALSE, FALSE, 0);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], FALSE, FALSE, 0);

label=gtk_label_new(gettext("Сальдо"));
gtk_box_pack_start (GTK_BOX (hboxopt1), label, FALSE, FALSE, 1);

class iceb_u_spisok bal_st;
bal_st.plus(gettext("Кредитовое"));
bal_st.plus(gettext("Дебетовое"));

iceb_pm_vibor(&bal_st,&data.opt1,data.rk->metka);
gtk_box_pack_start (GTK_BOX (hboxopt1), data.opt1, TRUE, TRUE,1);


gtk_box_pack_start (GTK_BOX (vbox),hboxopt1, FALSE, FALSE, 0);

gtk_box_pack_start (GTK_BOX (vbox),hboxknop, FALSE, FALSE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(xdkbanksw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта"));

data.entry[E_SHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(xdkbanksw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

sprintf(strsql,"%s (%s)",gettext("Дата сальдо"),gettext("д.м.г"));
data.knopka_enter[E_DATAS]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAS]), data.knopka_enter[E_DATAS], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAS],"clicked",G_CALLBACK(xdkbanksw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAS],iceb_u_inttochar(E_DATAS));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAS],gettext("Выбор даты"));

data.entry[E_DATAS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAS]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAS]), data.entry[E_DATAS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAS], "activate",G_CALLBACK(xdkbanksw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAS]),data.rk->datas.ravno());
gtk_widget_set_name(data.entry[E_DATAS],iceb_u_inttochar(E_DATAS));


sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KODKONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKONTR]), data.knopka_enter[E_KODKONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODKONTR],"clicked",G_CALLBACK(xdkbanksw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODKONTR],iceb_u_inttochar(E_KODKONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODKONTR],gettext("Выбор контагента"));

data.entry[E_KODKONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODKONTR]), data.entry[E_KODKONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODKONTR], "activate",G_CALLBACK(xdkbanksw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKONTR]),data.rk->kodkontr.ravno());
gtk_widget_set_name(data.entry[E_KODKONTR],iceb_u_inttochar(E_KODKONTR));


sprintf(strsql,"%s (,,)",gettext("Код группы контрагента"));
data.knopka_enter[E_KODGRKONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODGRKONTR]), data.knopka_enter[E_KODGRKONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODGRKONTR],"clicked",G_CALLBACK(xdkbanksw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODGRKONTR],iceb_u_inttochar(E_KODGRKONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODGRKONTR],gettext("Выбор группы контрагента"));

data.entry[E_KODGRKONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODGRKONTR]), data.entry[E_KODGRKONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODGRKONTR], "activate",G_CALLBACK(xdkbanksw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODGRKONTR]),data.rk->kodgrkontr.ravno());
gtk_widget_set_name(data.entry[E_KODGRKONTR],iceb_u_inttochar(E_KODGRKONTR));

sprintf(strsql,"%s",gettext("Счёт корреспондент"));
data.knopka_enter[E_SHETK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETK]), data.knopka_enter[E_SHETK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHETK],"clicked",G_CALLBACK(xdkbanksw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETK],iceb_u_inttochar(E_SHETK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETK],gettext("Выбор счёта"));

data.entry[E_SHETK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHETK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETK]), data.entry[E_SHETK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHETK], "activate",G_CALLBACK(xdkbanksw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETK]),data.rk->shetk.ravno());
gtk_widget_set_name(data.entry[E_SHETK],iceb_u_inttochar(E_SHETK));

sprintf(strsql,"%s (%s)",gettext("Дата проводки"),gettext("д.м.г"));
data.knopka_enter[E_DATAP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), data.knopka_enter[E_DATAP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAP],"clicked",G_CALLBACK(xdkbanksw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAP],iceb_u_inttochar(E_DATAP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAP],gettext("Выбор даты"));

data.entry[E_DATAP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAP]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), data.entry[E_DATAP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAP], "activate",G_CALLBACK(xdkbanksw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAP]),data.rk->datap.ravno());
gtk_widget_set_name(data.entry[E_DATAP],iceb_u_inttochar(E_DATAP));




sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(xdkbanksw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(xdkbanksw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(xdkbanksw_v_knopka),&data);
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
void  xdkbanksw_v_e_knopka(GtkWidget *widget,class xdkbanksw_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_SHET:
    iceb_vibrek(1,"Plansh",&data->rk->shet,data->window);
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno());
 
      
    return;  

  case E_DATAS:

    if(iceb_calendar(&data->rk->datas,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAS]),data->rk->datas.ravno());
      
    return;  

  case E_KODKONTR:
    iceb_vibrek(0,"Kontragent",&data->rk->kodkontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKONTR]),data->rk->kodkontr.ravno());
    return;  


  case E_KODGRKONTR:
    iceb_vibrek(0,"Gkont",&data->rk->kodgrkontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGRKONTR]),data->rk->kodgrkontr.ravno());
    return;

  case E_SHETK:
    iceb_vibrek(1,"Plansh",&data->rk->shetk,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETK]),data->rk->shetk.ravno());
    return;

  case E_DATAP:
    if(iceb_calendar(&data->rk->datap,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAS]),data->rk->datap.ravno());

    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   xdkbanksw_v_key_press(GtkWidget *widget,GdkEventKey *event,class xdkbanksw_data *data)
{

//printf("xdkbanksw_v_key_press\n");
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
/*********************************/
/*проверка введённых реквизитов*/
/******************************/
int xdkbanksw_m_prov(class xdkbanksw_data *data)
{
data->read_rek(); //Читаем реквизиты меню

if(data->rk->shet.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён счёт!"),data->window);
  return(1);
 }
if(data->rk->datas.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена дата сальдо!"),data->window);
  return(1);
 }
if(data->rk->datap.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена дата проводки!"),data->window);
  return(1);
 }

if(data->rk->datas.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата сальдо!"),data->window);
  return(1);
 }
if(data->rk->datap.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата проводки!"),data->window);
  return(1);
 }
struct OPSHET rs;
if(iceb_prsh1(data->rk->shet.ravno(),&rs,data->window) != 0)
 return(1);

if(rs.saldo != 3)
 {
  iceb_menu_soob(gettext("Счёт имеет свёрнутое сальдо!"),data->window);
  return(1);
 }

if(iceb_prsh1(data->rk->shetk.ravno(),&rs,data->window) != 0)
 return(1);

if(iceb_pvglkni(data->rk->datap.ravno(),data->window) != 0)
 return(1);

if(iceb_pbsh(data->rk->datap.ravno(),data->rk->shet.ravno(),data->rk->shetk.ravno(),"",data->window) != 0)
 return(1);
 
return(0);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  xdkbanksw_v_knopka(GtkWidget *widget,class xdkbanksw_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    if(xdkbanksw_m_prov(data) != 0)
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

void    xdkbanksw_v_vvod(GtkWidget *widget,class xdkbanksw_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("xdkbanksw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SHET:
    data->rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAS:
    data->rk->datas.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODKONTR:
    data->rk->kodkontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_KODGRKONTR:
    data->rk->kodgrkontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHETK:
    data->rk->shetk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
