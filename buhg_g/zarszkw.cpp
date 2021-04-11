/*$Id: zarszkw.c,v 1.3 2013/08/13 06:10:04 sasa Exp $*/
/*21.01.2012	21.01.2012	Белых А.И.	zarszkw.c
Распечтака свода начислей по работникам по месяцам c командировочными расходами
*/
#include "buhg_g.h"
#include "zarszkw.h"

int zarszkw_m(class zarszkw_rr *rek_ras,GtkWidget *wpredok);
int zarszkw_r(class zarszkw_rr *datark,GtkWidget *wpredok);




void zarszkw()
{
static class zarszkw_rr data;
data.imaf.free_class();
data.naimf.free_class();

data.naim_oth.new_plus(gettext("Распечатка свода начислений по месяцам"));

if(zarszkw_m(&data,NULL) != 0)
 return;

if(zarszkw_r(&data,NULL) != 0)
 return;


iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
