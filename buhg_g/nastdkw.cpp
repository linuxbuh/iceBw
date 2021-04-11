/* $Id: nastdkw.c,v 1.14 2014/02/28 05:21:00 sasa Exp $ */
/*12.07.2015    10.12.1993      Белых А.И.      nastdkw.c
Читаем настройку  для платежных документов
Если вернули 0 файл прочитан
	     1-нет
*/
#include        "buhg_g.h"

double          okrg1=0.01; /*Округление*/
short           kopk; /*0-копеек нет 1-копейки есть*/
double          smp0,smp1; /*Суммы за переводы внутри банка и вне банка*/
short		mpo;       /*0-искать реквизиты в файле 1-в базе данных*/
short           mvs;       /*1-с разбивкой суммы по счетам 0-без*/
short		kor; /*Корректор межстрочного растояния*/
short		ots; /*Отступ от левого края*/
short		kp=1; /*Количество копий платежки*/
short		startgodd; /*Стартовый год платежных документов*/
short		startgodb; /*Стартовый год просмотров главной книги*/
short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		kolstrdok; //Количество строк между документами
extern SQL_baza bd;

int             nastdkw(void)
{
class iceb_u_str st1("");

mpo=mvs=kor=ots=0;

kp=1;
kolstrdok=10;
vplsh=startgodd=startgodb=0;
smp0=smp1=0.;
okrg1=0.01;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='nastdok.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"nastdok.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  iceb_u_polen(row_alx[0],&st1,1,'|');

  if(iceb_u_SRAV(st1.ravno(),"Поиск организаций в базе",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
       mpo=1;
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Разбивка суммы",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
       mvs=1;
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],&startgodd,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Копейки",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     kopk=0;
     if(iceb_u_SRAV(st1.ravno(),"Есть",0) == 0)
       kopk=1;
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Округление суммы",0) == 0)
    {
     iceb_u_polen(row_alx[0],&okrg1,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Количество копий платежки",0) == 0)
    {
     iceb_u_polen(row_alx[0],&kp,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Количество строк между документами",0) == 0)
    {
     iceb_u_polen(row_alx[0],&kolstrdok,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Корректор межстрочного растояния",0) == 0)
    {
     iceb_u_polen(row_alx[0],&kor,2,'|');
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Отступ от левого края",0) == 0)
    {
     iceb_u_polen(row_alx[0],&ots,2,'|');
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Сумма платежа",0) == 0)
    {
     iceb_u_polen(row_alx[0],&smp0,2,'|');
     iceb_u_polen(row_alx[0],&smp1,2,'/');
     continue;
    }


 }

vplsh=0;
iceb_poldan("Многопорядковый план счетов",&st1,"nastrb.alx",NULL);
if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
 vplsh=1;

startgodb=0;
if(iceb_poldan("Стартовый год",&st1,"nastrb.alx",NULL) == 0)
 {
  startgodb=st1.ravno_atoi();
 }


if(kp == 0)
 kp=1;


return(0);
}
