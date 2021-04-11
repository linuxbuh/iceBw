/*$Id: iceb_menu_start.c,v 1.19 2014/04/30 06:15:00 sasa Exp $*/
/*28.04.2020	13.04.2010	Белых А.И.	iceb_menu_start.c
Главное меню подсистем
Возвращает номер нажатой клавиши
Если нажали выход возвращает -1
*/
#include        <vector>
#include "iceb_libbuh.h"
#include "../xpm/iceBw05.xpm"
#include "../xpm/flag_ua.xpm"
enum
 {
  SMENABAZ,
  VIHOD,
  OPROG,
  COPYRIGTH,
  KOLKNOP
 };

class glmenu_data
 {
  public:
  
  GtkWidget *window;
  GtkWidget *label_nameprinter;
  GtkWidget *knopka[KOLKNOP];
  int     nomkn;

  glmenu_data() // Конструктор
   {
    nomkn=-1;
   }
 };

void iceb_fotoraz(GtkWidget *wpredok);

extern char		*imabaz;
extern class iceb_u_str kodirovka_iceb;

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_menu_start_knopka(GtkWidget *widget,class glmenu_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

data->nomkn=knop;
if(knop == (OPROG*-1))
 {
  gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
  iceb_about();
  gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));
  return;
 }


if(knop == (COPYRIGTH*-1))
 {
  iceb_fotoraz(data->window);
  return;
 }

gtk_widget_destroy(data->window);
data->window=NULL;

}



/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_menu_start_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data)
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


/**********************************/
int iceb_menu_start(int nomstr_menu,
const char *version,
const char *dataver,
const char *naim_pods,
const char *fioop,
const char *naim_kontr00,
int nom_op,
class iceb_u_spisok *menustr,
const char *cvet_fona)
{
glmenu_data data;
char       bros[512];



data.window=gtk_window_new( GTK_WINDOW_TOPLEVEL );
//gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);


sprintf(bros,"%s %s",iceb_get_namesystem(),naim_pods);
gtk_window_set_title(GTK_WINDOW(data.window),bros);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),0);
gtk_window_set_default_size(GTK_WINDOW(data.window),-1,400); //горизонталь-вертикаль

g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_menu_start_key_press),&data);


GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(data.window),vbox);

GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE,0);


gtk_widget_realize(data.window);
/***************
GdkRGBA color1;
gdk_rgba_parse(&color1,cvet_fona);
gtk_widget_override_background_color(data.window,GTK_STATE_FLAG_NORMAL,&color1);
*****************/
GtkWidget *event_ris=gtk_event_box_new();
gtk_box_pack_start(GTK_BOX(hbox),event_ris,FALSE,FALSE,0);
/****************
gdk_rgba_parse(&color1,"white");
gtk_widget_override_background_color(event_ris,GTK_STATE_FLAG_NORMAL,&color1);
***************/

/*вставляем изображение iceBw*/
GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(iceBw05);
GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);
gtk_container_add(GTK_CONTAINER(event_ris),znak);

GtkWidget *event_text=gtk_event_box_new();
gtk_box_pack_start(GTK_BOX(hbox),event_text,TRUE,TRUE,0);
/****************
gdk_rgba_parse(&color1,"white");
gtk_widget_override_background_color(event_text,GTK_STATE_FLAG_NORMAL,&color1);
***************/
GtkWidget *vbox_text=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox_text),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(event_text),vbox_text);

GtkWidget *label;

GtkWidget *hbox_text=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox_text),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start(GTK_BOX(vbox_text), hbox_text, FALSE, FALSE,0);

//label=gtk_label_new(naim_pods);
label=gtk_label_new(NULL);
//iceb_label_set_style_text(label,"font-family=\"monospace\" color=\"red\"",naim_pods);
iceb_label_set_style_text(label,"font-size=\"large\" font-weight=\"bold\"",naim_pods);

