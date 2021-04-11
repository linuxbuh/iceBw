/*$Id: pib_strw.c,v 1.4 2013/05/17 14:58:23 sasa Exp $*/
/*12.07.2015	28.02.2005	Белых А.И.	pib_strw.c
Вывод строки в документ
*/
#include "buhg_g.h"

void pib_strw(int nomer,const char *bankshet,double sum,const char *fio,
const char *indkod,
FILE *ffkartr)
{

fprintf(ffkartr,"%3d %-*s %8.2f %10s %s\n",
nomer,
iceb_u_kolbait(16,bankshet),bankshet,
sum,indkod,fio);
}
