/*$Id: iceb_podpis.c,v 1.16 2013/09/26 09:47:05 sasa Exp $*/
/*16.04.2017	09.10.2000	Белых А.И.	iceb_podpis.c
Подпись в отчётах
*/
#include        <pwd.h>
#include        "iceb_libbuh.h"

extern char     *version;

void iceb_podpis(uid_t kod_operatora,FILE *ff,GtkWidget *wpredok)
{


fprintf(ff,"\n\n%s \"%s\" %s %s",
gettext("Система"),
iceb_get_namesystem(),
gettext("Версия"),version);

class iceb_u_str bros("");
if(iceb_poldan("Печатать фамилию оператора в распечатках отчётов",&bros,"nastsys.alx",wpredok) == 0)
 if(iceb_u_SRAV(bros.ravno(),"Вкл",1) != 0)
  return;  

struct  passwd  *ktor; /*Кто работает*/

if((ktor=getpwuid(kod_operatora)) != NULL)
  {
   fprintf(ff," %s:%d %.*s",
   gettext("Распечатал"),
   kod_operatora,
   iceb_u_kolbait(40,ktor->pw_gecos),
   ktor->pw_gecos);
  }
else
  {
   fprintf(ff," %s:%d %s",
   gettext("Распечатал"),
   kod_operatora,
   gettext("Неизвестный логин"));
  }

fprintf(ff,"\n");

} 
/**************************************/
/**************************************/

void iceb_podpis(FILE *ff,GtkWidget *wpredok)
{
fprintf(ff,"\n\n%s \"%s\" %s %s",
gettext("Система"),
iceb_get_namesystem(),
gettext("Версия"),version);

class iceb_u_str bros("");
if(iceb_poldan("Печатать фамилию оператора в распечатках отчётов",&bros,"nastsys.alx",wpredok) == 0)
 if(iceb_u_SRAV(bros.ravno(),"Вкл",1) != 0)
  return;

int nomer_op=iceb_getuid(wpredok);
class iceb_u_str fioop(iceb_getfioop(wpredok));

fprintf(ff," %s:%d %.*s\n",
gettext("Распечатал"),
nomer_op,
iceb_u_kolbait(40,fioop.ravno()),
fioop.ravno());

}
