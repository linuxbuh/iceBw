/*$Id: ukreksim_zstrw.c,v 1.5 2013/08/13 06:09:53 sasa Exp $*/
/*08.04.2015	25.01.2006	Белых А.И.	ukreksim_zstrw.c
Запись в файл распечатки и файл для записи на дискету для Укрэксимбанка
*/
#include <stdio.h>

void ukreksim_zstrw(FILE *fftmp,FILE *ffr,int *nomer,double suma,const char *fio,
const char *kartshet,const char *tabn,short dt,short mt,short gt)
{
*nomer+=1;

//fprintf(fftmp," UAH%-10.10s%04d%02d%02d%15.2f",kartshet,gt,mt,dt,suma);
fprintf(fftmp," %04d%02d%02d%-10.10s%15.2fUAH%32s%32s",gt,mt,dt,kartshet,suma,"","");


fprintf(ffr,"%4d %-9s %-14s %10.2f %s\n",*nomer,tabn,kartshet,suma,fio);

}
