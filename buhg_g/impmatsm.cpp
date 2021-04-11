/*$Id: impmatsm.c,v 1.1 2013/08/13 06:23:37 sasa Exp $*/
/*05.09.2015	22.06.2013	Белых А.И.	impmatsm.c
Импорт списка материалов из файла
*/
#include "buhg_g.h"

int impmatsm_r(const char *imaf,GtkWidget *wpredok);


void impmatsm(GtkWidget *wpredok)
{


static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

if(iceb_menu_import(&imaf,gettext("Импорт материалов"),"",__FUNCTION__,wpredok) != 0)
 return;

impmatsm_r(imaf.ravno(),wpredok);

}
