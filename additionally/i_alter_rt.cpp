/*$Id: i_alter_rt.c,v 1.25 2013/08/13 06:10:23 sasa Exp $*/
/*05.06.2008	29.04.2004	Белых А.И.	i_alter_rt.c
Программа модификации таблиц в базе данных
*/
#define   DVERSION "05.06.2008"
#include        <locale.h>
#include        <sys/stat.h>
#include <iceb_libbuh.h>
#include "../iceBw.h"
#define   NAME_SYSTEM "i_alter_rt"
#define   CONFIG_PATH "/etc/iceB"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_IMABAZ,
  E_HOST,
  E_PAROL,
  KOLENTER  
 };

 
class  alrter_rt
 {
  public:
  iceb_u_str  imabaz;
  iceb_u_str  host;
  iceb_u_str  parol;
  
  GtkWidget *entry[KOLENTER];
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  
 };

void    alrter_rt_vvod(GtkWidget *widget,class alrter_rt *data);
void  alrter_rt_knopka(GtkWidget *widget,class alrter_rt *data);
gboolean   alrter_rt_key_press(GtkWidget *widget,GdkEventKey *event,class alrter_rt *data);
void    alrter_rt_st(class alrter_rt *data);

void i_alter_rtr(const char *imabaz,const char *host,const char *parol,GtkWidget *wpredok);


extern SQL_baza	bd;
const char *name_system={NAME_SYSTEM};
const char *version={VERSION};


int main(int argc,char **argv)
{
alrter_rt data;
char bros[300];

(void)setlocale(LC_ALL,"");
umask(0117); /*Установка маски для записи и чтения группы*/

gtk_init( &argc, &argv );
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем разделяется целая и дробная часть числа

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
//gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,"Преобразовать базу");

gtk_window_set_title (GTK_WINDOW (data.window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(alrter_rt_key_press),&data);

sprintf(bros,"Преобразование таблиц базы данных\n\
для версии:%s от %s",VERSION,DVERSION);

GtkWidget *label=gtk_label_new(bros);


GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox2),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox3),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

label=gtk_label_new("Имя базы");
data.entry[E_IMABAZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_IMABAZ]),20);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_IMABAZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_IMABAZ], "activate",G_CALLBACK(alrter_rt_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMABAZ]),"");
gtk_widget_set_name(data.entry[E_IMABAZ],iceb_u_inttochar(E_IMABAZ));

label=gtk_label_new("Хост");
data.entry[E_HOST] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_HOST]),100);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_HOST], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_HOST], "activate",G_CALLBACK(alrter_rt_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HOST]),"localhost");
gtk_widget_set_name(data.entry[E_HOST],iceb_u_inttochar(E_HOST));

label=gtk_label_new("Пароль администратора базы данных");
data.entry[E_PAROL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PAROL]),20);
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_PAROL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PAROL], "activate",G_CALLBACK(alrter_rt_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PAROL]),"");
gtk_entry_set_visibility(GTK_ENTRY(data.entry[E_PAROL]),FALSE);
gtk_widget_set_name(data.entry[E_PAROL],iceb_u_inttochar(E_PAROL));


sprintf(bros,"F2 %s","Преобразовать");
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_widget_set_tooltip_text(data.knopka[FK2],"Начать преобразование таблиц базы данных");
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(alrter_rt_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(bros,"F10 %s","Выход");
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_widget_set_tooltip_text(data.knopka[FK10],"Завершение работы с программой");
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(alrter_rt_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   alrter_rt_key_press(GtkWidget *widget,GdkEventKey *event,class alrter_rt *data)
{
//char  bros[300];

//printf("alrter_rt_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");


    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  alrter_rt_knopka(GtkWidget *widget,class alrter_rt *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case FK2:
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(data->entry[i],"activate");
    alrter_rt_st(data);
    return;  


  case FK10:
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    alrter_rt_vvod(GtkWidget *widget,class alrter_rt *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("alrter_rt_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_IMABAZ:
    data->imabaz.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_HOST:
    data->host.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_PAROL:
    data->parol.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************/
/*Создание таблиц*/
/**************************/

void    alrter_rt_st(class alrter_rt *data)
{

i_alter_rtr(data->imabaz.ravno(),data->host.ravno(),data->parol.ravno(),data->window);

}
