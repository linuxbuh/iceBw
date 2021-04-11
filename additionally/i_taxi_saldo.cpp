/*$Id: i_taxi_saldo.c,v 1.19 2013/08/13 06:10:23 sasa Exp $*/
/*23.05.2016	31.03.2005	Белых А.И.	i_taxi_saldo.c
Программа для настройки 
*/
#define         DVERSIQ "31.03.2005"
#include        <stdlib.h>
#include        <unistd.h>
#include        <pwd.h>
#include        <locale.h>
#include        <errno.h>
#include        <sys/stat.h>
#include        "../iceBw.h"
#include        "taxi.h"
#include        "../xpm/iceB.xpm"
#include        "../xpm/flag.xpm"

enum
 {
  SALDO,
  SPISOK_KL_SL,
  DELETE,
  RAB_S_F_N,
  VIHOD,
  KOLKNOP
 };

class i_taxi_saldo_data
 {
  public:
  GtkWidget *label;
  GtkWidget *window;
  GtkWidget *hrift;
  int       knopka;
 };

gboolean i_taxi_saldo_destroy(GtkWidget *widget,GdkEvent *event,int *knopka);
void   i_taxi_saldo_knopka(GtkWidget *widget,class i_taxi_saldo_data *data);


int taxinast();
int glmenu(int knfokus);
int taxi_saldo_vibor();
int taxi_saldo_r(GtkWidget *wpredok);
void taxi_saldo_del_r(GtkWidget *wpredok);
int l_tk_saldo(GtkWidget*);
void taxi_ud_god(void);

extern char	*imabaz;
extern SQL_baza	bd;
extern char     *putnansi;
extern char	*host;
extern char	*parol;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
uid_t kod_operatora=getuid(); //Нужен для подпрограмм которые работают с i_rest где код оператора определяестя из файла настройки

int		kodpodr=0; //Код подразделения
iceb_u_str      naimpodr;
extern short start_god_taxi;

int main(int argc,char **argv)
{

char		strsql[300];

iceb_start(argc,argv);


if(kodpodr != 0)
 {
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select naik from Restpod where kod=%d",kodpodr);
  if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
   naimpodr.new_plus(row[0]);
 }
 

if(iceb_nastsys() != 0)
 iceb_exit(1);
if(taxinast() != 0)
 iceb_exit(1);

taxi_saldo_vibor();


sql_closebaz(&bd);
    
   
return( 0 );
}
/****************************************/
/*Выбор пункта меню*/
/**************************/

int taxi_saldo_vibor()
{
int kom=0;

while( kom >= 0 && kom < KOLKNOP-1 )
 {
  kom=glmenu(kom);
  switch(kom)
   {

    case  SALDO:
      taxi_saldo_r(NULL);
      
      break;

    case  SPISOK_KL_SL:
      l_tk_saldo(NULL);
      
      break;

    case DELETE:
      taxi_ud_god();
      break;

    case RAB_S_F_N:
      iceb_f_redfil("taxinast.alx",2,NULL);
      taxinast(); 
      break;


    case VIHOD:

      break;

   }

 }
return(0);
}
/***********************************/
/* Главное меню*/
/***********************************/
int glmenu(int knfokus)
{
iceb_u_str NADPIS;
char     bros[300];
i_taxi_saldo_data data;

//printf("glmenu\n");

short ddd;
short mmm;
short ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);

iceb_infosys(host,VERSION,DVERSIQ,ddd,mmm,ggg,imabaz,&NADPIS,0);

data.window = gtk_window_new( GTK_WINDOW_TOPLEVEL );

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,gettext("Перенос сальдо"));
gtk_window_set_title(GTK_WINDOW(data.window),bros);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);
/******************
gtk_window_set_default_size(GTK_WINDOW(data.window),
    500, //Горизонталь
    500); //вертикаль
*******************/
//gtk_widget_show(data.window); //Отрисовываем, чтобы можно было взять цвет

g_signal_connect(data.window,"delete_event",G_CALLBACK(i_taxi_saldo_destroy),&data.knopka);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_vihod),&data.knopka);

GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox1=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window),hbox);
gtk_container_add(GTK_CONTAINER(hbox),vbox);
gtk_container_add(GTK_CONTAINER(hbox),vbox1);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
iceb_u_str stroka;
stroka.plus(iceb_get_pnk("00",0,NULL));
stroka.ps_plus("Перенос сальдо по клиентам\nдля программы диспетчеризации такси.");
stroka.ps_plus(gettext("Стартовый год"));
stroka.plus(":");
stroka.plus(start_god_taxi);


data.label=gtk_label_new(stroka.ravno());



gtk_box_pack_start((GtkBox*)vbox,data.label,FALSE,FALSE,0);

//font_pango=pango_font_description_from_string("Sans 14");

