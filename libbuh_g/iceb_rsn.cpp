/*$Id: iceb_rsn.c,v 1.10 2013/05/17 14:58:37 sasa Exp $*/
/*02.12.2014	09.04.2004	Белых А.И.	iceb_rsn.c
Чтение системных настроек
*/
#include "iceb_libbuh.h"

extern SQL_baza bd;

int  iceb_rsn(class SYSTEM_NAST *data,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];


class iceb_u_str bros("");
class iceb_u_str nazvanie("");
//Читаем необходимые настройки
sprintf(strsql,"select str from Alx where fil='nastsys.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"Не найдены настройки nastsys.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }



while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],&nazvanie,1,'|') != 0)
    continue;
    
  if(iceb_u_SRAV(nazvanie.ravno(),"Системный принтер",0) == 0)
   {
    iceb_u_polen(row_alx[0],&bros,2,'|');
    data->prog_peh_sys.new_plus(bros.ravno());
    continue;
   }

  if(iceb_u_SRAV(nazvanie.ravno(),"Снять с очереди на печать",0) == 0)
   {
    iceb_u_polen(row_alx[0],&bros,2,'|');
    data->clear_peh.new_plus(bros.ravno());
    continue;
   }

  if(iceb_u_SRAV(nazvanie.ravno(),"Редактор для графического интерфейса",0) == 0)
   {
    iceb_u_polen(row_alx[0],&bros,2,'|');
    data->redaktor.new_plus(bros.ravno());
    continue;
   }

  if(iceb_u_SRAV(nazvanie.ravno(),"Просмотрщик файлов для графического интерфейса",0) == 0)
   {
    iceb_u_polen(row_alx[0],&bros,2,'|');
    data->prosmotr.new_plus(bros.ravno());
    continue;
   }

  if(iceb_u_SRAV(nazvanie.ravno(),"Просмотр очереди на печать",0) == 0)
   {
    iceb_u_polen(row_alx[0],&bros,2,'|');
    data->prosop.new_plus(bros.ravno());
    continue;
   }

 }

return(0);

}
