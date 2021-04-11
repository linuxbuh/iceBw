/*$Id:$*/
/*06.02.2017	06.02.2017	Белых А.И.	mu_pvkuw.cpp
проверка возможности корректировки или удаления в подсистеме "Материальный учёт"

может удалять и корректировать только тот кто сделал запись
или тот кто имеет полномочия на блокировку/разблокировку подсистемы "Материальный учёт"
Возвращаем 0-можно
           1-нельзя

*/
#include "buhg_g.h"

int mu_pvkuw(int kodop_v_prov,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select mub from icebuser where login='%s'",iceb_u_getlogin());

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
    iceb_menu_soob(gettext("Удалять и корректировать записи может тот кто их сделал\nили тот кому разрешена блокировка/разблокировка мат. учёта!"),wpredok);
    return(1);
   }  
 }
return(0);
}
