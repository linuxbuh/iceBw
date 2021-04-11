/*$Id: iceb_u_inttochar.c,v 1.1 2013/08/13 06:25:29 sasa Exp $*/
/*26.07.2013	26.07.2013	Белых А.И.	iceb_u_inttochar.c
Преобразование целого в символьую строку
*/
#include <stdio.h>
const char *iceb_u_inttochar(int znah)
{
static char tmp[32];
sprintf(tmp,"%d",znah);
return(tmp);
}
