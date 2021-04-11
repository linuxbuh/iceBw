/*$Id:$*/
/*16.03.2017	16.03.2017	Белых А.И.	usl_pvkuw.cpp
проверка возможности корректировки или удаления в подсистеме "Учёт услуг"

может удалять и корректировать только тот кто сделал запись
или тот кто имеет полномочия на блокировку/разблокировку подсистемы "Учёт услуг"
Возвращаем 0-можно
           1-нельзя

*/
#include "buhg_g.h"

int usl_pvkuw(int kodop_v_prov,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select uub from icebuser where login='%s'",iceb_u_getlogin());

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
    iceb_menu_soob(gettext("Удалять и корректировать записи может тот кто их сделал\nили тот кому разрешена блокировка/разблокировка учёта услуг!"),wpredok);
    return(1);
   }  
 }
return(0);
}
