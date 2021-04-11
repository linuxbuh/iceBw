/*$Id: uplnomdokw.c,v 1.7 2013/05/17 14:58:29 sasa Exp $*/
/*13.07.2015	14.04.2003	Белых А.И.	uplnomdokw.c
Получение номер путевого листа
*/
#include        "buhg_g.h"

extern SQL_baza bd;

void uplnomdokw(short god,const char *podr,class iceb_u_str *nomdok,GtkWidget *wpredok)
{
int		kolstr;
SQL_str         row;
char		strsql[512];
int		nomer=0;
int		nomerv=0;
class SQLCURSOR cur;

sprintf(strsql,"select nomd from Upldok where god=%d and kp=%s",god,podr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_pole(row[0],strsql,2,'/') != 0)
   continue;

  nomer=atoi(strsql);

  if(nomerv < nomer)
    nomerv=nomer;
 }
nomerv++;
sprintf(strsql,"%s/%d",podr,nomerv);
nomdok->new_plus(strsql);
}
