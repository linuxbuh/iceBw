/*$Id: eksmuw.c,v 1.1 2013/08/13 06:15:42 sasa Exp $*/
/*18.06.2013	18.06.2013	Белых А.И.	eksmuw.c
Экспорт остатков из материального учёта
*/
#include "buhg_g.h"
#include "eksmuw.h"

int eksmuw_m(class eksmuw_rr *rek_ras,GtkWidget *wpredok);
int eksmuw_r(class eksmuw_rr *datark,GtkWidget *wpredok);




void eksmuw()
{
static class eksmuw_rr data;
data.imaf.free_class();
data.naimf.free_class();


if(eksmuw_m(&data,NULL) != 0)
 return;

if(eksmuw_r(&data,NULL) != 0)
   return;

iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
