/*$Id: zarpdw.c,v 1.3 2013/08/13 06:10:03 sasa Exp $*/
/*22.03.2011	22.03.2011	Белых А.И.	zarpdw.c
Отчёты 
*/
#include "buhg_g.h"
#include "zarpdw.h"

int   zarpdw_m(class zarpdw_rr *rek_ras);
int zarpdw_r(class zarpdw_rr *datark,GtkWidget *wpredok);




void zarpdw()
{
static class zarpdw_rr data;
data.imaf.free_class();
data.naimf.free_class();


if(zarpdw_m(&data) != 0)
 return;

if(zarpdw_r(&data,NULL) != 0)
   return;


iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
