/*$Id: iceb_kolstrok.c,v 1.6 2013/08/13 06:10:10 sasa Exp $*/
/*27.11.2004	27.11.2004	Белых А.И.	iceb_kolstrok.c
Расчёт количества строк на листе
*/
#include <math.h>
#include "iceb_libbuh.h"

extern int kol_strok_na_liste;

int iceb_kolstrok(int kolstr)
{

double kk=kolstr*kol_strok_na_liste/59;
double rez;
modf(kk,&rez);
return((int)rez);
}
