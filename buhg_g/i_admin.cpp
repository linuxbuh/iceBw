/*$Id: i_admin.c,v 1.49 2014/08/31 06:19:19 sasa Exp $*/
/*27.02.2017	04.02.2009	Белых А.И.	i_admin.c
программа администрирования баз данных
*/
#define DVERSIQ "09.10.2020"
#include        <sys/stat.h>
#include        <locale.h>
#include        "../iceBw.h"
#include "buhg_g.h"
#include "../xpm/iceBw_icon.xpm"

enum
 {
  MAKEBAZ,
  DOSTUP,
  NEW_PAROL,
  SPISLOGINBAZ,
  SPISBAZ,
  ALTERBAZ,
  DEL_USER,
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

int admin_l(class iceb_u_str *parol,class iceb_u_str *login,class iceb_u_str *host,class iceb_u_str *kodirovka_iceb);


int admin_dostup(const char *admin_host);
void admin_user(GtkWidget *wpredok);
void admin_makebaz(const char *host,const char *parol);
void admin_alter(GtkWidget *wpredok);
int admin_new_parol(const char *host,GtkWidget *wpredok);
int admin_del_user(const char *host,GtkWidget *wpredok);

extern SQL_baza bd;
const char *name_system={NAME_SYSTEM};
extern class iceb_u_str kodirovka_iceb; 
const char            *version={VERSION};
extern int iceb_kod_podsystem;
class iceb_u_str admin_parol("");
class iceb_u_str admin_login("");
class iceb_u_str admin_host("");

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  glmenu_knopka(GtkWidget *widget,class glmenu_data *data)
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
/********************************/
/*Главное меню*/
/********************************/

void	glmenu(int *kom,const char *admin_login,const char *admin_host)
{
iceb_kod_podsystem=ICEB_PS_ABD;
iceb_u_str NADPIS;
class glmenu_data data;
char       bros[1024];


data.window=gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,gettext("Администрирование баз данных"));
gtk_window_set_title(GTK_WINDOW(data.window),bros);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(glmenu_key_press),&data);


GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add(GTK_CONTAINER(data.window),vbox);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(vbox),5);

sprintf(bros,"%s\n%s %s %s %s\n%s:%s\n%s:%s",
gettext("Администрирование баз данных"),
gettext("Версия"),
VERSION,
gettext("от"),
DVERSIQ,
gettext("Логин оператора"),
admin_login,
gettext("Хост на базу"),
admin_host);

GtkWidget *label=gtk_label_new(bros);
gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);



data.knopka[MAKEBAZ]=gtk_button_new_with_label(gettext("Разметить новую базу данных для iceB"));
g_signal_connect(data.knopka[MAKEBAZ],"clicked",G_CALLBACK(glmenu_knopka),&data);
gtk_widget_set_name(data.knopka[MAKEBAZ],iceb_u_inttochar(MAKEBAZ));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[MAKEBAZ], TRUE, TRUE, 0);

data.knopka[DOSTUP]=gtk_button_new_with_label(gettext("Рaзрешить доступ к базе данных"));
g_signal_connect(data.knopka[DOSTUP],"clicked",G_CALLBACK(glmenu_knopka),&data);
gtk_widget_set_name(data.knopka[DOSTUP],iceb_u_inttochar(DOSTUP));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[DOSTUP], TRUE, TRUE, 0);

data.knopka[NEW_PAROL]=gtk_button_new_with_label(gettext("Установить новый пароль для доступа к базе"));
g_signal_connect(data.knopka[NEW_PAROL],"clicked",G_CALLBACK(glmenu_knopka),&data);
gtk_widget_set_name(data.knopka[NEW_PAROL],iceb_u_inttochar(NEW_PAROL));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[NEW_PAROL], TRUE, TRUE, 0);

data.knopka[SPISLOGINBAZ]=gtk_button_new_with_label(gettext("Просмотр списка разрешённых доступов к базам данных"));
g_signal_connect(data.knopka[SPISLOGINBAZ],"clicked",G_CALLBACK(glmenu_knopka),&data);
gtk_widget_set_name(data.knopka[SPISLOGINBAZ],iceb_u_inttochar(SPISLOGINBAZ));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[SPISLOGINBAZ], TRUE, TRUE, 0);

