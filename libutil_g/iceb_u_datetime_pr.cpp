/*$Id: iceb_u_datetime_pr.c,v 1.6 2013/08/13 06:10:17 sasa Exp $*/
/*22.07.2013	15.01.2007	Белых А.И.	iceb_u_datetime_pr.c
Преобразование 	DATETIME в нормальное представление в символьном виде
*/
#include "iceb_util.h"

const char *iceb_u_datetime_pr(char *datatime)
{
static char datatime_pr[64];
class iceb_u_str date("");
class iceb_u_str vrem("");

iceb_u_polen(datatime,&date,1,' ');
iceb_u_polen(datatime,&vrem,2,' ');
short d=0,m=0,g=0;
iceb_u_rsdat(&d,&m,&g,date.ravno(),2);
sprintf(datatime_pr,"%02d.%02d.%d %s",d,m,g,vrem.ravno());
return(datatime_pr);

}
