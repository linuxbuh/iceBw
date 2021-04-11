/*$Id: iceb_menu_vibkat.c,v 1.5 2013/08/25 08:27:07 sasa Exp $*/
/*27.07.2011	27.07.2011	Белых А.И.	iceb_menu_vibkat.c
Меню ввода каталога
Если вернули 0 - каталог выбрали
             1 - нет
*/

#include	"iceb_libbuh.h"

enum
 {
  E_PUT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK10,
  KOL_FKL
 };
 

class iceb_menu_vibkat_data
 {
  public:
  GtkWidget *window;
  GtkWidget *entry[KOLENTER];
  int vihod;
  class iceb_u_str text;
  GtkWidget *knopka[KOL_FKL];
  GtkWidget *knopka_enter[KOLENTER];
  
  iceb_menu_vibkat_data()
   {
    vihod=0;
    text.plus("");
   }
 };

gboolean   iceb_menu_vibkat_key_press(GtkWidget *widget,GdkEventKey *event,iceb_menu_vibkat_data *data);
void iceb_menu_vibkat_get_text(GtkWidget *widget,class iceb_menu_vibkat_data *data);
void  iceb_menu_vibkat_vihod(GtkWidget *widget,class iceb_menu_vibkat_data *data);
gboolean  iceb_menu_vibkat_dest(GtkWidget *widget,GdkEvent *event,class iceb_menu_vibkat_data *data);
void  iceb_menu_vibkat_v_e_knopka(GtkWidget *widget,class iceb_menu_vibkat_data *data);

int iceb_menu_vibkat(const char *zapros,class iceb_u_str *stroka,GtkWidget *wpredok)
{
 class iceb_u_spisok prom;
 prom.plus(zapros);
 return(iceb_menu_vibkat(&prom,stroka,wpredok));

}

int iceb_menu_vibkat(class iceb_u_spisok *zapros,class iceb_u_str *stroka,GtkWidget *wpredok)
{
class iceb_u_str titl;
char bros[512];
class iceb_menu_vibkat_data data;
/*Избавляемся от перевода строки*/
if(iceb_u_polen(zapros->ravno(0),&titl,1,'\n') != 0)
 titl.new_plus(zapros->ravno(0));
 
int kolih=zapros->kolih();


data.window= gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW( data.window ), GTK_WIN_POS_CENTER );                      
gtk_container_set_border_width( GTK_CONTAINER( data.window ), 10 );
gtk_window_set_modal( GTK_WINDOW(data.window ) ,TRUE ); 
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/

class iceb_u_str repl(iceb_get_namesystem());
repl.plus(" ",titl.ravno());

gtk_window_set_title (GTK_WINDOW (data.window),repl.ravno());


g_signal_connect(data.window, "delete_event",G_CALLBACK(iceb_menu_vibkat_dest), &data);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_menu_vibkat_key_press),&data);
if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GtkWidget *hbox0=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox0),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window),vbox);

GtkWidget *label = NULL;
for(int i=0 ; i< kolih; i++)
 {
  label = gtk_label_new (zapros->ravno(i));
  gtk_box_pack_start(GTK_BOX (vbox),label,FALSE,FALSE,0);
 }
gtk_box_pack_start(GTK_BOX (vbox),hbox0,FALSE,FALSE,0);


sprintf(bros,"%s",gettext("Каталог"));
data.knopka_enter[E_PUT]=gtk_button_new_with_label(bros);
gtk_box_pack_start (GTK_BOX (hbox0), data.knopka_enter[E_PUT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_PUT],"clicked",G_CALLBACK(iceb_menu_vibkat_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PUT],iceb_u_inttochar(E_PUT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PUT],gettext("Поиск нужного каталога"));

data.entry[E_PUT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox0), data.entry[E_PUT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PUT], "activate",G_CALLBACK(iceb_menu_vibkat_get_text),&data);
if(stroka->getdlinna() > 1)
  gtk_entry_set_text(GTK_ENTRY(data.entry[E_PUT]),stroka->ravno());
gtk_widget_set_name(data.entry[E_PUT],iceb_u_inttochar(E_PUT));






gtk_box_pack_start(GTK_BOX (vbox),hbox,FALSE,FALSE,0);

sprintf(bros,"FK2 %s",gettext("Ввести"));
data.knopka[FK2] = gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK2],TRUE,TRUE,2);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(iceb_menu_vibkat_vihod), &data);

sprintf(bros,"FK10 %s",gettext("Выход"));
data.knopka[FK10] = gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK10],TRUE,TRUE,2);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(iceb_menu_vibkat_vihod), &data);

gtk_widget_show_all(data.window);
gtk_main();

if(data.vihod == 0)
 stroka->new_plus(data.text.ravno_fromutf());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.vihod);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  iceb_menu_vibkat_v_e_knopka(GtkWidget *widget,class iceb_menu_vibkat_data *data)
{
class iceb_u_str kod("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_PUT:
     if(iceb_dir_select(&kod,gettext("Поиск нужного каталога"),data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PUT]),kod.ravno());
      
    return;  
 }
}



/***************************/
/*Выход по нажатию крестика*/
/***************************/
gboolean  iceb_menu_vibkat_dest(GtkWidget *widget,GdkEvent *event,class iceb_menu_vibkat_data *data)
{
data->vihod=1;
data->text.new_plus("");
gtk_widget_destroy(data->window);
return(FALSE);
}

/*************************/
/*Выход по клавише*/
/*******************/

void  iceb_menu_vibkat_vihod(GtkWidget *widget,class iceb_menu_vibkat_data *data)
{


int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    g_signal_emit_by_name(data->entry[E_PUT],"activate");
    break;
  
  case FK10:
    data->vihod=1;
    data->text.new_plus("");
    gtk_widget_destroy(data->window);
    break;
 }

}
/*****************************/
/*Чтение введенной информации*/
/*****************************/
void iceb_menu_vibkat_get_text(GtkWidget *widget,class iceb_menu_vibkat_data *data)
{
data->text.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(widget)));
data->vihod=0;
gtk_widget_destroy(data->window);

}
/***************************/
/*Обработчик нажатия клавиш*/
/***************************/
gboolean   iceb_menu_vibkat_key_press(GtkWidget *widget,GdkEventKey *event,iceb_menu_vibkat_data *data)
{
switch(event->keyval)
 {
  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(FALSE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);
 }
 
return(TRUE);
}

