/*$Id:$*/
/*02.12.2014	02.12.2014	Белых А.И.	zarbolw.c
Расчёт отчислений с больничного
*/
#include "buhg_g.h"
int zarbolw_m(class iceb_u_str *mes_god,class iceb_u_str *kod_bol,GtkWidget *wpredok);
int zarbolw_r(const char *mes_god,const char *kod_bol,class spis_oth *oth,GtkWidget *wpredok);

void zarbolw(GtkWidget *wpredok)
{
class spis_oth oth;
static class iceb_u_str mes_god("");
static class iceb_u_str kod_bol("");

if(zarbolw_m(&mes_god,&kod_bol,wpredok) != 0)
 return;

if(zarbolw_r(mes_god.ravno(),kod_bol.ravno(),&oth,wpredok) != 0)
 return;

iceb_rabfil(&oth,wpredok);


}