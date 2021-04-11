/*$Id: buh_rnr_shw.c,v 1.1 2013/05/17 15:01:15 sasa Exp $*/
/*20.04.2013	20.04.2013	Белых А.И.	buh_rnr_shw.c
Расчёт шахматки по счёту
*/
#include "buhg_g.h"
#include "buh_rnr_shw.h"

int   buh_rnr_shw_m(class buh_rnr_shw_rr *rek_ras);
int buh_rnr_shw_r(class buh_rnr_shw_rr *datark,GtkWidget *wpredok);




void buh_rnr_shw()
{
static class buh_rnr_shw_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(buh_rnr_shw_m(&data) != 0)
 return;

if(buh_rnr_shw_r(&data,NULL) != 0)
   return;


iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
