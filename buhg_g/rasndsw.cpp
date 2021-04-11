/*$Id: rasndsw.c,v 1.8 2013/08/13 06:09:50 sasa Exp $*/
/*17.06.2006	24.04.2005	Белых А.И.	rasndsw.c
Расчёт реестра проводок по видам операций
*/


#include "buhg_g.h"
#include "rasndsw.h"

int rasndsw_m(class rasndsw_rr *rek_ras);
int rasndsw_r(class rasndsw_rr *datark,GtkWidget *wpredok);

void rasndsw()
{
static class rasndsw_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(rasndsw_m(&data) != 0)
 return;

if(rasndsw_r(&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
