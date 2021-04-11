/*$Id: zargetfiow.c,v 1.2 2013/08/13 06:10:03 sasa Exp $*/
/*09.04.2013	09.04.2013	Белых А.И.	zargetfiow.c
Получение фамилии по табельному номеру
*/
#include "buhg_g.h"

const char *zargetfiow(const char *tabnom,GtkWidget *wpredok)
{
char strsql[512];
static class iceb_u_str fio;
SQL_str row;
class SQLCURSOR cur;
fio.new_plus("");

sprintf(strsql,"select fio from Kartb where tabn=%s",tabnom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 fio.new_plus(row[0]);


return(fio.ravno());



}
