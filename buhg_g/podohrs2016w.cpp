/*$Id: podohrs2016.c,v 5.2 2013-01-15 10:17:56 sasa Exp $*/
/*19.06.2019	15.12.2003	Белых А.И.	podohrs2016w.c
Возвращяем величину подоходного налога
После 1.1.2016
*/
#include <math.h>
#include "buhg_g.h"

double podohrs2016_pr1(short mes,short god,int tabnom,float max_sum_for_soc,int metka_p_boln,class zar_sumtn *tnsum,GtkWidget *wpredok);


extern double   okrg;
extern float  kof_prog_min; //Коэффициент прожиточного минимума
extern float  pomzp; //Процент от минимальной заработной платы


double podohrs2016w(int tabn,
short mr,short gr,
int metka_zap_lgot, /*0-устанавливать/снимать льготу 1-нет*/
int metka_p_boln, /*0-обычный расчёт 1-перерасчёт вместе с больничным будущих периодов*/
class zar_sumtn *tnsum,
GtkWidget *wpredok)
{
char strsql[2048];
char stroka[1024];
 
double nalog=0.;
double  maxsum=0.;
short du=0,mu=0,gu=0;
class zar_read_tn1h nast_mz;
SQLCURSOR cur;
SQL_str   row;


//читаем дату увольнения
sprintf(stroka,"select datk from Zarn where tabn=%d and god=%d and mes=%d",tabn,gr,mr);
if(iceb_sql_readkey(stroka,&row,&cur,wpredok) == 1)
 iceb_u_rsdat(&du,&mu,&gu,row[0],2);

zar_read_tn1w(1,mr,gr,&nast_mz,wpredok);
  
maxsum=nast_mz.prog_min_ng*kof_prog_min;


sprintf(strsql,"\n%s\nМинимальная зарплата на начало года=%.2f\n",__FUNCTION__,nast_mz.minzar_ng);
zar_pr_insw(strsql,wpredok);
sprintf(strsql,"Прожиточный минимум на начало года=%.2f\n",nast_mz.prog_min_ng);
zar_pr_insw(strsql,wpredok);
sprintf(strsql,"Коэффициент прожиточного минимума=%.2f\n",kof_prog_min);
zar_pr_insw(strsql,wpredok);
sprintf(strsql,"Процент от минимальной зарплаты=%.2f\n",pomzp);
zar_pr_insw(strsql,wpredok);
sprintf(strsql,"Сумма на которую не распростаняется социальная льгота=%.2f\n",tnsum->suma_blg);
zar_pr_insw(strsql,wpredok);
sprintf(strsql,"Сумма на которую распростаняется социальная льгота=%.2f\n",tnsum->suma_slg);
zar_pr_insw(strsql,wpredok);
sprintf(strsql,"Сумма неденежных форм выплаты на которую распространяестя социальная льгота=%.2f\n",tnsum->suma_ndfv_slg);
zar_pr_insw(strsql,wpredok);
sprintf(strsql,"Дата расчёта зарплаты %d.%d\n",mr,gr);
zar_pr_insw(strsql,wpredok);
sprintf(strsql,"Дата увольнения %d.%d.%d\n",du,mu,gu);
zar_pr_insw(strsql,wpredok);
sprintf(strsql,"Максимальный доход для льгот:%.2f*%.2f = %.2f\n",nast_mz.prog_min_ng,kof_prog_min,maxsum);
zar_pr_insw(strsql,wpredok);

maxsum=iceb_u_okrug(maxsum,10.);

sprintf(strsql,"%s-Округлили %.2f\n",__FUNCTION__,maxsum);
zar_pr_insw(strsql,wpredok);



int kollgot=0;
class iceb_u_str kod_lgot("");
float procent_lg=zarlgotw(tabn,mr,gr,&kollgot,&kod_lgot,wpredok);


sprintf(strsql,"%s-Количество социальных льгот=%d\n",__FUNCTION__,kollgot);
zar_pr_insw(strsql,wpredok);

sprintf(strsql,"%s-Максимальная сумма для которой применяется льгота %.2f*%d=%.2f\n",
__FUNCTION__,maxsum,kollgot,maxsum*kollgot);
zar_pr_insw(strsql,wpredok);

maxsum*=kollgot;




if(procent_lg == 0. || maxsum < tnsum->suma_slg+tnsum->suma_blg)
 {
  /*Льгота не применялась*/
  if(metka_zap_lgot == 0)
   {
    sprintf(stroka,"update Zarn set lgot='' where tabn=%d and god=%d and mes=%d",tabn,gr,mr);
    iceb_sql_zapis(stroka,1,0,wpredok);
   }
  if(procent_lg == 0.)
   {
    sprintf(strsql,"%s-Не имеет льгот!\n%s\n",__FUNCTION__,stroka);
    zar_pr_insw(strsql,wpredok);
   }
  else
    if( maxsum < tnsum->suma_slg+tnsum->suma_blg)
     {
      sprintf(strsql,"%s-Льгота не применяется так как %.2f+%.2f > %.2f\n",__FUNCTION__,tnsum->suma_slg,tnsum->suma_blg,maxsum);
      zar_pr_insw(strsql,wpredok);
     }
    
 }
else
 {
  if( maxsum >= tnsum->suma_slg+tnsum->suma_blg)
   {
    sprintf(strsql,"%s-Льгота применяется так как %.2f+%.2f <= %.2f\n",__FUNCTION__,tnsum->suma_slg,tnsum->suma_blg,maxsum);
    zar_pr_insw(strsql,wpredok);
   }

  if(metka_zap_lgot == 0)
   {
    sprintf(stroka,"update Zarn set lgot='%s' where tabn=%d and god=%d and mes=%d",kod_lgot.ravno(),tabn,gr,mr);
    iceb_sql_zapis(stroka,1,0,wpredok);
   }

  tnsum->suma_lgot=nast_mz.prog_min_ng*(procent_lg/100)*(pomzp/100);
  
  sprintf(strsql,"%s-Расчётная сумма льготы=%.2f*%.2f/100*%.2f/100=%.2f\n",__FUNCTION__,nast_mz.prog_min_ng,procent_lg,pomzp,tnsum->suma_lgot);
  zar_pr_insw(strsql,wpredok);
 }



nalog=podohrs2016_pr1(mr,gr,tabn,nast_mz.max_sum_for_soc,metka_p_boln,tnsum,wpredok);

if(nalog != 0.)
  nalog=iceb_u_okrug(nalog,okrg);

return(nalog);

}
/***********************************/
/**********************************************************/

