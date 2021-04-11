/*$Id: matuddokw.c,v 1.7 2013/08/13 06:09:47 sasa Exp $*/
/*12.07.2015	01.05.2005	Белых А.И.	matuddokw.c
Удаление документов за год
*/

#include "buhg_g.h"

int matuddokw_r(short,GtkWidget*);


void matuddokw()
{

char strsql[512];

SQL_str		row;
SQLCURSOR       cur;





/*проверяем полномочия оператора*/
sprintf(strsql,"select mub,mun from icebuser where login='%s'",iceb_u_getlogin());
if(iceb_sql_readkey(strsql,&row,&cur,NULL) != 1)
 {
  sprintf(strsql,"%s %s %s icebuser",gettext("Не найден логин"),iceb_u_getlogin(),gettext("в таблице"));
  iceb_menu_soob(strsql,NULL);
  return;
 }
if(atoi(row[0]) != 0 || atoi(row[1]) != 0)
 {
  iceb_menu_soob(gettext("У вас нет полномочий для этой операции!"),NULL);
  return;
 }


iceb_u_str repl;
repl.plus(gettext("\
В н и м а н и е !!!\n\
Режим удаления документов за год.\n\
Удаление документов за прошедшие годы выполняется с целью уменьшения\n\
объёма базы данных."));

repl.ps_plus(gettext("Введите год"));
iceb_u_str god;
god.plus("");
if(iceb_menu_vvod1(&repl,&god,5,"",NULL) != 0)
 return;

if(iceb_parol(0,NULL) != 0)
 return;



matuddokw_r((short)god.ravno_atoi(),NULL);

}
