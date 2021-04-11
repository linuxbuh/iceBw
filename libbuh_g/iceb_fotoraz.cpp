/*$Id: iceb_fotoraz.c,v 1.9 2013/08/25 08:27:05 sasa Exp $*/
/*29.07.2013	02.09.2007	Белых А.И.	iceb_fotoraz.c
Фотография разработчика
*/
//#include        "../knopki/port.xpm"
#include        "../xpm/foto_raz.xpm"
#include        "iceb_libbuh.h"

enum
 {
  VIHOD,
  KOLKNOP
 };

class glmenu_data
 {
  public:
  
  GtkWidget *window;
  GtkWidget *knopka[KOLKNOP];
  short     nomkn;

  glmenu_data() // Конструктор
   {
    nomkn=-1;
   }
 };

gboolean   iceb_fotoraz_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data);
void  iceb_fotoraz_knopka(GtkWidget *widget,class glmenu_data *data);


void iceb_fotoraz(GtkWidget *wpredok)
{

class glmenu_data data;


data.window=gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

gtk_window_set_title(GTK_WINDOW(data.window),gettext("Разработчик"));

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_fotoraz_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(data.window),vbox);




//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
gtk_widget_realize(data.window);
//gtk_widget_show(data.window);

/*вставляем фото*/
GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(foto_raz);
GtkWidget *foto=gtk_image_new_from_pixbuf(pixbuf);

gtk_container_add(GTK_CONTAINER(vbox),foto);


char bros[512];
sprintf(bros,"%s:",gettext("Разработчик"));


GtkWidget *label=gtk_label_new(bros);

gtk_box_pack_start((GtkBox*)vbox,label,TRUE,TRUE,0);

label=gtk_label_new(gettext("Белых Александр Иванович"));

gtk_box_pack_start((GtkBox*)vbox,label,TRUE,TRUE,0);

label=gtk_label_new("e-mail:belih1957@gmail.com");

gtk_box_pack_start((GtkBox*)vbox,label,TRUE,TRUE,0);

data.knopka[VIHOD]=gtk_button_new_with_label(gettext("Выход"));
//gtk_widget_set_tooltip_text(data.knopka[VIHOD],gettext("Завершение работы в этом окне."));
g_signal_connect(data.knopka[VIHOD],"clicked",G_CALLBACK(iceb_fotoraz_knopka),&data);
gtk_widget_set_name(data.knopka[VIHOD],iceb_u_inttochar(VIHOD));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[VIHOD], TRUE, TRUE,0);



gtk_widget_grab_focus(data.knopka[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_fotoraz_knopka(GtkWidget *widget,class glmenu_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("glkni_v_knopka knop=%d\n",knop);
data->nomkn=knop;
gtk_widget_destroy(data->window);
data->window=NULL;

}



/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_fotoraz_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data)
{

switch(event->keyval)
 {

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[VIHOD],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

//    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

