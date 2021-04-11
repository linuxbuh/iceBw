/*$Id: iceb_u_tolower_str.c,v 1.5 2013/05/17 14:58:43 sasa Exp $*/
/*22.09.2009	21.02.2007	Белых А.И.	iceb_u_tolower_str.c
Преобразование строки символов в маленькие буквы
*/
#include <ctype.h>
#include <string.h>
#include <glib.h>
void iceb_u_tolower_str(char *stroka)
{
if(g_utf8_validate(stroka,-1,NULL) != TRUE)
 {
  for(unsigned i=0; i < strlen(stroka); i++)
   stroka[i]=tolower(stroka[i]);
 }
else
 {
  strcpy(stroka,g_utf8_strdown(stroka,-1));
 }

}
