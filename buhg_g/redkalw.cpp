/* $Id: redkalw.c,v 1.11 2013/08/13 06:09:51 sasa Exp $ */
/*12.07.2015    06.01.1994      Белых А.И.      redkalw.c
Чтение количества рабочих дней и часов за нужный месяц
*/

#include        "buhg_g.h"

extern SQL_baza bd;


void redkalw(short m,short g, //дата
float *dnei,float *has,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
*dnei=0.;
*has=0.;

sprintf(strsql,"select krd,krh from Zarskrdh where data='%04d-%02d-01'",g,m);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  *dnei=atof(row[0]);
  *has=atof(row[1]);
  sprintf(strsql,"%s-Количество рабочих дней %.2f часов %.2f  %d.%d\n",__FUNCTION__,*dnei,*has,m,g);
  zar_pr_insw(strsql,wpredok);
 }
else
 {
  sprintf(strsql,"%s-Не введено количество рабочих дней и часов в месяце %d.%d\n",__FUNCTION__,m,g);
  zar_pr_insw(strsql,wpredok);
 }
}
