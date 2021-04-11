/*$Id: i_pros_vv.c,v 1.16 2013/08/13 06:10:23 sasa Exp $*/
/*23.05.2016	17.08.2006	Белых А.И.	i_pros_vv.c
Просмотр записей входов-выходов
*/
#define         DVERSIQ "02.09.2006"
#include "i_rest.h"
#include        "../xpm/iceB.xpm"
#include        "../xpm/flag.xpm"

enum
 {
  PROS_ZAP,
  UD_ZAP,
  PROS_ZAP_KAS,
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
void  glmenu_knopka(GtkWidget *widget,class glmenu_data *data);
gboolean   glmenu_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data);
void	glmenu(int *kom);

void l_vhvi(void);
void ud_zap_vv(void);
void l_spis_opl_z(int metka_ud_zap,GtkWidget *wpredok);

extern SQL_baza	bd;

const char		*name_system={NAME_SYSTEM};
extern char		*imabaz;
extern char            *putnansi;
extern char		*host ;
extern char		*parol;
const char            *version={VERSION};
uid_t kod_operatora=getuid();

int main(int argc,char **argv)
{
int		kom=0;

iceb_start(argc,argv);

if(iceb_nastsys() != 0)
 iceb_exit(1);
//if(buhnast_g() != 0)
// iceb_exit(1);
if(iceb_perzap(1,NULL))
 iceb_exit(1);
 
while( kom >= 0 && kom < VIHOD )   
 {
  glmenu(&kom);
  printf("kom=%d\n",kom);
  switch(kom)
   {
    case PROS_ZAP:
      l_vhvi();
      break;

    case UD_ZAP:
      ud_zap_vv();
      break;

  case PROS_ZAP_KAS: //Просмотр списка записей по кассе
      l_spis_opl_z(1,NULL);
      break;

    case VIHOD:
      
      break;


   }


 }    

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}

/********************************/
/*Главное меню*/
/********************************/

void	glmenu(int *kom)
{
iceb_u_str NADPIS;
class glmenu_data data;
char       bros[300];
short		dd,mm,gg;
iceb_u_poltekdat(&dd,&mm,&gg);

iceb_infosys(host,VERSION,DVERSIQ,dd,mm,gg,imabaz,&NADPIS,0);

data.window=gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,gettext("Расчёт заработной платы"));
gtk_window_set_title(GTK_WINDOW(data.window),bros);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);
/******************
gtk_window_set_default_size(GTK_WINDOW(data.window),    500, 500);
*******************/
//g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
//g_signal_connect(data.window,"delete_event",G_CALLBACK(event_delete),&KNOP);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(glmenu_key_press),&data);

GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(data.window),hbox);

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(hbox),vbox);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
gtk_widget_realize(data.window);
//gtk_widget_show(data.window);

GtkWidget *vbox1=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(iceB);
GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);

gtk_container_add(GTK_CONTAINER(vbox1),znak);

pixbuf=gdk_pixbuf_new_from_xpm_data(flag_xpm);
znak=gtk_image_new_from_pixbuf(pixbuf);

gtk_container_add(GTK_CONTAINER(vbox1),znak);

gtk_container_add(GTK_CONTAINER(hbox),vbox1);

GtkWidget *label=gtk_label_new(gettext("Просмотр записей на вход/выход"));
gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);

memset(bros,'\0',sizeof(bros));
strncpy(bros,iceb_get_pnk("00",0,NULL),40);
label=gtk_label_new(bros);
gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);

data.knopka[PROS_ZAP]=gtk_button_new_with_label(gettext("Просмотр записей на вход/выход"));
g_signal_connect(data.knopka[PROS_ZAP],"clicked",G_CALLBACK(glmenu_knopka),&data);
gtk_widget_set_name(data.knopka[PROS_ZAP],iceb_u_inttochar(PROS_ZAP));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[PROS_ZAP], TRUE, TRUE, 0);

data.knopka[UD_ZAP]=gtk_button_new_with_label(gettext("Удаление записей"));
g_signal_connect(data.knopka[UD_ZAP],"clicked",G_CALLBACK(glmenu_knopka),&data);
gtk_widget_set_name(data.knopka[UD_ZAP],iceb_u_inttochar(UD_ZAP));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[UD_ZAP], TRUE, TRUE, 0);

data.knopka[PROS_ZAP_KAS]=gtk_button_new_with_label(gettext("Просмотр списка оплат"));
g_signal_connect(data.knopka[PROS_ZAP_KAS],"clicked",G_CALLBACK(glmenu_knopka),&data);
gtk_widget_set_name(data.knopka[PROS_ZAP_KAS],iceb_u_inttochar(PROS_ZAP_KAS));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[PROS_ZAP_KAS], TRUE, TRUE, 0);


data.knopka[VIHOD]=gtk_button_new_with_label(gettext("Выход"));
g_signal_connect(data.knopka[VIHOD],"clicked",G_CALLBACK(glmenu_knopka),&data);
gtk_widget_set_name(data.knopka[VIHOD],iceb_u_inttochar(VIHOD));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[VIHOD], TRUE, TRUE,0);

label = gtk_label_new (NADPIS.ravno());

gtk_box_pack_start (GTK_BOX (vbox1), label, TRUE, TRUE, 1);


gtk_widget_grab_focus(data.knopka[*kom]);

gtk_widget_show_all (data.window);


gtk_main();

*kom=data.nomkn;

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  glmenu_knopka(GtkWidget *widget,class glmenu_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

data->nomkn=knop;
gtk_widget_destroy(data->window);
data->window=NULL;

}



/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   glmenu_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data)
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
