/*$Id: iceb_u_poltekgod.c,v 1.3 2013/05/17 14:58:43 sasa Exp $*/
/*10.06.2009	10.06.2009	Белых А.И.	iceb_u_poltekgod.c
Получение текущего года
*/
#include <time.h>

int iceb_u_poltekgod()
{
time_t vrem;
time(&vrem);
struct tm *bf=localtime(&vrem);

return(bf->tm_year+1900);

}
