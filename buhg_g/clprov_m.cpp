/*$Id: clprov_m.c,v 1.11 2013/08/13 06:09:33 sasa Exp $*/
/*10.07.2015	13.04.2004	Белых А.И.	clprov_m.c
Удаление проводок
*/
#include        "buhg_g.h"

void clprov_r(int metka_r,short mu,short gu);


void clprov_m()
{
char strsql[512];

SQL_str		row;
SQLCURSOR       cur;





/*проверяем полномочия оператора*/
sprintf(strsql,"select gkb,gkn from icebuser where login='%s'",iceb_u_getlogin());
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




iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Удаление проводок"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Удаление проводок за месяц"));//0
punkt_m.plus(gettext("Удаление проводок за год"));//1


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

iceb_u_str repl;
iceb_u_str data;
short m,g;

switch(nomer)
 {
  case -1:
   return;


  case 0:
    repl.new_plus(gettext("Удаление проводок за месяц"));
    repl.ps_plus(gettext("Введите дату"));
    repl.plus(" ");
    repl.plus(gettext("(м.р)"));
    if(iceb_menu_vvod1(&repl,&data,8,"",NULL) != 0)
     return;
    if(iceb_u_rsdat1(&m,&g,data.ravno()) != 0)
     {
      repl.new_plus(gettext("Не верно введена дата !"));
      iceb_menu_soob(&repl,NULL);
      return;
     }

    break;

  case 1:
    repl.new_plus(gettext("Удаление проводок за год"));
    repl.ps_plus(gettext("Введите год"));
    if(iceb_menu_vvod1(&repl,&data,8,"",NULL) != 0)
     return;
    g=atoi(data.ravno());    
    break;
     
 }
if(data.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели дату!"),NULL);
  return;
 }

repl.new_plus(gettext("Удалить проводки за"));
repl.plus(" ");
repl.plus(data.ravno());
if(iceb_menu_danet(&repl,2,NULL) == 2)
 return;

if(iceb_parol(0,NULL) != 0)
  return;

clprov_r(nomer,m,g);
  
}
