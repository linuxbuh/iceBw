/*$Id:$*/
/*26.05.2016	26.05.2016	Белых А.И.	iceb_rsdatp1.c
Расшифровка двух дат
*/

#include "iceb_libbuh.h"

int iceb_rsdatp1(short *mn,short *gn,const char *datan,short *mk,short *gk,const char *datak,GtkWidget *wpredok)
{

if(iceb_u_rsdat1(mn,gn,datan) != 0)
 {
  iceb_menu_soob(gettext("Не верно ведена дата начала !"),wpredok);
  return(1);
 }

if(datak[0] == '\0')
 {
  *mk=*mn;
  *gk=*gn;
 }
else
 if(iceb_u_rsdat1(mk,gk,datak) != 0)
  {
   iceb_menu_soob(gettext("Не верно ведена дата конца !"),wpredok);
   return(1);
  }

if(iceb_u_sravmydat(1,*mn,*gn,1,*mk,*gk) > 0)
 {
  iceb_menu_soob(gettext("Дата начала не может быть больше даты конца !"),wpredok);
  return(1);
 }

return(0);
}