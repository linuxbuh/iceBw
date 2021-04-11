/*$Id: rasamortnuw.c,v 1.3 2013/09/26 09:46:53 sasa Exp $*/
/*12.07.2015	02.02.2012	Белых А.И.	rasamortnuw.c
Расчёт амортизации для налогового учёта
с 01.04.2011 изменились правила расчёта амортизации
*/
#include "buhg_g.h"
int rasamortnuw_r(short mr,short gr,GtkWidget *wpredok);

extern SQL_baza bd;

extern short metkabo; //Если 1 то организация бюджетная

void rasamortnuw(GtkWidget *wpredok)
{
short m=0,g=0;
class iceb_u_str dr("");

naz:;

if(metkabo == 0) //не бюджетная организация
 {
naz1:;
  if(iceb_menu_vvod1(gettext("Расчёт амортизации для налогового учёта\nВведите дату расчёта (м.г)"),&dr,8,"",wpredok) != 0)
   return;

  if(iceb_u_rsdat1(&m,&g,dr.ravno()) != 0)
   {
    iceb_menu_soob(gettext("Не верно введена дата !"),wpredok);
    goto naz1;
   }
 } 

if(metkabo == 1) //бюджетная организация
 {
naz2:;
  if(iceb_menu_vvod1(gettext("Введите год расчёта"),&dr,8,"",wpredok) != 0)
   return;

  g=dr.ravno_atoi();
  if(g < 2000)
   {
    iceb_menu_soob(gettext("Не верно введена дата !"),wpredok);
    goto naz2;
   }
 } 

if(iceb_pbpds(m,g,wpredok) != 0)
  goto naz;

rasamortnuw_r(m,g,wpredok);



}
