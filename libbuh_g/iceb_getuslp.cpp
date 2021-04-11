/*$Id: iceb_getuslp.c,v 1.5 2013/12/31 11:49:18 sasa Exp $*/
/*27.04.2018	16.02.2011	Белых А.И.	iceb_getuslp.c
Получить условие продажи 
*/
#include "iceb_libbuh.h"

int iceb_getuslp(const char *kodkon,class iceb_u_str *uslprod,int metka,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
uslprod->new_plus("");

sprintf(strsql,"select datd,nomd,vidd,vnn from Kontragent2 where kodkon='%s' order by pu desc,datd desc limit 1",kodkon);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
//  sprintf(strsql,"%s:%s:%s",row[2],iceb_u_datzap(row[0]),row[1]);
  if(metka == 0)
   {
    sprintf(strsql,"%s:%s:%s",row[2],iceb_u_datzap(row[0]),row[1]);
    if(row[3][0] != '\0')
     {
      strcat(strsql,":");
      strcat(strsql,row[3]);
     }    
   }
  if(metka == 1)
    sprintf(strsql,"%s %s %s N%s",row[2],gettext("от"),iceb_u_datzap(row[0]),row[1]);
  uslprod->new_plus(strsql);
  return(0);
 }

return(1);
}
/****************************************/
int iceb_getuslp(const char *kodkon,class iceb_u_str *uslprod,GtkWidget *wpredok)
{
return(iceb_getuslp(kodkon,uslprod,0,wpredok));
}




/**************************************/
int iceb_getuslp(int un_nom_zap,class iceb_u_str *uslprod,int metka,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
uslprod->new_plus("");

sprintf(strsql,"select datd,nomd,vidd,vnn from Kontragent2 where nz=%d",un_nom_zap);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(metka == 0)
   {
    sprintf(strsql,"%s:%s:%s",row[2],iceb_u_datzap(row[0]),row[1]);
    if(row[3][0] != '\0')
     {
      strcat(strsql,":");
      strcat(strsql,row[3]);
     }
   }
  if(metka == 1)
   {
    /*после номера документа могут поставить код документа через двоеточие для реестра налоговых накладных*/
    class iceb_u_str nomdok("");
    if(iceb_u_polen(row[1],&nomdok,1,':') != 0)
     nomdok.new_plus(row[1]);
    sprintf(strsql,"%s %s %s N%s",row[2],gettext("от"),iceb_u_datzap(row[0]),nomdok.ravno());
   }
  uslprod->new_plus(strsql);
  return(0);
 }

return(1);
}
/*********************************/
int iceb_getuslp(int un_nom_zap,class iceb_u_str *uslprod,GtkWidget *wpredok)
{ 
return(iceb_getuslp(un_nom_zap,uslprod,0,wpredok));
}