gtk_box_pack_start(GTK_BOX(hbox_text), label, FALSE, FALSE,0);
GtkWidget *event_liniq=gtk_event_box_new();
/**************
gdk_rgba_parse(&color1,"#0E76BC");
gtk_widget_override_background_color(event_liniq,GTK_STATE_FLAG_NORMAL,&color1);
**************/
gtk_box_pack_start(GTK_BOX(vbox_text), event_liniq, FALSE, FALSE,0);

GtkWidget *hbox_text1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox_text1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start(GTK_BOX(vbox_text), hbox_text1, FALSE, FALSE,0);
label=gtk_label_new(naim_kontr00);
gtk_box_pack_start(GTK_BOX(hbox_text1), label, FALSE, FALSE,0);


GtkWidget *hbox_text2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
gtk_box_set_homogeneous (GTK_BOX(hbox_text2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start(GTK_BOX(vbox_text), hbox_text2, FALSE, FALSE,0);

GtkWidget *vbox_text1=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox_text1),TRUE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox_text2=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox_text2),TRUE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start(GTK_BOX(hbox_text2), vbox_text1, FALSE, FALSE,0);
gtk_box_pack_start(GTK_BOX(hbox_text2), vbox_text2, FALSE, FALSE,0);



sprintf(bros,"%s:",gettext("Оператор"));

label=gtk_label_new(bros);
gtk_box_pack_start(GTK_BOX(vbox_text1), label, FALSE, FALSE,0);
sprintf(bros,"%d %s %-.*s",nom_op,iceb_u_getlogin(),iceb_u_kolbait(20,fioop),fioop);

label=gtk_label_new(bros);
gtk_box_pack_start(GTK_BOX(vbox_text2), label, FALSE, FALSE,0);


//Кнопки

data.knopka[SMENABAZ]=gtk_button_new_with_label(gettext("База"));
//gtk_button_set_alignment(GTK_BUTTON(data.knopka[SMENABAZ]),0.,0.); //Левое выравнивание текста
g_signal_connect(data.knopka[SMENABAZ],"clicked",G_CALLBACK(iceb_menu_start_knopka),&data);
gtk_widget_set_name(data.knopka[SMENABAZ],iceb_u_inttochar(SMENABAZ));
gtk_box_pack_start(GTK_BOX(vbox_text1), data.knopka[SMENABAZ], FALSE, FALSE, 0);
gtk_widget_set_tooltip_text(data.knopka[SMENABAZ],gettext("Сменить базу"));

GtkWidget *hbox_v_text1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox_v_text1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start(GTK_BOX(vbox_text2), hbox_v_text1, FALSE, FALSE,0);

sprintf(bros,"%s/%s",imabaz,kodirovka_iceb.ravno());

label=gtk_label_new(bros);
gtk_box_pack_start(GTK_BOX(hbox_v_text1), label, FALSE, FALSE,0);


/*разделитель между шапкой и кнопками*/
GtkWidget *event_razd=gtk_event_box_new();
gtk_box_pack_start(GTK_BOX(vbox),event_razd,FALSE,FALSE,0);
/**********
gdk_rgba_parse(&color1,"#E7E7E8");
gtk_widget_override_background_color(event_razd,GTK_STATE_FLAG_NORMAL,&color1);
*************/

GtkWidget *hbox_razd=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox_razd),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(event_razd),hbox_razd);

sprintf(bros,"%s %s %s %s %s",gettext("Версия"),version,gettext("от"),dataver,gettext("г."));
label=gtk_label_new(bros);
gtk_box_pack_start(GTK_BOX(hbox_razd), label, FALSE, FALSE,10);

label=gtk_label_new(gettext("О программе"));
data.knopka[OPROG]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data.knopka[OPROG]),label);

gtk_button_set_relief(GTK_BUTTON(data.knopka[OPROG]),GTK_RELIEF_NONE);
g_signal_connect(data.knopka[OPROG],"clicked",G_CALLBACK(iceb_menu_start_knopka),&data);
gtk_widget_set_name(data.knopka[OPROG],iceb_u_inttochar((OPROG*-1)));
gtk_box_pack_end(GTK_BOX(hbox_razd),data.knopka[OPROG], FALSE, FALSE,0);

