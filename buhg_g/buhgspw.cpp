/*$Id: buhgspw.c,v 1.5 2013/08/13 06:09:32 sasa Exp $*/
/*01.03.2007	01.03.2007	Белых А.И.	buhgspw.c
Расчёт по спискам групп контрагентов
*/

#include "buhg_g.h"
#include "buhgspw.h"

int buhgspw_m(class buhgspw_rr*,GtkWidget *wpredok);
int buhgspw_r(class buhgspw_rr*,GtkWidget *wpredok);

void buhgspw()
{
static class buhgspw_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(buhgspw_m(&data,NULL) != 0)
 return;
if(buhgspw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
