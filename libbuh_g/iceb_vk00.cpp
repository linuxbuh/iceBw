/*$Id: iceb_vk00.c,v 1.2 2013/08/13 06:10:14 sasa Exp $*/
/*31.05.2012	31.05.2012	Белых А.И.	iceb_vk00.c
Ввод кода контрагента реквизиты которого будут попадать в распечатки документов
*/
#include "iceb_libbuh.h"

void iceb_vk00(class iceb_u_str *k00,GtkWidget *wpredok)
{

iceb_menu_vvod1(gettext("Введите код контрагента с вашими реквизитами"),k00,16,"",wpredok);

char strsql[512];
sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",k00->ravno());
if(iceb_sql_readkey(strsql,wpredok) != 1)
 {
  sprintf(strsql,"%s %s %s!",gettext("Нет контрагента"),k00->ravno(),gettext("в справочнике контрагентов"));
  iceb_menu_soob(strsql,wpredok);
  k00->new_plus("00");
 }


}
