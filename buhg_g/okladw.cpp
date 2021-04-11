/* $Id: okladw.c,v 1.14 2014/02/28 05:21:00 sasa Exp $ */
/*30.11.2016	01.08.2000	Белых А.И.	okladw.c
Найти в файле оклад на нужный табельный номер
*/
#include "buhg_g.h"

extern SQL_baza bd;

double okladw(int tabn,
short mr,short gr,
int *has, /*0-оклад 1-часовая ставка 2-дневная ставка*/
GtkWidget *wpredok)
{
char strsql[512];
double		oklad;
class iceb_u_str bros("");
SQL_str row_alx;
class SQLCURSOR cur_alx;

*has=0;

sprintf(strsql,"select sm,mt from Zarsdo where tn=%d and dt <= '%04d-%02d-01' order by dt desc limit 1",tabn,gr,mr);
if(iceb_sql_readkey(strsql,&row_alx,&cur_alx,wpredok) != 1)
 return(0.);

oklad=atof(row_alx[0]);

*has=atoi(row_alx[1]);

if(*has == 5 || *has == 6)
  oklad=okladtrw(mr,gr,oklad,wpredok);




return(oklad);
}
/**********************************************/
double okladw(int tabn,
int *has, /*0-оклад 1-часовая ставка 2-дневная ставка*/
GtkWidget *wpredok)
{
char strsql[512];
double		oklad;
class iceb_u_str bros("");
SQL_str row_alx;
class SQLCURSOR cur_alx;
short dr=0,mr=0,gr=0;
iceb_u_poltekdat(&dr,&mr,&gr);

*has=0;

sprintf(strsql,"select sm,mt from Zarsdo where tn=%d and dt <= '%04d-%02d-01' limit 1",tabn,gr,mr);
if(iceb_sql_readkey(strsql,&row_alx,&cur_alx,wpredok) != 1)
 return(0.);

oklad=atof(row_alx[0]);

*has=atoi(row_alx[1]);

if(*has == 5 || *has == 6)
  oklad=okladtrw(mr,gr,oklad,wpredok);
return(oklad);
}
