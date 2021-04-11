/*$Id: in_d12_itw.c,v 1.7 2013/05/17 14:58:21 sasa Exp $*/
/*14.05.2013	23.10.2007	Белых А.И.	in_d12_itw.c
Распечатка итога по счетам
*/

#include "buhg_g.h"

void in_d12_itw(class iceb_u_spisok *sp_shet,class iceb_u_double *kol_shet,class iceb_u_double *sum_shet,FILE *ffipsh,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

class iceb_u_str naim_shet("");
double i_psh_kol=0.;
double i_psh_sum=0.;
for(int ii=0; ii < sp_shet->kolih(); ii++)
 {
  naim_shet.new_plus(sp_shet->ravno(ii));
  /*узнаём наименование счёта*/
  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shet->ravno(ii));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    naim_shet.plus(" ",row[0]);
   }
  

  fprintf(ffipsh,"%5d %-*.*s|%10.10g|    |%10.10g|          |    |          |          |    |          |\n",
  ii+1,
  iceb_u_kolbait(30,naim_shet.ravno()),iceb_u_kolbait(30,naim_shet.ravno()),naim_shet.ravno(),
  kol_shet->ravno(ii),sum_shet->ravno(ii));

  if(iceb_u_strlen(naim_shet.ravno()) > 30)
   fprintf(ffipsh,"%5s %s\n","",iceb_u_adrsimv(30,naim_shet.ravno()));

  fprintf(ffipsh,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
   
  i_psh_kol+=kol_shet->ravno(ii);
  i_psh_sum+=sum_shet->ravno(ii);
 }

fprintf(ffipsh,"\
----------------------------------------------------------------------------------------------------------------------\n");
fprintf(ffipsh,"%*s %10.10g      %10.10g\n",
iceb_u_kolbait(36,"Разом"),"Разом",
i_psh_kol,i_psh_sum);


}
