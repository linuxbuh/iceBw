/*$Id:$*/
/*30.03.2016	30.03.2016	Белых А.И.	iceb_kor_kod_kontr.c
Корректировка кода контрагента
Вернули 0-если выполнили корректировку
        1-не выполнили
*/
#include "iceb_libbuh.h"

extern SQL_baza bd;

int iceb_kor_kod_kontr(const char *old_kod,const char *new_kod,GtkWidget *wpredok)
{
char strsql[512];

iceb_refresh();

sprintf(strsql,"update Skontr set kodkon='%s' where kodkon='%s'",new_kod,old_kod);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
   return(1);
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
 }

iceb_refresh();

sprintf(strsql,"update Prov set kodkon='%s' where kodkon='%s'",new_kod,old_kod);

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);


iceb_refresh();

sprintf(strsql,"update Saldo set kodkon='%s' where kodkon='%s'",new_kod,old_kod);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DUP_ENTRY) //Запись уже есть
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

iceb_refresh();

sprintf(strsql,"update Dokummat set kontr='%s' where kontr='%s'",new_kod,old_kod);

iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Dokummat set k00='%s' where kontr='%s'",new_kod,old_kod);

iceb_sql_zapis(strsql,1,0,wpredok);

iceb_refresh();

sprintf(strsql,"update Gnali set kontr='%s' where kontr='%s'",new_kod,old_kod);

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

iceb_refresh();

sprintf(strsql,"update Uosdok set kontr='%s' where kontr='%s'",new_kod,old_kod);

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

iceb_refresh();

sprintf(strsql,"update Usldokum set kontr='%s' where kontr='%s'",new_kod,old_kod);

iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Usldokum set k00='%s' where k00='%s'",new_kod,old_kod);

iceb_sql_zapis(strsql,1,0,wpredok);

iceb_refresh();

sprintf(strsql,"update Kasord1 set kontr='%s' where kontr='%s'",new_kod,old_kod);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

iceb_refresh();

sprintf(strsql,"update Kontragent1 set kodkon='%s' where kodkon='%s'",new_kod,old_kod);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Kontragent2 set kodkon='%s' where kodkon='%s'",new_kod,old_kod);
iceb_sql_zapis(strsql,1,0,wpredok);

iceb_refresh();

SQLCURSOR cur;
SQL_str row;
char bros[512];

sprintf(strsql,"select distinct polu from Pltp where polu like '%s#%%'",
old_kod);
if(cur.make_cursor(&bd,strsql) > 0)
 while(cur.read_cursor(&row) != 0)
  {      
  
   class iceb_u_str rek(old_kod);
   iceb_u_polen(row[0],bros,sizeof(bros),1,'#');
   rek.plus(iceb_u_adrsimv(iceb_u_strlen(bros),row[0]));

   sprintf(strsql,"update Pltp set polu='%s' where polu='%s'",rek.ravno_filtr(),row[0]);
   iceb_sql_zapis(strsql,1,0,wpredok);

  }

sprintf(strsql,"select distinct polu from Pltt where polu like '%s#%%'",
old_kod);
if(cur.make_cursor(&bd,strsql) > 0)
 while(cur.read_cursor(&row) != 0)
  {      
  
   class iceb_u_str rek(old_kod);
   iceb_u_polen(row[0],bros,sizeof(bros),1,'#');
   rek.plus(iceb_u_adrsimv(iceb_u_strlen(bros),row[0]));

   sprintf(strsql,"update Pltt set polu='%s' where polu='%s'",rek.ravno_filtr(),row[0]);
   iceb_sql_zapis(strsql,1,0,wpredok);


  }


sprintf(strsql,"select distinct polu from Tpltt where polu like '%s#%%'",
old_kod);
if(cur.make_cursor(&bd,strsql) > 0)
 while(cur.read_cursor(&row) != 0)
  {      
  
   class iceb_u_str rek(old_kod);
   iceb_u_polen(row[0],bros,sizeof(bros),1,'#');
   rek.plus(iceb_u_adrsimv(iceb_u_strlen(bros),row[0]));

   sprintf(strsql,"update Tpltt set polu='%s' where polu='%s'",rek.ravno_filtr(),row[0]);
   iceb_sql_zapis(strsql,1,0,wpredok);


  }
sprintf(strsql,"select distinct polu from Tpltp where polu like '%s#%%'",
old_kod);
if(cur.make_cursor(&bd,strsql) > 0)
 while(cur.read_cursor(&row) != 0)
  {      
  
   class iceb_u_str rek(old_kod);
   iceb_u_polen(row[0],bros,sizeof(bros),1,'#');
   rek.plus(iceb_u_adrsimv(iceb_u_strlen(bros),row[0]));

   sprintf(strsql,"update Tpltp set polu='%s' where polu='%s'",rek.ravno_filtr(),row[0]);
   iceb_sql_zapis(strsql,1,0,wpredok);

  }

iceb_refresh();

sprintf(strsql,"update Ukrdok1 set kontr='%s' where kontr='%s'",new_kod,old_kod);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

iceb_refresh();

sprintf(strsql,"update Ukrdok1 set kdrnn='%s' where kdrnn='%s'",new_kod,old_kod);

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

return(0);

}