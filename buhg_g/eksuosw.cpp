/*$Id: eksuosw.c,v 1.1 2013/08/13 06:15:42 sasa Exp $*/
/*18.06.2013	18.06.2013	Белых А.И.	eksuosw.c
Экспорт основных средств
*/
#include "buhg_g.h"
#include "eksuosw.h"

int eksuosw_m(class eksuosw_rr *rek_ras,GtkWidget *wpredok);
int eksuosw_r(class eksuosw_rr *datark,GtkWidget *wpredok);




void eksuosw()
{
static class eksuosw_rr data;
data.imaf.free_class();
data.naimf.free_class();


if(eksuosw_m(&data,NULL) != 0)
 return;

if(eksuosw_r(&data,NULL) != 0)
   return;

iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
