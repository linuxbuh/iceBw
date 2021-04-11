/*$Id: muinvw.c,v 1.9 2013/08/13 06:09:47 sasa Exp $*/
/*09.10.2015	21.11.2004	Белых А.И.	muinvw.c
Расчёт инвентаризационных ведомостей
*/
#include "buhg_g.h"
#include "muinv.h"

int   muinv_m(class muinv_data *rek_ras);
int muinv_r(class spis_oth *oth,class muinv_data *datark,GtkWidget *wpredok);

void muinvw()
{

static class muinv_data data;
class spis_oth oth;

if(data.datao.getdlinna() <= 1)
  data.datao.plus_tek_dat();
   

if(muinv_m(&data) != 0)
 return;

if(muinv_r(&oth,&data,NULL) == 0)
  iceb_rabfil(&oth,NULL);

}


