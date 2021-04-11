/*$Id:$*/
/*27.10.2016	27.10.2016	Белых А.И.	iceb_get_fiotbn.c
Получить фамилию по табельному номеру
*/
#include "iceb_libbuh.h"

const char *iceb_get_fiotbn(int tabnom,GtkWidget *wpredok)
{
static class iceb_u_str fio("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select fio from Kartb where tabn = %d",tabnom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 fio.new_plus(row[0]);
else
 {
  sprintf(strsql,"%s-%s %d\n",__FUNCTION__,gettext("Не нашли карточку для табельного номера"),tabnom);
  fio.new_plus(strsql);
 }
return(fio.ravno());
}
