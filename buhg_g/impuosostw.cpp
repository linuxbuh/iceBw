/*$Id: impuosostw.c,v 1.1 2013/08/13 06:23:37 sasa Exp $*/
/*08.09.2015	18.06.2013	Белых А.И.	impuosostw.c
Импорт материалов из файла
*/
#include "buhg_g.h"

int impuosost_r(const char *imaf,int tipz,short dd,short md,short gd,const char *nomdok,const char *kodop,int podr,int kodol,GtkWidget *wpredok);


void impuosostw(int tipz,short dd,short md,short gd,const char *nomdok,const char *kodop,int podr,int kodol,GtkWidget *wpredok)
{

if(tipz == 2)
 {
  iceb_menu_soob(gettext("Импорт осуществляестя только в приходные документы!"),wpredok);
  return;
 }

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

if(iceb_menu_import(&imaf,gettext("Импорт основных средств"),"",__FUNCTION__,wpredok) != 0)
 return;

impuosost_r(imaf.ravno(),tipz,dd,md,gd,nomdok,kodop,podr,kodol,wpredok);

}
