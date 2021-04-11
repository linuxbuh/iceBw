/*$Id: upl_ovdt.c,v 1.5 2013/08/13 06:09:56 sasa Exp $*/
/*02.04.2008	02.04.2008	Белых А.И.	upl_ovdt.c
Расчёт оборотной ведомости движения топлива
*/

#include "buhg_g.h"
#include "upl_ovdt.h"

int upl_ovdt_m(class upl_ovdt_data *rek_ras);
int upl_ovdt_r(class upl_ovdt_data *datark,GtkWidget *wpredok);

void upl_ovdt()
{


static class upl_ovdt_data data;
data.imaf.free_class();
data.naim.free_class();


if(upl_ovdt_m(&data) != 0)
 return;

if(upl_ovdt_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,NULL);


}

