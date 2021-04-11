/*$Id: iceb_menu_vvod1.c,v 1.38 2013/08/25 08:27:07 sasa Exp $*/
/*25.03.2017	16.09.2003	Белых А.И.	iceb_menu_vvod1.c
Ввод строки на запрос
Если вернули 0- строка введена
             1-нет
*/
#include	"iceb_libbuh.h"

enum
 {
  FK1,
  FK2,
  FK10,
  KOL_FKL
 };
 

struct vihodvvod1
 {
  GtkWidget *window;
  GtkWidget *entry;
  GtkWidget *label_link;
  int vihod;
  class iceb_u_str text;
  GtkWidget *knopka[KOL_FKL];
 };

gboolean  vvod1_dest(GtkWidget*,GdkEvent*,struct vihodvvod1*);
void  vvod1_vihod(GtkWidget*,struct vihodvvod1*);
void vvod1_get_text(GtkWidget *widget,struct vihodvvod1 *data);
gboolean   vvod1_key_press(GtkWidget *widget,GdkEventKey *event,struct vihodvvod1 *data);


/*********************************************/
/*********************************************/
int iceb_menu_vvod1(const char *zapros,class iceb_u_str *stroka,int DLINNA,const char *helpfil,GtkWidget *wpredok)
{
class iceb_u_spisok zz;
zz.plus(zapros);

return(iceb_menu_vvod1(&zz,stroka,DLINNA,helpfil,wpredok));
}

/*********************************************/
/*********************************************/
int iceb_menu_vvod1(class iceb_u_str *zapros,class iceb_u_str *stroka,int DLINNA,const char *helpfil,GtkWidget *wpredok)
{
class iceb_u_spisok zz;
zz.plus(zapros->ravno());

return(iceb_menu_vvod1(&zz,stroka,DLINNA,helpfil,wpredok));
}

/***************************************/
/********Вторая функция*****************/
/***************************************/

int iceb_menu_vvod1(class iceb_u_spisok *zapros,class iceb_u_str *stroka,int DLINNA,const char *helpfil,GtkWidget *wpredok)
{
class iceb_u_str titl;
char bros[1024];
class vihodvvod1 data;
/*Избавляемся от перевода строки*/
if(iceb_u_polen(zapros->ravno(0),&titl,1,'\n') != 0)
 titl.new_plus(zapros->ravno(0));
 
int kolih=zapros->kolih();

data.vihod=1;
data.text.plus("");

data.window= gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW( data.window ), GTK_WIN_POS_CENTER );                      
gtk_container_set_border_width( GTK_CONTAINER( data.window ), 10 );
gtk_window_set_modal( GTK_WINDOW(data.window ) ,TRUE ); 
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/

class iceb_u_str repl(iceb_get_namesystem());
repl.plus(" ",titl.ravno());

gtk_window_set_title (GTK_WINDOW (data.window),repl.ravno());


g_signal_connect(data.window, "delete_event",G_CALLBACK(vvod1_dest), &data);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vvod1_key_press),&data);
if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

data.entry = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry),DLINNA-1);

if(stroka->getdlinna() > 1)
  gtk_entry_set_text (GTK_ENTRY (data.entry),stroka->ravno());

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window),vbox);

GtkWidget *label = NULL;
for(int i=0 ; i< kolih; i++)
 {

  label = gtk_label_new (zapros->ravno(i));
  gtk_box_pack_start(GTK_BOX (vbox),label,FALSE,FALSE,0);
 }
gtk_box_pack_start (GTK_BOX (vbox), data.entry, TRUE, TRUE, 0);

gtk_box_pack_start(GTK_BOX (vbox),hbox,FALSE,FALSE,0);
if(helpfil != NULL)
 if(helpfil[0] != '\0')
  {

   sprintf(bros,"<a href=\"%s/%s.html\" title=\"%s\">F1 %s</a>",ICEB_PUT_NA_HTML,helpfil,gettext("Просмотр документации"),gettext("Помощь"));
   data.label_link=gtk_label_new(bros);
   gtk_label_set_use_markup (GTK_LABEL (data.label_link), TRUE);
   g_signal_connect(data.label_link, "activate-current-link", G_CALLBACK (iceb_activate_link), NULL);

   data.knopka[FK1]=gtk_button_new();
   gtk_container_add (GTK_CONTAINER (data.knopka[FK1]), data.label_link);
   g_signal_connect(data.knopka[FK1],"clicked",G_CALLBACK(vvod1_vihod),&data);
   gtk_box_pack_start(GTK_BOX(hbox), data.knopka[FK1], TRUE, TRUE, 0);


  }

sprintf(bros,"FK2 %s",gettext("Ввести"));
data.knopka[FK2] = gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK2],TRUE,TRUE,2);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(vvod1_vihod), &data);

sprintf(bros,"FK10 %s",gettext("Выход"));
data.knopka[FK10] = gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK10],TRUE,TRUE,2);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(vvod1_vihod), &data);

g_signal_connect(data.entry, "activate",G_CALLBACK(vvod1_get_text),&data);

gtk_widget_show_all(data.window);
gtk_main();

