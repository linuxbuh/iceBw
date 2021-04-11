/*$Id: i_sbtaxi.c,v 1.16 2013/08/13 06:10:23 sasa Exp $*/
/*19.07.2016	24.09.2003	Белых А.И.	i_sbtaxi.c
Создание базы данных
*/
#define         DVERSIQ "05.09.2008"
#include   <stdlib.h>
#include   <errno.h>
#include <time.h>
#include        <sys/stat.h>
#include        <locale.h>
#include        <unistd.h>
#include        "../iceBw.h"
#include   "taxi.h"


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
  E_PUTNANSI,
  E_KODIROVKA,
  E_KODIROVKA_ZAPROSOV,
  KOLENTER
 };

 
class  sbtaxi_data
 {
  public:
  iceb_u_str  imabaz;
  iceb_u_str  host;
  iceb_u_str  parol;
  class iceb_u_str putnansi;
  class iceb_u_str kodirovka;
  class iceb_u_str kodirovka_zaprosov;      
  GtkWidget *entry[KOLENTER];
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];

  sbtaxi_data()
   {
    imabaz.new_plus("");
    parol.new_plus("");
    host.new_plus("localhost");
    putnansi.new_plus("./bx"); 
    kodirovka.new_plus("koi8u");
    kodirovka_zaprosov.new_plus("utf8");
   }
  
 };


void    sbtaxi_knop(GtkWidget *,struct sbtaxi_data *);
gboolean   sbtaxi_key_press(GtkWidget *,GdkEventKey *,struct sbtaxi_data *);
void   sbtaxi_soz(struct sbtaxi_data *);
void    sbtaxi_vvod(GtkWidget *widget,class sbtaxi_data *data);
void i_sbtaxi_r(const char *imabaz,const char *host,const char *parol,const char *putnansi,const char *kodirovka,const char *kodirovka_zaprosov,GtkWidget *wpredok);

extern SQL_baza        bd;
const char *name_system={"i_sbtaxi"};
const char *version={"VERSION"};

int main(int argc,char **argv)
{
char	bros[512];
class sbtaxi_data data;


(void)setlocale(LC_ALL,"");
umask(0117); /*Установка маски для записи и чтения группы*/

gtk_init( &argc, &argv );
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем разделяется целая и дробная часть числа


data.window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,"Программа создания базы");
gtk_window_set_title(GTK_WINDOW(data.window),bros);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(sbtaxi_key_press),&data);

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox1=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GtkWidget *hbox1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox2),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GtkWidget *hbox3=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox3),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox4=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox4),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GtkWidget *hbox5=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox5),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox6=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox6),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GtkWidget *hbox7=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox7),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox8=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox8),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GtkWidget *hbox9=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox9),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox10=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox10),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GtkWidget *hbox11=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox11),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox12=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox12),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

iceb_u_str repl;
repl.new_plus("Создание базы данных для программы диспетчеризации такси.");
repl.ps_plus("Версия:");
repl.plus(VERSION);
repl.plus(" от ");
repl.plus(DVERSIQ);

GtkWidget *label=gtk_label_new(repl.ravno());

gtk_container_add(GTK_CONTAINER(data.window),vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hbox, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (hbox),vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox),vbox2, TRUE, TRUE, 0);

gtk_box_pack_start (GTK_BOX (vbox1),hbox1, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),hbox2, TRUE, TRUE, 0);

gtk_box_pack_start (GTK_BOX (vbox1),hbox3, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),hbox4, TRUE, TRUE, 0);

gtk_box_pack_start (GTK_BOX (vbox1),hbox5, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),hbox6, TRUE, TRUE, 0);

gtk_box_pack_start (GTK_BOX (vbox1),hbox7, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),hbox8, TRUE, TRUE, 0);

gtk_box_pack_start (GTK_BOX (vbox1),hbox9, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),hbox10, TRUE, TRUE, 0);

gtk_box_pack_start (GTK_BOX (vbox1),hbox11, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),hbox12, TRUE, TRUE, 0);



//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(vbox),5);

label=gtk_label_new("Имя базы данных");
gtk_box_pack_end (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
data.entry[E_IMABAZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_IMABAZ]),20);
gtk_box_pack_start (GTK_BOX (hbox2),data.entry[E_IMABAZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_IMABAZ], "activate",G_CALLBACK(sbtaxi_vvod),&data);
gtk_widget_set_name(data.entry[E_IMABAZ],iceb_u_inttochar(E_IMABAZ));

label=gtk_label_new("Хост");
data.entry[E_HOST] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_HOST]),60);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HOST]),data.host.ravno());
g_signal_connect(data.entry[E_HOST], "activate",G_CALLBACK(sbtaxi_vvod),&data);
gtk_box_pack_end (GTK_BOX (hbox3), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4),data.entry[E_HOST], TRUE, TRUE, 0);
gtk_widget_set_name(data.entry[E_HOST],iceb_u_inttochar(E_HOST));

label=gtk_label_new("Пароль администратора базы данных");
gtk_box_pack_end (GTK_BOX (hbox5), label, FALSE, FALSE, 0);

