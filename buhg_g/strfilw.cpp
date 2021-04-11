/*$Id: strfilw.c,v 1.6 2013/05/17 14:58:27 sasa Exp $*/
/*12.07.2015	06.09.2006	Белых А.И.	strfilw.c
Выдача строки в корешок
*/
#include "buhg_g.h"

void strfilw(const char *a1,const char *a2,short mt,FILE *kaw)
{
short           i;
if(kaw == NULL)
 return;
for(i=0;i < mt;i++)
  fprintf(kaw,"\n");
fprintf(kaw,"%-*s || %s\n",iceb_u_kolbait(62,a1),a1,a2);
}
