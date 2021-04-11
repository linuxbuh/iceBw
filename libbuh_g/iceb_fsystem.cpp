/*$Id: iceb_fsystem.c,v 1.8 2013/05/17 14:58:34 sasa Exp $*/
/*21.05.2016	17.03.2009	Белых А.И.	iceb_fsystem.c
Чтение из файла команд и выполнение 
*/
#include "iceb_libbuh.h"

extern SQL_baza bd;

void iceb_fsystem(const char *imaf_nast,const char *zamena,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_nast);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  return;
 }
class iceb_u_str stroka("");

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  stroka.new_plus(row_alx[0]);
  if(zamena[0] != '\0')
    iceb_u_zvstr(&stroka,"ICEB_ZAMENA",zamena);

//  voz=iceb_system(stroka.ravno(),0); нужно сначала организовать откно
       
  if(system(stroka.ravno()) != 0)
   {
      class iceb_u_str soob(__FUNCTION__);
      soob.plus("-");
      soob.plus(gettext("Не выполнена команда"));
      soob.ps_plus(stroka.ravno());
      iceb_menu_soob(soob.ravno(),wpredok);
   }
 }
}
