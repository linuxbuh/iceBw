/*$Id: zvb_kievrus_strw.c,v 1.3 2013/08/13 06:10:04 sasa Exp $*/
/*22.10.2019	11.10.2011	Белых А.И.	zvb_kievrus_strw.c
Вывод строк в файлы
*/
#include "buhg_g.h"

void zvb_kievrus_strw(int nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *tabnom,
FILE *ff_dbf_tmp,FILE *ffras)
{

fprintf(ff_dbf_tmp," %-29.29s%-12.2f%-*.*s%-*.*s",
nomersh,
suma,
iceb_u_kolbait(10,inn),
iceb_u_kolbait(10,inn),
inn,
iceb_u_kolbait(50,fio),
iceb_u_kolbait(50,fio),
fio);
/**********************
fprintf(ffras,"\
%6d %-*s %-*s %-*s %-*s %10.2f\n",
nomerpp,
iceb_u_kolbait(6,tabnom),tabnom,
iceb_u_kolbait(40,fio),fio,
iceb_u_kolbait(10,inn),inn,
iceb_u_kolbait(14,nomersh),nomersh,
suma);
*************************/
fprintf(ffras,"\
%6d %-*s %-*s %*s %-*s %10.2f\n",
nomerpp,
iceb_u_kolbait(6,tabnom),tabnom,
iceb_u_kolbait(40,fio),fio,
iceb_u_kolbait(29,nomersh),nomersh,
iceb_u_kolbait(10,inn),inn,
suma);

}
