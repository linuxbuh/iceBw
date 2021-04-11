/*$Id: udgodsw.c,v 1.2 2013/08/13 06:09:53 sasa Exp $*/
/*13.07.2015	18.11.2012	Белых А.И.	udgodsw.c
Удаление документов в подсистеме "Учёт командировочных расходов"
*/
#include "buhg_g.h"
int udgodsw_r(short god_ud,GtkWidget *wpredok);

void udgodsw(GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
/*проверяем полномочия оператора*/
sprintf(strsql,"select udb,udn from icebuser where login='%s'",iceb_u_getlogin());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s %s %s icebuser",gettext("Не найден логин"),iceb_u_getlogin(),gettext("в таблице"));
  iceb_menu_soob(strsql,wpredok);
  return;
 }
if(atoi(row[0]) != 0 || atoi(row[1]) != 0)
 {
  iceb_menu_soob(gettext("У вас нет полномочий для этой операции!"),wpredok);
  return;
 }

class iceb_u_str repl(gettext("Удаление документов за год выполняется\nдля уменьшения размеров базы данных\nВведите год"));
class iceb_u_str god("");
if(iceb_menu_vvod1(repl.ravno(),&god,5,"",wpredok) != 0)
 return;

if(iceb_parol(0,wpredok) != 0)
 return;

udgodsw_r(god.ravno_atoi(),wpredok);



}
