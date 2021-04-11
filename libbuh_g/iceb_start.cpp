/*$Id: iceb_start.c,v 1.25 2014/01/31 12:13:02 sasa Exp $*/
/*18.01.2016	28.03.2005	Белых А.И.	iceb_start.c
Начало работы подсистем
*/
#include        <pwd.h>
#include        <locale.h>
#include        <sys/stat.h>
#include        "iceb_libbuh.h"
#include "../xpm/iceBw_icon.xpm"


SQL_baza	bd;
char *host=NULL;
char *imabaz=NULL;
char *parol=NULL;
class iceb_u_str kodirovka_iceb;
class iceb_u_str login_op(""); /*логин оператора*/
void iceb_start(int argc,char **argv)
{
struct  passwd  *ktor; /*Кто работает*/
class iceb_u_str locale;
ktor=getpwuid(getuid());
login_op.new_plus(ktor->pw_name);

setlocale(LC_ALL,"");/*Устанавливаются переменные локали из окружающей среды*/

gtk_init( &argc, &argv ); /*здесь устанавливается и локализация*/

//Делаем обязятельно после gtk_ini
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем разделяется целая и дробная часть числа

locale.plus(setlocale(LC_MESSAGES,""));

if(iceb_u_polen(locale.ravno(),&kodirovka_iceb,2,'.') != 0)
 {
  if(iceb_u_SRAV(locale.ravno(),"ru",0) == 0)
   kodirovka_iceb.new_plus("koi8r");
  if(iceb_u_SRAV(locale.ravno(),"ru_UA",0) == 0)
   kodirovka_iceb.new_plus("koi8u");
 }

iceb_locale(); /*включаем локализацию*/

umask(0117); /*Установка маски для записи и чтения группы*/


/*************************
Пытался настроить чтение шрифта из файла
GtkCssProvider *provider= gtk_css_provider_get_default();
GFile *fil=  g_file_new_for_path ("./gtkrc_iceb");
gboolean voz=gtk_css_provider_load_from_file(provider,fil,NULL);

if(voz == TRUE)
 printf("%s-true\n",__FUNCTION__);
if(voz == FALSE)
 printf("%s-false\n",__FUNCTION__);
*************************/ 
GdkPixbuf *iconca=gdk_pixbuf_new_from_xpm_data(iceBw_icon_xpm);  
gtk_window_set_default_icon(iconca);

/*Устанавливаем иконку для всех меню из корневого каталога настроечных файлов*/
/**********
int metka_icon=0;
if(gtk_window_set_default_icon_from_file(iceb_imafnsi("iceB_icon.png"),NULL) == TRUE)
 metka_icon=1;
************/
/*Читаем ключи запуска программы*/
iceb_readkey(argc,argv,&parol,&imabaz,&host);

//printf("imabaz=%s host=%s kto=%s parol=%s\n",imabaz,host,kto,parol);
if(sql_openbaz(&bd,imabaz,host,ktor->pw_name,parol) != 0)
    iceb_eropbaz(imabaz,ktor->pw_uid,ktor->pw_name,0);

iceb_start_rf();/*читаем файл с командами и выполняем их*/


//printf("База открыта. %.2f\n",10.33);
SQLCURSOR cur;
SQL_str row;
/*Читаем наименование организации*/
char bros[512];
sprintf(bros,"select naikon from Kontragent where kodkon='00'");
if(sql_readkey(&bd,bros,&row,&cur) != 1)
 {
  iceb_vkk00(NULL);
 }

}
