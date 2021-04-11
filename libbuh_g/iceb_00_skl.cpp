/*$Id: iceb_00_skl.c,v 1.8 2013/08/13 06:10:09 sasa Exp $*/
/*31.03.2013	29.07.2004	Белых А.И.	iceb_00_skl.c
Получениe кода склада из кода контрагента
Возвращает код склада
*/
#include "iceb_libbuh.h"

int iceb_00_skl(const char *kontr)
{

if(kontr[0] == '0' && kontr[1] == '0' && kontr[2] == '-')
 {
  class iceb_u_str sklad("");
  iceb_u_polen(kontr,&sklad,2,'-');
  return(sklad.ravno_atoi());  
 }

return(0);

}
