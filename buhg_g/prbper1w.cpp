/* $Id: prbper1w.c,v 1.5 2013/08/13 06:09:49 sasa Exp $ */
/*12.07.2015    27.12.1996	Белых А.И.	prbpern1w.c
Определяем сумму обязательных удержаний за месяц расчёта плюс все выплаты в счёт
месяца расчёта 
если зарплата выплачена то эта сумма должна равнятся сумме начислений в месяце расчёта
*/
#include        "buhg_g.h"
extern SQL_baza bd;
extern short	*obud; /*Обязательные удержания*/

double prbper1w(short mz,short gz,short dk,short mk,short gk,
int tabb,FILE *ff_prot,GtkWidget *wpredok)
{
double          sum=0.;
long		kolstr;
SQL_str         row;
char		strsql[512];

sprintf(strsql,"select datz,knah,suma,godn,mesn from Zarp where godn=%d and mesn=%d and \
tabn=%d and prn='2' and datz <= '%d-%d-%d' and suma <> 0. \
order by tabn,godn,mesn asc",gz,mz,tabb,gk,mk,dk);

if(ff_prot != NULL)
 fprintf(ff_prot,"%s-%s\n",__FUNCTION__,strsql);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"__FUNCTION__",strsql,wpredok);
  return(0.);
 }
short d,m,g;

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  /*Если в будущих датах есть перерасчёт подоходного в счёт месяца расчёта то 
  он не должен попадать*/
  if(provkodw(obud,atoi(row[1])) >= 0) 
   {
    if(m > mz || g > gz)
      continue;
   }    
  sum+=atof(row[2]);
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %3s %10s %s.%s\n",row[0],row[1],row[2],row[4],row[3]);
 }

/*Обязательные ужержания не в счёт расчётного месяца сделанные в расчётном
месяце должны тоже попадать- перерасчёт подоходного */
sprintf(strsql,"select datz,knah,suma,godn,mesn from Zarp where \
datz >= '%d-%d-%d' and datz <= '%d-%d-%d' and (godn != %d or mesn != %d) and \
tabn=%d and prn='2' and suma <> 0. \
order by tabn,godn,mesn asc",gz,mz,1,gz,mz,31,gz,mz,tabb);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"__FUNCTION__",strsql,wpredok);
  return(0.);
 }

if(ff_prot != NULL)
 fprintf(ff_prot,"%s-%s\n",__FUNCTION__,strsql);
 
while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(obud,atoi(row[1])) < 0) 
   continue;

  sum+=atof(row[2]);
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %3s %10s %s.%s\n",row[0],row[1],row[2],row[4],row[3]);
 }

if(sum != 0.)
 sum=iceb_u_okrug(sum,0.01);

return(sum);

}
