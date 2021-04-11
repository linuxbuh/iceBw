/*$Id: iceb_menu_mv.c,v 1.24 2013/08/25 08:27:07 sasa Exp $*/
/*23.05.2016	07.01.2004	Белых А.И.	iceb_menu_mv.c
Стандартное меню выбора
Возвращаем номер нажатой кнопки или -1 если кнопка не выбрана

*/
#include "iceb_libbuh.h"
class iceb_u_str iceb_menu_mv_str1("");
class iceb_u_str iceb_menu_mv_str2("");

gboolean menu_mv_event_delete(GtkWidget *window,GdkEvent *event,class vibknop_data *data);

int  iceb_menu_mv(class iceb_u_str *titl,class iceb_u_str *zagolovok,class iceb_u_spisok *punkt_menu,
int nomer, //Номер строки в меню куда надо установить фокус
GtkWidget *wpredok)
{
return(iceb_menu_mv(titl->ravno(),zagolovok->ravno(),punkt_menu,nomer,wpredok));

}

/***************************************************/

int  iceb_menu_mv(const char *titl,const char *zagolovok,class iceb_u_spisok *punkt_menu,
int nomer, //Номер строки в меню куда надо установить фокус
GtkWidget *wpredok)
{
//printf("iceb_menu_mv\n");
vibknop_data data;
char bros[512];
//GdkColor color;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


sprintf(bros,"%s %s",iceb_get_namesystem(),titl);

gtk_window_set_title (GTK_WINDOW (data.window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data.window),0);
//gtk_window_set_default_size(GTK_WINDOW(data.window),367,550); //горизонталь-вертикаль

g_signal_connect(data.window,"delete_event",G_CALLBACK(menu_mv_event_delete),&data);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_vihod),&data.knopka);

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(data.window),vbox);

GtkWidget *hbox_hap=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox_hap),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start(GTK_BOX(vbox), hbox_hap, FALSE, FALSE,10);

GtkWidget *label=gtk_label_new(NULL);

iceb_label_set_style_text(label,"font-size=\"large\" font-weight=\"bold\"",zagolovok);

gtk_box_pack_start(GTK_BOX(hbox_hap),label,FALSE,FALSE,10);

/*разделитель между шапкой и кнопками*/
GtkWidget *event_razd=gtk_event_box_new();
gtk_box_pack_start(GTK_BOX(vbox),event_razd,FALSE,FALSE,0);
/*************
GdkRGBA color1;
gdk_rgba_parse(&color1,"#E7E7E8");
gtk_widget_override_background_color(event_razd,GTK_STATE_FLAG_NORMAL,&color1);
***********/
//gdk_color_parse("#E7E7E8",&color);
//gtk_widget_modify_bg(event_razd,GTK_STATE_NORMAL,&color);

GtkWidget *hbox_razd=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox_razd),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(event_razd),hbox_razd);

//label=gtk_label_new(iceb_menu_mv_str1.ravno());
label=gtk_label_new(NULL);
iceb_label_set_style_text(label,"font-size=\"large\"",iceb_menu_mv_str1.ravno());

gtk_box_pack_start(GTK_BOX(hbox_razd), label, FALSE, FALSE,10);



GtkWidget *hbox_knop=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox_knop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start(GTK_BOX(vbox), hbox_knop, TRUE, TRUE,10);

GtkWidget *vbox_knop=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox_knop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start(GTK_BOX(hbox_knop), vbox_knop, FALSE, FALSE,10);


int kolknop=punkt_menu->kolih();
GtkWidget *knopka[kolknop+1];
ptrdiff_t nom;
for(nom=0; nom < kolknop; nom++)
 {
  label=gtk_label_new(punkt_menu->ravno(nom));
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  knopka[nom]=gtk_button_new();
  gtk_container_add (GTK_CONTAINER (knopka[nom]), label);
//  knopka[nom]=gtk_button_new_with_label(punkt_menu->ravno(nom));
//  gtk_button_set_alignment(GTK_BUTTON(knopka[nom]),0.,0.); /*Левое выравнивание текста*/
  gtk_box_pack_start(GTK_BOX(vbox_knop),knopka[nom],FALSE,FALSE,0);
  g_signal_connect(knopka[nom],"clicked",G_CALLBACK(iceb_menu_vibknop),&data);
  gtk_widget_set_name(knopka[nom],iceb_u_inttochar(nom));
 }
sprintf(bros,"%s F10",gettext("Выход"));
label=gtk_label_new(bros);
gtk_widget_set_halign (label, GTK_ALIGN_START);
knopka[nom]=gtk_button_new();
gtk_container_add (GTK_CONTAINER (knopka[nom]), label);

//knopka[nom]=gtk_button_new_with_label(bros);
//gtk_button_set_alignment(GTK_BUTTON(knopka[nom]),0.,0.); /*Левое выравнивание текста*/
gtk_box_pack_end(GTK_BOX(vbox_knop),knopka[nom],FALSE,FALSE,0);
g_signal_connect(knopka[nom],"clicked",G_CALLBACK(iceb_menu_vibknop),&data);
gtk_widget_set_name(knopka[nom],iceb_u_inttochar(nom));


/*завершающая строка*/
GtkWidget *event_end=gtk_event_box_new();
gtk_box_pack_end(GTK_BOX(vbox),event_end,FALSE,FALSE,0);
/*****************
gdk_rgba_parse(&color1,"#000000");
gtk_widget_override_background_color(event_end,GTK_STATE_FLAG_NORMAL,&color1);
****************/
//gdk_color_parse("#000000",&color);
//gtk_widget_modify_bg(event_end,GTK_STATE_NORMAL,&color);

GtkWidget *hbox_end=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox_end),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(event_end),hbox_end);
label=gtk_label_new(iceb_menu_mv_str2.ravno());

//iceb_label_set_text_color(label,iceb_menu_mv_str2.ravno(),"white");

gtk_box_pack_start(GTK_BOX(hbox_end), label, FALSE, FALSE,10);




gtk_widget_grab_focus(knopka[nomer]);




gtk_widget_show_all(data.window);
/****************
gdk_rgba_parse(&color1,"white");
gtk_widget_override_background_color(data.window,GTK_STATE_FLAG_NORMAL,&color1);
************/
//gdk_color_parse("white",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


gtk_main();

if(data.knopka == kolknop)
  data.knopka=-1;

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
  
return(data.knopka);

}
/****************************/
/*Выход по delete_event     */
/*****************************/
gboolean menu_mv_event_delete(GtkWidget *window,GdkEvent *event,class vibknop_data *data)
{
 gtk_widget_destroy(data->window);
 data->knopka=-1;
 return(FALSE); 
}

