/*$Id: buhnast_g.c,v 1.21 2013/09/26 09:46:46 sasa Exp $ */
/*10.07.2015    21.12.1993      Белых А.И.      buhnast_g.c
Чтение настройки для бугалтерии
Если вернули 0-файл прочитан
	     1-нет
*/
#include        "buhg_g.h"

double		okrcn=0.;  /*Округление цены*/
double		okrg1=0.;  /*Округление суммы*/
short		koolk; /*Корректор отступа от левого края*/
short		startgod=0; /*Стартовый год материального учёта*/
short		startgodb=0; /*Стартовый год главной книги*/
short		startgoduos=0; /*Стартовый год для Учёта основных средств*/
short		vplsh; /*0-двух порядковый план счетов 1-многпорядковый*/
char		*sbshet=NULL; //Список бюджетных счетов
extern SQL_baza bd;

int             buhnast_g()
{
class iceb_u_str st1("");

vplsh=startgodb=koolk=0;

if(sbshet != NULL)
 {
  delete [] sbshet;
  sbshet=NULL;
 }
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

  if(iceb_u_SRAV(st1.ravno(),"Корректор отступа от левого края",0) == 0)
    {
     iceb_u_polen(row_alx[0],&koolk,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Многопорядковый план счетов",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
       vplsh=1;
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],&startgodb,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Список бюджетных счетов",0) == 0)
   {
    class iceb_u_str bros("");
    iceb_u_polen(row_alx[0],&bros,2,'|');
    if(bros.ravno()[0] == '\0')
      continue;
    sbshet=new char[strlen(bros.ravno())+1];
    strcpy(sbshet,bros.ravno());
   }

 }

startgod=0;

okrg1=okrcn=0.;
sprintf(strsql,"select str from Alx where fil='matnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"matnast.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;


  iceb_u_polen(row_alx[0],&st1,1,'|');

  if(iceb_u_SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],&startgod,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Округление цены",0) == 0)
    {
     iceb_u_polen(row_alx[0],&okrcn,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Округление суммы",0) == 0)
    {
     iceb_u_polen(row_alx[0],&okrg1,2,'|');
     continue;
    }

 }
startgoduos=0;

sprintf(strsql,"select str from Alx where fil='uosnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uosnast.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  iceb_u_polen(row_alx[0],&st1,1,'|');

  if(iceb_u_SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],&startgoduos,2,'|');
     continue;
    }
 }

return(0);
}
