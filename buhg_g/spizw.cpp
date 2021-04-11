/*$Id: spizw.c,v 1.7 2013/05/17 14:58:27 sasa Exp $*/
/*20.10.2009	17.05.2005	Белых А.И.	spizw.c
Списание изделий в материальном учёте
*/


#include "buhg_g.h"
#include "spizw.h"

int spizw_m(class spizw_rr *rek_ras,GtkWidget*);
int spizw_r(class spizw_rr *datark,GtkWidget *wpredok);
void spizw(const char *kod_izdel,GtkWidget *wpredok)
{
class spizw_rr data;
data.kod_izdel.new_plus(kod_izdel);

if(spizw_m(&data,wpredok) != 0)
 return;

spizw_r(&data,wpredok);

}
