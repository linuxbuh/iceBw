/*$Id: iceb_zageks.c,v 1.1 2013/10/04 07:27:20 sasa Exp $*/
/*29.09.2013	29.09.2013	Белых А.И.	iceb_zageks.c
Заголовок файлов экспорта данных
*/
#include "iceb_libbuh.h"

void iceb_zageks(const char *naim,FILE *ff,GtkWidget *wpredok)
{

fprintf(ff,"#%s\n#%s %s %s\n",
naim,
gettext("Выгружено из базы"),
iceb_get_namebase(),
iceb_get_pnk("00",1,wpredok));

time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

fprintf(ff,"\
#%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"#%s:%d %s\n",gettext("Оператор"),iceb_getuid(wpredok),iceb_getfioop(wpredok));
}
