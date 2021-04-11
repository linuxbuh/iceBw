/*$Id: iceb_get_pnk.c,v 1.4 2013/08/13 06:10:10 sasa Exp $*/
/*13.03.2020	01.09.2011	Белых А.И.	iceb_get_pnk.c
Получение наименования контрагента короткого или полного если полного нет выдаётся короткое
*/
#include "iceb_libbuh.h"

extern SQL_baza bd;

const char *iceb_get_pnk(const char *kodkontr,
int metka, /*0-короткое наименование 1-длинное 2-короткое с содержимым в скобках 3-длинное с содержимым в скобках*/
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
static class iceb_u_str pnaim;
pnaim.new_plus("");

if(iceb_u_SRAV(kodkontr,"00-",1) == 0)
 sprintf(strsql,"select naikon,pnaim from Kontragent where kodkon='%s'","00");
else
 sprintf(strsql,"select naikon,pnaim from Kontragent where kodkon='%s'",kodkontr);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(metka == 0)
   {
    if(iceb_u_polen(row[0],&pnaim,1,'(') != 0)
      pnaim.new_plus(row[0]);
   }
  if(metka == 1)
   {  
    if(row[1][0] == '\0')
     {
      if(iceb_u_polen(row[0],&pnaim,1,'(') != 0)
        pnaim.new_plus(row[0]);
     }
    else
      pnaim.new_plus(row[1]);
   }
  if(metka == 2)
   {
    pnaim.new_plus(row[0]);
   }
  if(metka == 3)
   {  
    if(row[1][0] == '\0')
     {
      pnaim.new_plus(row[0]);
     }
    else
      pnaim.new_plus(row[1]);
   }
 } 
else
 {
  pnaim.new_plus(gettext("Не найден код контрагента"));
  pnaim.plus(" ",kodkontr);
 }
return(pnaim.ravno());
}