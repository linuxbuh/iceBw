/*$Id: zarsnpmw.c,v 1.3 2013/08/13 06:10:03 sasa Exp $*/
/*20.01.2012	20.01.2012	Белых А.И.	zarsnpmw.c
Распечтака свода начислей по работникам по месяцам
*/
#include "buhg_g.h"
#include "zarsnpmw.h"

int zarsnpmw_m(class zarsnpmw_rr *rek_ras,GtkWidget *wpredok);
int zarsnpmw_r(class zarsnpmw_rr *datark,GtkWidget *wpredok);




void zarsnpmw()
{
static class zarsnpmw_rr data;
data.imaf.free_class();
data.naimf.free_class();

data.naim_oth.new_plus(gettext("Распечатка свода начислений по месяцам"));

if(zarsnpmw_m(&data,NULL) != 0)
 return;

if(zarsnpmw_r(&data,NULL) != 0)
 return;


iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
