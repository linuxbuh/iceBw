/* $Id: profsw.c,v 1.15 2013/09/26 09:46:53 sasa Exp $ */
/*22.07.2019    02.12.1992      Белых А.И.      profsw.c
Подпрограмма расчёта отчисления в профсоюзный фонд
*/
#include        "buhg_g.h"

extern short	metkarnb; //0-расчёт налогов для бюджета пропорциональный 1-последовательный
extern double   okrg;
extern short	kuprof; //Код удержания в профсоюзный фонд
extern float	procprof; //Процент отчисления в профсоюзный фонд
extern short	*knvrprof; //Коды не входящие в расчёт удержаний в профсоюзный фонд
extern short    *kodmp;   /*Коды материальной помощи*/

void profsw(int tn,short mp,short gp,int podr,const char *uder_only,GtkWidget *wpredok)
{
double          nah,nah1,bb2;
short           d;
class iceb_u_str shet("");
class iceb_u_str shetb("");
SQL_str         row;
char		strsql[1024];
double		sumanb,sumabu;
class ZARP     zp;
double		matpomb=0.;
double          suma_zarp_o; //Сумма чистой зарплаты общая
double          suma_zarp_b; //Сумма чистой зарплаты бюджетная
double suma_boln_rm=0.;
SQLCURSOR cur;
/*
printw("profs---------------\n");
refresh();
*/
if(kuprof == 0)
  return;

iceb_u_dpm(&d,&mp,&gp,5);
zp.tabnom=tn;
zp.prn=2;
zp.knu=kuprof;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
  
/*Проверяем есть ли код удержания в списке*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and \
knah=%d",tn,kuprof); 
//printw("%s\n",strsql);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
 {
  
  sprintf(strsql,"\n%s-Расчёт профсоюзного взноса\n\
-----------------------------------------------\n",__FUNCTION__);  
  zar_pr_insw(strsql,wpredok);
  
    if(knvrprof != NULL)
     {
      sprintf(strsql,"%s-Коды невходящие в расчёт:",__FUNCTION__);
      zar_pr_insw(strsql,wpredok);
     
      for(int kkk=1; kkk < knvrprof[0] ; kkk ++)
       {
        sprintf(strsql,"%d ",knvrprof[kkk]);
        zar_pr_insw(strsql,wpredok);
       }
      sprintf(strsql,"\n");
      zar_pr_insw(strsql,wpredok);
     }

  class zar_read_tn1h nastr;
  zar_read_tn1w(1,mp,gp,&nastr,wpredok);

  if(iceb_u_proverka(uder_only,kuprof,0,0) != 0)
   {
    sprintf(strsql,"%s-Код %d исключён из расчёта\n",__FUNCTION__,kuprof);
    zar_pr_insw(strsql,wpredok);
    return;
   }
   
  shet.new_plus(row[0]);

  sprintf(strsql,"select shet,shetb from Uder where kod=%d",kuprof);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s-Не найден код удержания %d в справочнике удержаний!\n",__FUNCTION__,kuprof );
    zar_pr_insw(strsql,wpredok);
    return;
   }

  shetb.new_plus(row[1]);
  if(shetb.getdlinna() > 1)
    shet.new_plus(row[0]);

  double suma_nfnz,suma_nfnz_b;
  nah=findnahmw(mp,gp,tn,3,knvrprof,&sumanb,&sumabu,&suma_nfnz,&suma_nfnz_b,&suma_zarp_o,&suma_zarp_b,&suma_boln_rm,wpredok);



  sprintf(strsql,"Берем в расчёт %.2f\n",nah);
  zar_pr_insw(strsql,wpredok);
/************************
  if(nah > nastr.max_sum_for_soc)
   {
    sprintf(strsql,"Превышение максимума %.2f > %.2f\n",nah,nastr.max_sum_for_soc);
    zar_pr_insw(strsql,wpredok);
    nah=nastr.max_sum_for_soc;      
   }
**********************/  

  nah1=nah*procprof/100.*(-1);
  nah1=iceb_u_okrug(nah1,okrg);
  sprintf(strsql,"%.2f*%.2f/100=%.2f\n",nah,procprof,nah*procprof/100.);
  zar_pr_insw(strsql,wpredok);

  if(shetb.getdlinna() <= 1)
   {
    strcpy(zp.shet,shet.ravno());
    sprintf(strsql,"%.2f%%",procprof);
    zapzarpw(&zp,nah1,d,mp,gp,0,shet.ravno(),strsql,0,podr,"",wpredok);
   }
  else
   {
    bb2=0.;
    if(metkarnb == 1)
     {
      if(sumabu > nastr.max_sum_for_soc)
       {
        sprintf(strsql,"Превышение максимума суммы для бюджета %.2f > %.2f\n",sumabu,nastr.max_sum_for_soc);
        zar_pr_insw(strsql,wpredok);
        sumabu=nastr.max_sum_for_soc;
       }

      bb2=(sumabu-matpomb)*procprof/100.*(-1);
      sprintf(strsql,"Бюджетный профсоюз (%.2f-%.2f)*%f/100.*-1=%.2f",
        sumabu,matpomb,procprof,bb2);
      zar_pr_insw(strsql,wpredok);
     }
    if(metkarnb == 0)
     {
      sprintf(strsql,"Разбивка суммы:%.2f не бюджет=%.2f бюджет:%.2f\n",
       nah,sumanb,sumabu);
      zar_pr_insw(strsql,wpredok);
      if(sumabu > 0.009)
       {
        //Если хозрасчёт не равен нолю то долю определять надо
        if(sumanb != 0.)
         bb2=nah1*sumabu/nah;
        else
         bb2=nah1;
       }
     }

    bb2=iceb_u_okrug(bb2,okrg);
    sprintf(strsql,"Налог бюджет=%.2f\n",bb2);
    zar_pr_insw(strsql,wpredok);
    if(shetb.ravno()[0] != '\0')
     {
      strcpy(zp.shet,shetb.ravno());
      sprintf(strsql,"%s %.2f%%",gettext("Бюджет"),procprof); 
      zapzarpw(&zp,bb2,d,mp,gp,0,shetb.ravno(),strsql,0,podr,"",wpredok);
     }
    
    bb2=nah1-bb2;
    bb2=iceb_u_okrug(bb2,okrg);
    if(shet.ravno()[0] != '\0')
     {
      strcpy(zp.shet,shet.ravno());
      sprintf(strsql,"%s %.2f%%",gettext("Хозрасчёт"),procprof);
      zapzarpw(&zp,bb2,d,mp,gp,0,shet.ravno(),strsql,0,podr,"",wpredok);
     }
   }
 }

}
