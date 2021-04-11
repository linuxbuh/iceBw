/*$Id: iceb_str_poisk.c,v 1.6 2013/05/17 14:58:37 sasa Exp $*/
/*11.11.2008	17.05.2004	Белых А.И.	iceb_str_poisk.c
Формирование строки реквизитов поиска
*/
#include "iceb_libbuh.h"

void  iceb_str_poisk(iceb_u_str *stroka,const char *data,const char *naim)
{

if(data[0] == '\0')
 return;

iceb_u_str stroka_prom;

stroka_prom.plus(naim);
stroka_prom.plus(": ");
stroka_prom.plus(data);
stroka->ps_plus(stroka_prom.ravno());

}
