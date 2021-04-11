/*$Id: kriostw.c,v 1.9 2013/08/13 06:09:36 sasa Exp $*/
/*01.05.2005	01.05.2005	Белых А.И.	kriostw.c
Расчёт критических остатков по материаллам
*/


#include "buhg_g.h"
#include "kriostw.h"

int kriostw_m(class kriostw_rr *rek_ras);
int kriostw_r(class kriostw_rr *datark,GtkWidget *wpredok);

void kriostw()
{
static class kriostw_rr data;

if(kriostw_m(&data) != 0)
 return;

if(kriostw_r(&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
