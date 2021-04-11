/*$Id: buh_hpsw.c,v 1.1 2013/05/17 15:01:15 sasa Exp $*/
/*17.04.2013	14.01.2008	Белых А.И.	buh_hpsw.c
Получение из шахматки по субсчетам шахматки по счетам
*/

#include "buhg_g.h"

void buh_hpsw(class iceb_u_spisok *deb,class iceb_u_spisok *kre,class iceb_u_double *suma,
class iceb_u_spisok *debsh,class iceb_u_spisok *kresh,class iceb_u_double *sumash,GtkWidget *wpredok)
{
char shet[64];

/*создаём дебетовый список по счетам*/
for(int ii=0; ii < deb->kolih(); ii++)
 {
  sprintf(shet,"%*.*s",
  iceb_u_kolbait(2,deb->ravno(ii)),
  iceb_u_kolbait(2,deb->ravno(ii)),
  deb->ravno(ii));
  if(debsh->find(shet) < 0)
   debsh->plus(shet);
 }

/*создаём кредитовый список по счетам*/
for(int ii=0; ii < kre->kolih(); ii++)
 {
  sprintf(shet,"%*.*s",
  iceb_u_kolbait(2,kre->ravno(ii)),
  iceb_u_kolbait(2,kre->ravno(ii)),
  kre->ravno(ii));
  if(kresh->find(shet) < 0)
   kresh->plus(shet);
 }
sumash->make_class(kresh->kolih()*debsh->kolih());

/*Заполняем массив суммами*/

int nomdsh=0,nomksh=0;
for(int nomer_deb=0; nomer_deb < deb->kolih(); nomer_deb++)
 {
  if((nomdsh=debsh->find(deb->ravno(nomer_deb),1)) < 0)
   {
    char bros[1024];
    sprintf(bros,"%s-%s %s\n",__FUNCTION__,gettext("Не найден дебетовый счёт"),deb->ravno(nomer_deb));
    iceb_menu_soob(bros,wpredok);
    continue;
   }   
 
   for(int nomer_kre=0; nomer_kre < kre->kolih(); nomer_kre++)
    {
     if((nomksh=kresh->find(kre->ravno(nomer_kre),1)) < 0)
      {
       char bros[1024];
       sprintf(bros,"%s-%s %s\n",__FUNCTION__,gettext("Не найден кредитовый счёт"),kre->ravno(nomer_kre));
       iceb_menu_soob(bros,wpredok);
       continue;
      
      }   
     
     sumash->plus(suma->ravno(nomer_deb*kre->kolih()+nomer_kre), nomdsh*kresh->kolih()+nomksh);
    }
 }

}
