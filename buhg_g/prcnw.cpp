/* $Id: prcnw.c,v 1.5 2013/08/13 06:09:49 sasa Exp $ */
/*23.05.2013	30.10.1997	Белых А.И.	prcnw.c
Преобразование цены так чтобы дробная часть выводилась как
надо. Либо с 2мя нолями либо со всеми значащими цифрами после
запятой.
*/
#include	"buhg_g.h"

extern double		okrcn;  /*Округление цены*/

char            *prcnw(double cn)
{
static char     str[1024]; /*Символьная строка с числом*/
class iceb_u_str br("");

/*printw("\ncn=%.10f okr=%.10f\n",cn,okr);*/
sprintf(str,"%.10g",cn);
iceb_u_pole(str,&br,2,'.');
if(strlen(br.ravno()) < 2)
  sprintf(str,"%.2f",cn);
 
return(str);
}
