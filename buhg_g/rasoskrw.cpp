/*$Id: rasoskrw.c,v 1.13 2013/08/13 06:09:50 sasa Exp $*/
/*17.06.2006	22.10.2004	Белых А.И.	rasoskrw.c
Распечатка остатков на карточках
*/
#include "buhg_g.h"
#include "rasoskr.h"



int   rasoskr_m(class rasoskr_data *rek_ras,GtkWidget*);
int   rasoskr_r(class rasoskr_data *rek_ras,GtkWidget*);

void rasoskrw(GtkWidget *wpredok)
{
static class rasoskr_data data_rasoskr;

data_rasoskr.imaf.free_class();
data_rasoskr.naimf.free_class();

if(rasoskr_m(&data_rasoskr,wpredok) != 0)
 return;

if(rasoskr_r(&data_rasoskr,wpredok) != 0)
 return;

iceb_rabfil(&data_rasoskr.imaf,&data_rasoskr.naimf,wpredok);

}