data.knopka[SPISBAZ]=gtk_button_new_with_label(gettext("Просмотр списка размеченных баз"));
g_signal_connect(data.knopka[SPISBAZ],"clicked",G_CALLBACK(glmenu_knopka),&data);
gtk_widget_set_name(data.knopka[SPISBAZ],iceb_u_inttochar(SPISBAZ));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[SPISBAZ], TRUE, TRUE, 0);

data.knopka[ALTERBAZ]=gtk_button_new_with_label(gettext("Преобразование баз для новой версии системы"));
g_signal_connect(data.knopka[ALTERBAZ],"clicked",G_CALLBACK(glmenu_knopka),&data);
gtk_widget_set_name(data.knopka[ALTERBAZ],iceb_u_inttochar(ALTERBAZ));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[ALTERBAZ], TRUE, TRUE, 0);

data.knopka[DEL_USER]=gtk_button_new_with_label(gettext("Удаление доступа к базе"));
g_signal_connect(data.knopka[DEL_USER],"clicked",G_CALLBACK(glmenu_knopka),&data);
gtk_widget_set_name(data.knopka[DEL_USER],iceb_u_inttochar(DEL_USER));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[DEL_USER], TRUE, TRUE, 0);

data.knopka[VIHOD]=gtk_button_new_with_label(gettext("Выход"));
g_signal_connect(data.knopka[VIHOD],"clicked",G_CALLBACK(glmenu_knopka),&data);
gtk_widget_set_name(data.knopka[VIHOD],iceb_u_inttochar(VIHOD));
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[VIHOD], TRUE, TRUE,0);


gtk_widget_grab_focus(data.knopka[*kom]);

gtk_widget_show_all (data.window);


gtk_main();

*kom=data.nomkn;

}
/*******************/
/*Главная программа*/
/*******************/
int main(int argc,char **argv)
{
//char strsql[512];
int		kom=0;

kodirovka_iceb.new_plus("utf8");
umask(0117); /*Установка маски для записи и чтения группы*/

//gtk_set_locale();
gtk_init( &argc, &argv );
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем разделяется целая и дробная часть числа

iceb_locale(); /*включаем локализацию*/

/*Устанавливаем иконку для всех меню*/
GdkPixbuf *iconca=gdk_pixbuf_new_from_xpm_data(iceBw_icon_xpm);  
gtk_window_set_default_icon(iconca);




if(admin_l(&admin_parol,&admin_login,&admin_host,&kodirovka_iceb) != 0)
 return(0);

if(mysql_init(&bd) == NULL)
 {
  class iceb_u_str repl;
  repl.plus(gettext("Ошибка инициализации базы mysql"));
  repl.plus((int)sql_nerror(&bd));
  repl.plus(" ",sql_error(&bd));
  
  iceb_menu_soob(&repl,NULL);
  return(1);  
 }

if(mysql_real_connect(&bd,admin_host.ravno(),admin_login.ravno(),admin_parol.ravno(),"mysql",0,NULL,0) == NULL)
 { 
  class iceb_u_str repl(gettext("Ошибка соединения с демоном базы mysql"));
  repl.plus((int)sql_nerror(&bd));
  repl.plus(" ");
  repl.plus(sql_error(&bd));
  
  iceb_menu_soob(&repl,NULL);
  return(1);  

 } 

iceb_start_rf();

while( kom >= 0 && kom < VIHOD )   
 {
  glmenu(&kom,admin_login.ravno(),admin_host.ravno());

  class iceb_u_str imabaz("");
  class iceb_u_str kodirovka("");
  class iceb_u_str fromnsi("");
  class iceb_u_str tonsi("");
  switch(kom)
   {
    case MAKEBAZ:
      admin_makebaz(admin_host.ravno(),admin_parol.ravno());
      break;

    case DOSTUP:
      admin_dostup(admin_host.ravno());
      break;

    case NEW_PAROL:
      admin_new_parol(admin_host.ravno(),NULL);
      break;

    case SPISLOGINBAZ:
      admin_user(NULL);
      break;

    case SPISBAZ:
      admin_bases(0,&imabaz,NULL);
      break;

    case ALTERBAZ:
      admin_alter(NULL);
      break;

    case DEL_USER:
      admin_del_user(admin_host.ravno(),NULL);
      break;


    case VIHOD:
      break;

   }


 }    

sql_closebaz(&bd);

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

return( 0 );
}
