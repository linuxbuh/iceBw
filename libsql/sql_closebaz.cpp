/* $Id: sql_closebaz.c,v 1.3 2013/05/17 14:58:39 sasa Exp $ */
/*26.01.1999	26.01.1999	Белых А.И.	sql_closebaz.c
Закрыть SQL базу данных
*/
#include	<stdio.h>
#include	"libsql.h"

void    	sql_closebaz(SQL_baza *bd)
{
mysql_close(bd); 
}
