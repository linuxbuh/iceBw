/*$Id: iceb_l_smenabaz_v.c,v 1.9 2013/08/25 08:27:06 sasa Exp $*/
/*28.02.2016	17.04.2010	Белых А.И.	iceb_l_smenabaz_v.c
Меню для ввода реквизитов поиска 
*/
#include <pwd.h>
#include "iceb_libbuh.h"

enum
 {
  E_HOSTNABAZU,
  E_PAROLNAHOST,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class iceb_l_smenabaz_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str hostnabazu;
  class iceb_u_str parolnahost;  
  
  iceb_l_smenabaz_v_data() //Конструктор
   {
    clear_rek();
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    hostnabazu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_HOSTNABAZU])));
    parolnahost.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PAROLNAHOST])));
   }

  void clear_data()
   {
    clear_rek();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }

  void clear_rek()
   {
    hostnabazu.new_plus("");
    parolnahost.new_plus("");
   }
 };

void iceb_l_smenabaz_v_clear(class iceb_l_smenabaz_v_data *data);
void    iceb_l_smenabaz_v_vvod(GtkWidget *widget,class iceb_l_smenabaz_v_data *data);
void  iceb_l_smenabaz_v_knopka(GtkWidget *widget,class iceb_l_smenabaz_v_data *data);
gboolean   iceb_l_smenabaz_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_smenabaz_v_data *data);

extern char *host;
extern char *parol;

int iceb_l_smenabaz_v(GtkWidget *wpredok)
{
class iceb_l_smenabaz_v_data data;
char strsql[512];
int gor=0,ver=0;

data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Сменить хост на базу"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_l_smenabaz_v_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
 gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 0);


GtkWidget *label=gtk_label_new(gettext("Хост на базу"));
gtk_box_pack_start (GTK_BOX (hbox[E_HOSTNABAZU]), label, FALSE, FALSE, 0);

data.entry[E_HOSTNABAZU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HOSTNABAZU]), data.entry[E_HOSTNABAZU], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_HOSTNABAZU], "activate",G_CALLBACK(iceb_l_smenabaz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HOSTNABAZU]),data.hostnabazu.ravno());
gtk_widget_set_name(data.entry[E_HOSTNABAZU],iceb_u_inttochar(E_HOSTNABAZU));

label=gtk_label_new(gettext("Пароль для доступа к базе"));
gtk_box_pack_start (GTK_BOX (hbox[E_PAROLNAHOST]), label, FALSE, FALSE, 0);

data.entry[E_PAROLNAHOST] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PAROLNAHOST]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_PAROLNAHOST]), data.entry[E_PAROLNAHOST], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PAROLNAHOST], "activate",G_CALLBACK(iceb_l_smenabaz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PAROLNAHOST]),data.parolnahost.ravno());
gtk_widget_set_name(data.entry[E_PAROLNAHOST],iceb_u_inttochar(E_PAROLNAHOST));
gtk_entry_set_visibility(GTK_ENTRY(data.entry[E_PAROLNAHOST]),FALSE);



sprintf(strsql,"F2 %s",gettext("Сменить"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Сменить хост на базу"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(iceb_l_smenabaz_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(iceb_l_smenabaz_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Закончить работу в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(iceb_l_smenabaz_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
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

gboolean   iceb_l_smenabaz_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_smenabaz_v_data *data)
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
void  iceb_l_smenabaz_v_knopka(GtkWidget *widget,class iceb_l_smenabaz_v_data *data)
{
SQL_baza bdhost;
struct  passwd  *ktor; /*Кто работает*/
int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    ktor=getpwuid(getuid());

    if(sql_openbaz(&bdhost,"",data->hostnabazu.ravno(),ktor->pw_name,data->parolnahost.ravno()) != 0)
     {
      iceb_eropbaz("",ktor->pw_uid,ktor->pw_name,1);
      return;
     }

    sql_closebaz(&bdhost);

    delete [] parol;
    delete [] host;    

    parol=new char[data->parolnahost.getdlinna()];
    strcpy(parol,data->parolnahost.ravno());

    host=new char[data->hostnabazu.getdlinna()];
    strcpy(host,data->hostnabazu.ravno());
//    printf("%s-host=%s parol=%s\n",__FUNCTION__,host,parol);    
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

void    iceb_l_smenabaz_v_vvod(GtkWidget *widget,class iceb_l_smenabaz_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_HOSTNABAZU:
    data->hostnabazu.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_HOSTNABAZU])));
    break;
  case E_PAROLNAHOST:
    data->parolnahost.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_PAROLNAHOST])));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
