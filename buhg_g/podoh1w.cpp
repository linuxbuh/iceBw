/* $Id: podoh1w.c,v 1.10 2013/09/26 09:46:52 sasa Exp $ */
/*02.09.2016    01.12.1992      Белых А.И.      podoh1w.c
Подпрограмма расчёта подоходного налога 
отчисления в пенсионный фонд и отчисления на безработицу
с общей суммы
Возвращаем сумму налога
*/
#include        "buhg_g.h"

extern short	metkarnb; //0-Расчёт налогов для бюджета пропорциональный 1-последовательный
extern char	*shetb; /*Бюджетные счета*/

void  podoh1w(int tn, //Табельный номер*/
short mp,short gp,
int podr,
const char *uder_only,
GtkWidget *wpredok)
{
char strsql[1024];

if(metkarnb == 1 && shetb != NULL) //Последовательный расчёт бюджет/хозрасчёт
 {
  sprintf(strsql,"%s-Последовательный расчёт для бюджета\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  
 }

if(iceb_u_sravmydat(1,mp,gp,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
 {
  sprintf(strsql,"%s-Расчёт удержаний до %d.%d.%d г. не выполняется!\n",__FUNCTION__,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV);
  zar_pr_insw(strsql,wpredok);
  
 }
else
 {
  double suma_esv=0.;
  double suma_esvb=0.;
//  zaresvw(tn,mp,gp,podr,&suma_esv,&suma_esvb,0,0.,uder_only,wpredok);

  //расчёт подоходного налога
  ras_podoh1w(tn,mp,gp,podr,suma_esv,suma_esvb,0,0.,uder_only,wpredok);
 }

return;
}
