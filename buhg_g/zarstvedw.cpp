/*$Id: zarstvedw.c,v 1.4 2013/05/17 14:58:30 sasa Exp $*/
/*10.01.2018	22.06.2009	Белых А.И.	zarstvedw.c
Вывод строки ведомости на выдачу зарплаты
*/
#include "buhg_g.h"

void zarstvedw(int *nomer_vedom,const char *tabb,const char *fio,double suma,FILE *ff)
{
*nomer_vedom+=1;
fprintf(ff,"%-3d %-*s %-*.*s",*nomer_vedom,
iceb_u_kolbait(9,tabb),
tabb,iceb_u_kolbait(35,fio),iceb_u_kolbait(35,fio),fio);
fprintf(ff,"%10s\n",iceb_u_prnbr(suma));
fprintf(ff,"\
..............................................................................\n");
}
/************************************/
/************************************/

void zarstvedw(int *nomer_vedom,int tabb,const char *fio,double suma,FILE *ff)
{
*nomer_vedom+=1;
fprintf(ff,"%-3d %-9d %-*.*s",*nomer_vedom,tabb,iceb_u_kolbait(35,fio),iceb_u_kolbait(35,fio),fio);
fprintf(ff,"%10s\n",iceb_u_prnbr(suma));
fprintf(ff,"\
..............................................................................\n");
}
