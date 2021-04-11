/*$Id: iceb_getk00.c,v 1.2 2013/08/13 06:10:10 sasa Exp $*/
/*28.12.2011	28.12.2011	Белых А.И.	iceb_getk00.c
Получение кода контрагента для операции
*/
#include "iceb_libbuh.h"

const char *iceb_getk00(int metka, /*0-мат-учёт 1-учёт услуг 2-учёт основных средств*/
const char *kodop,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
static iceb_u_str kod_kontr("");

kod_kontr.new_plus("00");

if(metka == 0)
 sprintf(strsql,"select kk from Rashod where kod='%s'",kodop);
if(metka == 1)
 sprintf(strsql,"select kk from Usloper2 where kod='%s'",kodop);
if(metka == 2)
 sprintf(strsql,"select kk from Uosras where kod='%s'",kodop);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 kod_kontr.new_plus(row[0]);

return(kod_kontr.ravno());
}