/*$Id: upl_ptw.c,v 1.6 2013/05/17 14:58:28 sasa Exp $*/
/*13.07.2015	02.05.2005	Белых А.И.	upl_ptw.c
Подпрограмма определения количества переданного топлива другим водителям
*/
#include "buhg_g.h"

extern SQL_baza bd;

double upl_ptw(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kod_avt,
const char *kod_vod,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
double suma=0.;
int kolstr;

//Смотрим передачи топлива другим водителям
sprintf(strsql,"select kolih,nomd from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' and kap=%s and kodsp=%s and tz=1",
gn,mn,dn,gk,mk,dk,kod_avt,kod_vod);

//printw("\nupl_pt-%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  suma+=atof(row[0]); 
 }
if(suma != 0.)
 suma=iceb_u_okrug(suma,0.001);
return(suma);
}
