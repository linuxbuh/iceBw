/*$Id: iceb_pbsh.c,v 1.11 2013/08/13 06:10:12 sasa Exp $*/
/*09.03.2016	27.03.2008	Белых А.И.	iceb_pbsh.c
Проверка блокировки счёта
Если вернули 0- счёт не заблокирован
             1- заблокирован
*/
#include "iceb_libbuh.h"

int iceb_pbsh(const char *data,const char *shet,const char *shetkor,const char *repl,GtkWidget *wpredok)
{
short den=0,mes=0,god=0;
iceb_u_rsdat(&den,&mes,&god,data,1);
return(iceb_pbsh(mes,god,shet,shetkor,repl,wpredok));

}

/******************************/

int iceb_pbsh(short mes,short god,const char *shet,const char *shetkor,const char *repl,GtkWidget *wpredok)
{
SQL_str row;
char strsql[512];
class SQLCURSOR cur;

sprintf(strsql,"select bs from Plansh where ns='%s'",shet);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(atoi(row[0]) == 1)
   {
    sprintf(strsql,"%s %s %s!",gettext("Счёт"),shet,gettext("заблокирован в плане счетов"));
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
 }
sprintf(strsql,"select bs from Plansh where ns='%s'",shetkor);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(atoi(row[0]) == 1)
   {
    sprintf(strsql,"%s %s %s!",gettext("Счёт"),shetkor,gettext("заблокирован в плане счетов"));
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
 }

sprintf(strsql,"select log,shet from Blok where kod=%d and god=%d and mes=%d",ICEB_PS_GK,god,mes);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  sprintf(strsql,"select log,shet from Blok where kod=%d and god=%d and mes=0",
  ICEB_PS_GK,god);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
   return(0);
 }

if(iceb_u_proverka(row[0],iceb_u_getlogin(),0,1) == 0)
 return(0);

if(shet[0] != '\0')
 if(iceb_u_proverka(row[1],shet,1,1) == 0)
  {
   class iceb_u_str rrr;
   
   sprintf(strsql,gettext("Счёт %s на %d.%d г. заблокирован!"),shet,mes,god);
   rrr.plus(strsql);
   
   if(repl[0] != '\0')
      rrr.ps_plus(repl);
   iceb_menu_soob(&rrr,wpredok);
   return(1);
  }

if(shetkor[0] != '\0')
 if(iceb_u_proverka(row[1],shetkor,1,1) == 0)
  {
   class iceb_u_str rrr;
   sprintf(strsql,gettext("Счёт %s на %d.%d г. заблокирован!"),shetkor,mes,god);
   rrr.plus(strsql);

   if(repl[0] != '\0')
      rrr.ps_plus(repl);

   iceb_menu_soob(&rrr,wpredok);
   return(1);
  }


return(0);
}
