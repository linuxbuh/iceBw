/*$Id: uosdiw.c,v 1.2 2013/08/13 06:09:54 sasa Exp $*/
/*15.05.2017	04.01.2012	Белых А.И.	uosdiw.c
Загрузка/выгрузка дополнительной информации
*/
#include <errno.h>
#include "buhg_g.h"

extern SQL_baza bd;

int uosdiw(char mr, //+ загрузить - выгрузить
int md,
int innom,
const char *imaf,
GtkWidget *wpredok)
{
char strok[2048];
FILE *ff;
char strsql[2048];
if(mr == '+')
 {
  int nomz=0;

  if((ff = fopen(imaf,"r")) == NULL)
   {
     if(errno == ENOENT)
      return(0);
     else
      {
       iceb_er_op_fil(imaf,"__FUNCTION__",errno,wpredok);
       return(1);
      }
   }

  class iceb_lock_tables kkk("LOCK TABLES Uosin1 WRITE");

  sprintf(strsql,"delete from Uosin1 where innom=%d and md=%d",innom,md);
  iceb_sql_zapis(strsql,1,0,wpredok);

  while(fgets(strok,sizeof(strok),ff) != NULL)
   {
    sprintf(strsql,"insert into Uosin1 \
values (%d,%d,'%s',%d)",innom,++nomz,sql_escape_string(strok,0),md);
    iceb_sql_zapis(strsql,1,0,wpredok);
    
   }  
  fclose(ff);
  unlink(imaf);
  return(0);
 }

if(mr == '-')
 {
  if((ff = fopen(imaf,"w")) == NULL)
    {
     iceb_er_op_fil(imaf,"__FUNCTION__",errno,wpredok);
     return(1);
    }
  int kolstr=0;  
  sprintf(strsql,"select zapis from Uosin1 where innom=%d and md=%d order by nomz asc",innom,md);
  SQL_str row;
  SQLCURSOR cur;  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return(1);
   }

  if(kolstr == 0)
   {
    if(md == 1)
     iceb_insfil("uosoz6end.alx",ff,wpredok);
    if(md == 2)
     iceb_insfil("uosik_end.alx",ff,wpredok);
   }
  else
   while(cur.read_cursor(&row) != 0)
    fprintf(ff,"%s",row[0]);


  fclose(ff);
  return(0);
 }


return(1);

}