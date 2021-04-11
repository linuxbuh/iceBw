/* $Id: sumprpdw.c,v 1.7 2013/05/17 14:58:27 sasa Exp $ */
/*12.07.2015	29.05.1998	Белых А.И.	sumprpdw.c
Определение суммы выполненных проводок для определенного
платежного документа.
*/
#include        "buhg_g.h"

extern SQL_baza bd;

double sumprpdw(short d,short m,short g,const char *nn,int pod,
const char *kto,const char *shet,
GtkWidget *wpredok)
{
double		sum;
long		kolstr;
SQL_str         row;
char		strsql[512];

/*Суммиреум выполненные проводки*/
sprintf(strsql,"select deb from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%d-%02d-%02d' and \
shk='%s'",kto,pod,nn,g,m,d,shet);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

if(kolstr == 0)
  return(0.);

sum=0.;

while(cur.read_cursor(&row) != 0)
  sum+=atof(row[0]);


return(sum);

}
