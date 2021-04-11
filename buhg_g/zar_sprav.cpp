/*$Id: zar_sprav.c,v 1.6 2013/09/26 09:47:00 sasa Exp $*/
/*26.05.2016	08.11.2006	Белых А.И.	zar_sprav.c
Распечатка справок о зарплате
*/
#include "buhg_g.h"
#include "zar_sprav.h"

int zar_sprav_m(class zar_sprav_rek *rek,GtkWidget *wpredok);
int zar_sprav_r(class zar_sprav_rek *rek,GtkWidget *wpredok);

void zar_sprav(int metka,
//Если metkah= 1 - расчёт справки о зарплате
//               2 - расчёт средней зарплаты для больничного
//               3 - расчёт справки для уволенного для соцстраха
//               4 - расчёт справок по датам в счёт которых были сделаны начисления
//               5 - удалено !!!! расчёт отпускных преподпвателя
GtkWidget *wpredok)
{
static class zar_sprav_rek data;

data.metka_vr=metka;
  

if(zar_sprav_m(&data,wpredok) != 0)
 return;

if(zar_sprav_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.oth,wpredok);

}
