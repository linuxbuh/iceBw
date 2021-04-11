/*$Id: sumprvedw.c,v 1.11 2013/08/13 06:09:53 sasa Exp $*/
/*12.01.2018	18.02.2003	Белых А.И. 	sumprvedw.c
Вставка суммы прописью в шапку ведомости
*/
#include <errno.h>
#include <math.h>
#include "buhg_g.h"


void sumprvedw(iceb_u_double *SUMPODR, //Суммы по ведомостям
iceb_u_int *KOLLIST, //Количество листов в распечатках
char *imaf,
GtkWidget *wpredok)
{
char	imaftmp[64];
class iceb_u_str stroka("");
class iceb_u_str strokasum("");
FILE	*fftmp,*ff;
double  suma=0.;
int	shethik=0;
int	shethikl=0;
char    bros[512];

if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

sprintf(imaftmp,"prom%d.tmp",getpid());


if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return;
 }
class iceb_fioruk_rk fio_vip;
class iceb_fioruk_rk fio_prov;
iceb_fioruk(3,&fio_vip,wpredok);
iceb_fioruk(4,&fio_prov,wpredok);

class iceb_u_str naim_kontr_00(iceb_get_pnk("00",0,wpredok));

double suma_cel=0.;
double suma_drob=0.;
int nomstr=0;
while(iceb_u_fgets(&stroka,ff) == 0)
 {
  nomstr++;

  if(iceb_u_strstrm(stroka.ravno(),naim_kontr_00.ravno()) == 1) /*строка с наименованием организации*/
   {
    nomstr=5; /*В одном файле может быть несколько ведомостей*/
   }

  switch(nomstr)
   {
    case 14:
   
     suma=SUMPODR->ravno(shethik++);
     iceb_u_preobr(suma,&strokasum,0);

     suma_drob=modf(suma,&suma_cel)*100;

     sprintf(bros,"(%.f грн. %02.f коп.)",suma_cel,suma_drob);

     strokasum.plus(" ",bros);

     iceb_u_vstav(&stroka,strokasum.ravno(),7,39,1);
     break;

    case 15:
      if(iceb_u_strlen(strokasum.ravno()) > 33)
        iceb_u_vstav(&stroka,iceb_u_adrsimv(33,strokasum.ravno()),0,39,1);
      break;

    case 16:
      if(iceb_u_strlen(strokasum.ravno()) > 73)
       iceb_u_vstav(&stroka,iceb_u_adrsimv(73,strokasum.ravno()),0,39,1);
      break;
    case 23:
      sprintf(bros,"%d",KOLLIST->ravno(shethikl++));
      iceb_u_vstav(&stroka,bros,71,79,1);
      break;

    case 31: /*фамилия выплатившего*/
      iceb_u_vstav(&stroka,fio_vip.famil_inic.ravno(),55,80,1);
      break;

    case 33: /*фамилия проверившего*/
      iceb_u_vstav(&stroka,fio_prov.famil_inic.ravno(),55,80,1);
      break;
   }
        
  fprintf(fftmp,"%s",stroka.ravno());

 }
fclose(ff);
fclose(fftmp);
unlink(imaf);
rename(imaftmp,imaf);
}
