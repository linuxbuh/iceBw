/*$Id: iceb_getfioop.c,v 1.4 2014/01/31 12:13:01 sasa Exp $*/
/*06.01.2014	07.09.2010	Белых А.И.	iceb_getfioop.c
Получение фамилии оператора
*/

#include "iceb_libbuh.h"
extern class iceb_u_str login_op;

const char *iceb_getfioop(GtkWidget *wpredok)
{
static iceb_u_str fio("");

if(fio.getdlinna() <= 1)
 {
  SQL_str row;
  class SQLCURSOR cur;
  char strsql[512];

  fio.new_plus("");
//  sprintf(strsql,"select fio from icebuser where login='%s'",iceb_u_getlogin());
  sprintf(strsql,"select fio from icebuser where login='%s'",login_op.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   fio.new_plus(row[0]);
  
 } 
return(fio.ravno());

}
