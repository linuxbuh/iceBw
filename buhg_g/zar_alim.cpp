/*$Id: zar_alim.c,v 1.9 2013/08/13 06:10:00 sasa Exp $*/
/*26.03.2018	08.12.2006	Белых А.И.	zar_alim.c
Распечатать реестр почтових перечислений на алименты
*/
#include "buhg_g.h"

int zar_alim_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_alim(GtkWidget *wpredok)
{
static class iceb_u_str data_d;
  
class iceb_u_spisok imaf;
class iceb_u_spisok naimf;

if(iceb_menu_mes_god(&data_d,gettext("Распечатать реестр почтовых перечислений алиментов"),"zaralim.alx",wpredok) != 0)
 return;
if(zar_alim_r(data_d.ravno(),&imaf,&naimf,wpredok) != 0)
 return;

iceb_rabfil(&imaf,&naimf,wpredok);

}
