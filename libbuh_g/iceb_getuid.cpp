/*$Id: iceb_getuid.c,v 1.4 2014/01/31 12:13:02 sasa Exp $*/
/*06.01.2014	06.09.2010	Белых А.И.	iceb_getuid.c
Получение номера оператора
*/
#include "iceb_libbuh.h"

extern class iceb_u_str login_op;

int iceb_getuid(GtkWidget *wpredok)
{
static int nom_op=0;

if(nom_op == 0)
 {
  SQL_str row;
  class SQLCURSOR cur;
  char strsql[512];

//  sprintf(strsql,"select un from icebuser where login='%s'",iceb_u_getlogin());
  sprintf(strsql,"select un from icebuser where login='%s'",login_op.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   nom_op=atoi(row[0]);
 } 
return(nom_op);

}
