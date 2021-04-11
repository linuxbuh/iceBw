/*$Id: iceb_fioruk.c,v 1.11 2013/09/26 09:47:04 sasa Exp $*/
/*18.03.2016	04.04.2008	Белых А.И.	iceb_fioruk.c
Получение фамилий главного бухгалтера и руководителя предприятия
Если вернули 0-нашли
             1-нет
*/
#include "iceb_libbuh.h"



int iceb_fioruk(int metka, /*1-ищем фамилию руководителя 2-главного бухгалтера 3-выплачивающего зарплату 4-проверяющего ведомости на зарплату*/
class iceb_fioruk_rk *rek,
GtkWidget *wpredok)
{
class iceb_u_str imaf_nz("zarnast.alx");

rek->clear();

class iceb_u_str tabnom("");

if(metka == 1)
  iceb_poldan("Табельный номер руководителя",&tabnom,imaf_nz.ravno(),wpredok);
if(metka == 2)
  iceb_poldan("Табельный номер бухгалтера",&tabnom,imaf_nz.ravno(),wpredok);
if(metka == 3)
  iceb_poldan("Табельный номер выплачивающего зарплату",&tabnom,imaf_nz.ravno(),wpredok);
if(metka == 4)
  iceb_poldan("Табельный номер проверяющего ведомости на зарплату",&tabnom,imaf_nz.ravno(),wpredok);

return(iceb_get_rtn(tabnom.ravno_atoi(),rek,wpredok));


}
