/*$Id: t_msql_error.c,v 1.4 2013/05/17 14:58:49 sasa Exp $*/
/*03.02.2006	03.02.2006	Белых А.И.	t_msql_error.c
Выдача сообщения об ошибке открытия базы на экран
*/
#include <libsql.h>

void t_msql_error(SQL_baza *bd,const char *zapros)
{
printf("t_msql_error-%d %s\n%s\n",sql_nerror(bd),sql_error(bd),zapros);
}
