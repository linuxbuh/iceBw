/*$Id: zar_pi.c,v 1.10 2013/08/13 06:10:01 sasa Exp $*/
/*29.01.2007	29.01.2007	Белых А.И.	zar_pi.c
Отчёт по пенсионерам и инвалидам
*/
#include "buhg_g.h"

int zar_pi_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok);


void zar_pi(GtkWidget *wpredok)
{
static class iceb_u_str data_d;
  
class iceb_u_spisok imaf;
class iceb_u_spisok naimf;

if(iceb_menu_mes_god(&data_d,gettext("Распечатать отчёт по пенсионерам и инвалидам"),"",wpredok) != 0)
 return;
if(zar_pi_r(data_d.ravno(),&imaf,&naimf,wpredok) != 0)
 return;

iceb_rabfil(&imaf,&naimf,wpredok);

}