data.entry[E_PAROL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PAROL]),20);
gtk_entry_set_visibility(GTK_ENTRY(data.entry[E_PAROL]),FALSE); //невидимый ввод
g_signal_connect(data.entry[E_PAROL], "activate",G_CALLBACK(sbtaxi_vvod),&data);
gtk_box_pack_start (GTK_BOX (hbox6),data.entry[E_PAROL], TRUE, TRUE, 0);
gtk_widget_set_name(data.entry[E_PAROL],iceb_u_inttochar(E_PAROL));

label=gtk_label_new("Путь на НСИ");
gtk_box_pack_end(GTK_BOX (hbox7), label, FALSE, FALSE, 0);

data.entry[E_PUTNANSI] = gtk_entry_new();
g_signal_connect(data.entry[E_PUTNANSI], "activate",G_CALLBACK(sbtaxi_vvod),&data);
gtk_box_pack_start (GTK_BOX (hbox8),data.entry[E_PUTNANSI], TRUE, TRUE, 0);
gtk_widget_set_name(data.entry[E_PUTNANSI],iceb_u_inttochar(E_PUTNANSI));
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PUTNANSI]),data.putnansi.ravno());

label=gtk_label_new("Кодировка хранения данных");
gtk_box_pack_end(GTK_BOX (hbox9), label, FALSE, FALSE, 0);

data.entry[E_KODIROVKA] = gtk_entry_new();
g_signal_connect(data.entry[E_KODIROVKA], "activate",G_CALLBACK(sbtaxi_vvod),&data);
gtk_box_pack_start (GTK_BOX (hbox10),data.entry[E_KODIROVKA], TRUE, TRUE, 0);
gtk_widget_set_name(data.entry[E_KODIROVKA],iceb_u_inttochar(E_KODIROVKA));
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODIROVKA]),data.kodirovka.ravno());

label=gtk_label_new("Кодировка запросов к базе данных");
gtk_box_pack_end(GTK_BOX (hbox11), label, FALSE, FALSE, 0);

data.entry[E_KODIROVKA_ZAPROSOV] = gtk_entry_new();
g_signal_connect(data.entry[E_KODIROVKA_ZAPROSOV], "activate",G_CALLBACK(sbtaxi_vvod),&data);
gtk_box_pack_start (GTK_BOX (hbox12),data.entry[E_KODIROVKA_ZAPROSOV], TRUE, TRUE, 0);
gtk_widget_set_name(data.entry[E_KODIROVKA_ZAPROSOV],iceb_u_inttochar(E_KODIROVKA_ZAPROSOV));
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODIROVKA_ZAPROSOV]),data.kodirovka_zaprosov.ravno());



data.knopka[FK2]=gtk_button_new_with_label ("F2 Создать базу.");
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(sbtaxi_knop),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start (GTK_BOX (hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

data.knopka[FK10]=gtk_button_new_with_label ("F10 Выход.");
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(sbtaxi_knop),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start (GTK_BOX (hboxknop), data.knopka[FK10], TRUE, TRUE, 0);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_widget_show_all (data.window);

gtk_main();


}
/*************************/
/*Обработчик кнопок      */
/**************************/

void    sbtaxi_knop(GtkWidget *widget,struct sbtaxi_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch ((gint)knop)
 {
  case FK2:
   sbtaxi_soz(data);
   return;
   
  case FK10:
    gtk_widget_destroy(data->window);
    return;

 }



}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   sbtaxi_key_press(GtkWidget *widget,GdkEventKey *event,struct sbtaxi_data *data)
{

switch(event->keyval)
 {
  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

 }
return(TRUE);
}

/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    sbtaxi_vvod(GtkWidget *widget,class sbtaxi_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

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
  case E_PUTNANSI:
    data->putnansi.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KODIROVKA:
    data->kodirovka.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODIROVKA_ZAPROSOV:
    data->kodirovka_zaprosov.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}

/***************************************/
/*Рзметка базы данных       */
/***************************************/

void   sbtaxi_soz(struct sbtaxi_data *data)
{

for(int i=0; i < KOLENTER; i++)
  g_signal_emit_by_name(data->entry[i],"activate");


//printf("sbtaxi_soz- imabaz=%s %s\n",data->imabaz,data->parol);

/*Проверяем возможность открытия настроечного файла*/   
class iceb_u_str imafilprov(data->putnansi.ravno());
imafilprov.plus(G_DIR_SEPARATOR_S,"taxinast.alx");

FILE *ff;
if((ff = fopen(imafilprov.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(imafilprov.ravno(),gettext("Неправильно указан путь откуда брать настроечные файлы!"),errno,data->window);
  return;
 }
fclose(ff);

char bros[512];
if(data->imabaz.getdlinna() <= 1)
 {
  iceb_u_str SP;
  strcpy(bros,"Не введено имя базы данных !");
  SP.plus(bros);
  iceb_menu_soob(&SP,NULL);
  return;
 }
/***********
if(data->parol.getdlinna() <= 1)
 {
  iceb_u_str SP;
  strcpy(bros,"Не введен пароль !");
  SP.plus(bros);
  iceb_menu_soob(&SP,NULL);
 }
************/
if(data->host.getdlinna() <= 1)
 {
  iceb_u_str SP;
  strcpy(bros,"Не введен хост !");
  SP.plus(bros);
  iceb_menu_soob(&SP,NULL);
  return;
 }

i_sbtaxi_r(data->imabaz.ravno(),data->host.ravno(),data->parol.ravno(),data->putnansi.ravno(),data->kodirovka.ravno(),data->kodirovka_zaprosov.ravno(),data->window);

}
