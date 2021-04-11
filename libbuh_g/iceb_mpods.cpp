/*$Id: iceb_mpods.c,v 1.7 2013/08/13 06:10:12 sasa Exp $*/
/*05.03.20013	22.04.2008	Белых А.И.	iceb_mpods.c
Получение по номеру подсистемы её символьной метки или
получение по метке подсистемы её номера
*/
#include "iceb_libbuh.h"

void iceb_mpods(int metka_ps,char *kto)
{

if(metka_ps == 1) /*Материальный учёт*/
  strcpy(kto,ICEB_MP_MATU);

if(metka_ps == 2) /*зароботная плата*/
  strcpy(kto,ICEB_MP_ZARP);

if(metka_ps == 3) /*учёт основных средств*/
  strcpy(kto,ICEB_MP_UOS);

if(metka_ps == 4) /*платёжные поручения*/
  strcpy(kto,ICEB_MP_PPOR);

if(metka_ps == 5) /*платёжные требования*/
  strcpy(kto,ICEB_MP_PTRE);


if(metka_ps == 6) /*учёт услуг*/
  strcpy(kto,ICEB_MP_USLUGI);

if(metka_ps == 7) /*учёт кассовых ордеров*/
  strcpy(kto,ICEB_MP_KASA);

if(metka_ps == 8) /*учёт командировочных расходов*/
  strcpy(kto,ICEB_MP_UKR);

}
/******************************************/
/*получение по метке подсистемы её номера*/
/****************************************/
int iceb_mpods(char *kto)
{
if(kto[0] == '\0')
 return(0);
 
if(iceb_u_SRAV(ICEB_MP_MATU,kto,0) == 0)
  return(1);

if(iceb_u_SRAV(ICEB_MP_ZARP,kto,0) == 0)
  return(2);

if(iceb_u_SRAV(ICEB_MP_UOS,kto,0) == 0)
  return(3);


if(iceb_u_SRAV(ICEB_MP_PPOR,kto,0) == 0)
  return(4);

if(iceb_u_SRAV(ICEB_MP_PTRE,kto,0) == 0)
  return(5);

if(iceb_u_SRAV(ICEB_MP_USLUGI,kto,0) == 0)
  return(6);

if(iceb_u_SRAV(ICEB_MP_KASA,kto,0) == 0)
  return(7);

if(iceb_u_SRAV(ICEB_MP_UKR,kto,0) == 0)
  return(8);

return(0);   
}
