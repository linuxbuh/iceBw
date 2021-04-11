/*$Id: xbudkzw.c,v 1.1 2013/12/31 11:49:16 sasa Exp $*/
/*29.11.2013	25.11.2013	Белых А.И.	xbudkzw.c
Определение даты возникновения дебетовой/кредитовой задолженности по контрагенту
*/
#include <math.h>
#include "buhg_g.h"

extern SQL_baza bd;

int xbudkzw(int dk, /*1-кредитовая задолженность 2-дебетовая задолженность*/
const char *kodkontr,
const char *shet,
short dz,short mz,short gz, /*Дата на которую задолженность есть*/
double sum_z, /*Сумма задолженности*/
class iceb_u_str *dataz,
GtkWidget *wpredok)
{

char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

dataz->new_plus("");
if(fabs(sum_z) < 0.01)
 return(0);
 
if(dk == 1)
 sprintf(strsql,"select datp,kre from Prov where datp <= '%04d-%02d-%02d' and sh='%s' and kodkon='%s' and kre <> 0. order by datp desc",
 gz,mz,dz,shet,kodkontr);
if(dk == 2)
 sprintf(strsql,"select datp,deb from Prov where datp <= '%04d-%02d-%02d' and sh='%s' and kodkon='%s' and deb <> 0. order by datp desc",
 gz,mz,dz,shet,kodkontr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }

int nom=0;
double suma=0.;
while(cur.read_cursor(&row) != 0)
 {
  suma+=atof(row[1]);
  if(suma >= sum_z)
   {
    dataz->new_plus(iceb_u_datzap(row[0]));
    return(0);
   }  

  nom++;
  if(nom == kolstr)
   {
    dataz->new_plus(iceb_u_datzap(row[0]));
    return(0);
   }

 }


return(0);

}
