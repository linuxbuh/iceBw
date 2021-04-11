/* $Id: nomkmw.c,v 1.10 2014/02/28 05:21:00 sasa Exp $ */
/*12.07.2015	09.03.2000	Белих А.И.	nomkmw.c
Автоматическое определение кода материалла и услуги
*/
#include        "buhg_g.h"

extern SQL_baza	bd;

int nomkmw(GtkWidget *wpredok)
{
return(nomkmw(0,wpredok));
}
/**********************************/
int nomkmw(int nom_start,GtkWidget *wpredok)
{
int             i;
char		strsql[512];
SQL_str         row;
long		kolstr;

class iceb_clock sss(wpredok);
i=nom_start;


sprintf(strsql,"select kodm from Material where kodm > %d order by kodm asc",nom_start);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

while(cur.read_cursor(&row) != 0)
 {
  i++;
naz:;
  if(i != atoi(row[0]))
   {
    sprintf(strsql,"select kodus from Uslugi where kodus=%d",i);
    if(iceb_sql_readkey(strsql,wpredok) <= 0)
     {
      return(i);
     }
    i++;
    goto naz;
   }
 }


sprintf(strsql,"select kodus from Uslugi where kodus > %d order by kodus asc",nom_start);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(++i);
 }

while(cur.read_cursor(&row) != 0)
 {
  i++;

naz1:;
  if(i != atoi(row[0]))
   {
    sprintf(strsql,"select kodm from Material where kodm=%d",i);
    if(sql_readkey(&bd,strsql) == 0)
     {
      sprintf(strsql,"select kodus from Uslugi where kodus=%d",i);
      if(iceb_sql_readkey(strsql,wpredok) <= 0)
       {
        return(i);
       }
     }
    i++;
    goto naz1;
   }
 }

i++;

return(i);

}
