/* $Id: alimw.c,v 1.17 2013/09/26 09:46:45 sasa Exp $ */
/*12.08.2015    01.12.1992      Белых А.И.     alimw .c
Подпрограмма расчёта алиментов
Переменные данные лежат в файле alim.alx
*/
#include <math.h>
#include        "buhg_g.h"

void alim_ps(class iceb_u_double *suma,class iceb_u_double *p_sbor,class iceb_u_double *suma_plus,const char *stroka);
double alim_ps_r(class iceb_u_double *suma,class iceb_u_double *p_sbor,class iceb_u_double *suma_plus,double suma_nah,double *sum_plus);
//void alim_ps(class iceb_u_double *suma,class iceb_u_double *p_sbor,const char *stroka);
//double alim_ps_r(class iceb_u_double *suma,class iceb_u_double *p_sbor,double suma_nah);

extern double   okrg;
extern SQL_baza bd;

void alimw(int tn,short mp,short gp,int podr,const char *uder_only,GtkWidget *wpredok)
{
int kolstr=0;
class iceb_u_str br("");
short           ku=0;   /*Код начисления*/
short           kps=0;   /*Код почтового сбора*/
double          nah=0.;
double          proc,prsb;
double          bb,bb1;
int            tnn;
short		d;
class iceb_u_str knr(""); /*Коды не входящие в расчёт*/
short           shetchik;
SQL_str         row,row_alx;
class SQLCURSOR cur,cur_alx;
char		strsql[1024];
class iceb_u_str shetalim("");
class iceb_u_str shetps("");
class iceb_u_str koment("");
class iceb_u_str fio("");
class iceb_u_str shetban(""); /*счет на который перечисляются алименты*/
class iceb_u_str alimudvr(""); //Удержания входящие в расчёт алиментов
short metka_proh=0;
class iceb_u_str imaf("zaralim.alx");

class ZARP     zp;

sprintf(strsql,"select str from Alx where fil='zaralim.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s-Не найдены настройки zaralim.alx\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return;
 }

iceb_u_dpm(&d,&mp,&gp,5);

zp.tabnom=tn;
zp.prn=2;
zp.knu=ku;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

prsb=proc=0.;



class iceb_u_double suma_p_ua; //диапазоны сумм по Украине
class iceb_u_double suma_p_sng; //диапазоны сумм по СНГ

class iceb_u_double suma_p_sng_ps; //Проценты почтового сбора по СНГ
class iceb_u_double suma_p_ua_ps; //Проценты почтового сбора по Украине
class iceb_u_double suma_plus_ua; //Сумма которую прибавляют по Украине
class iceb_u_double suma_plus_sng; //Сумма которую прибавляют по СНГ

tnn=0;
shetchik=ku=kps=0;
int metka_ps=0;
double nds1=0.;
double min_sum_ua=0.;
double min_sum_sng=0.;
double min_sum_ps=0.;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],&br,1,'|') != 0)
     continue;
  if(iceb_u_SRAV("Код удержания алиментов",br.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&ku,2,'|');
//    ku=(short)iceb_u_atof(br);
   
      /*Проверяем есть ли код удержания в списке*/

    sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and \
    knah=%d",tn,ku); 
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
     {
      return;
     }
    shetalim.new_plus(row[0]);

      
    nds1=iceb_pnds(1,mp,gp,wpredok);

    sprintf(strsql,"\n%s-Расчёт алиментов\n\
---------------------------------------------\n\
Код алиментов=%d\n\
НДС=%.2f\n",__FUNCTION__,ku,nds1);
    zar_pr_insw(strsql,wpredok);
    if(iceb_u_proverka(uder_only,ku,0,0) != 0)
     {
      sprintf(strsql,"%s-Код %d исключён из расчёта\n",__FUNCTION__,ku);
      zar_pr_insw(strsql,wpredok);
      return; 
     }
    continue;
   }

  if(iceb_u_SRAV("Код удержания почтового сбора",br.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&kps,2,'|');
 //   kps=(short)iceb_u_atof(br);

    sprintf(strsql,"%s-Код почтового сбора=%d\n",__FUNCTION__,kps);
    zar_pr_insw(strsql,wpredok);

    sprintf(strsql,"select shet from Uder where kod=%d",kps);

    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
     {
      sprintf(strsql,"%s-Не найден код почтового сбора %d\n",__FUNCTION__,kps);
      zar_pr_insw(strsql,wpredok);
     }
    else
      shetps.new_plus(row[0]);
    continue;
   }

  if(iceb_u_SRAV("Коды удержаний входящие в расчёт алиментов",br.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&alimudvr,2,'|');
    sprintf(strsql,"%s-Коды удержаний входящие в расчёт алиментов=%s\n",__FUNCTION__,alimudvr.ravno());
    zar_pr_insw(strsql,wpredok);
    continue;
   }
  if(iceb_u_SRAV("Коды начислений не входящие в расчёт",br.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&knr,2,'|');
    sprintf(strsql,"%s-Коды начислений не входящие в расчёт=%s\n",__FUNCTION__,knr.ravno());
    zar_pr_insw(strsql,wpredok);
    continue;
   }
  if(iceb_u_SRAV("Почтовый сбор по Украине",br.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&br,2,'|');
    sprintf(strsql,"%s-Почтовый сбор по Украине=%s\n",__FUNCTION__,br.ravno());
    zar_pr_insw(strsql,wpredok);
    alim_ps(&suma_p_ua,&suma_p_ua_ps,&suma_plus_ua,br.ravno());
    continue;
   }
  if(iceb_u_SRAV("Почтовый сбор по СНД",br.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&br,2,'|');
    sprintf(strsql,"%s-Почтовый сбор по СНД=%s\n",__FUNCTION__,br.ravno());
    zar_pr_insw(strsql,wpredok);
    alim_ps(&suma_p_sng,&suma_p_sng_ps,&suma_plus_sng,br.ravno());
    continue;
   }
  if(iceb_u_SRAV("Минимальная сумма почтового сбора по Украине без НДС",br.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&min_sum_ua,2,'|');
//    min_sum_ua=iceb_u_atof(br);
    sprintf(strsql,"%s-Минимальная сумма почтового сбора по Украине без НДС=%.2f\n",__FUNCTION__,min_sum_ua);
    zar_pr_insw(strsql,wpredok);
    continue;
   }
  if(iceb_u_SRAV("Минимальная сумма почтового сбора по СНГ без НДС",br.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&min_sum_sng,2,'|');
//    min_sum_sng=iceb_u_atof(br);
    sprintf(strsql,"%s-Минимальная сумма почтового сбора по СНГ без НДС=%.2f\n",__FUNCTION__,min_sum_sng);
    zar_pr_insw(strsql,wpredok);
    continue;
   }

  tnn=br.ravno_atoi();
  if(tnn != tn)
   continue;
  
  sprintf(strsql,"%s-Табельный номер=%d\n",__FUNCTION__,tnn);
  zar_pr_insw(strsql,wpredok);
