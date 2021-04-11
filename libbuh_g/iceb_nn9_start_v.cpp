/*$Id:*/
/*22.03.2017	17.03.2015	Белых А.И.	iceb_nn9_start_v.c
Меню для ввода реквизитов 
*/
#include "iceb_libbuh.h"

enum
 {
  E_ZVED_PN,
  E_SNOZ,
  E_NEVID,
  E_TIPPR,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class iceb_nn9_start_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  
  class iceb_u_str nevid;
  class iceb_u_str tippr;
  class iceb_u_str zved_pn;
  class iceb_u_str snoz;
  
  iceb_nn9_start_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {

    zved_pn.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ZVED_PN])));
    snoz.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SNOZ])));
    nevid.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NEVID])));
    tippr.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TIPPR])));

  
   }

  void clear_data()
   {
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void iceb_nn9_start_v_clear(class iceb_nn9_start_v_data *data);
void    iceb_nn9_start_v_vvod(GtkWidget *widget,class iceb_nn9_start_v_data *data);
void  iceb_nn9_start_v_knopka(GtkWidget *widget,class iceb_nn9_start_v_data *data);
gboolean   iceb_nn9_start_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_nn9_start_v_data *data);


int iceb_nn9_start_v(class iceb_u_str *zved_pn,
class iceb_u_str *snoz,
class iceb_u_str *nevid,
class iceb_u_str *tippr,
GtkWidget *wpredok)
{
class iceb_nn9_start_v_data data;
char strsql[512];
int gor=0,ver=0;

data.zved_pn.new_plus(zved_pn->ravno());
data.snoz.new_plus(snoz->ravno());
data.nevid.new_plus(nevid->ravno());
data.tippr.new_plus(tippr->ravno());
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_nn9_start_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);



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
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
 gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 0);


GtkWidget *label=gtk_label_new("Зведена податкова накладна");
gtk_box_pack_start (GTK_BOX (hbox[E_ZVED_PN]), label, FALSE, FALSE, 0);

data.entry[E_ZVED_PN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ZVED_PN]),1);
gtk_box_pack_start (GTK_BOX (hbox[E_ZVED_PN]), data.entry[E_ZVED_PN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ZVED_PN], "activate",G_CALLBACK(iceb_nn9_start_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZVED_PN]),data.zved_pn.ravno());
gtk_widget_set_name(data.entry[E_ZVED_PN],iceb_u_inttochar(E_ZVED_PN));

label=gtk_label_new("Складена на операції, звільнені від оподаткування");
gtk_box_pack_start (GTK_BOX (hbox[E_SNOZ]), label, FALSE, FALSE, 0);

data.entry[E_SNOZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SNOZ]),1);
gtk_box_pack_start (GTK_BOX (hbox[E_SNOZ]), data.entry[E_SNOZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SNOZ], "activate",G_CALLBACK(iceb_nn9_start_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SNOZ]),data.snoz.ravno());
gtk_widget_set_name(data.entry[E_SNOZ],iceb_u_inttochar(E_SNOZ));

label=gtk_label_new(gettext("Не выдаётся покупателю"));
gtk_box_pack_start (GTK_BOX (hbox[E_NEVID]), label, FALSE, FALSE, 0);

data.entry[E_NEVID] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NEVID]),1);
gtk_box_pack_start (GTK_BOX (hbox[E_NEVID]), data.entry[E_NEVID], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NEVID], "activate",G_CALLBACK(iceb_nn9_start_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NEVID]),data.nevid.ravno());
gtk_widget_set_name(data.entry[E_NEVID],iceb_u_inttochar(E_NEVID));


label=gtk_label_new(gettext("Тип причины"));
gtk_box_pack_start (GTK_BOX (hbox[E_TIPPR]), label, FALSE, FALSE, 0);

data.entry[E_TIPPR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_TIPPR]),8);
gtk_box_pack_start (GTK_BOX (hbox[E_TIPPR]), data.entry[E_TIPPR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TIPPR], "activate",G_CALLBACK(iceb_nn9_start_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TIPPR]),data.tippr.ravno());
gtk_widget_set_name(data.entry[E_TIPPR],iceb_u_inttochar(E_TIPPR));




sprintf(strsql,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввести информацию"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(iceb_nn9_start_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(iceb_nn9_start_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Выйти из меню"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(iceb_nn9_start_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 {
  zved_pn->new_plus(data.zved_pn.ravno());
  snoz->new_plus(data.snoz.ravno());
  nevid->new_plus(data.nevid.ravno());
  tippr->new_plus(data.tippr.ravno());

 }
return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_nn9_start_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_nn9_start_v_data *data)
{
//char  bros[512];

//printf("vl_zarkateg_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(FALSE);

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
void  iceb_nn9_start_v_knopka(GtkWidget *widget,class iceb_nn9_start_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    iceb_nn9_start_v_vvod(GtkWidget *widget,class iceb_nn9_start_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_ZVED_PN:
    data->zved_pn.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_ZVED_PN])));
    break;
  case E_SNOZ:
    data->snoz.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_SNOZ])));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
