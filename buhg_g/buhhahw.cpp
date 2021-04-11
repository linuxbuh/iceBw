/*$Id: buhhahw.c,v 1.6 2013/08/13 06:09:32 sasa Exp $*/
/*20.06.2006	20.06.2006	Белых А.И.	buhhahw.c
Расчёт шахматки
*/

#include "buhg_g.h"
#include "buhhahw.h"

int buhhahw_m(class buhhahw_rr*,GtkWidget *wpredok);
int buhhahw_r(class buhhahw_rr*,GtkWidget *wpredok);

void buhhahw()
{
static class buhhahw_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(buhhahw_m(&data,NULL) != 0)
 return;
if(buhhahw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
