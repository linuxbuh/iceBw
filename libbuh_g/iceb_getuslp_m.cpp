/*$Id: iceb_getuslp_m.c,v 1.3 2013/12/31 11:49:18 sasa Exp $*/
/*27.12.2013	21.02.2011	Белых А.И.	iceb_getusl_m.c
Получение условия продажи
*/
#include "iceb_libbuh.h"

int iceb_getuslp_m(const char *kodkon,class iceb_u_str *usl_prod,GtkWidget *wpredok)
{
char strsql[2048];
int kolzap=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select datd,nomd,vidd,vnn from Kontragent2 where kodkon='%s'",kodkon);
if((kolzap=iceb_sql_readkey(strsql,&row,&cur,wpredok)) == 0)
 {
  
  iceb_menu_soob(gettext("Не введён договор для этого контрагента!"),wpredok);
  return(1);
  
 }
if(kolzap == 1)
 {
  sprintf(strsql,"%s:%s:%s",row[2],iceb_u_datzap(row[0]),row[1]);
  if(row[3][0] != '\0')
   {
    strcat(strsql,":");
    strcat(strsql,row[3]);
   }
  usl_prod->new_plus(strsql);
  return(0);
 } 
int un_nom_zap=0;
if((un_nom_zap=iceb_l_kontdog(1,kodkon,wpredok)) == -1)
 return(1);

return(iceb_getuslp(un_nom_zap,usl_prod,wpredok));

}
