/* $Id: provvkp.c,v 1.6 2013/05/17 14:58:24 sasa Exp $ */
/*12.07.2015	07.04.1998	Белых А.И.	prvkp.c
Проверка возможности корректировки или удаления проводки
Если вернули 0- работать можно
*/
#include        "buhg_g.h"


int provvkp(const char *kto,GtkWidget *wpredok)
{
 
if(iceb_u_SRAV(kto,ICEB_MP_MATU,0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Материальный учет\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(1);
 } 
if(iceb_u_SRAV(kto,ICEB_MP_UOS,0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Учет основных средств\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(2);
 } 
if(iceb_u_SRAV(kto,ICEB_MP_ZARP,0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Заработная плата\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(3);
 } 

if(iceb_u_SRAV(kto,ICEB_MP_PPOR,0) == 0 || \
iceb_u_SRAV(kto,ICEB_MP_PTRE,0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Платежные документы\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(4);
 } 

if(iceb_u_SRAV(kto,ICEB_MP_KASA,0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Учет кассовых ордеров\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(5);
 } 

if(iceb_u_SRAV(kto,ICEB_MP_USLUGI,0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Учет услуг\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(6);
 } 

if(iceb_u_SRAV(kto,ICEB_MP_UKR,0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Учет командировочных расходов\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(6);
 } 

return(0);

}
