/*$Id:$*/
/*12.02.2016	12.02.2016	Белых А.И.	iceb_pvkup.c

проверка возможности корректировки или удаления проводки

Проводку может удалять и корректировать только тот кто её сделал
или тот кто имеет полномочия на блокировку/разблокировку подсистемы
"Главная книга"
Возвращаем 0-можно
           1-нельзя

*/
#include "iceb_libbuh.h"

int iceb_pvkup(int kodop_v_prov,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select gkb from icebuser where login='%s'",iceb_u_getlogin());

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s-%s\n%s %s",
  __FUNCTION__,
  gettext("У Вас нет полномочий для выполнения этой операции!"),
  gettext("Ненайден логин"),
  iceb_u_getlogin());

  iceb_menu_soob(strsql,wpredok);

  return(1);
  
 }

if(atoi(row[0]) == 1)
 {
  if(kodop_v_prov != iceb_getuid(wpredok))
   {
    iceb_menu_soob(gettext("Удалять и корректировать проводки может тот кто её сделал\nили тот кому разрешена блокировка/разблокировка проводок!"),wpredok);
    return(1);
   }  
 }
return(0);
}