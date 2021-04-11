/*$Id:$*/
/*22.11.2016	22.11.2016	Белых А.И.	iceb_get_edrpou.c
Получение кода ЄДРПОУ
*/

#include "iceb_libbuh.h"

const char *iceb_get_edrpou(const char *kod_kontr,GtkWidget *wpredok)
{
static class iceb_u_str edrpou("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select kod from Kontragent where kodkon='%s'",kod_kontr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 edrpou.new_plus(row[0]);
else
 {
  sprintf(strsql,"%s-%s %s!",__FUNCTION__,gettext("Не найден код контрагента"),kod_kontr);
  edrpou.new_plus(strsql);
 }
return(edrpou.ravno());
}