//   printw("Табельный номер=%d\n",tnn);
//   refresh();
  if(metka_proh == 0)
   {
    nah=0.;
    
    SQL_str row;
    class SQLCURSOR cur;
    int kolstr=0;

    sprintf(strsql,"select prn,knah,suma from Zarp where tabn=%d and datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' \
    and suma <> 0.",tn,gp,mp,gp,mp);

    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
      return;
     }

    if(kolstr == 0)
     {
      return;
     }
    while(cur.read_cursor(&row) != 0)
     {
      if(atoi(row[0]) == 1)
       {
        if(iceb_u_proverka(knr.ravno(),row[1],0,1) == 0)
         continue;
       }      
      else
       {
        if(iceb_u_proverka(alimudvr.ravno(),row[1],0,1) != 0)
            continue;
       }
      sprintf(strsql,"%s-prn=%s knah=%s suma=%s\n",__FUNCTION__,row[0],row[1],row[2]);
      zar_pr_insw(strsql,wpredok);
      
      nah+=atof(row[2]);
     }

    if(nah <=0.)
     {
      return;
     }
   }
  metka_proh++;

  iceb_u_polen(row_alx[0],&proc,2,'|');

  iceb_u_polen(row_alx[0],&metka_ps,3,'|');
  iceb_u_polen(row_alx[0],&fio,4,'|');
  iceb_u_polen(row_alx[0],&shetban,6,'|');

  if(shetban.ravno()[0] != '\0')
    koment.new_plus(shetban.ravno());    
  else
    koment.new_plus(fio.ravno());    
  

  bb=nah*proc/100.;
  bb=iceb_u_okrug(bb,okrg)*-1;

  prsb=0.;
  double sum_plus=0.;
  if(metka_ps == 1)
   {
    sprintf(strsql,"%s-Почтовый сбор по ставкам Украины\n",__FUNCTION__);
    zar_pr_insw(strsql,wpredok);
    prsb=alim_ps_r(&suma_p_ua,&suma_p_ua_ps,&suma_plus_ua,bb,&sum_plus);
    min_sum_ps=min_sum_ua;
   }
  if(metka_ps == 2)
   {
    sprintf(strsql,"%s-Почтовый сбор по ставкам СНГ\n",__FUNCTION__);
    zar_pr_insw(strsql,wpredok);
    prsb=alim_ps_r(&suma_p_sng,&suma_p_sng_ps,&suma_plus_sng,bb,&sum_plus);
    min_sum_ps=min_sum_sng;
   }
  sprintf(strsql,"%s-Сумма с которой начисляются алименты=%.2f\n\
Просент алиментов =%.2f\n\
Сумма алиментов=%.2f\n\
Процент почтового сбора=%.2f\n",
  __FUNCTION__,nah,proc,bb,prsb);
  zar_pr_insw(strsql,wpredok);
  
  strcpy(zp.shet,shetalim.ravno());
      
  zp.knu=ku;
  zapzarpw(&zp,bb,d,mp,gp,0,shetalim.ravno(),koment.ravno(),0,podr,"",wpredok);

  bb1=bb*prsb/100.-sum_plus; /*Отнимаем потому что сумма отрицательная*/
  bb1=iceb_u_okrug(bb1,okrg);


  sprintf(strsql,"%s-Сумма почтового сбора без НДС=%.2f*%.2f/100.+%.2f=%.2f\n",__FUNCTION__,bb*-1,prsb,sum_plus,bb1*-1);
  zar_pr_insw(strsql,wpredok);

  if(min_sum_ps != 0. && min_sum_ps > bb1*-1)
   {
    sprintf(strsql,"%s-Почтовый сбор меньше минимальной суммы почтового сбора %.2f !\n\
Устанавливаем почтовый сбор в размере %.2f\n",__FUNCTION__,min_sum_ps,min_sum_ps);     
    zar_pr_insw(strsql,wpredok);
    bb1=min_sum_ps*-1;
   }