double podohrs2016_pr1(short mes,short god,
int tabnom,
float max_sum_for_soc,
int metka_p_boln, /*0-обычный расчёт 1-перерасчёт вместе с больничным будущих периодов*/
class zar_sumtn *tnsum,
GtkWidget *wpredok)
{
float procent_pn=ICEB_PROCENT_PODOH_NAL_2016;

char strsql[2048];
double suma_nal=0.;


sprintf(strsql,"%s-Рассчитываем налог\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);

if(metka_p_boln == 1)
if(tnsum->suma_boln_bp != 0.)
 {
  sprintf(strsql,"%s-Увеличиваем сумму на сумму больничных будущих периодов %.2f+%.2f=%.2f\n",
  __FUNCTION__,
  tnsum->suma_nah_all_podoh,tnsum->suma_boln_bp,tnsum->suma_nah_all_podoh+tnsum->suma_boln_bp);
  tnsum->suma_nah_all_podoh+=tnsum->suma_boln_bp;
  zar_pr_insw(strsql,wpredok);

  /*Если льгота не распространяется то пересчёт не имеет смысла*/
  tnsum->suma_slg+=tnsum->suma_boln_bp;
 }

double suma_for_srav=tnsum->suma_nah_all_podoh;

sprintf(strsql,"%s-Сумма для сравнения %.2f=%.2f\n",
__FUNCTION__,tnsum->suma_nah_all_podoh,suma_for_srav);

zar_pr_insw(strsql,wpredok);

if(tnsum->suma_ndfv_slg != 0.)
  rudnfvw(&tnsum->suma_slg,tnsum->suma_ndfv_slg,procent_pn);

if(tnsum->suma_ndfv_blg != 0.)
  rudnfvw(&tnsum->suma_blg,tnsum->suma_ndfv_blg,procent_pn);

double suma_slg=tnsum->suma_slg; /*сумма для которой применяется соц.льгота*/
double suma_blg=tnsum->suma_blg; /*сумма для которой не применяется льгота*/
double suma_lgot=0.; /*cумма льгот*/

if(suma_slg != 0. && tnsum->suma_slg >= tnsum->suma_lgot)
 {  
  suma_lgot=tnsum->suma_lgot;
 }    
else
 {
  sprintf(strsql,"%s-Cумма для расчёта %.2f меньше льготы %.2f\n",__FUNCTION__,tnsum->suma_slg,tnsum->suma_lgot);
  zar_pr_insw(strsql,wpredok);
  suma_slg=0.;
 }

suma_nal=(suma_slg-suma_lgot)*procent_pn/100.+suma_blg*procent_pn/100.;

sprintf(strsql,"%s-подоходный налог %f процентов\n",__FUNCTION__,procent_pn);
zar_pr_insw(strsql,wpredok);

sprintf(strsql,"%s-Подоходный налог:(%.2f-%.2f)*%.2f/100.+%.2f*%.2f/100.=%.2f\n",
__FUNCTION__,suma_slg,suma_lgot,procent_pn,suma_blg,procent_pn,suma_nal);
zar_pr_insw(strsql,wpredok);

suma_nal*=-1;  
return(suma_nal);

}
