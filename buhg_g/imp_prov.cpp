/*$Id: imp_prov.c,v 1.1 2013/08/25 08:26:33 sasa Exp $*/
/*04.02.2016	21.08.2013	Белых А.И.	imp_prov.c
Импорт проводок
*/
#include "buhg_g.h"

int imp_prov_r(const char *imaf,GtkWidget *wpredok);


void imp_prov(GtkWidget *wpredok)
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

 
if(iceb_menu_import(&imaf,gettext("Импорт проводок из файла"),"impprov.alx",__FUNCTION__,wpredok) != 0)
 return;

imp_prov_r(imaf.ravno(),wpredok);

}
