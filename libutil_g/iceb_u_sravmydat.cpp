/*$Id: iceb_u_sravmydat.c,v 1.9 2014/02/28 05:21:05 sasa Exp $*/
/*13.11.2011	06.05.2001	Белых А.И.	iceb_u_sravmydat.c
Сравнение двух дат
Если даты равны возвращаем      =  0
Если первая дата меньше         <  -1
Если первая дата больше         >  +1
*/
#include	<stdio.h>
#include	<stdlib.h>
#include "iceb_util.h"

int iceb_u_sravmydat(short d1,short m1,short g1,short d2,short m2,short g2)
{

if(g1 == g2 && m1 == m2 && d1 == d2)
  return(0);
  
if(g1 > g2)
  return(1);

if(g1 < g2)
  return(-1);

//Годы равны
if(m1 > m2)
 return(2);
if(m1 < m2)
 return(-2);

//Месяцы равны

if(d1 > d2)
 return(3);
if(d1 < d2)
 return(-3);

return(0);

}
/**********************************/
int iceb_u_sravmydat(const char *data1,const char *data2)
{
char data11[64];
memset(data11,'\0',sizeof(data11));
char data22[64];
memset(data22,'\0',sizeof(data22));
short d1,m1,g1,d2,m2,g2;

strncpy(data11,data1,sizeof(data11)-1);
strncpy(data22,data2,sizeof(data22)-1);

iceb_u_rsdat(&d1,&m1,&g1,data11,1);
iceb_u_rsdat(&d2,&m2,&g2,data22,1);

return(iceb_u_sravmydat(d1,m1,g1,d2,m2,g2));

}
/******************************/

int iceb_u_sravmydat(const char *data1,const char *data2,int metka)
{
short d1,m1,g1,d2,m2,g2;
iceb_u_rsdat(&d1,&m1,&g1,data1,metka);
iceb_u_rsdat(&d2,&m2,&g2,data2,metka);

return(iceb_u_sravmydat(d1,m1,g1,d2,m2,g2));

}
/*****************************/
int iceb_u_sravmydat(short d,short m,short g,const char *data2,int metka)
{
short d2,m2,g2;
iceb_u_rsdat(&d2,&m2,&g2,data2,metka);

return(iceb_u_sravmydat(d,m,g,d2,m2,g2));

}
