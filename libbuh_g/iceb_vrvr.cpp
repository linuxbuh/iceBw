/*$Id:$*/
/*22.11.2016	22.11.2016	Белых А.И.	iceb_vrvr.c
вставка реквизитов в распечатку
если вернули 0-вставлено
             1-нет
*/
#include "iceb_libbuh.h"
extern SQL_baza bd;

int iceb_vrvr(const char *imaf,class iceb_vrvr *rv,FILE *ff,GtkWidget *wpredok)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str stroka("");
int nom_str=0;

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }
if(kolstr == 0)
 {
  sprintf(strsql,"%s-%s %s",__FUNCTION__,gettext("Не найден файл"),imaf);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  nom_str++;
  stroka.new_plus(row[0]);

  if(rv != NULL)
  for(int nom=0; nom < rv->ns.kolih(); nom++)
   {
    if(rv->ns.ravno(nom) == nom_str)
     iceb_u_vstav(&stroka,rv->str.ravno(nom),rv->begin.ravno(nom),rv->end.ravno(nom),rv->hag.ravno(nom));     
   }  
  fprintf(ff,"%s",stroka.ravno());
 }

return(0);

}
