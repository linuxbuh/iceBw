/*$Id:$*/
/*22.10.2019	22.10.2019	Белых А.И.	zvb_str_lstw.c
*/
#include "buhg_g.h"



void zvb_str_lstw(int nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *tabnom,
FILE *ff)
{

fprintf(ff,"\
%6d %-*s %-*.*s %-*s %-*s %11.2f\n",
nomerpp,
iceb_u_kolbait(6,tabnom),tabnom,
iceb_u_kolbait(40,fio),
iceb_u_kolbait(40,fio),
fio,
iceb_u_kolbait(29,nomersh),nomersh,
iceb_u_kolbait(10,inn),inn,
suma);



}
