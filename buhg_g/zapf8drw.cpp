/*$Id: zapf8drw.c,v 1.2 2013/08/13 06:09:59 sasa Exp $*/
/*17.07.2013	30.10.2012	Белых А.И.	zapf8drw.c
Запись в dbf файл              
*/
#include <stdio.h>

void  zapf8drw(int npp1,short kvrt,short gn,const char *koded,short TYP,const char *inn,
double suman,double sumav,double podohn,double podohv,const char *priz,const char *datn,const char *datk,
short kodlg,short vidz,FILE *ff4)
{
/*******до 1.1.2011
fprintf(ff4, " %5.5d%1d%04d%-10.10s%d%-10.10s%12.2f%12.2f%12.2f%12.2f%2.2s%8.8s%8.8s%2.2d%1.1d",
npp1,kvrt,gn,koded,TYP,inn,suman,sumav,podohn,podohv,priz,datn,datk,kodlg,vidz);
******************/
/************
fprintf(ff4, " %5.5d%1d%04d%-10.10s%d%-10.10s%12.2f%12.2f%12.2f%12.2f%3.3s%8.8s%8.8s%2.2d%1.1d",
npp1,kvrt,gn,koded,TYP,inn,suman,sumav,podohn,podohv,priz,datn,datk,kodlg,vidz);
**************/
char kodlgot[64];
sprintf(kodlgot,"%2d",kodlg); /*код льготы должен быть 2 знака*/
fprintf(ff4, " %5.5d%1d%4d%-10.10s%d%-10.10s%12.2f%12.2f%12.2f%12.2f%3.3s%8.8s%8.8s%2.2s%1.1d",
npp1,kvrt,gn,koded,TYP,inn,suman,sumav,podohn,podohv,priz,datn,datk,kodlgot,vidz);

}
