/*$Id: buhrpznpw.c,v 1.6 2013/08/13 06:09:32 sasa Exp $*/
/*01.03.2007	01.03.2007	Белых А.И.	buhrpznpw.c
Расчёт распределения административных затрат на доходы
*/

#include "buhg_g.h"
#include "buhrpznpw.h"

int buhrpznpw_m(class buhrpznpw_rr*,GtkWidget *wpredok);
int buhrpznpw_r(class buhrpznpw_rr*,GtkWidget *wpredok);

void buhrpznpw()
{
static class buhrpznpw_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(buhrpznpw_m(&data,NULL) != 0)
 return;
if(buhrpznpw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
