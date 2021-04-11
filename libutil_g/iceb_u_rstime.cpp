/*$Id: iceb_u_rstime.c,v 1.12 2013/08/13 06:10:17 sasa Exp $*/
/*22.07.2013	16.04.2003	Белых А.И.	iceb_u_rstime.c
Расшифровка времени. Если вернули 0 - время введено правильно
                                  1 - нет
*/
#include	<stdlib.h>
#include	<string.h>
#include	"iceb_util.h"

int    iceb_u_rstime(short *has,short *min,short *sek,const char *vremq)
{
class iceb_u_str bros("");

*has=*min=*sek=0;
if(vremq[0] == '\0')
 return(0);
 
char vrem1[strlen(vremq)+1];
strcpy(vrem1,vremq);

for(int i=0; i < (int)strlen(vrem1); i++)
  if(vrem1[i] == '.' || vrem1[i] == ',')
    vrem1[i]=':';
     
if(iceb_u_polen(vrem1,&bros,1,':') != 0)
 bros.new_plus(vrem1);
*has=bros.ravno_atoi();

iceb_u_polen(vrem1,min,2,':');
//*min=atoi(bros);

iceb_u_polen(vrem1,sek,3,':');
//*sek=atoi(bros);

if(*has < 0 || *has > 23)
 return(1);

if(*min < 0 || *min > 60)
 return(1);

if(*sek < 0 || *sek > 60)
 return(1);

return(0);
}
