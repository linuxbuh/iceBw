/*$Id: iceb_kszap.c,v 1.7 2013/08/13 06:10:10 sasa Exp $*/
/*15.05.2012	14.09.2010	Белых А.И.	iceb_kszap.c
Формирование символной строки с номером и фамилией оператора
*/
#include "iceb_libbuh.h"

const char *iceb_kszap(int kodop,
GtkWidget *wpredok)
{
static char stroka[512];
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select fio from icebuser where un=%d",kodop);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  sprintf(stroka,"%d %.*s",kodop,iceb_u_kolbait(20,row[0]),row[0]);
  
 }
else
 {
  sprintf(stroka,"%d %s",kodop,gettext("Неизвестный логин"));
 }
return(stroka);
}
/**********************************/
const char *iceb_kszap(const char *kodop,GtkWidget *wpredok)
{
if(kodop != NULL)
 return(iceb_kszap(atoi(kodop),wpredok));
else
 return("NULL");

}
