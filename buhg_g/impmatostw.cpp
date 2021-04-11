/*$Id: impmatostw.c,v 1.1 2013/08/13 06:23:37 sasa Exp $*/
/*05.09.2015	17.06.2013	Белых А.И.	impmatostw.c
Импорт остатков материалов из файла
*/
#include "buhg_g.h"

int impmatost_r(const char *imaf,int tipz,int sklad,short dd,short md,short gd,const char *nomdok,GtkWidget *wpredok);


void impmatostw(int tipz,int sklad,short dd,short md,short gd,const char *nomdok,GtkWidget *wpredok)
{

if(tipz == 2)
 {
  iceb_menu_soob(gettext("Импорт осуществляестя только в приходные документы!"),wpredok);
  return;
 }

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

if(iceb_menu_import(&imaf,gettext("Импорт материалов"),"",__FUNCTION__,wpredok) != 0)
 return;

impmatost_r(imaf.ravno(),tipz,sklad,dd,md,gd,nomdok,wpredok);

}
