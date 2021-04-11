/*$Id: rudnfvw.c,v 1.6 2013/08/13 06:09:52 sasa Exp $*/
/*11.01.2016	01.08.2004	Белых А.И.	rudnfvw.c
Расчёт увеличения дохода при неденежных формах начисления зарплаты
*/
#include        "buhg_g.h"

extern double   okrg;


void rudnfvw(double *suma_zar,double suma_ndfnz,double proc_podoh)
{
if(suma_ndfnz == 0.)
 return;

float suma_ud=0.;
 
double suma_snn=0; //Сумма с которой должен начисляться налог
suma_snn=suma_ndfnz*(100/(100-proc_podoh));
suma_snn=iceb_u_okrug(suma_snn,okrg);
 //вычисляем сумму увеличуния дохода
suma_ud=suma_snn-suma_ndfnz;
   
char strsql[1024];

sprintf(strsql,"%s-Неденежные формы пачисления зарплаты !!!\n",__FUNCTION__);
zar_pr_insw(strsql,NULL);

sprintf(strsql,"%s-Сумма с которой нужно начислять налог=%.2f*(100/(100-%f))=%.2f\n",
__FUNCTION__,
suma_ndfnz,proc_podoh, suma_snn);
zar_pr_insw(strsql,NULL);

sprintf(strsql,"%s-Увеличение дохода на %.2f-%.2f=%.2f\n",__FUNCTION__,suma_snn, suma_ndfnz, suma_ud);    
zar_pr_insw(strsql,NULL);

sprintf(strsql,"%s-Увеличиваем сумму облагаемого налогом дохода=%.2f+%.2f=%.2f\n",
__FUNCTION__,
*suma_zar,suma_ud,*suma_zar+suma_ud);
zar_pr_insw(strsql,NULL);

*suma_zar+=suma_ud;

}
