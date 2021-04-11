/*$Id: imp_zar_kart.c,v 1.10 2013/08/13 06:09:35 sasa Exp $*/
/*08.09.2015	09.10.2006	Белых А.И.	imp_zar_kart.c
Импорт 
*/
#include "buhg_g.h"

int imp_zar_kart_r(const char *imaf,GtkWidget *wpredok);


void imp_zar_kart(GtkWidget *wpredok)
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");
class iceb_u_str naim;
naim.plus(gettext("Импорт списка работников"));
 
if(iceb_menu_import(&imaf,naim.ravno(),"",__FUNCTION__,wpredok) != 0)
 return;

imp_zar_kart_r(imaf.ravno(),wpredok);

}
