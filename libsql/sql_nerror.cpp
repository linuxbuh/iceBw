/* $Id: sql_nerror.c,v 1.3 2013/05/17 14:58:40 sasa Exp $ */
/*12.01.2000	12.01.2000	Белых А.И.	sql_nerror.c
Выдача номера ошибки
*/
#include	<stdio.h>
#include	"libsql.h"

unsigned int	sql_nerror(SQL_baza *bd)
{
return(mysql_errno(bd));
}
