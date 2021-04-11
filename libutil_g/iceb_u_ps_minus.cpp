/*$Id: iceb_u_ps_minus.c,v 1.4 2013/05/17 14:58:43 sasa Exp $*/
/*22.09.2003	22.09.2003	Белых А.И.	iceb_u_ps_minus.c
Удаление символа перевода строки в конце строки
*/
#include  <string.h>

void     iceb_u_ps_minus(char *stroka)
{
int i=strlen(stroka);
if(stroka[i-1] == '\n')
 stroka[i-1]='\0';

}
