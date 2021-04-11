/*$Id: opss_uw.c,v 1.7 2013/08/13 06:09:47 sasa Exp $*/
/*16.06.2006	24.12.2005	Белых А.И.	opss_uw.c
Расчёт по счётам списания услуг
*/


#include "buhg_g.h"
#include "opss_uw.h"

int opss_uw_m(class opss_uw_rr *rek_ras);
int opss_uw_r(class opss_uw_rr *datark,GtkWidget *wpredok);


void opss_uw(int metka) //1-по счетам получения 2-по счетам списания
{
static class opss_uw_rr data;
data.metka=metka;
if(opss_uw_m(&data) != 0)
 return;

if(opss_uw_r(&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
