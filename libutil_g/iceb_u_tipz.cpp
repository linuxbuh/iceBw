/*$Id:$*/
/*01.04.2015	01.04.2015	Белых А.И.	iceb_u_tipz.c
возвращает знак плюс или минус
*/
#include <stdlib.h>
#include <string.h>
const char *iceb_u_tipz(const char *pm)
{
//static char buf[16];
//memset(buf,'\0',sizeof(buf));

if(pm == NULL)
 return("");

if(pm[0] == '1')
 return("+");
if(pm[0] == '2')
 return("-");
 
return("");
}