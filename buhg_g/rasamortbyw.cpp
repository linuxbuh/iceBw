/* $Id: rasamortbyw.c,v 1.3 2013/09/26 09:46:53 sasa Exp $ */
/*12.07.2015    06.12.2000      Белых А.И.      rasamortbyw.c
Расчёт амортизаци по организации для бухгалтерского учёта
*/
#include        "buhg_g.h"
int rasamortbyw_r(short mr,short gr,GtkWidget *wpredok);

extern SQL_baza bd;

void rasamortbyw(GtkWidget *wpredok)
{
class iceb_u_str dr("");
short           m,g;

m=g=0;
naz:;

if(iceb_menu_vvod1(gettext("Расчёт амортизации для бухгалтерского учёта\nВведите дату расчёта (м.г)"),&dr,8,"",wpredok) != 0)
 return;

if(iceb_u_rsdat1(&m,&g,dr.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Не верно введена дата !"),wpredok);
  goto naz;
 }

if(iceb_pbpds(m,g,wpredok) != 0)
  goto naz;

rasamortbyw_r(m,g,wpredok);
}
