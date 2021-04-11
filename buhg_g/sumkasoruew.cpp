/* $Id: sumkasoruew.c,v 1.1 2013/09/26 09:46:55 sasa Exp $ */
/*09.11.2013	09.11.2013	Белых А.И.	sumkasoruew.c
Вычисление суммы по кассовому оpдеpу с переводом в гривню
Если веpнули 0-записи в документе есть
             1-нет
*/
#include        "buhg_g.h"

extern SQL_baza bd;

int sumkasoruew(const char *kassa,short tipz,const char *nomd,short god,double *sumd,double *sumpod,GtkWidget *wpredok)
{
char		strsql[512];
int		kolstr;
SQL_str		row;
SQLCURSOR cur;
class iceb_u_str kod_val("");

*sumd=*sumpod=0.;


sprintf(strsql,"select kv from Kas where kod=%d",atoi(kassa));
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 kod_val.new_plus(row[0]);

class iceb_getkue_dat kue(kod_val.ravno(),wpredok);



sprintf(strsql,"select datp,suma from Kasord1 where kassa=%s and nomd='%s' and tp=%d and god=%d",
kassa,nomd,tipz,god);
//printw("sumkasor=%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);

if(kolstr <= 0)
  return(1);
double sum=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  sum=kue.fromue(row[1],row[0]);  

  *sumd+=sum;
  if(row[0][0] == '0')
    continue;
  *sumpod+=sum;
 }

return(0);

}
