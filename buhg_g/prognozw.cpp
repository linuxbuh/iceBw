/*$Id: prognozw.c,v 1.9 2013/08/13 06:09:49 sasa Exp $*/
/*17.06.2006	01.05.2005	Белых А.И.	prognozw.c
Расчёт завоза товара (прогноз реализации)
Смотрим реализацию за заказанный период времени
Вычисляем среднюю реализацию за день
Делим остаток на эту реализацию и вычисляем на сколько 
дней торговли хватит товара
*/


#include "buhg_g.h"
#include "prognozw.h"

int prognozw_m(class prognozw_rr *rek_ras);
int prognozw_r(class prognozw_rr *datark,GtkWidget *wpredok);


void prognozw()
{
static class prognozw_rr data;

if(prognozw_m(&data) != 0)
 return;

if(prognozw_r(&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
