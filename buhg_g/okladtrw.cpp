/*$Id: okladtrw.c,v 1.7 2013/08/13 06:09:47 sasa Exp $*/
/*28.11.2008	28.11.2008	Белых А.И.	okladtrw.c
Расчёт оклада по тарифу
*/

#include "buhg_g.h"


double okladtrw(short mr,short gr,int roz,GtkWidget *wpredok)
{
double oklad=0.;
char strsql[1024];

class zar_read_tn1h nastr;
zar_read_tn1w(1,mr,gr,&nastr,wpredok);

sprintf(strsql,"%s-Расчёт оклада по тарифному разряду:\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);

sprintf(strsql,"Разряд:%d\n",roz);
zar_pr_insw(strsql,wpredok);
    
float kof=zartarrozw(roz);
sprintf(strsql,"Коэффициент:%f\n",kof);
zar_pr_insw(strsql,wpredok);
  
double okk=0.;
if(kof > 0.)
 {
  okk=nastr.minzar*kof;
  oklad=iceb_u_okrug(okk,1.);              
 }
else
 {
  sprintf(strsql,"Не нашли коэффициент!\n");
  zar_pr_insw(strsql,wpredok);
  oklad=0.;
  kof=0.;
 }   

sprintf(strsql,"Оклад:%.2f*%f=%f округляем=%.2f\n",nastr.minzar,kof,okk,oklad);
zar_pr_insw(strsql,wpredok);

return(oklad);

}
