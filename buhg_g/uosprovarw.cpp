/*$Id: uosprovarw.c,v 1.5 2013/05/17 14:58:28 sasa Exp $*/
/*13.07.2015	13.04.2009	Белых А.И.	uosprvarw.c
Получение алгоритма расчёта амортизации по группе
*/
#include "buhg_g.h"

int uosprovarw(const char *kodgr,int metkau, /*0-налоговый учёт 1-бухучёт*/
GtkWidget *wpredok)
{
int ar=-1;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
if(metkau == 0)
  sprintf(strsql,"select ar from Uosgrup where kod='%s'",kodgr);
else
  sprintf(strsql,"select ar from Uosgrup1 where kod='%s'",kodgr);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
  ar=atoi(row[0]);

return(ar);   

}
