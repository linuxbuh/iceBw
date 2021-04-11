/*$Id: impuslsu.c,v 1.1 2013/08/13 06:23:37 sasa Exp $*/
/*08.09.2015	23.06.2013	Белых А.И.	impuslsu.c
Импорт списка услуг из файла
*/
#include "buhg_g.h"

int impuslsu_r(const char *imaf,GtkWidget *wpredok);


void impuslsu(GtkWidget *wpredok)
{


static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

if(iceb_menu_import(&imaf,gettext("Импорт справочника услуг"),"",__FUNCTION__,wpredok) != 0)
 return;

impuslsu_r(imaf.ravno(),wpredok);

}
