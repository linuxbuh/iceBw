/*$Id: zarlgotw.c,v 1.16 2013/08/13 06:10:03 sasa Exp $*/
/*13.07.2015	26.04.2004	Белых А.И.	zarlgotw.c
Чтение процента льгот по подоходному налогу
*/
#include "buhg_g.h"

extern SQL_baza bd;
extern int metka_psl; /*Метка применения социальной льготы 0-применяется 1-нет*/

float  zarlgotw(int tabn,short mr,short gr,int *kollgot,class iceb_u_str *kod_lgot,GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
float procent=0.;

*kollgot=0;
kod_lgot->new_plus("");

if(metka_psl == 1)
 {
  sprintf(strsql,"%s-Расчёт без применения социальной льготы!!!\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return(0.);
 }

sprintf(strsql,"select pl,koll,kodl from Zarlgot where tn=%d and dndl <= '%04d-%02d-01' order by dndl desc limit 1",tabn,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  *kollgot=atoi(row[1]);
  procent=atof(row[0]);
  kod_lgot->new_plus(row[2]);
 }
else
 {
  sprintf(strsql,"%s-Льгота не обнаружена!\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
 }  
return(procent);

}
