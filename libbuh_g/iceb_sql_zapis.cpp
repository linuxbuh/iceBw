/*$Id: iceb_sql_zapis.c,v 1.19 2014/04/30 06:15:00 sasa Exp $*/
/*20.03.2010	29.10.2003	Белых А.И.	iceb_sql_zapis.c
Запись в базу данных
Если вернули 0 - запиь сделана - иначе код ошибки
*/
#include	"iceb_libbuh.h"

extern SQL_baza	bd;

int iceb_sql_zapis(const char *strsql,
int metka, //0-выдать Ждите 1-нет
int metkasoob, //0-выдать сообщение о дублированной записи 1-нет
GtkWidget *wpredok) 
{

if(metka == 0)
 if(wpredok != NULL)
   gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
//printf("iceb_sql_zapis\n");
if(sql_zap(&bd,strsql) != 0)
 {
  
  if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
   {
    if(metkasoob == 0)
     {
      iceb_u_str repl;
      repl.plus(strsql);
      repl.ps_plus(gettext("Такая запись уже есть !"));
      iceb_menu_soob(&repl,wpredok);
     }
    return(ER_DUP_ENTRY);
   }

  if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
   {
    class iceb_u_str repl;
    class iceb_u_str npole;
    
    repl.plus_ps(gettext("У вас нет полномочий для выполнения этой операции !"));
    for(int nom=1; nom <= 3; nom++)
     {
      if(iceb_u_polen(strsql,&npole,nom,' ') == 0)
       repl.plus(" ",npole.ravno());
      
     }
    iceb_menu_soob(&repl,wpredok);
    return(ER_DBACCESS_DENIED_ERROR);
   }

  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(sql_nerror(&bd));
 }
return(0);
}
