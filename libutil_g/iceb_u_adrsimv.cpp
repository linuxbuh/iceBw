/*$Id: iceb_u_adrsimv.c,v 1.4 2013/11/05 10:51:30 sasa Exp $*/
/*25.10.2013	30.09.2009	Белых А.И.	iceb_u_adrsimv.c
Возвращает адрес символа 
*/
#include <glib.h>
#include "iceb_util.h"
const char *iceb_u_adrsimv(int nomer_simv,const char *str)
{
static const char *kk={""};

if(str == NULL)
 return(kk);
 
if(g_utf8_validate(str,-1,NULL) == TRUE) /*текст в utf8 кодировке*/
 {
  if(iceb_u_strlen(str) >= nomer_simv)
   return(g_utf8_offset_to_pointer(str,nomer_simv));
  else
    return(kk);
 }

if((int)strlen(str) >= nomer_simv)
  return(&str[nomer_simv]);
else
  return(kk);

}