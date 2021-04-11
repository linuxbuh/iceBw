/*$Id: impmatdokw.c,v 1.12 2013/05/17 14:58:21 sasa Exp $*/
/*14.02.2016	14.11.2004	Белых А.И.	impmatdokw.c
Импорт документов изфайла
*/
#include "buhg_g.h"

int impmatdok_r(const char *imaf,GtkWidget *wpredok);
int impmatdok_xml_r(const char *imaf,GtkWidget *wpredok);


void impmatdokw()
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Импорт документов"));

zagolovok.plus(gettext("Импорт документов"));

punkt_m.plus(gettext("Импорт документов в формате iceB"));//0
punkt_m.plus(gettext("Импорт полученных налоговых накладных в формате XML"));//1


if((nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL)) == -1)
  return;


static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

if(nomer == 0)
 if(iceb_menu_import(&imaf,gettext("Материальный учёт\nИмпорт документов в формате iceB"),"impmatdok.alx",__FUNCTION__,NULL) != 0)
  return;

if(nomer == 1)
 if(iceb_menu_import(&imaf,gettext("Материальный учёт\nИмпорт полученных налоговых накладных в формате XML"),"","",NULL) != 0)
  return;

if(nomer == 0)
  impmatdok_r(imaf.ravno(),NULL);
if(nomer == 1)
  impmatdok_xml_r(imaf.ravno(),NULL);

}
