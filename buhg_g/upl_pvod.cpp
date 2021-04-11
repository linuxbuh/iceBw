/*$Id: upl_pvod.c,v 1.5 2013/08/13 06:09:56 sasa Exp $*/
/*30.03.2008	30.03.2008	Белых А.И.	upl_pvod.c
Расчёт реестра путевых листов по водителям
*/

#include "buhg_g.h"
#include "upl_pvod.h"

int upl_pvod_m(class upl_pvod_data *rek_ras);
int upl_pvod_r(class upl_pvod_data *datark,GtkWidget *wpredok);

void upl_pvod()
{


static class upl_pvod_data data;
data.imaf.free_class();
data.naim.free_class();


if(upl_pvod_m(&data) != 0)
 return;

if(upl_pvod_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,NULL);


}

