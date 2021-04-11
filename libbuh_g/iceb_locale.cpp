/*$Id: iceb_locale.c,v 1.2 2013/05/17 14:58:35 sasa Exp $*/
/*21.10.2011	17.10.2011	Белых А.И.	iceb_locale.c
Включение локализации
*/
#include "iceb_libbuh.h"

// название файла с переводом для нашего приложения
#define ICEB_LOCALE "iceBw"

// каталог, где лежат файлы с переводами
#ifdef WIN32 // для Windows подразумевается, что приложение лежит в папке bin, а рядом в папке share/locale находятся переводы
#define PACKAGE_LOCALE_DIR "../share/locale"
#else
#define PACKAGE_LOCALE_DIR "/usr/share/locale"
#endif
extern class iceb_u_str kodirovka_iceb;

void iceb_locale()
{
// настройка папок с переводами;
bindtextdomain( ICEB_LOCALE,PACKAGE_LOCALE_DIR );//установить каталог, содержащий переводы
// кодировка перевода;
bind_textdomain_codeset (ICEB_LOCALE,kodirovka_iceb.ravno());// Устанавливаем кодировку получаемых сообщений
textdomain(ICEB_LOCALE);// устанавливаем файл из которого будут браться строки для переводов
}
