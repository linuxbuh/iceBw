/*$Id: iceb_alxout.c,v 1.6 2014/05/07 10:58:10 sasa Exp $*/
/*07.09.2010	07.09.2010	Белых А.И.	iceb_alxout.c
Выгрузка настроек в файл
*/
#include <errno.h>
#include "iceb_libbuh.h"
extern SQL_baza bd;

int iceb_alxout(const char *imaf,const char *imafin,GtkWidget *wpredok)
{
FILE *ff;
char strsql[512];
int kolstr=0;
class iceb_u_str fil(imafin);
SQL_str row;
class SQLCURSOR cur;

if(imafin == NULL || imafin[0] == '\0')
 fil.new_plus(imaf);

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"",strsql,wpredok);
  return(1);
 }  

if(kolstr == 0)
 {
//  sprintf(strsql,"%s-%s\n%s",__FUNCTION__,gettext("Не найдено ни одной записи !"),imaf);
//  iceb_t_soob(strsql);
  return(1);
 }

if((ff=fopen(fil.ravno(),"w")) == NULL)
  {
   iceb_er_op_fil(fil.ravno(),"",errno,wpredok);
   return(1);
  }
while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff,"%s",row[0]);
 }

fclose(ff);

return(0);
}
/****************************************************/
int iceb_alxout(const char *imaf,GtkWidget *wpredok)
{

return(iceb_alxout(imaf,NULL,wpredok));
}


