/* $Id: sumkasorw.c,v 1.8 2013/05/17 14:58:27 sasa Exp $ */
/*12.07.2015	05.09.2000	Белых А.И.	sumkasorw.c
Вычисление суммы по кассовому оpдеpу
Если веpнули 0-записи в документе есть
             1-нет
            -1-ошибка чтения
*/
#include        "buhg_g.h"

extern SQL_baza bd;

int	sumkasorw(const char kassa[],short tipz,const char nomd[],short god,
double *sumd,double *sumpod,GtkWidget *wpredok)
{
char		strsql[512];
int		kolstr;
SQL_str		row;

*sumd=*sumpod=0.;

sprintf(strsql,"select datp,suma from Kasord1 where \
kassa=%s and nomd='%s' and tp=%d and god=%d",
kassa,nomd,tipz,god);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(-1);
 }

if(kolstr == 0)
  return(1);

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  
  *sumd=*sumd+atof(row[1]);
  if(row[0][0] == '0')
    continue;
  *sumpod=*sumpod+atof(row[1]);
 }
//OSTANOV();
return(0);

}
