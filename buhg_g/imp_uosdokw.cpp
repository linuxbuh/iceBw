/*$Id: imp_uosdokw.c,v 1.6 2013/05/17 14:58:20 sasa Exp $*/
/*07.09.2015	20.12.2007	Белых А.И.	imp_uosdokw.c
Импорт документров в подсистему "Учёт основных средств"
*/
#include "buhg_g.h"

int imp_uosdokw_r(const char *imaf,GtkWidget *wpredok);


void imp_uosdokw(GtkWidget *wpredok)
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

class iceb_u_str naim;
naim.plus(gettext("Импорт документов"));
 
if(iceb_menu_import(&imaf,naim.ravno(),"impuosdok.alx",__FUNCTION__,wpredok) != 0)
 return;

imp_uosdokw_r(imaf.ravno(),wpredok);

}
