/*$Id: impusldokw.c,v 1.8 2013/05/17 14:58:21 sasa Exp $*/
/*08.09.2015	14.11.2004	Белых А.И.	impusldokw.c
Импорт документов изфайла
*/
#include "buhg_g.h"

int impusldok_r(const char *imaf,GtkWidget *wpredok);
int impusldok_xml_rw(const char *imaf,GtkWidget *wpredok);


void impusldokw()
{

static iceb_u_str imaf;

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




if(imaf.getdlinna() == 0)
 imaf.plus("");

if(nomer == 0)
 if(iceb_menu_import(&imaf,gettext("Импорт документов"),"impusldok.alx",__FUNCTION__,NULL) != 0)
  return;
if(nomer == 1)
 if(iceb_menu_import(&imaf,gettext("Импорт документов"),"","",NULL) != 0)
  return;

if(nomer == 0)
  impusldok_r(imaf.ravno(),NULL);
if(nomer == 1)
  impusldok_xml_rw(imaf.ravno(),NULL);

}
