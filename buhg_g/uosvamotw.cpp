/*$Id: uosvamotw.c,v 1.6 2013/08/13 06:09:56 sasa Exp $*/
/*31.12.2007	31.12.2007	Белых А.И.	uosvamotw.c
Расчёт ведомости амортизационных отчислений
*/

#include "buhg_g.h"
#include "uosvamotw.h"

int uosvamotw_m(class uosvamotw_data *rek_ras);
int uosvamotw_r(class uosvamotw_data *datark,GtkWidget *wpredok);
int uosvamot1w_r(class uosvamotw_data *datark,GtkWidget *wpredok);

void uosvamotw()
{


static class uosvamotw_data data;
data.imaf.free_class();
data.naim.free_class();


if(uosvamotw_m(&data) != 0)
 return;
if(iceb_u_sravmydat(1,data.mesn.ravno_atoi(),data.god.ravno_atoi(),1,4,2011) < 0)
 {
  if(uosvamotw_r(&data,NULL) != 0)
   return;
 }
else
  if(uosvamot1w_r(&data,NULL) != 0)
   return;

iceb_rabfil(&data.imaf,&data.naim,NULL);


}