/*******************
  double suma_nds=bb1*nds1/100.;
  suma_nds=iceb_u_okrug(suma_nds,okrg);

  sprintf(strsql,"%s-Сумма НДС=%.2f*%.2f/100.=%.2f\n",__FUNCTION__,bb1,nds1,suma_nds);
  zar_pr_insw(strsql,wpredok);

  bb1+=suma_nds;

  sprintf(strsql,"%s-Сумма почтового сбора с НДС=%.2f\n",__FUNCTION__,bb1);
  zar_pr_insw(strsql,wpredok);
*****************/

  if(kps != 0 )
   {
    strcpy(zp.shet,shetps.ravno());
    
    zp.knu=kps;
    zapzarpw(&zp,bb1,d,mp,gp,0,shetps.ravno(),koment.ravno(),0,podr,"",wpredok);
   }
  shetchik++;
  zp.nomz=shetchik;
 }

}
/**********************************/
/*Расшифровка строки с процентами почтового сбора*/
/***********************************************/
void alim_ps(class iceb_u_double *suma,class iceb_u_double *p_sbor,class iceb_u_double *suma_plus,const char *stroka)
{
int kolpol=iceb_u_pole2(stroka,',');
if(kolpol == 0)
 return;
class iceb_u_str bros("");
class iceb_u_str bros1("");
for(int i=0; i < kolpol;i++)
 {
  iceb_u_polen(stroka,&bros,i+1,',');

  iceb_u_polen(bros.ravno(),&bros1,1,'/');
  suma->plus(bros1.ravno_atof(),-1);  

  iceb_u_polen(bros.ravno(),&bros1,2,'/');
  p_sbor->plus(bros1.ravno_atof(),-1);
  iceb_u_polen(bros1.ravno(),&bros,2,'+');
  
  suma_plus->plus(bros.ravno_atof(),-1);  
 }
}
/*************************/
/*Расчёт почтового сбора*/
/**************************/
double alim_ps_r(class iceb_u_double *suma,class iceb_u_double *p_sbor,class iceb_u_double *suma_plus,double suma_nah,double *sum_plus)
{
double ps=0.;
for(int i=0 ; i < suma->kolih() ; i++)
 {
  if(fabs(suma_nah) <= suma->ravno(i) || suma->ravno(i) == 0.)
   {
    ps=p_sbor->ravno(i);
    *sum_plus=suma_plus->ravno(i);
    break;
   }
 }

return(ps);
}
