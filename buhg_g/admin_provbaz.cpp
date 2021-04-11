/*$Id: admin_provbaz.c,v 1.2 2013/05/17 14:58:18 sasa Exp $*/
/*30.08.2011	30.08.2011	Белых А.И.	admin_provbaz.c
Проверка на наличие базы данных
Если вернули 0-база есть
             1-базы нет
*/
#include "buhg_g.h"

extern SQL_baza bd;

int admin_provbaz(const char *imapb,GtkWidget *wpredok)
{

/*проверяем существует ли база данных с таким именем*/
SQL_str row;
int kolstr=0;
class SQLCURSOR cur;

const char *strsql={"show databases"};
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(-1);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_SRAV(imapb,row[0],0) == 0)
   {
    return(0);
   }
 }
return(1);
}
