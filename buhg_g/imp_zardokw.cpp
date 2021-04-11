/*$Id: imp_zardokw.c,v 1.7 2013/05/17 14:58:20 sasa Exp $*/
/*08.09.2015	09.09.2008	Белых А.И.	imp_zardokw.c
Импорт начислений/удержаний в документ
*/

#include "buhg_g.h"
#include "imp_zardokw.h"

int imp_zardokw_r(class imp_zardokw_rr *datark,GtkWidget *wpredok);

void imp_zardokw(int prn,const char *datadok,const char *nomdok,GtkWidget *wpredok)
{
class imp_zardokw_rr rk;
class iceb_u_str imaf("");

if(prn == 1)
 rk.naim_oth.new_plus(gettext("Импорт начислений"));
if(prn == 2)
 rk.naim_oth.new_plus(gettext("Импорт удержаний"));

if(iceb_menu_import(&rk.imaf_imp,rk.naim_oth.ravno(),"",__FUNCTION__,wpredok) != 0)
 return;
 
rk.datadok.new_plus(datadok);
rk.prn=prn;
rk.nomdok.new_plus(nomdok);
imp_zardokw_r(&rk,wpredok);




}
