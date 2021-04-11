/*$Id:$*/
/*06.12.2018	18.12.2014	Белых А.И.	iceb_getkoddok.c
получить код налоговой накладной в формате xml
*/
#include "iceb_libbuh.h"

const char *iceb_getkoddok(short d,short m,short g,
int metka) /*1-код налоговой накладной 
             2-реестр налоговых накладных
             3-Расчёт корректировки сумм НДС к налоговой декларации с налога на добавленную стоимость-додаток 1
             4-додаток 5
             5-додаток 2
             6-форма 1 дф
             7-ЕСВ таблица 5
             8-ЕСВ таблица 6
             9-ЕСВ таблица 7
            */
{
static char kod_nn[64];

memset(kod_nn,'\0',sizeof(kod_nn));
switch(metka)
 {
  case 1: /*Налоговая накладная*/
//    strcpy(kod_nn,"J1201009");
    strcpy(kod_nn,"J1201010");
   break;

  case 2: /*Реестр налоговых накладных*/
   
   if(iceb_u_sravmydat(d,m,g,1,3,2013) < 0)
    strcpy(kod_nn,"J1201505");
   
   if(iceb_u_sravmydat(d,m,g,1,3,2013) >= 0 && iceb_u_sravmydat(d,m,g,1,1,2014) < 0)
    strcpy(kod_nn,"J1201506");
   
   if(iceb_u_sravmydat(d,m,g,1,1,2014) >= 0 && iceb_u_sravmydat(d,m,g,1,12,2014) < 0)
    strcpy(kod_nn,"J1201507");

   if(iceb_u_sravmydat(d,m,g,1,12,2014) >= 0)
    strcpy(kod_nn,"J1201508");

   break;

  case 3:   /*Расчёт корректировки сумм НДС к налоговой декларации с налога на добавленную стоимость-додаток 1*/

 //  strcpy(kod_nn,"J0200516");
   strcpy(kod_nn,"J0200519");
   
   break;

  case 4:   /*Додаток 5   до податкової декларації з податку на додану вартість*/
//   strcpy(kod_nn,"J0215109");
   strcpy(kod_nn,"J0215119");
   break;
   
  case 5:   /*Додаток 2*/
//    strcpy(kod_nn,"J0285106");
    strcpy(kod_nn,"J0285107");
   break;

  case 6:   /*Форма 1дф*/
   if(iceb_u_sravmydat(d,m,g,1,1,2015) < 0)
    strcpy(kod_nn,"J0500104");
   else
    strcpy(kod_nn,"J0500105");
   break;

  case 7: /*ЕСВ таблица 5*/
   strcpy(kod_nn,"J3040512");
   break;

  case 8: /*ЕСВ таблица 6*/
    strcpy(kod_nn,"J3040612");
    break;

  case 9: /*ЕСВ таблица 7*/
   strcpy(kod_nn,"J3040712");
   break;
             
  default:
   char stroka[1024];
   sprintf(stroka,"%s-%s %d!",__FUNCTION__,gettext("Не известный код документа"),metka);
   iceb_menu_soob(stroka,NULL);
  
   break;
 }
 
return(kod_nn);


}

