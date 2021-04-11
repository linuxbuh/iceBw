/*$Id: xdkudgod.c,v 1.7 2013/08/13 06:09:58 sasa Exp $*/
/*13.07.2015	18.05.2006	Белых А.И.	xdkudgod.c
Удаление данных за год
*/
#include "buhg_g.h"

int xdkudgod_r(short god,GtkWidget *wpredok);


void xdkudgod()
{
char strsql[512];

SQL_str		row;
SQLCURSOR       cur;





/*проверяем полномочия оператора*/
sprintf(strsql,"select pdb,pdn from icebuser where login='%s'",iceb_u_getlogin());
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
repl.plus(gettext("Удаление документов за год"));
repl.ps_plus(gettext("Введите год"));
char god_h[6];
memset(god_h,'\0',sizeof(god_h));

if(iceb_menu_vvod1(&repl,god_h,5,"",NULL) != 0)
 return;

short god=atoi(god_h);

sprintf(strsql,gettext("Удалить документы за %d год ? Вы уверены ?"),god);

if(iceb_menu_danet(strsql,2,NULL) == 2)
  return;
if(iceb_parol(0,NULL) != 0)
 return;

xdkudgod_r(god,NULL);


}
