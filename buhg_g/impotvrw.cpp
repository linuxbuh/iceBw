/*$Id: impotvrw.c,v 1.5 2013/05/17 14:58:21 sasa Exp $*/
/*07.09.2015	17.03.2009	Белых А.И.	impotvrw.c
Импорт кассовых ордеров из файла
*/
#include "buhg_g.h"


void impotvrw(GtkWidget *wpredok)
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

class iceb_u_str naim;
naim.plus(gettext("Импорт отработанного времени"));
 
if(iceb_menu_import(&imaf,naim.ravno(),"zarimpot.alx",__FUNCTION__,wpredok) != 0)
 return;

}
