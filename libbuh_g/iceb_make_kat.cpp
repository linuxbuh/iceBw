/*$Id: iceb_make_kat.c,v 1.6 2014/05/07 10:58:10 sasa Exp $*/
/*02.12.2014	19.02.2009	Белых А.И.	iceb_t_make_kat.c
Создание каталога и подкаталога
Если вернули 0-каталог создан или уже существует
             1-нет
*/
#include <pwd.h>
#include <sys/stat.h>
#include        <errno.h>
#include "iceb_libbuh.h"


int iceb_make_kat(class iceb_u_str *putnakat,const char *podkat,GtkWidget *wpredok)
{
char strsql[1024];

struct passwd *rek_login;
rek_login=getpwuid(getuid());

putnakat->new_plus(rek_login->pw_dir);


putnakat->plus("/iceB");

umask(0000); 

if(mkdir(putnakat->ravno(),0775) != 0)
 {
//  printw("%d\n%d\n%d\n%d\n%d\n%d\n",EPERM,EFAULT,EACCES,ENAMETOOLONG,ENOENT,EEXIST);
  if(errno != EEXIST) //Уже есть такой каталог
   { 
    class iceb_u_str repl;
    repl.plus(gettext("Немогу создать каталог!"));
    sprintf(strsql,"%d %s",errno,strerror(errno));
    repl.ps_plus(strsql);
    repl.ps_plus(putnakat->ravno());
    iceb_menu_soob(&repl,wpredok);
    umask(0117); /*Установка маски для записи и чтения группы*/
    return(1);
   }
 }

if(podkat[0] != '\0')
 {
  putnakat->plus("/");
  putnakat->plus(podkat);

  if(mkdir(putnakat->ravno(),0775) != 0)
   {
  //  printw("%d\n%d\n%d\n%d\n%d\n%d\n",EPERM,EFAULT,EACCES,ENAMETOOLONG,ENOENT,EEXIST);
    if(errno != EEXIST) //Уже есть такой каталог
     { 
      class iceb_u_str repl;
      repl.plus(gettext("Немогу создать каталог!"));
      sprintf(strsql,"%d %s",errno,strerror(errno));
      repl.ps_plus(strsql);
      repl.ps_plus(putnakat->ravno());
      iceb_menu_soob(&repl,wpredok);
      umask(0117); /*Установка маски для записи и чтения группы*/
      return(1);
     }
   }
 }
umask(0117); /*Установка маски для записи и чтения группы*/

return(0);
}
