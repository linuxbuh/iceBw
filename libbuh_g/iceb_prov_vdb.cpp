/*$Id: iceb_prov_vdb.c,v 1.1 2014/08/31 06:19:25 sasa Exp $*/
/*01.08.2014	01.08.2014	Белых А.И.	iceb_prov_vdb.c
Проверка возможности оператору выгружать дам базы
*/

#include "iceb_libbuh.h"

extern SQL_baza bd;


int iceb_prov_vdb(const char *namebases,GtkWidget *wpredok)
{


char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur,cur1;

if(namebases[0] != '\0')
 {
  sprintf(strsql,"select vdb from %s.icebuser where login='%s'",namebases,iceb_u_getlogin());

  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    if(atoi(row[0]) != 0)
     {
      sprintf(strsql,"%s %s",gettext("У оператора нет разрешения на выгрузку дамба для базы"),namebases);
      iceb_menu_soob(strsql,wpredok);
      return(1);
     }
   }
  else
   return(1);
 }
else
 {
  sprintf(strsql,"SHOW DATABASES");
  int kolstr=0;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return(1);
   }

  if(kolstr == 0)
   {
    iceb_menu_soob(gettext("Не найдено ни одной доступной базы!"),wpredok);
    return(1);
   }

  /*так как база ещё не выбрана то она показывает все базы и те к которым у оператора нет доступа*/
  while(cur.read_cursor(&row) != 0)
   {
    sprintf(strsql,"select vdb from %s.icebuser where login='%s'",row[0],iceb_u_getlogin());
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      if(atoi(row1[0]) != 0)
       {
        sprintf(strsql,"%s %s",gettext("У оператора нет разрешения на выгрузку дамба для базы"),row[0]);
        iceb_menu_soob(strsql,wpredok);
        return(1);
       }
     }
    else
     {
      int kod=sql_nerror(&bd);
      if(kod  == ER_UNKNOWN_TABLE) /*нет в базе такой таблицы*/
       continue;
      if(kod  == ER_NO_SUCH_TABLE) /*в таблице нет такой колонки*/
       continue;
      printf("\n%s-%d\n",__FUNCTION__,sql_nerror(&bd));
      return(1);
     }
   }
 }
return(0);
}
