/* $Id: saldo_kontr.c,v 1.11 2013/08/13 06:09:52 sasa Exp $ */
/*12.07.2015    07.11.2005      Белых А.И.      saldo_kontr.c
Получение сальдо по оpганизации и счету
*/
#include        "buhg_g.h"


extern short	startgodb; /*Стартовый год*/
extern SQL_baza bd;

void saldo_kontr(const char *sh, //Счет
const char *kor,  //Код контрагента
double *db,  //Дебет
double *kr,  //Дебет
short dn,short mn,short gn, //Дата начала
short dk,short mk,short gk, //Дата конца
GtkWidget *wpredok)
{
short           d,m,g;
double          ddn,kkn;
double		dd,kk;
char		strsql[512];
SQL_str         row;
long		kolstr;
short		godn;
double		deb=0.,kre=0.;

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

*db=0.;
*kr=0.;


ddn=kkn=0.;
dd=kk=0.;
sprintf(strsql,"select deb,kre from Saldo where kkk='1' and gs=%d and \
ns='%s' and kodkon='%s'",godn,sh,kor);
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  ddn=atof(row[0]);
  kkn=atof(row[1]);
 }

g=godn;
m=1;
d=1;
while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {

   sprintf(strsql,"select * from Prov \
where val=0 and datp='%d-%02d-%02d' and sh='%s' and kodkon='%s'",
   g,m,d,sh,kor);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
      iceb_u_dpm(&d,&m,&g,1);
      continue;
     }
   while(cur.read_cursor(&row) != 0)
    {

     deb=atof(row[9]);
     kre=atof(row[10]);

     if(iceb_u_sravmydat(g,m,d,gn,mn,dn) >= 0)
     if(iceb_u_sravmydat(gk,mk,dk,g,m,d) >= 0)
      {
       dd+=deb;
       kk+=kre;

      }
     if(iceb_u_sravmydat(g,m,d,gn,mn,dn) < 0)
      {
       ddn+=deb;
       kkn+=kre;
      }

    }
  iceb_u_dpm(&d,&m,&g,1);
 }

*db+=dd+ddn;
*kr+=kk+kkn;

}
