/*$Id: zvb_alfabank_strw.c,v 1.7 2014/08/31 06:19:22 sasa Exp $*/
/*29.06.2013	15.10.2009	Белых А.И.	zvb_alfabank_strw.c
Вывод строк в файлы
*/
#include "buhg_g.h"

void zvb_alfabank_strw(int nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *kedrpou,
FILE *ff_dbf_tmp,FILE *ffras)
{
class iceb_u_str famil("");
class iceb_u_str ima("");
class iceb_u_str oth("");

iceb_u_polen(fio,&famil,1,' ');
iceb_u_polen(fio,&ima,2,' ');
iceb_u_polen(fio,&oth,3,' ');



fprintf(ff_dbf_tmp," %6d%-*.*s%-*.*s%-*.*s%-14.14s%10.2f%-14.14s%-14.14s",
nomerpp,
iceb_u_kolbait(30,famil.ravno()),iceb_u_kolbait(30,famil.ravno()),famil.ravno(),
iceb_u_kolbait(30,ima.ravno()),iceb_u_kolbait(30,ima.ravno()),ima.ravno(),
iceb_u_kolbait(30,oth.ravno()),iceb_u_kolbait(30,oth.ravno()),oth.ravno(),
nomersh,suma,inn,kedrpou);

fprintf(ffras,"\
%6d %-*s %*s %10.2f %*s %*s\n",
nomerpp,
iceb_u_kolbait(40,fio),fio,
iceb_u_kolbait(14,nomersh),nomersh,
suma,
iceb_u_kolbait(10,inn),inn,
iceb_u_kolbait(10,kedrpou),kedrpou);



}
