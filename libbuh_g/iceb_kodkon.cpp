/*$Id: iceb_kodkon.c,v 1.5 2013/12/31 11:49:18 sasa Exp $*/
/*29.11.2013	24.06.2004	Белых А.И.	iceb_kodkon.c
извлечение кода контрагента из заданной стороки
*/
#include "iceb_libbuh.h"
void  iceb_kodkon(char *kodkon,char *str)
{

if(str[0] == '0' && str[1] == '0' && str[2] == '-')
 strcpy(kodkon,"00");
else
 strcpy(kodkon,str);
 
}
/***************************/
void  iceb_kodkon(class iceb_u_str *kodkon,char *str)
{

if(str[0] == '0' && str[1] == '0' && str[2] == '-')
 kodkon->new_plus("00");
else
 kodkon->new_plus(str);
 
}