GtkWidget *knopka[KOLKNOP];

GtkWidget *labelk=gtk_label_new(gettext("Перенос сальдо"));

knopka[SALDO]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[SALDO]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[SALDO],TRUE,TRUE,0);
g_signal_connect(knopka[SALDO],"clicked",G_CALLBACK(i_taxi_saldo_knopka),&data);
gtk_widget_set_name(knopka[SALDO],iceb_u_inttochar(SALDO));


labelk=gtk_label_new("Работа со списком сальдо по клиентам");
knopka[SPISOK_KL_SL]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[SPISOK_KL_SL]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[SPISOK_KL_SL],TRUE,TRUE,0);
g_signal_connect(knopka[SPISOK_KL_SL],"clicked",G_CALLBACK(i_taxi_saldo_knopka),&data);
gtk_widget_set_name(knopka[SPISOK_KL_SL],iceb_u_inttochar(SPISOK_KL_SL));

labelk=gtk_label_new(gettext("Удаление данных за год"));
knopka[DELETE]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[DELETE]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[DELETE],TRUE,TRUE,0);
g_signal_connect(knopka[DELETE],"clicked",G_CALLBACK(i_taxi_saldo_knopka),&data);
gtk_widget_set_name(knopka[DELETE],iceb_u_inttochar(DELETE));

labelk=gtk_label_new(gettext("Работа с файлом настройки"));
knopka[RAB_S_F_N]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[RAB_S_F_N]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[RAB_S_F_N],TRUE,TRUE,0);
g_signal_connect(knopka[RAB_S_F_N],"clicked",G_CALLBACK(i_taxi_saldo_knopka),&data);
gtk_widget_set_name(knopka[RAB_S_F_N],iceb_u_inttochar(RAB_S_F_N));

labelk=gtk_label_new(gettext("Выход."));
knopka[VIHOD]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[VIHOD]),labelk);
//knopka[VIHOD]=gtk_button_new_with_label("Выход.");
gtk_box_pack_start((GtkBox*)vbox,knopka[VIHOD],TRUE,TRUE,0);
g_signal_connect(knopka[VIHOD],"clicked",G_CALLBACK(i_taxi_saldo_knopka),&data);
gtk_widget_set_name(knopka[VIHOD],iceb_u_inttochar(VIHOD));

GtkWidget *mainvbox1=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
gtk_box_set_homogeneous (GTK_BOX(mainvbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_widget_realize(data.window);
/*********************
GtkStyle *style;
GdkPixmap *pixmap;
GdkBitmap *mask;
GtkWidget *pixmapWidget;

style = gtk_widget_get_style(data.window);
pixmap=gdk_pixmap_create_from_xpm_d(gtk_widget_get_window(data.window),&mask,&style->bg[GTK_STATE_NORMAL],
                                    (gchar **)icebr200_xpm);
pixmapWidget=gtk_pixmap_new(pixmap,mask);
gtk_container_add(GTK_CONTAINER(mainvbox1),pixmapWidget);

pixmap=gdk_pixmap_create_from_xpm_d(gtk_widget_get_window(data.window),&mask,&style->bg[GTK_STATE_NORMAL],
                                    (gchar **)flag_xpm);
pixmapWidget=gtk_pixmap_new(pixmap,mask);
gtk_container_add(GTK_CONTAINER(mainvbox1),pixmapWidget);
**********************/
GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(iceB);
GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);

gtk_container_add(GTK_CONTAINER(mainvbox1),znak);

pixbuf=gdk_pixbuf_new_from_xpm_data(flag_xpm);
znak=gtk_image_new_from_pixbuf(pixbuf);

gtk_container_add(GTK_CONTAINER(mainvbox1),znak);


gtk_container_add(GTK_CONTAINER(vbox1),mainvbox1);

GtkWidget *label_nadpis = gtk_label_new (NADPIS.ravno());

gtk_box_pack_start (GTK_BOX (vbox1), label_nadpis, TRUE, TRUE, 0);

gtk_widget_grab_focus(knopka[knfokus]);

gtk_widget_show_all( data.window );
//pango_font_description_free(font_pango);

gtk_main();


return(data.knopka);

}

/***************************/
/*Обработчик нажатия кнопок*/
/***************************/

void   i_taxi_saldo_knopka(GtkWidget *widget,class i_taxi_saldo_data *data)
{


int knop=atoi(gtk_widget_get_name(widget));

data->knopka=(int)knop;

gtk_widget_destroy(data->window);


}
/*************************************/
/*Обработчик  сигнала destroy_event*/
/************************************/
gboolean i_taxi_saldo_destroy(GtkWidget *widget,GdkEvent *event,int *knopka)
{

*knopka=-1;

gtk_widget_destroy(widget);
return(FALSE); 
}
