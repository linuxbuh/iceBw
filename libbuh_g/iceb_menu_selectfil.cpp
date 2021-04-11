/*$Id: iceb_menu_selectfil.c,v 1.4 2013/09/26 09:47:04 sasa Exp $*/
/*26.08.2012	14.11.2004	Белых А.И.	iceb_menu_selectfil.c
Меню для ввода имени файла из которого нужно импортировать документы 
*/
#include        <sys/stat.h>
#include "iceb_libbuh.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_IMAF,
  KOLENTER  
 };

class iceb_menu_import_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  class iceb_u_str *imafz;
  iceb_menu_import_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    imafz->new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_IMAF])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };

gboolean   iceb_menu_import_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_menu_import_m_data *data);
void    iceb_menu_import_m_v_vvod(GtkWidget *widget,class iceb_menu_import_m_data *data);
void  iceb_menu_import_m_v_knopka(GtkWidget *widget,class iceb_menu_import_m_data *data);
void  iceb_menu_import_v_e_knopka(GtkWidget *widget,class iceb_menu_import_m_data *data);


extern SQL_baza bd;

int iceb_menu_selectfil(class iceb_u_str *imafz,const char *zagolov,GtkWidget *wpredok)
{
iceb_menu_import_m_data data;

char strsql[312];
data.imafz=imafz;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),zagolov);
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_menu_import_m_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(zagolov);

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

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Введите имя файла"));
data.knopka_enter=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_IMAF]), data.knopka_enter, FALSE, FALSE,1);
g_signal_connect(data.knopka_enter,"clicked",G_CALLBACK(iceb_menu_import_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter,iceb_u_inttochar(E_IMAF));
gtk_widget_set_tooltip_text(data.knopka_enter,gettext("Поиск нужного файла"));


/********************
sprintf(strsql,"%s",gettext("Введите имя файла"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_IMAF]), label, FALSE, FALSE, 0);
******************/
data.entry[E_IMAF] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_IMAF]), data.entry[E_IMAF], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_IMAF], "activate",G_CALLBACK(iceb_menu_import_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMAF]),data.imafz->ravno());
gtk_widget_set_name(data.entry[E_IMAF],iceb_u_inttochar(E_IMAF));



sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Продолжить выполнение программы"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(iceb_menu_import_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(iceb_menu_import_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Выйти из этого меню"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(iceb_menu_import_m_v_knopka),&data);
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
void  iceb_menu_import_v_e_knopka(GtkWidget *widget,class iceb_menu_import_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_IMAF:
    iceb_file_selection(data->imafz,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_IMAF]),data->imafz->ravno());

    return;  

 }
}



/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_menu_import_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_menu_import_m_data *data)
{

//printf("iceb_menu_import_m_v_key_press\n");
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
void  iceb_menu_import_m_v_knopka(GtkWidget *widget,class iceb_menu_import_m_data *data)
{
struct stat work;

int knop=atoi(gtk_widget_get_name(widget));
/*g_print("iceb_menu_import_m_v_knopka knop=%d\n",*knop);*/

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->imafz->getdlinna() <= 1)
     {
      iceb_u_str repl;
      iceb_menu_soob(gettext("Не введено имя файла !"),data->window);
      return;
     }    


    if(stat(data->imafz->ravno(),&work) != 0)
     {
      iceb_u_str repl; 
      repl.new_plus(gettext("Нет такого файла !"));
      repl.ps_plus(data->imafz->ravno());
      iceb_menu_soob(&repl,data->window);
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

void    iceb_menu_import_m_v_vvod(GtkWidget *widget,class iceb_menu_import_m_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//g_print("iceb_menu_import_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_IMAF:
    data->imafz->new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }
 
}
