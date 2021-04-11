/*$Id: imp_zar_kartf.c,v 1.1 2013/08/13 06:23:37 sasa Exp $*/
/*08.09.2015	18.06.2013	Белых А.И.	imp_zar_kartf.c
Импорт 
*/
#include "buhg_g.h"

int imp_zar_kartf_r(const char *imaf,GtkWidget *wpredok);


void imp_zar_kartf(GtkWidget *wpredok)
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");
class iceb_u_str naim;
naim.plus(gettext("Импорт карточек работников"));
 
if(iceb_menu_import(&imaf,naim.ravno(),"",__FUNCTION__,wpredok) != 0)
 return;

imp_zar_kartf_r(imaf.ravno(),wpredok);

}
