/*$Id: zvb_privatbank_str.c,v 5.1 2014/08/31 06:18:19 sasa Exp $*/
/*22.10.2019	17.10.2014	Белых А.И.	zvb_privatbank_strw.cВывод строк в файлы
*/
#include "buhg_g.h"

void zvb_privatbank_strw(int nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *tabnom,
const char *branch,
const char *zpkod,
const char *rlkod,
FILE *ff_dbf_tmp,FILE *ffras)
{
class iceb_u_str fam("");
class iceb_u_str ima("");
class iceb_u_str otch("");
iceb_u_polen(fio,&fam,1,' ');
iceb_u_polen(fio,&ima,2,' ');
iceb_u_polen(fio,&otch,3,' ');


fprintf(ff_dbf_tmp," %-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%15.2f%-*.*s%-*.*s",
iceb_u_kolbait(3,branch),
iceb_u_kolbait(3,branch),
branch,
iceb_u_kolbait(3,zpkod),
iceb_u_kolbait(3,zpkod),
zpkod,
iceb_u_kolbait(16,nomersh),
iceb_u_kolbait(16,nomersh),
nomersh,
iceb_u_kolbait(10,tabnom),
iceb_u_kolbait(10,tabnom),
tabnom,
iceb_u_kolbait(50,fam.ravno()),
iceb_u_kolbait(50,fam.ravno()),
fam.ravno(),
iceb_u_kolbait(50,ima.ravno()),
iceb_u_kolbait(50,ima.ravno()),
ima.ravno(),
iceb_u_kolbait(50,otch.ravno()),
iceb_u_kolbait(50,otch.ravno()),
otch.ravno(),
suma,
iceb_u_kolbait(20,inn),
iceb_u_kolbait(20,inn),
inn,
iceb_u_kolbait(30,rlkod),
iceb_u_kolbait(30,rlkod),
rlkod);


fprintf(ffras,"\
%6d %-*s %-*s %*s %-*s %10.2f\n",
nomerpp,
iceb_u_kolbait(6,tabnom),tabnom,
iceb_u_kolbait(40,fio),fio,
iceb_u_kolbait(29,nomersh),nomersh,
iceb_u_kolbait(10,inn),inn,
suma);



}
