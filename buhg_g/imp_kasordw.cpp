/*$Id: imp_kasordw.c,v 1.8 2013/05/17 14:58:20 sasa Exp $*/
/*05.09.2015	13.04.2007	Белых А.И.	imp_kasordw.c
Импорт кассовых ордеров из файла
*/
#include "buhg_g.h"

int imp_kasordw_r(const char *imaf,GtkWidget *wpredok);


void imp_kasordw(GtkWidget *wpredok)
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

class iceb_u_str naim;
naim.plus(gettext("Импорт кассовых ордеров из файла"));
 
if(iceb_menu_import(&imaf,naim.ravno(),"kasniko.alx",__FUNCTION__,wpredok) != 0)
 return;

imp_kasordw_r(imaf.ravno(),wpredok);

}
