/*$Id: ssf8drw.c,v 1.2 2013/08/13 06:09:53 sasa Exp $*/
/*12.07.2015	30.10.2012	Белых А.И.	ssf8drw.c
*/
/***********************/
/*Счетчик строк        */
/***********************/
#include "buhg_g.h"
extern int      kol_strok_na_liste;

void ssf8drw(int *klst,int *kollist,FILE *ff)
{
*klst+=1;
if(*klst < kol_strok_na_liste)
  return; 
fprintf(ff,"\f");
*klst=0;
spkvdw(klst,ff);
*kollist+=1;
}