if(data.vihod == 0)
 stroka->new_plus(data.text.ravno_fromutf());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.vihod);

}



/************************/
/*Перегружаемая функция*/
/*************************/

int iceb_menu_vvod1(const char *zapros,char *stroka,int DLINNA,const char *helpfil,GtkWidget *wpredok)
{
class iceb_u_str titl;
char bros[512];
class vihodvvod1 data;

/*Избавляемся от перевода строки*/
if(iceb_u_polen(zapros,&titl,1,'\n') != 0)
 titl.new_plus(zapros);

data.vihod=1;
data.text.plus("");

data.window= gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW( data.window ), GTK_WIN_POS_CENTER );                      
gtk_container_set_border_width( GTK_CONTAINER( data.window ), 10 );
//gtk_widget_set_name(data.window,iceb_u_inttochar("1")); НЕПОНЯТНО ЗАЧЕМ
gtk_window_set_modal( GTK_WINDOW(data.window ) ,TRUE ); 
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/


class iceb_u_str repl(iceb_get_namesystem());
repl.plus(" ",titl.ravno());

gtk_window_set_title (GTK_WINDOW (data.window),repl.ravno());

g_signal_connect(data.window, "delete_event",G_CALLBACK(vvod1_dest), &data);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vvod1_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label = gtk_label_new (zapros);


data.entry = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry),DLINNA-1);

if(stroka[0] != '\0')
 gtk_entry_set_text (GTK_ENTRY (data.entry),stroka);


GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window),vbox);

gtk_box_pack_start(GTK_BOX (vbox),label,FALSE,FALSE,0);

gtk_box_pack_start (GTK_BOX (vbox), data.entry, TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX (vbox),hbox,FALSE,FALSE,0);

if(helpfil != NULL)
 if(helpfil[0] != '\0')
  {

   sprintf(bros,"<a href=\"%s/%s.html\" title=\"%s\">F1 %s</a>",ICEB_PUT_NA_HTML,helpfil,gettext("Просмотр документации"),gettext("Помощь"));
   data.label_link=gtk_label_new(bros);
   gtk_label_set_use_markup (GTK_LABEL (data.label_link), TRUE);
   g_signal_connect(data.label_link, "activate-current-link", G_CALLBACK (iceb_activate_link), NULL);

   data.knopka[FK1]=gtk_button_new();
   gtk_container_add (GTK_CONTAINER (data.knopka[FK1]), data.label_link);
   g_signal_connect(data.knopka[FK1],"clicked",G_CALLBACK(vvod1_vihod),&data);
   gtk_box_pack_start(GTK_BOX(hbox), data.knopka[FK1], TRUE, TRUE, 0);


  }

sprintf(bros,"FK2 %s",gettext("Ввести"));
data.knopka[FK2] = gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK2],TRUE,TRUE,2);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(vvod1_vihod), &data);

sprintf(bros,"FK10 %s",gettext("Выход"));
data.knopka[FK10] = gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK10],TRUE,TRUE,2);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(vvod1_vihod), &data);

g_signal_connect(data.entry, "activate",G_CALLBACK(vvod1_get_text),&data);

//gtk_box_pack_start(GTK_BOX (vbox),knvihod,FALSE,FALSE,0);

//g_signal_connect(knvihod, "clicked",G_CALLBACK(vvod1_vihod), &data);
//g_signal_connect(data.entry, "activate",G_CALLBACK(vvod1_get_text),&data);

gtk_widget_show_all(data.window);
gtk_main();

if(data.vihod == 0)
 strncpy(stroka,data.text.ravno_fromutf(),DLINNA-1);
 
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.vihod);

}
/**************************************/

int iceb_menu_vvod1(iceb_u_str *zapros,char *stroka,int DLINNA,const char *helpfil,GtkWidget *wpredok)
{

return(iceb_menu_vvod1(zapros->ravno(),stroka,DLINNA,helpfil,wpredok));

}

/***************************/
/*Выход по нажатию крестика*/
/***************************/
gboolean  vvod1_dest(GtkWidget *widget,GdkEvent *event,struct vihodvvod1 *data)
{
data->vihod=1;
/*printf("vvod1_dest=%d Удаляем виджет\n",data->vihod);*/
data->text.new_plus("");
gtk_widget_destroy(data->window);
return(FALSE);
}

/*************************/
/*Выход по клавише*/
/*******************/

void  vvod1_vihod(GtkWidget *widget,struct vihodvvod1 *data)
{


int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK1:
    g_signal_emit_by_name(data->label_link,"activate-current-link");
    return;
  case FK2:
    g_signal_emit_by_name(data->entry,"activate");
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
void vvod1_get_text(GtkWidget *widget,struct vihodvvod1 *data)
{
data->text.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(widget)));
data->vihod=0;
gtk_widget_destroy(data->window);

}
/***************************/
/*Обработчик нажатия клавиш*/
/***************************/
gboolean   vvod1_key_press(GtkWidget *widget,GdkEventKey *event,struct vihodvvod1 *data)
{
switch(event->keyval)
 {
  case GDK_KEY_F1:
    g_signal_emit_by_name(data->knopka[FK1],"clicked");
    return(FALSE);

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

