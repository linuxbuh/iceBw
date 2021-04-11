/*$Id: uplztfw.c,v 1.2 2013/08/13 06:09:57 sasa Exp $*/
/*13.07.2015	13.04.2012	Белых А.И.	uplztfw.c
Получение фактических затрат и по норме топлива по путевому листу
*/
#include "buhg_g.h"

extern SQL_baza bd;

double uplztfw(short dd,short md,short gd,int podr,const char *nom_dok,double *ztpn,GtkWidget *wpredok)
{
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];
double ztf=0.;
*ztpn=0.;
sprintf(strsql,"select zn,zf from Upldok3 where datd='%04d-%02d-%02d' and kp=%d and nomd='%s'",gd,md,dd,podr,nom_dok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
if(kolstr == 0)
 return(0.);
while(cur.read_cursor(&row) != 0)
 {
  *ztpn+=atof(row[0]);
  ztf+=atof(row[1]);
 }

return(ztf);
}
/*****************************************/

double uplztfw(const char *datadok,int podr,const char *nom_dok,GtkWidget *wpredok)
{
double ztpn=0.;
short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,datadok,1);
return(uplztfw(dd,md,gd,podr,nom_dok,&ztpn,wpredok));
}
