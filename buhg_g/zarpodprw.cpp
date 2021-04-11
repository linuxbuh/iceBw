/* $Id: zarpodprw.c,v 1.11 2013/08/13 06:10:03 sasa Exp $ */
/*13.07.2015	18.03.2000	Белых А.И.	zarpodprw.c
Поиск по коду подразделения процента выполнения плана
*/
#include        "buhg_g.h"

extern SQL_baza bd;

double zarpodprw(int kodpr,//Код подразделения
double *procb,  //Процент выполнения плана бюджетной части
GtkWidget *wpredok)
{
class iceb_u_str bros("");
double		procent;

*procb=0.;
procent=100.;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zarpodpr.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],&bros,1,'|') == 0)
   if(bros.ravno_atoi() == kodpr)
    {
     iceb_u_polen(row_alx[0],&procent,2,'|');
//     procent=iceb_u_atof(bros);
     iceb_u_polen(bros.ravno(),procb,2,'/');
//     *procb=iceb_u_atof(strok);
     break;
    }
 }

return(procent);

}
