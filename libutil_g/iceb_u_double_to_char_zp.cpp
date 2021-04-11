/*$Id: iceb_tu_double_to_char_zp.c,v 5.1 2013/11/24 08:24:21 sasa Exp $*/
/*21.11.2013	21.11.2013	Белых А.И.	iceb_tu_double_to_char_zp.c
Вернуть число в символьном виде с разделителем дробной части в виде запятой
*/
#include <glib.h>
#include "iceb_util.h"
const char *iceb_u_double_to_char_zp(double suma,int drob)
{

static gchar *str_out=NULL;
if(str_out != NULL)
 g_free(str_out);

str_out=g_strdup_printf("%.*f",drob,suma);
for(unsigned int nom=0; nom < strlen(str_out); nom++)
  if(str_out[nom] == '.')
   {
    str_out[nom]=',';
    break;
   }   
return(str_out);

}

