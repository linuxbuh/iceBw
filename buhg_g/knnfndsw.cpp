/*$Id: knnfndsw.c,v 1.6 2013/08/13 06:09:36 sasa Exp $*/
/*12.01.2013	12.08.2009	Белых А.И.	knnfndsw.c
Начисление НДС на неденежные формы оплаты
*/
#include "buhg_g.h"
extern short *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short *knnf; /*Коды начислений неденежными формами*/
extern short *knnf_nds; /*Коды начислений неденежными формами на которые начисляется НДС*/
extern float pnds; 
extern double okrg;
extern char *shetb; /*Бюджетные счета*/

double knnfndsw(int metka,/*0-для расчёта подоходного налога 1-все остальные случаи*/
int knu,const char *shet,double suma,double *sumab,int metka_repl) /*0-без реплик 1-с репликами*/
{
/***************
Коды на которые не распространяется социальная льгота, могут также быть
в списке кодов не денежных форм начисления зарплаты. Тоесть на них может быть 
начислен НДС и применён коэффициент увеличения. При расчёте подоходного налога это
делается в другом месте
***********/
if(metka == 0)
 if(provkodw(kn_bsl,knu) >= 0) /*Для них отдельно начисляется если это нужно*/
  {
  return(0.);
  }

if(provkodw(knnf,knu) < 0)
 {
  return(0.);
 }
char strsql[512];

if(metka_repl == 1)
 {
  sprintf(strsql,"%s-Не денежная форма оплаты %d %.2f\n",__FUNCTION__,knu,suma);
  zar_pr_insw(strsql,NULL);
 }

double suma_s_nds=suma;
if(provkodw(knnf_nds,knu) >= 0)
 {
  double suma_nds=suma*pnds/100.;
  suma_nds=iceb_u_okrug(suma_nds,okrg);

  suma_s_nds+=suma_nds;
  sprintf(strsql,"%s-На начисление начисляется НДС = %f+%f*%f/100.=%f\n",__FUNCTION__,suma,suma,pnds,suma_s_nds);
  zar_pr_insw(strsql,NULL);

  if(iceb_u_proverka(shetb,shet,0,1) == 0)
   {
    sprintf(strsql,"%s-Не денежная форма оплаты (бюджет) %d %.2f\n",__FUNCTION__,knu,suma);
    zar_pr_insw(strsql,NULL);
    *sumab+=suma+suma_nds;  
   }

 }
 
return(suma_s_nds);

}

