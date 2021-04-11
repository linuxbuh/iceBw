/*$Id: zvb_corp2_strw.c,v 1.1 2014/08/31 06:19:23 sasa Exp $*/
/*20.11.2019	19.08.2014	Белых А.И.	zvb_corp2_strw.c
Вывод строк в файлы
Ім’я поля 	Тип		Довжина	Найменування реквізиту
NSC		Char		29	рахунок отримувача
SUMMA		Numeric		20.2	сума платежу (в грн)
FIO		Character	38	найменування отримувача
ID_KOD		Numeric		14	ідентифікатор код отримувача
*/
#include "buhg_g.h"

void zvb_corp2_strw(int nomer_zap,const char *fio,
const char *shet_b,
double suma,
const char *inn,
const char *tabnom,
FILE *ff_dbf_tmp,FILE *ff_lst)
{



fprintf(ff_dbf_tmp," %-*.*s%20.2f%-*.*s%-*.*s",
iceb_u_kolbait(29,shet_b),
iceb_u_kolbait(29,shet_b),
shet_b,
suma,
iceb_u_kolbait(38,fio),
iceb_u_kolbait(38,fio),
fio,
iceb_u_kolbait(14,inn),
iceb_u_kolbait(14,inn),
inn);


zvb_str_lstw(nomer_zap,fio,shet_b,suma,inn,tabnom,ff_lst);

/******************************
fprintf(ff_lst,"\
%6d %-*s %-*s %*s %-*s %10.2f\n",
nomer_zap,
iceb_u_kolbait(6,tabnom),tabnom,
iceb_u_kolbait(40,fio),fio,
iceb_u_kolbait(14,shet_b),shet_b,
iceb_u_kolbait(10,inn),inn,
suma);
***************************************/


}
