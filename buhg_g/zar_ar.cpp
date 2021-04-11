/*$Id: zar_ar.c,v 1.16 2013/08/13 06:10:00 sasa Exp $*/
/*13.07.2015	19.09.2006	Белых А.И.	zar_ar.c
Автоматизированный расчёт по всем карточкам
*/
#include "buhg_g.h"
#include "zar_ar.h"

int zar_ar_v(class zar_ar_rek *rk,GtkWidget *wpredok);
int zar_ar_r(class zar_ar_rek *datark,GtkWidget *wpredok);


void zar_ar()
{
static class zar_ar_rek data;


iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Австоматизированный расчёт запрлат по всем карточкам"));


zagolovok.plus(gettext("Австоматизированный расчёт запрлат по всем карточкам"));


punkt_m.plus(gettext("Расчёт только удержаний по всем карточкам"));//0
punkt_m.plus(gettext("Расчёт только начислений по всем карточкам"));//1
punkt_m.plus(gettext("Расчёт начислений и удержаний по всем карточкам"));//2
punkt_m.plus(gettext("Расчёт только начислений на фонд оплаты труда"));//3
punkt_m.plus(gettext("Расчёт только проводок"));//4

int nomer=0;

if((nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,NULL)) == -1)
 return;

if(nomer == 0)
 data.metka_r=2;
if(nomer == 1)
 data.metka_r=1;
if(nomer == 2)
 data.metka_r=3;
if(nomer == 3)
 data.metka_r=4;
if(nomer == 4)
 data.metka_r=5;

if(zar_ar_v(&data,NULL) != 0)
 return;


if(data.metka_ff == 1) //Нужен протокол хода расчёта
 {
 }

zar_ar_r(&data,NULL);

if(data.metka_ff == 1) //Нужен протокол хода расчёта
 {
  short mr,gr;
  iceb_u_rsdat1(&mr,&gr,data.datar.ravno());
  
  zar_pr_showw(0,0,mr,gr,NULL);
 }


}


