/*$Id: iceb_getkue.c,v 1.1 2013/09/26 09:47:04 sasa Exp $*/
/*05.09.2013	03.09.2013	Белых А.И.	iceb_t_getkue.h
получение cуммы в валюте

*/
#include <math.h>
#include "iceb_libbuh.h"

/************************/
double iceb_getkue_dat::toue(const char *suma,const char *data)
{
 return(iceb_getkue_dat::get(suma,data,0));
}
/**************************/
double iceb_getkue_dat::fromue(const char *suma,const char *data)
{
 return(iceb_getkue_dat::get(suma,data,1));
}
/***************************************/
double iceb_getkue_dat::get(const char *suma,const char *data,int metka_r)
{
if(suma == NULL)
 return(0.);

double suma_voz=atof(suma);

if(data == NULL)
  return(suma_voz);

if(data[0] == '\0')
  return(suma_voz);

if(kod_val.getdlinna() <= 1)
 return(suma_voz);
 
char strsql[512];
SQL_str row;
class SQLCURSOR cur;


if(fabs(suma_voz) < 0.01)
  return(0.);
  

if(iceb_u_SRAV(dataz.ravno(),data,0) != 0)
 {
  sprintf(strsql,"select kurs from Glkkv where datk <= '%s' order by datk desc limit 1",data);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    kurs_ue=atof(row[0]);
  dataz.new_plus(data);
 }

if(metka_r == 0)
 suma_voz=suma_voz/kurs_ue;
else
 suma_voz=suma_voz*kurs_ue;

suma_voz=iceb_u_okrug(suma_voz,0.01);

return(suma_voz);
}

