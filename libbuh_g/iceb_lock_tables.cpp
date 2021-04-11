/*$Id: iceb_lock_tables.c,v 1.9 2013/05/17 14:58:35 sasa Exp $*/
/*20.11.2014	22.02.2004 	Белых А.И.	iceb_lock_tables.c
Класс для блокировки/разблокировки таблиц
*/
#include "iceb_libbuh.h"

extern SQL_baza	bd;

iceb_lock_tables::iceb_lock_tables() //конструктор без параметров
 {
  metka=0;
 }

iceb_lock_tables::iceb_lock_tables(iceb_u_str *tabl) //конструктор с параметрами
 {
  lock(tabl);
 }

iceb_lock_tables::iceb_lock_tables(const char *tabl) //конструктор с параметрами
 {
  lock(tabl);
 }

iceb_lock_tables::~iceb_lock_tables() //деструктор
 {
  unlock();    
 }

int iceb_lock_tables::lock(const char *tabl) //блокировка таблиц
 {
  if(sql_zap(&bd,tabl) != 0)
   {
    iceb_msql_error(&bd,__FUNCTION__,tabl,NULL);
    return(1);
   } 
  metka=1;
  return(0);    
 }

int iceb_lock_tables::lock(iceb_u_str *tabl) //блокировка таблиц
 {
  if(sql_zap(&bd,tabl->ravno()) != 0)
    return(1);
    
  metka=1;
  return(0);    
 }

int iceb_lock_tables::unlock() //разблокировка таблиц
 {
  if(metka == 0)
   return(0);

  const char *strsql={"UNLOCK TABLES"};

  if(sql_zap(&bd,strsql) != 0)
   {
    iceb_msql_error(&bd,__FUNCTION__,strsql,NULL);
    return(1);
   }
  metka=0;
  
  return(0);    

 }
