/*$Id: mumow.c,v 1.5 2013/08/13 06:09:47 sasa Exp $*/
/*17.12.2009	17.12.2009	Белых А.И.	mumow.c
Материальный отчёт
*/
#include "buhg_g.h"
#include "mumow.h"

int   mumow_m(class mumow_rr *rek_ras);
int mumow_r(class mumow_rr *datark,GtkWidget *wpredok);




void mumow()
{
static class mumow_rr data;
data.imaf.free_class();
data.naimf.free_class();


if(mumow_m(&data) != 0)
 return;

if(mumow_r(&data,NULL) != 0)
   return;


iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
