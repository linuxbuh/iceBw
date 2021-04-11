/*$Id: buhvdzw.c,v 1.5 2013/08/13 06:09:33 sasa Exp $*/
/*02.03.2007	02.03.2007	Белых А.И.	buhvdzw.c
Расчёт валовых доходов и затрат
*/

#include "buhg_g.h"
#include "buhvdzw.h"

int buhvdzw_m(class buhvdzw_rr*,GtkWidget *wpredok);
int buhvdzw_r(class buhvdzw_rr*,GtkWidget *wpredok);

void buhvdzw()
{
static class buhvdzw_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(buhvdzw_m(&data,NULL) != 0)
 return;
if(buhvdzw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
