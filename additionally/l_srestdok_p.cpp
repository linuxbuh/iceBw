/*$Id: l_srestdok_p.c,v 1.21 2013/08/13 06:10:24 sasa Exp $*/
/*06.11.2007	01.03.2004	Белых А.И.	l_srestdok_p.c
Ввод и коррктировка реквизитов поиска
*/
#include <unistd.h>
#include "i_rest.h"
#include "l_srestdok.h"

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
  E_DATAK,
  E_NOMDOK,
  E_NOMERST,
  E_KODKL,
  E_FIO,
  E_KOMENT,
  E_PODR,
  KOLENTER  
 };

class  srestdok_p_data
 {
  public:
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopkap[KOL_PFK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  
  srestdok_r_data *rk; //Реквизиты поиска документов
  short kl_shift;
  
 };
 
gboolean   srestdok_p_key_press(GtkWidget *widget,GdkEventKey *event,class srestdok_p_data *data);
void  srestdok_p_knopka(GtkWidget *widget,class srestdok_p_data *data);
void  srestdok_v_e_knopka(GtkWidget *widget,class srestdok_p_data *data);
void    srestdok_p_vvod(GtkWidget *widget,class srestdok_p_data *data);

//extern SQL_baza	bd;
extern char *name_system;


void l_srestdok_p(class srestdok_r_data *datap,GtkWidget *wpredok)
{
printf("l_srestdok_p\n");

class  srestdok_p_data data;
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(srestdok_p_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0 ; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0 ; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


data.knopka_enter[E_DATAN]=gtk_button_new_with_label(gettext("Дата начала"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(srestdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

data.knopka_enter[E_DATAK]=gtk_button_new_with_label(gettext("Дата конца"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(srestdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));


data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(gettext("Номер документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NOMDOK],"clicked",G_CALLBACK(srestdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdokp.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));

data.knopka_enter[E_NOMERST]=gtk_button_new_with_label(gettext("Номер столика"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERST]), data.knopka_enter[E_NOMERST], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NOMERST],"clicked",G_CALLBACK(srestdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMERST],iceb_u_inttochar(E_NOMERST));

data.entry[E_NOMERST] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMERST]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERST]), data.entry[E_NOMERST], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMERST], "activate",G_CALLBACK(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMERST]),data.rk->nomstol.ravno());
gtk_widget_set_name(data.entry[E_NOMERST],iceb_u_inttochar(E_NOMERST));

data.knopka_enter[E_KODKL]=gtk_button_new_with_label(gettext("Код клиента"));
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.knopka_enter[E_KODKL], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODKL],"clicked",G_CALLBACK(srestdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODKL],iceb_u_inttochar(E_KODKL));

data.entry[E_KODKL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODKL]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.entry[E_KODKL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODKL], "activate",G_CALLBACK(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKL]),data.rk->kodkl.ravno());
gtk_widget_set_name(data.entry[E_KODKL],iceb_u_inttochar(E_KODKL));

data.knopka_enter[E_FIO]=gtk_button_new_with_label(gettext("Фамилия клиента"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.knopka_enter[E_FIO], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_FIO],"clicked",G_CALLBACK(srestdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_FIO],iceb_u_inttochar(E_FIO));

data.entry[E_FIO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FIO]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO], "activate",G_CALLBACK(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rk->fio.ravno());
gtk_widget_set_name(data.entry[E_FIO],iceb_u_inttochar(E_FIO));

data.knopka_enter[E_KOMENT]=gtk_button_new_with_label(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.knopka_enter[E_KOMENT], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOMENT],"clicked",G_CALLBACK(srestdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOMENT],iceb_u_inttochar(E_KOMENT));

data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk->koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));


sprintf(strsql,"%s (,,)",gettext("Подразделение"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(srestdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));

data.entry[E_PODR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PODR]),99);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->koment.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));



sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopkap[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopkap[PFK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopkap[PFK2],"clicked",G_CALLBACK(srestdok_p_knopka),&data);
gtk_widget_set_name(data.knopkap[PFK2],iceb_u_inttochar(PFK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopkap[PFK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopkap[PFK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopkap[PFK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopkap[PFK4],"clicked",G_CALLBACK(srestdok_p_knopka),&data);
gtk_widget_set_name(data.knopkap[PFK4],iceb_u_inttochar(PFK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopkap[PFK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopkap[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopkap[PFK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopkap[PFK10],"clicked",G_CALLBACK(srestdok_p_knopka),&data);
gtk_widget_set_name(data.knopkap[PFK10],iceb_u_inttochar(PFK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopkap[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  srestdok_v_e_knopka(GtkWidget *widget,class srestdok_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case E_DATAN:
    g_signal_emit_by_name(data->entry[E_DATAN],"activate");
    iceb_mous_klav(gettext("Дата начала"),&data->rk->datan,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
    return;  

  case E_DATAK:
    g_signal_emit_by_name(data->entry[E_DATAK],"activate");
    iceb_mous_klav(gettext("Дата конца"),&data->rk->datak,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
    return;  

  case E_NOMDOK:
    g_signal_emit_by_name(data->entry[E_NOMDOK],"activate");
    iceb_mous_klav(gettext("Номер документа"),&data->rk->datak,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk->datak.ravno());
    return;  

  case E_NOMERST:
    g_signal_emit_by_name(data->entry[E_NOMERST],"activate");
    iceb_mous_klav(gettext("Номер столика"),&data->rk->nomstol,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMERST]),data->rk->nomstol.ravno());
    return;  

  case E_KODKL:
    g_signal_emit_by_name(data->entry[E_KODKL],"activate");
    iceb_mous_klav(gettext("Код клиента"),&data->rk->kodkl,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKL]),data->rk->kodkl.ravno());
    return;  

  case E_FIO:
    g_signal_emit_by_name(data->entry[E_FIO],"activate");
    iceb_mous_klav(gettext("Фамилия клиента"),&data->rk->fio,59,1,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_FIO]),data->rk->fio.ravno());
    return;  

  case E_KOMENT:
    g_signal_emit_by_name(data->entry[E_KOMENT],"activate");
    iceb_mous_klav(gettext("Коментарий"),&data->rk->koment,99,1,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOMENT]),data->rk->koment.ravno());
    return;  

  case E_PODR:
    g_signal_emit_by_name(data->entry[E_PODR],"activate");
    iceb_mous_klav(gettext("Подразделение"),&data->rk->podr,99,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
    return;  
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   srestdok_p_key_press(GtkWidget *widget,GdkEventKey *event,class srestdok_p_data *data)
{
//char  bros[300];

//printf("srestdok_p_key_press\n");
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
void  srestdok_p_knopka(GtkWidget *widget,class srestdok_p_data *data)
{
//char bros[300];
short d,m,g;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("srestdok_p_knopka knop=%d\n",knop);

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

    data->rk->metkapoi=1;
    gtk_widget_destroy(data->window);
    data->window=NULL;

    return;  

  case PFK4:
    data->rk->clear_data(); 
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");
    return;

  case PFK10:
    data->rk->metkapoi=0;
    gtk_widget_destroy(data->window);
    data->window=NULL;

    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    srestdok_p_vvod(GtkWidget *widget,class srestdok_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("srestdok_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
     
    break;
  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_FIO:
    data->rk->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMERST:
    data->rk->nomstol.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODKL:
    data->rk->kodkl.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMDOK:
    data->rk->nomdokp.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOMENT:
    data->rk->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PODR:
    data->rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
