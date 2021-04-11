/*$Id: xrnn_rasp.c,v 1.6 2014/02/28 05:21:01 sasa Exp $*/
/*06.05.2008	05.05.2008	Белых А.И.	xrnn_rasp.c
Расчёт реестра налоговых накладных
*/

#include "buhg_g.h"
#include "xrnn_poiw.h"

int xrnn_rasp_m(class xrnn_poi *rek_ras);

void xrnn_rasp()
{


static class xrnn_poi data;
data.imaf.free_class();
data.naim.free_class();


if(xrnn_rasp_m(&data) != 0)
 return;

if(xrnn_rasp_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,NULL);


}

