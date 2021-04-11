/*$Id: xkasplcw.c,v 1.3 2013/08/13 06:09:58 sasa Exp $*/
/*13.07.2015	11.10.2012	Белых А.И.	xkasplcw.c
Проверка логической целосности базы в подсистеме "Учёт кассвых ордеров"
*/
#include "buhg_g.h"
int xkasplcw_r(int metka_prov,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok);

void xkasplcw(GtkWidget *wpredok)
{
class iceb_u_spisok imaf;
class iceb_u_spisok naimf;
int metka_prov=0;
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Проверка логической целосности базы"));


zagolovok.plus(gettext("Проверка логической целосности базы"));

punkt_m.plus(gettext("Проверка без удаления не логичных записей"));//0
punkt_m.plus(gettext("Проверка с удалением не логичных записей"));//1


metka_prov=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,wpredok);
switch(metka_prov)
 {
  case -1:
   return;
 }

if(metka_prov == 1)
 {
  char strsql[512];
  SQL_str row;
  class SQLCURSOR cur;
  /*проверяем полномочия оператора*/
  sprintf(strsql,"select kob,kon,gkb,gkn from icebuser where login='%s'",iceb_u_getlogin());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s %s %s icebuser",gettext("Не найден логин"),iceb_u_getlogin(),gettext("в таблице"));
    iceb_menu_soob(strsql,wpredok);
    return;
   }
  if(atoi(row[0]) != 0 || atoi(row[1]) != 0 || atoi(row[2]) != 0 || atoi(row[3]) != 0)
   {
    iceb_menu_soob(gettext("У вас нет полномочий для этой операции!"),wpredok);
    return;
   }

 }

if(xkasplcw_r(metka_prov,&imaf,&naimf,wpredok) != 0)
 return;

iceb_rabfil(&imaf,&naimf,NULL);

}