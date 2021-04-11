/*$Id: iceb_u_toupper_str.c,v 1.5 2013/05/17 14:58:43 sasa Exp $*/
/*22.09.2009	23.02.2007	Белых А.И.	iceb_u_toupper_str.c
Перевод в строке всех букв в большие
*/
#include <ctype.h>
#include <string.h>
#include <glib.h>
void iceb_u_toupper_str(char *stroka)
{

if(g_utf8_validate(stroka,-1,NULL) != TRUE)
 {
  for(unsigned i=0; i < strlen(stroka); i++)
   stroka[i]=toupper(stroka[i]);
 }
else
 {
  strcpy(stroka,g_utf8_strup(stroka,-1));
 }


}

