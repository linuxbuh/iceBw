/*$Id: zar_sprav.c,v 1.6 2013/09/26 09:47:00 sasa Exp $*/
/*16.01.2019	26.05.2016	Белых А.И.	sprtabotpw.c
Расчёт отпускных
*/
#include "buhg_g.h"
#include "sprtabotpw.h"

int sprtabotpw_m(class sprtabotp_poi *rek,GtkWidget *wpredok);
int sprtabotpw_r(class sprtabotp_poi *rek,class spis_oth *oth,GtkWidget *wpredok);


void sprtabotpwv(GtkWidget *wpredok)
{
static class sprtabotp_poi data;

class spis_oth oth;  

if(sprtabotpw_m(&data,wpredok) != 0)
 return;


if(sprtabotpw_r(&data,&oth,wpredok) != 0)
 return;

iceb_rabfil(&oth,wpredok);

}
