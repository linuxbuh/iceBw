/*$Id: iceb_uionp.c,v 1.7 2013/08/13 06:10:14 sasa Exp $*/
/*23.05.2016	22.05.2008	Белых А.И.	iceb_uionp.c
Удаление из очереди на печать
*/
#include "iceb_libbuh.h"


void iceb_uionp(GtkWidget *wpredok)
{
class iceb_u_str nomer_r("");
if(iceb_menu_vvod1(gettext("Введите номер очереди на печать"),&nomer_r,20,"",wpredok) != 0)
 return;

if(nomer_r.ravno()[0] == '\0')
 return;
 
class iceb_u_str kom_ud;
if(iceb_poldan("Снять с очереди на печать",&kom_ud,"nastsys.alx",wpredok) != 0)
 {
  kom_ud.new_plus("lprm");
 }

class iceb_u_str komanda(kom_ud.ravno());
komanda.plus(" ",nomer_r.ravno());

if(system(komanda.ravno()) != 0)
 {
  char bros[1024];
  sprintf(bros,"%s-%s %s",__FUNCTION__,gettext("Не выполнена команда"),komanda.ravno());
  iceb_menu_soob(bros,wpredok);
 }

}
