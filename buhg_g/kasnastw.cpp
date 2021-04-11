/* $Id: kasnastw.c,v 1.10 2014/08/31 06:19:20 sasa Exp $ */
/*11.07.2015	220.09.2000	Белых А.И.	kasnastw.c
Чтение настроек для программы учета кассовых ордеров
Если вернули 0-файл прочитан
	     1-нет
*/
#include        "buhg_g.h"

short		vplsh; /*0-двух порядковый план счетов 1-мнопорядковый*/
short		startgodb; /*Стартовый год главной книги*/
extern SQL_baza bd;

int kasnastw()
{
class iceb_u_str st1("");


vplsh=startgodb=0;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='nastrb.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"nastrb.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  
  iceb_u_polen(row_alx[0],&st1,1,'|');
  if(iceb_u_SRAV(st1.ravno(),"Многопорядковый план счетов",0) == 0)
   {
    iceb_u_polen(row_alx[0],&st1,2,'|');
    if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
      vplsh=1;
    continue;
   }
  if(iceb_u_SRAV(st1.ravno(),"Стартовый год",0) == 0)
   {
    iceb_u_polen(row_alx[0],&st1,2,'|');
    startgodb=st1.ravno_atoi(); 
    continue;
   }

 }
return(0);
}
