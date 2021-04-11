/*$Id: iceb_u_datzap_mg.c,v 1.2 2013/08/13 06:10:17 sasa Exp $*/
/*13.10.2011	13.10.2011	Белых А.И.	iceb_u_datzap_mg.c
Получение символьной строки с датой в формате мм.гг
*/
#include  "iceb_util.h"

const char *iceb_u_datzap_mg(const char *data)
{
short d=0,m=0,g=0;
static char stroka[64];
memset(stroka,'\0',sizeof(stroka));

if(data[0] == '\0')
 return(stroka);

iceb_u_rsdat(&d,&m,&g,data,2);
if(d != 0)
 sprintf(stroka,"%02d.%04d",m,g);
return(stroka);

}
