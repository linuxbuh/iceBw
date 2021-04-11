/*$Id: rnn_rkontrw_m.c,v 1.5 2013/09/26 09:46:54 sasa Exp $*/
/*12.07.2015	20.01.2012	Белых А.И.	rnn_rkontrw_m.c
Меню для получения отчёта 
*/
#include "buhg_g.h"
#include "rnn_rkontrw.h"

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
  E_INN,
  KOLENTER  
 };

class rnn_rkontrw_data
 {
  public:
  rnn_rkontrw_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  rnn_rkontrw_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rk->inn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_INN])));

    rk->metka_pr=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));

   }

  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }

 };




gboolean   rnn_rkontrw_v_key_press(GtkWidget *widget,GdkEventKey *event,class rnn_rkontrw_data *data);
void    rnn_rkontrw_v_vvod(GtkWidget *widget,class rnn_rkontrw_data *data);
void  rnn_rkontrw_v_knopka(GtkWidget *widget,class rnn_rkontrw_data *data);
void  rnn_rkontrw_v_e_knopka(GtkWidget *widget,class rnn_rkontrw_data *data);


extern SQL_baza bd;

int rnn_rkontrw_m(class rnn_rkontrw_rr *rek_ras,GtkWidget *wpredok)
{
rnn_rkontrw_data data;

data.rk=rek_ras;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),data.rk->naim_oth.ravno());
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rnn_rkontrw_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

class iceb_u_str repl;
repl.plus(gettext("Заработная плата"));

repl.ps_plus(data.rk->naim_oth.ravno());
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
GtkWidget *hbox_opt = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox_opt),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);

gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i],FALSE, FALSE, 0);
//  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_box_pack_start (GTK_BOX (vbox), hbox_opt, FALSE, FALSE, 0);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_box_pack_start (GTK_BOX (vbox),separator1,FALSE, FALSE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), separator1);



GtkWidget *separator4=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_box_pack_start (GTK_BOX (vbox),separator4,FALSE, FALSE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), separator4);


gtk_box_pack_start (GTK_BOX (vbox),hboxknop,FALSE, FALSE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(rnn_rkontrw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(rnn_rkontrw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(rnn_rkontrw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(rnn_rkontrw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));


data.knopka_enter[E_INN]=gtk_button_new_with_label(gettext("Индивидуальный налоговый номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.knopka_enter[E_INN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_INN],"clicked",G_CALLBACK(rnn_rkontrw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_INN],iceb_u_inttochar(E_INN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_INN],gettext("Выбор контрагента"));

data.entry[E_INN] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.entry[E_INN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_INN], "activate",G_CALLBACK(rnn_rkontrw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INN]),data.rk->inn.ravno());
gtk_widget_set_name(data.entry[E_INN],iceb_u_inttochar(E_INN));


sprintf(strsql,"%s",gettext("Приход/расход"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox_opt), label, FALSE, FALSE, 0);

class iceb_u_spisok vidnak;
vidnak.plus(gettext("Все налоговые накладные"));
vidnak.plus(gettext("Только полученные налоговые накладные"));
vidnak.plus(gettext("Только выданные налоговые накладные"));

iceb_pm_vibor(&vidnak,&data.opt,data.rk->metka_pr);

gtk_box_pack_start (GTK_BOX (hbox_opt), data.opt, TRUE, TRUE, 0);



sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(rnn_rkontrw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(rnn_rkontrw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(rnn_rkontrw_v_knopka),&data);
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
void  rnn_rkontrw_v_e_knopka(GtkWidget *widget,class rnn_rkontrw_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

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

  case E_INN:
    if(iceb_vibrek(1,"Kontragent",&kod,data->window) == 0)
     {
      SQL_str row;
      class SQLCURSOR cur;
      char strsql[512];
      sprintf(strsql,"select innn from Kontragent where kodkon='%s'",kod.ravno_filtr());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        if(row[0][0] == '\0')
         {
          sprintf(strsql,"%s %s!",gettext("Не введён индивидуальный налоговый номер для контрагента"),kod.ravno());
          iceb_menu_soob(strsql,data->window);
         }
        data->rk->inn.new_plus(row[0]);
       }
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_INN]),data->rk->inn.ravno());
    return;  


 }
}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rnn_rkontrw_v_key_press(GtkWidget *widget,GdkEventKey *event,class rnn_rkontrw_data *data)
{

//printf("rnn_rkontrw_v_key_press\n");
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
void  rnn_rkontrw_v_knopka(GtkWidget *widget,class rnn_rkontrw_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    printf("%s-metka_pr=%d\n",__FUNCTION__,data->rk->metka_pr);
    if(iceb_rsdatp_str(&data->rk->datan,&data->rk->datak,data->window) != 0)
      return;
    if(data->rk->inn.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введён индивидуальный налоговый номер!"),data->window);
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

void    rnn_rkontrw_v_vvod(GtkWidget *widget,class rnn_rkontrw_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//g_print("rnn_rkontrw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_INN:
    data->rk->inn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
