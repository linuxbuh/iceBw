/*$Id: i_taxi.c,v 1.46 2013/09/26 09:47:12 sasa Exp $*/
/*23.05.2016	24.09.2003	Белых А.И.	i_taxi.c
Программа для работы диспетчеров такси
*/
#define         DVERSIQ "03.08.2013"
#include        <stdio.h>
#include        <stdlib.h>
#include        <time.h>
#include        <pwd.h>
#include        <unistd.h>
#include        <sys/stat.h>
#include        <string.h>
#include        <locale.h>
#include        "taxi.h"
#include        "../iceBw.h"
#include        "car.xpm"
enum
 {
  RABKLIENT,
  POSTKLIENT,
  VODITELI,
  KODZAV,
  ADRECAU,
  OTHET,
  FILNAST,
  VIHOD,
  KOLKNOP
 };
 
struct vmenu_data
 {
  GtkWidget *label;
  GtkWidget *window;
  GtkWidget *hrift;
  int       knopka;
 };

int      glmenu(int);
void     vmenu_knopka(GtkWidget *,struct vmenu_data *);
gboolean vmenu_destroy(GtkWidget *,GdkEvent *,int *);
void     l_zaktaxi(GtkWidget *wpredok);
int l_taxitel(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
void  hrift_des(GtkWidget *,struct vmenu_data *);
void     taxi_oth(void);
int      taxinast(void);
extern char		*imabaz;
extern SQL_baza	bd;
extern char		*host;

const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};

extern short start_god_taxi;
uid_t kod_operatora=getuid(); //Нужен для подпрограмм которые работают с i_rest где код оператора определяестя из файла настройки

#ifdef READ_CARD
//tdcon           td_server; 
#endif

int main(int argc,char **argv)
{
int		kom=0;


iceb_start(argc,argv);

kom=0;

if(iceb_nastsys() != 0)
 iceb_exit(1);

if(taxinast() != 0)
 iceb_exit(1);
/***************
if(organ == NULL)
 {
  organ=new char[50];
  strcpy(organ,"Диспетчиризация такси");
 }
**************/
while( kom >= 0 && kom < KOLKNOP-1 )
 {
  kom=glmenu(kom);
//  printf("%s-kom=%d KOLKNOP=%d\n",__FUNCTION__,kom,KOLKNOP);
  class iceb_u_str kod("");
  class iceb_u_str naim("");  
  switch(kom)
   {
    case  RABKLIENT:
     l_zaktaxi(NULL);
     break;

    case POSTKLIENT:
     l_taxisk(0,&kod,&naim,NULL);
     break;

    case VODITELI:
     l_vod(0,&kod,&naim,NULL);
     break;

    case KODZAV:
      l_taxikz(0,&kod,&naim,NULL);
     break;

    case ADRECAU:
      l_taxitel(0,&kod,&naim,NULL);
     break;

    case FILNAST:
     iceb_f_redfil("taxinast.alx",2,NULL);
     break;

    case OTHET:
     taxi_oth();
     break;

   }

 }    

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );

}

/***************************************/
/*Главное меню                         */
/***************************************/
int       glmenu(int knfokus)
{
vmenu_data data;
struct  tm      *bf;
time_t  tmm;
iceb_u_str NADPIS;
char     bros[312];

memset(&data,'\0',sizeof(data));

//printf("glmenu\n");

time(&tmm);
bf=localtime(&tmm);


short ddd=bf->tm_mday;
short mmm=bf->tm_mon+1;
short ggg=bf->tm_year+1900;

iceb_infosys(host,VERSION,DVERSIQ,ddd,mmm,ggg,imabaz,&NADPIS,0);

data.window = gtk_window_new( GTK_WINDOW_TOPLEVEL );

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,"Диспечеризация такси");
gtk_window_set_title(GTK_WINDOW(data.window),bros);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);
/******************
gtk_window_set_default_size(GTK_WINDOW(data.window),
    500, //Горизонталь
    500); //вертикаль
*******************/
//gtk_widget_show(data.window); //Отрисовываем, чтобы можно было взять цвет

g_signal_connect(data.window,"delete_event",G_CALLBACK(vmenu_destroy),&data.knopka);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_vihod),&data.knopka);

GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox1=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window),hbox);
gtk_container_add(GTK_CONTAINER(hbox),vbox);
gtk_container_add(GTK_CONTAINER(hbox),vbox1);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(vbox),5);

gtk_widget_realize(data.window);

GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(car);
GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);
gtk_container_add(GTK_CONTAINER(vbox1),znak);



sprintf(bros,"%s\n%s %d","Программа диспечеризации такси","Стартовый год",start_god_taxi);
data.label=gtk_label_new(bros);



gtk_box_pack_start((GtkBox*)vbox,data.label,FALSE,FALSE,0);


