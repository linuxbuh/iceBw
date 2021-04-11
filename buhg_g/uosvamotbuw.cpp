/*$Id: uosvamotbuw.c,v 1.5 2013/08/13 06:09:56 sasa Exp $*/
/*31.12.2007	31.12.2007	Белых А.И.	uosvamotbuw.c
Расчёт ведомости амортизационных отчислений
*/

#include "buhg_g.h"
#include "uosvamotbuw.h"

int uosvamotbuw_m(class uosvamotbuw_data *rek_ras);
int uosvamotbuw_r(class uosvamotbuw_data *datark,GtkWidget *wpredok);

void uosvamotbuw()
{


static class uosvamotbuw_data data;
data.imaf.free_class();
data.naim.free_class();


if(uosvamotbuw_m(&data) != 0)
 return;

if(uosvamotbuw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,NULL);


}

