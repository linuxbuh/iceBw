/*$Id: buhrpzw.c,v 1.7 2013/08/13 06:09:32 sasa Exp $*/
/*01.03.2007	01.03.2007	Белых А.И.	buhrpzw.c
Расчёт видов затрат по элементам
*/

#include "buhg_g.h"
#include "buhrpzw.h"

int buhrpzw_m(class buhrpzw_rr*,GtkWidget *wpredok);
int buhrpzw_r(class buhrpzw_rr*,GtkWidget *wpredok);

void buhrpzw()
{
static class buhrpzw_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(buhrpzw_m(&data,NULL) != 0)
 return;
if(buhrpzw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