GtkWidget *hbox_knop=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox_knop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start(GTK_BOX(vbox), hbox_knop, TRUE, TRUE,5);

GtkWidget *vbox_knop=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox_knop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start(GTK_BOX(hbox_knop), vbox_knop, FALSE, FALSE,5);

int kolih_str_menu=menustr->kolih();

using namespace std;
vector <GtkWidget*> knopka(kolih_str_menu);

for(ptrdiff_t nom=0; nom < kolih_str_menu; nom++)
 {
  label=gtk_label_new(menustr->ravno(nom));
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  knopka[nom]=gtk_button_new();
  gtk_container_add (GTK_CONTAINER (knopka[nom]), label);
  
  g_signal_connect(knopka[nom],"clicked",G_CALLBACK(iceb_menu_start_knopka),&data);
  gtk_widget_set_name(knopka[nom],iceb_u_inttochar((nom+1)));
  gtk_box_pack_start(GTK_BOX(vbox_knop),knopka[nom], FALSE, FALSE,0);
 }

label=gtk_label_new(gettext("Выход"));
gtk_widget_set_halign (label, GTK_ALIGN_START);
data.knopka[VIHOD]=gtk_button_new();
gtk_container_add (GTK_CONTAINER (data.knopka[VIHOD]), label);

g_signal_connect(data.knopka[VIHOD],"clicked",G_CALLBACK(iceb_menu_start_knopka),&data);
gtk_widget_set_name(data.knopka[VIHOD],iceb_u_inttochar((VIHOD*-1)));
gtk_box_pack_end(GTK_BOX(vbox_knop), data.knopka[VIHOD], FALSE, FALSE, 0);

if(nomstr_menu-VIHOD < 0)
 gtk_widget_grab_focus(knopka[0]);
else
 if(nomstr_menu-VIHOD >= 0 && nomstr_menu-VIHOD < kolih_str_menu)
   gtk_widget_grab_focus(knopka[nomstr_menu-VIHOD]);

/*завершающая строка*/
GtkWidget *event_end=gtk_event_box_new();
gtk_box_pack_end(GTK_BOX(vbox),event_end,FALSE,FALSE,0);
/*****************
gdk_rgba_parse(&color1,"#000000");
gtk_widget_override_background_color(event_end,GTK_STATE_FLAG_NORMAL,&color1);
***************/
GtkWidget *hbox_end=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox_end),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(event_end),hbox_end);

/*вставляем флаг*/
pixbuf=gdk_pixbuf_new_from_xpm_data(flag_ua_xpm);
GtkWidget *flag=gtk_image_new_from_pixbuf(pixbuf);
gtk_box_pack_start(GTK_BOX(hbox_end), flag, FALSE, FALSE,10);


data.knopka[COPYRIGTH]=gtk_button_new();
gtk_box_pack_start(GTK_BOX(hbox_end), data.knopka[COPYRIGTH], FALSE, FALSE,0);
gtk_button_set_relief(GTK_BUTTON(data.knopka[COPYRIGTH]),GTK_RELIEF_NONE);
g_signal_connect(data.knopka[COPYRIGTH],"clicked",G_CALLBACK(iceb_menu_start_knopka),&data);
gtk_widget_set_name(data.knopka[COPYRIGTH],iceb_u_inttochar((COPYRIGTH*-1)));
gtk_widget_set_tooltip_text(data.knopka[COPYRIGTH],gettext("Просмотр фотографии разработчика"));

sprintf(bros,"%s © %s 1992-2020",gettext("Автор"),gettext("Александр Белых"));
label=gtk_label_new(bros);
//iceb_label_set_text_color(label,bros,"gray"); /*серый цвет*/

gtk_container_add(GTK_CONTAINER(data.knopka[COPYRIGTH]),label);

//pango_font_description_free(font_pango);

gtk_widget_show_all (data.window);

gtk_main();

return(data.nomkn);
}

