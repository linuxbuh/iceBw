/*$Id: iceb_u_strlen.c,v 1.3 2013/05/17 14:58:43 sasa Exp $*/
/*22.09.2009	22.09.2009	Белых А.И.	iceb_u_strlen.c
Вычисляет количество симоволов в строке
*/
#include <glib.h>
#include <string.h>
int iceb_u_strlen(const char *stroka)
{
int kolih=0;

if(g_utf8_validate(stroka,-1,NULL) == FALSE)
 kolih=strlen(stroka);
else
 kolih=g_utf8_strlen(stroka,-1);

return(kolih);

}
