/*$Id: zvb_hreshatik_strw.c,v 1.5 2013/08/13 06:10:04 sasa Exp $*/
/*20.11.2009	23.05.2007	Белых А.И.	zvb_hreshatik_strw.c
Строка записи для банка Крещатик
*/
#include "buhg_g.h"

void zvb_hreshatik_strw(short d_pp,short m_pp,short g_pp,
int *nomer_zap,
const char *fio,
const char *shet_b,
const char *inn,
double suma,
FILE *ff,FILE *ffr)
{
*nomer_zap+=1;

fprintf(ffr,"%5d %-*.*s %10s %-13s %10.2f\n",
*nomer_zap,iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,inn,shet_b,suma);

fprintf(ff,"11\
%-19s\
%04d%02d%02d\
%07d\
13\
%012.2f\
UAH\
03\
048\n",
shet_b,
g_pp,m_pp,d_pp,
*nomer_zap,
suma);
}
