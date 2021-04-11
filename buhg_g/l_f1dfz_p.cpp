/*$Id: l_f1dfz_p.c,v 1.13 2013/08/25 08:26:35 sasa Exp $*/
/*28.02.2016	09.01.2007	Белых А.И.	l_f1dfz_p.c
Ввод и корректировка записи по постороннему человеку в документ формы 1ДФ
Если вернули 0-искать 1- не искать
*/
#include "buhg_g.h"
#include "l_f1dfz.h"

enum
 {
  E_FIO,
  E_INN,
  E_OZ_DOH,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_f1dfz_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  class iceb_u_str name_window;
  short kl_shift;
  
  class l_f1dfz_rek *rk;
        
    
  l_f1dfz_p_data() //Конструктор
   {
    kl_shift=0;
   }

  void read_rek()
   {
    rk->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FIO])));
    rk->inn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_INN])));
    rk->kod_doh.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_OZ_DOH])));
   }
 };

gboolean   l_f1dfz_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfz_p_data *data);
void  l_f1dfz_p_knopka(GtkWidget *widget,class l_f1dfz_p_data *data);
void    l_f1dfz_p_vvod(GtkWidget *widget,class l_f1dfz_p_data *data);

extern SQL_baza  bd;

int l_f1dfz_p(class l_f1dfz_rek *rek,
GtkWidget *wpredok)
{
int gor=0,ver=0;
class l_f1dfz_p_data data;
char strsql[512];
data.rk=rek;
data.name_window.plus(__FUNCTION__);

 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Поиск нужных записей"));
label=gtk_label_new(gettext("Поиск нужных записей"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_f1dfz_p_key_press),&data);

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
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 1);


label=gtk_label_new(gettext("Фамилия"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);

data.entry[E_FIO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FIO]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO], "activate",G_CALLBACK(l_f1dfz_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rk->fio.ravno());
gtk_widget_set_name(data.entry[E_FIO],iceb_u_inttochar(E_FIO));



label=gtk_label_new(gettext("Инд. нал. номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), label, FALSE, FALSE, 0);

data.entry[E_INN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_INN]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.entry[E_INN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_INN], "activate",G_CALLBACK(l_f1dfz_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INN]),data.rk->inn.ravno());
gtk_widget_set_name(data.entry[E_INN],iceb_u_inttochar(E_INN));



label=gtk_label_new(gettext("Признак дохода"));
gtk_box_pack_start (GTK_BOX (hbox[E_OZ_DOH]), label, FALSE, FALSE, 0);

data.entry[E_OZ_DOH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_OZ_DOH]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_OZ_DOH]), data.entry[E_OZ_DOH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_OZ_DOH], "activate",G_CALLBACK(l_f1dfz_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OZ_DOH]),data.rk->kod_doh.ravno());
gtk_widget_set_name(data.entry[E_OZ_DOH],iceb_u_inttochar(E_OZ_DOH));




sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_f1dfz_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_f1dfz_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.rk->metka_poi);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_f1dfz_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfz_p_data *data)
{
//char  bros[512];

//printf("l_f1dfz_p_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");

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
void  l_f1dfz_p_knopka(GtkWidget *widget,class l_f1dfz_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    data->rk->metka_poi=0;
     
    gtk_widget_destroy(data->window);
    return;  


  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->rk->metka_poi=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_f1dfz_p_vvod(GtkWidget *widget,class l_f1dfz_p_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_INN:
    data->rk->inn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_FIO:
    data->rk->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    break;

  case E_OZ_DOH:
    data->rk->kod_doh.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
