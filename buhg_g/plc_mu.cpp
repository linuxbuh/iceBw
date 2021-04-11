/*$Id: plc_mu.c,v 1.10 2013/08/13 06:09:48 sasa Exp $*/
/*27.02.2017	18.11.2004	Белых А.И.	plc_mu.c
Проверка логической целосности базы
*/
#include "buhg_g.h"

int plc_mu_r(int metka,iceb_u_str*,GtkWidget *wpredok);


void plc_mu(GtkWidget *wpredok)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Проверка логичной целосности базы"));//0
punkt_m.plus(gettext("Проверка логической целосности базы и удаление нелогичных записей"));//1


int nomer=0;
if((nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL)) == -1)
 return;

if(nomer == 1)
 {
  char strsql[512];
  SQL_str row;
  class SQLCURSOR cur;
  /*проверяем полномочия оператора*/
  sprintf(strsql,"select mub,mun,gkb,gkn from icebuser where login='%s'",iceb_u_getlogin());
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
class iceb_u_str imaf_p;

if(plc_mu_r(nomer,&imaf_p,wpredok) != 0)
 {
  class iceb_u_spisok imaf_s;
  class iceb_u_spisok naim_s;
  
  imaf_s.plus(imaf_p.ravno());
  naim_s.plus(gettext("Протокол ошибок"));
  
  iceb_rabfil(&imaf_s,&naim_s,NULL);
  
 }


}
