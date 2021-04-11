/*$Id: l_kaskcnsl_p.c,v 1.15 2013/08/25 08:26:36 sasa Exp $*/
/*29.02.2016	01.10.2006	Белых А.И.	l_kaskcnsl_p.c
Ввод и корректировка групп подразделений
*/
#include "buhg_g.h"
#include "l_kaskcnsl.h"
enum
 {
  E_GOD,
  E_KAS,
  E_KOD,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_kaskcnsl_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_kaskcnsl_rek *poi;


  //Ключевые реквизиты корректируемой строки
  class iceb_u_str kod_cn_z;  
  class iceb_u_str kas_z;  
  class iceb_u_str god_z;  
  
  l_kaskcnsl_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    poi->kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    poi->god.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GOD])));
    poi->kas.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KAS])));
   }

  void clear_rek()
   {
    for(int ii=0; ii < KOLENTER; ii++)
      gtk_entry_set_text(GTK_ENTRY(entry[ii]),"");
    poi->clear_data();    
   }
 };


gboolean   l_kaskcnsl_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_kaskcnsl_p_data *data);
void  l_kaskcnsl_p_knopka(GtkWidget *widget,class l_kaskcnsl_p_data *data);
void    l_kaskcnsl_p_vvod(GtkWidget *widget,class l_kaskcnsl_p_data *data);

void  kaskcnsl_p_e_knopka(GtkWidget *widget,class l_kaskcnsl_p_data *data);

extern SQL_baza  bd;

int l_kaskcnsl_p(class l_kaskcnsl_rek *poi,
GtkWidget *wpredok)
{
class l_kaskcnsl_p_data data;
char strsql[512];
int gor=0,ver=0;
data.poi=poi;
data.name_window.plus(__FUNCTION__);


 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Поиск"));
label=gtk_label_new(gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_kaskcnsl_p_key_press),&data);

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
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);



label=gtk_label_new(gettext("Год"));
data.entry[E_GOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GOD]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GOD], "activate",G_CALLBACK(l_kaskcnsl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.poi->god.ravno());
gtk_widget_set_name(data.entry[E_GOD],iceb_u_inttochar(E_GOD));


sprintf(strsql,"%s (,,)",gettext("Код кассы"));
data.knopka_enter[E_KAS]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KAS]), data.knopka_enter[E_KAS], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KAS],"clicked",G_CALLBACK(kaskcnsl_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KAS],iceb_u_inttochar(E_KAS));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KAS],gettext("Выбор кассы"));

data.entry[E_KAS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KAS]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KAS]), data.entry[E_KAS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KAS], "activate",G_CALLBACK(l_kaskcnsl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KAS]),data.poi->kas.ravno());
gtk_widget_set_name(data.entry[E_KAS],iceb_u_inttochar(E_KAS));


sprintf(strsql,"%s (,,)",gettext("Код целевого назначения"));
data.knopka_enter[E_KOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.knopka_enter[E_KOD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD],"clicked",G_CALLBACK(kaskcnsl_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD],iceb_u_inttochar(E_KOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD],gettext("Выбор кода целевого назначения"));

data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_kaskcnsl_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.poi->kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));



sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_kaskcnsl_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_kaskcnsl_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_kaskcnsl_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  kaskcnsl_p_e_knopka(GtkWidget *widget,class l_kaskcnsl_p_data *data)
{
class iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_KAS:
    if(l_spis_kas(1,&kod,&naim,data->window) == 0)
      data->poi->kas.z_plus(kod.ravno());
   
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KAS]),data->poi->kas.ravno());
    return;

  case E_KOD:
    if(l_kaskcn(1,&kod,&naim,data->window) == 0)
     data->poi->kod.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->poi->kod.ravno());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_kaskcnsl_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_kaskcnsl_p_data *data)
{
//char  bros[512];

//printf("l_kaskcnsl_p_key_press\n");
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
void  l_kaskcnsl_p_knopka(GtkWidget *widget,class l_kaskcnsl_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    data->voz=0;
    data->poi->metka_poi=1;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    data->poi->metka_poi=0;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_kaskcnsl_p_vvod(GtkWidget *widget,class l_kaskcnsl_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KOD:
    data->poi->kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GOD:
    data->poi->god.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KAS:
    data->poi->kas.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
