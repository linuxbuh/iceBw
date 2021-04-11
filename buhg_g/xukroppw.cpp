/*$Id:$*/
/*07.08.2015	07.08.2015	Белых А.И.	xukroppw.c
расчёт свода по контрагентам поставщикам
*/

#include "buhg_g.h"
#include "xukropp.h"

int xukroppw_m(class xukropp_poi *rek_ras);
int xukroppw_r(class xukropp_poi *poi,class spis_oth *oth,GtkWidget *wpredok);

void xukroppw()
{
static class xukropp_poi poi;
class spis_oth oth;

if(xukroppw_m(&poi) != 0)
 return;

if(xukroppw_r(&poi,&oth,NULL) == 0)
  iceb_rabfil(&oth,NULL);


}