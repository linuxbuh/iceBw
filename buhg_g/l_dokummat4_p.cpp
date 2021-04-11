/*$Id: l_dokummat4_p.c,v 1.19 2013/08/25 08:26:45 sasa Exp $*/
/*25.02.2017	25.02.2017	Белых А.И.	l_dokummat4_p.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "l_dokummat4.h"
enum
 {
  E_KOD_M,
  E_NAIM_M,
  E_KOD_D,
  E_NAIM_D,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_dokummat4_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_dokummat4_rek *rk;
  
  
  l_dokummat4_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->kod_mat.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_M])));
    rk->naim_km.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM_M])));
    rk->kod_det.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_D])));
    rk->naim_kd.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM_D])));
   }

  void clear_data()
   {
    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void l_dokummat4_p_clear(class l_dokummat4_p_data *data);
void    l_dokummat4_p_vvod(GtkWidget *widget,class l_dokummat4_p_data *data);
void  l_dokummat4_p_knopka(GtkWidget *widget,class l_dokummat4_p_data *data);
gboolean   l_dokummat4_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_dokummat4_p_data *data);


int l_dokummat4_p(class l_dokummat4_rek *rek_poi,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class l_dokummat4_p_data data;
char strsql[512];
data.rk=rek_poi;
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_dokummat4_p_key_press),&data);

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

sprintf(strsql,"%s (,,)",gettext("Код материала"));
GtkWidget *label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_M]), label, FALSE, FALSE, 0);

data.entry[E_KOD_M] = gtk_entry_new();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_M]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_M]), data.entry[E_KOD_M], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_M], "activate",G_CALLBACK(l_dokummat4_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_M]),data.rk->kod_mat.ravno());
gtk_widget_set_name(data.entry[E_KOD_M],iceb_u_inttochar(E_KOD_M));/******************/

sprintf(strsql,"%s (,,)",gettext("Наименование материала"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_M]), label, FALSE, FALSE, 0);

data.entry[E_NAIM_M] = gtk_entry_new();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM_M]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_M]), data.entry[E_NAIM_M], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM_M], "activate",G_CALLBACK(l_dokummat4_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM_M]),data.rk->naim_km.ravno());

gtk_widget_set_name(data.entry[E_NAIM_M],iceb_u_inttochar(E_NAIM_M));/******************/

sprintf(strsql,"%s (,,)",gettext("Код детали"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_D]), label, FALSE, FALSE, 0);

data.entry[E_KOD_D] = gtk_entry_new();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_D]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_D]), data.entry[E_KOD_D], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_D], "activate",G_CALLBACK(l_dokummat4_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_D]),data.rk->kod_det.ravno());
gtk_widget_set_name(data.entry[E_KOD_D],iceb_u_inttochar(E_KOD_D));/******************/

sprintf(strsql,"%s (,,)",gettext("Наименование детали"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_D]), label, FALSE, FALSE, 0);

data.entry[E_NAIM_D] = gtk_entry_new();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM_D]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_D]), data.entry[E_NAIM_D], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM_D], "activate",G_CALLBACK(l_dokummat4_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM_D]),data.rk->naim_kd.ravno());

gtk_widget_set_name(data.entry[E_NAIM_D],iceb_u_inttochar(E_NAIM_D));/******************/

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_dokummat4_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_dokummat4_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_dokummat4_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_dokummat4_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_dokummat4_p_data *data)
{

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
void  l_dokummat4_p_knopka(GtkWidget *widget,class l_dokummat4_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    data->voz=0;
    data->rk->metka_poi=1;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    data->rk->metka_poi=0;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_dokummat4_p_vvod(GtkWidget *widget,class l_dokummat4_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
switch (enter)
 {
  case E_KOD_M:
    data->rk->kod_mat.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_KOD_M])));
    break;
  case E_NAIM_M:
    data->rk->naim_km.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NAIM_M])));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
