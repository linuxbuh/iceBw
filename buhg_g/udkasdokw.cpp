/* $Id: udkasdokw.c,v 1.10 2013/08/13 06:09:53 sasa Exp $ */
/*11.11.2012   29.09.2000      Белых А.И.      udkasdokw.c
Удаление документа в подсистеме "Учет кассовых ордеров"
Если номера документа нет значит удалить за год
Если вернули 0 удалили
             1 нет

*/
#include        "buhg_g.h"

extern SQL_baza bd;

int udkasdokw(const char *kassa,const char *nomd,short dd,short md,short gd,
short tipz,GtkWidget *wpredok)
{
char		strsql[512];

if(nomd[0] != '\0')
 sprintf(strsql,"DELETE FROM Kasord1 where kassa=%s and tp=%d and \
 nomd='%s' and god=%d",kassa,tipz,nomd,gd);
else
 sprintf(strsql,"DELETE FROM Kasord1 where god=%d",gd);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
   return(1);
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,wpredok);
 }
 
if(nomd[0] != '\0')
 sprintf(strsql,"DELETE FROM Kasord where kassa=%s and tp=%d and \
 nomd='%s' and god=%d",kassa,tipz,nomd,gd);
else
 sprintf(strsql,"DELETE FROM Kasord where god=%d",gd);

iceb_sql_zapis(strsql,1,0,wpredok);

return(0);
}
