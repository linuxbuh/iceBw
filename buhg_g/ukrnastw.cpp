/*$Id: ukrnastw.c,v 1.11 2013/09/26 09:46:56 sasa Exp $*/
/*13.07.2015	22.10.2002	Белых А.И.	ukrnastw.c
Чтение настроечной информации для подсистемы "Учет командировочных расходов."
*/
#include	"buhg_g.h"


short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		startgod=0;
short		startgodb=0; /*Стартовый год просмотров главной книги*/
short           mfnn=0; //Метка формирования номера в этой подсистеме не задействована
double		okrcn=0.01;  /*Округление цены*/
double		okrg1=0.01;  /*Округление суммы*/
extern SQL_baza bd;

int ukrnastw()
{
class iceb_u_str st1("");

startgodb=startgod=0;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='ukrnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"ukrnast.alx");
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
//    startgod=(short)iceb_u_atof(st1); 
    continue;
   }
 }

iceb_poldan("Многопорядковый план счетов",&st1,"nastrb.alx",NULL);
if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
 vplsh=1;

startgodb=0;
if(iceb_poldan("Стартовый год",&st1,"nastrb.alx",NULL) == 0)
  startgodb=st1.ravno_atoi();

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
okrg1=0.01;
okrcn=0.01;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  iceb_u_polen(row_alx[0],&st1,1,'|');

  if(iceb_u_SRAV(st1.ravno(),"Округление суммы",0) == 0)
   {
     iceb_u_polen(row_alx[0],&okrg1,2,'|');
//     okrg1=iceb_u_atof(st1);
     continue;
   }
  if(iceb_u_SRAV(st1.ravno(),"Округление цены",0) == 0)
   {
    iceb_u_polen(row_alx[0],&okrcn,2,'|');
//    okrcn=iceb_u_atof(st1);
    continue;
   }
 }
return(0); 
}
