/* $Id: provblokzarpw.c,v 1.8 2013/05/17 14:58:24 sasa Exp $ */
/*10.08.2015	26.05.2001	Белых А.И.	provblokzarpw.c
Проверка блокировки корректировки данных в зарплате
*/
#include        "buhg_g.h"


int provblokzarpw(int tabn,short mp,short gp,short flagrk,int metka_soob,GtkWidget *wpredok)
{
char		strsql[1024];
SQL_str		row;
int		blok;

if(flagrk != 0)
 {
  if(metka_soob == 0)
    iceb_menu_soob(gettext("С карточкой работает другой оператор !\nВаши изменения записаны не будут !"),wpredok);
  return(1);
 }

if(iceb_pblok(mp,gp,ICEB_PS_ZP,wpredok) != 0)
 {
  if(metka_soob == 0)
   {
    sprintf(strsql,gettext("Дата %d.%dг. заблокирована !"),mp,gp);
    iceb_menu_soob(strsql,wpredok);  
   }
  return(1);
 }
SQLCURSOR curr;
sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
tabn=%d",gp,mp,tabn);

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
 {
  blok=atoi(row[0]);
  if(blok == 0)
   return(0);
  if(metka_soob == 0)
   {

    sprintf(strsql,"%s %s",gettext("Карточка заблокирована !"),iceb_kszap(atoi(row[0]),wpredok));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }
return(0);
}