GtkWidget *knopka[KOLKNOP];
GtkWidget *labelk=gtk_label_new("Работа с клиентами");



knopka[RABKLIENT]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[RABKLIENT]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[RABKLIENT],TRUE,TRUE,0);
g_signal_connect(knopka[RABKLIENT],"clicked",G_CALLBACK(vmenu_knopka),&data);
gtk_widget_set_name(knopka[RABKLIENT],iceb_u_inttochar(RABKLIENT));


labelk=gtk_label_new("Постоянные клиенты");

knopka[POSTKLIENT]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[POSTKLIENT]),labelk);
//knopka[POSTKLIENT]=gtk_button_new_with_label("Постоянные клиенты");
gtk_box_pack_start((GtkBox*)vbox,knopka[POSTKLIENT],TRUE,TRUE,0);
g_signal_connect(knopka[POSTKLIENT],"clicked",G_CALLBACK(vmenu_knopka),&data);
gtk_widget_set_name(knopka[POSTKLIENT],iceb_u_inttochar(POSTKLIENT));

labelk=gtk_label_new("Водители");
knopka[VODITELI]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[VODITELI]),labelk);
//knopka[VODITELI]=gtk_button_new_with_label("Водители");
gtk_box_pack_start((GtkBox*)vbox,knopka[VODITELI],TRUE,TRUE,0);
g_signal_connect(knopka[VODITELI],"clicked",G_CALLBACK(vmenu_knopka),&data);
gtk_widget_set_name(knopka[VODITELI],iceb_u_inttochar(VODITELI));

labelk=gtk_label_new("Коды завершения");
knopka[KODZAV]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[KODZAV]),labelk);
//knopka[KODZAV]=gtk_button_new_with_label("Коды завершения");
gtk_box_pack_start((GtkBox*)vbox,knopka[KODZAV],TRUE,TRUE,0);
g_signal_connect(knopka[KODZAV],"clicked",G_CALLBACK(vmenu_knopka),&data);
gtk_widget_set_name(knopka[KODZAV],iceb_u_inttochar(KODZAV));

labelk=gtk_label_new("Адреса установки телефонов");
knopka[ADRECAU]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[ADRECAU]),labelk);
//knopka[ADRECAU]=gtk_button_new_with_label("Адреса установки телефонов");
gtk_box_pack_start((GtkBox*)vbox,knopka[ADRECAU],TRUE,TRUE,0);
g_signal_connect(knopka[ADRECAU],"clicked",G_CALLBACK(vmenu_knopka),&data);
gtk_widget_set_name(knopka[ADRECAU],iceb_u_inttochar(ADRECAU));

labelk=gtk_label_new("Получение отчётов");
knopka[OTHET]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[OTHET]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[OTHET],TRUE,TRUE,0);
g_signal_connect(knopka[OTHET],"clicked",G_CALLBACK(vmenu_knopka),&data);
gtk_widget_set_name(knopka[OTHET],iceb_u_inttochar(OTHET));

labelk=gtk_label_new("Установка стартового года");
knopka[FILNAST]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[FILNAST]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[FILNAST],TRUE,TRUE,0);
g_signal_connect(knopka[FILNAST],"clicked",G_CALLBACK(vmenu_knopka),&data);
gtk_widget_set_name(knopka[FILNAST],iceb_u_inttochar(FILNAST));


labelk=gtk_label_new("Выход");
knopka[VIHOD]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[VIHOD]),labelk);
//knopka[VIHOD]=gtk_button_new_with_label("Выход");
gtk_box_pack_start((GtkBox*)vbox,knopka[VIHOD],TRUE,TRUE,0);
g_signal_connect(knopka[VIHOD],"clicked",G_CALLBACK(vmenu_knopka),&data);
gtk_widget_set_name(knopka[VIHOD],iceb_u_inttochar(VIHOD));

GtkWidget *label = gtk_label_new (NADPIS.ravno());

gtk_box_pack_start (GTK_BOX (vbox1), label, TRUE, TRUE, 0);

gtk_widget_grab_focus(knopka[knfokus]);


gtk_widget_show_all( data.window );
//pango_font_description_free(font_pango);

gtk_main();


return(data.knopka);

}



/***************************/
/*Обработчик нажатия кнопок*/
/***************************/

void   vmenu_knopka(GtkWidget *widget,struct vmenu_data *data)
{

//int knop=GPOINTER_TO_INT(gtk_widget_get_name(widget);
//int knop=atoi(gtk_widget_get_name(widget));

int knop=atoi(gtk_widget_get_name(widget));

data->knopka=(int)knop;

gtk_widget_destroy(data->window);


}
/*************************************/
/*Обработчик  сигнала destroy_event*/
/************************************/
gboolean vmenu_destroy(GtkWidget *widget,GdkEvent *event,int *knopka)
{

*knopka=-1;

gtk_widget_destroy(widget);
return(FALSE);
}

/****************************/
