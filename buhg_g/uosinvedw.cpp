/*$Id: uosinvedw.c,v 1.5 2013/08/13 06:09:55 sasa Exp $*/
/*09.10.2015	13.01.2008	Белых А.И.	uosinvedw.c
Расчёт инвентаризационной ведомости основных средств
*/

#include "buhg_g.h"
#include "uosinvedw.h"

int uosinvedw_m(class uosinvedw_data *rek_ras);
int uosinvedw_r(class spis_oth *oth,class uosinvedw_data *datark,GtkWidget *wpredok);

void uosinvedw()
{


static class uosinvedw_data data;
class spis_oth oth;

if(uosinvedw_m(&data) != 0)
 return;

if(uosinvedw_r(&oth,&data,NULL) != 0)
 return;

iceb_rabfil(&oth,NULL);


}

