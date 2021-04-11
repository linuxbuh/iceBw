/*$Id: uosvibuw.c,v 1.2 2013/08/13 06:09:56 sasa Exp $*/
/*06.01.2012	06.01.2012	Белых А.И.	uosvibuw.c
Выбор вида учёта
*/
#include "buhg_g.h"

int uosvibuw(int *viduh,GtkWidget *wpredok)
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите из какого учёта брать данные"));
punkt_m.plus(gettext("Налоговый учёт"));//0
punkt_m.plus(gettext("Бухгалтерский учёт"));//1


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);
*viduh=nomer;
switch(nomer)
 {
  case 0:
   return(0);
  case 1:
   return(0);
 };
return(1);
}
