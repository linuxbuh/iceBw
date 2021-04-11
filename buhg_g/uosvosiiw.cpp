/*$Id: uosvosiiw.c,v 1.7 2013/08/13 06:09:56 sasa Exp $*/
/*14.06.2015	07.01.2008	Белых А.И.	uosvosiiw.c
Расчёт ведомости износа и остаточной стоимости
*/
#include "buhg_g.h"
#include "uosvosiiw.h"

int uosvosiiw_m(int metka_r,class uosvosiiw_data *rek_ras);
int uosvosiiw_r(int metka_r,class uosvosiiw_data *datark,GtkWidget *wpredok);

void uosvosiiw(int metka_oth)  /*0-налоговый учет 1-бухгалтерский*/
{


static class uosvosiiw_data data;
data.imaf.free_class();
data.naim.free_class();


if(uosvosiiw_m(metka_oth,&data) != 0)
 return;
data.viduh=metka_oth;
if(uosvosiiw_r(metka_oth,&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naim,NULL);


}
