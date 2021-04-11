/*$Id: zar_pr_showw.c,v 1.3 2013/08/13 06:10:01 sasa Exp $*/
/*13.07.2015	26.11.2011	Белых А.И.	zar_pr_showw.c
Просмотр протокола расчёта заработной платы
*/
#include <errno.h>
#include "buhg_g.h"
extern SQL_baza bd;

void zar_pr_showw(int tipz,int tabn,short mr,short gr,GtkWidget *wpredok)
{
FILE *ff=NULL;
char imaf[64];
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
int metka_fopen=0;
sprintf(imaf,"zarpr%d.lst",getpid());

if(tabn == 0)
 {
  if(tipz != 0)
    sprintf(strsql,"select sz from Zarpr where tz=%d and dr='%04d-%02d-01' order by tn asc,tz asc, nz asc",tipz,gr,mr);
  else
    sprintf(strsql,"select sz from Zarpr where dr='%04d-%02d-01' order by tn asc,tz asc, nz asc",gr,mr);
 }
else
 {
  sprintf(strsql,"select sz from Zarpr where tn=%d and tz=%d and dr='%04d-%02d-01' order by tz asc, nz asc",0,0,gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }
  
  if(kolstr > 0)
   {
    if((ff = fopen(imaf,"w")) == NULL)
     {
      iceb_er_op_fil(imaf,"",errno,wpredok);
      return;
     }
    metka_fopen=1;
    while(cur.read_cursor(&row) != 0)
     {
      fprintf(ff,"%s",row[0]);
     }
   }

  sprintf(strsql,"select sz from Zarpr where tn=%d and tz=%d and dr='%04d-%02d-01' order by tz asc, nz asc",tabn,tipz,gr,mr);
 }


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  if(metka_fopen > 0)
   fclose(ff);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Протокол хода расчёта не найден!"),wpredok);
  if(metka_fopen > 0)
   fclose(ff);
  return;
 }






if(metka_fopen == 0)
 {
  if((ff = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return;
   }
 }
 
while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff,"%s",row[0]);
 }

iceb_podpis(ff,wpredok);
fclose(ff);


class iceb_u_spisok imafs;
imafs.plus(imaf);
class iceb_u_spisok naimf;

switch(tipz)
 {
  case 1:
    naimf.plus(gettext("Протокол расчёта начислений"));
    break;

  case 2:
    naimf.plus(gettext("Протокол расчёта удержаний"));
    break;

  case 3:
    naimf.plus(gettext("Протокол расчёта coц. начислений на зарплату"));
    break;

  default:
    naimf.plus(gettext("Протокол расчёта зарплаты"));
    break;    
 }

iceb_ustpeh(imaf,3,wpredok);
iceb_rabfil(&imafs,&naimf,wpredok);


}
