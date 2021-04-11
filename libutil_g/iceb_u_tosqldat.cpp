/*$Id: iceb_u_tosqldat.c,v 1.2 2013/08/13 06:10:17 sasa Exp $*/
/*10.11.2011	10.11.2011	Белых А.И.	iceb_u_tosqldat.c
Преобразование даты в SQL формат
*/
#include "iceb_util.h"

const char *iceb_u_tosqldat(const char *data)
{
static char stroka[64];
memset(stroka,'\0',sizeof(stroka));

short d=0,m=0,g=0;


if(iceb_u_rsdat(&d,&m,&g,data,1) != 0)
 {
  if(iceb_u_rsdat1(&m,&g,data) != 0)
   strcpy(stroka,"0000-00-00"); 
  else
   sprintf(stroka,"%04d.%02d.01",g,m);
 }
else
 sprintf(stroka,"%04d.%02d.%02d",g,m,d);

return(stroka);

}
