/*$Id: iceb_u_nofwords.c,v 1.4 2014/07/31 07:08:30 sasa Exp $*/
/*29.07.2014	05.07.2011	Белых А.И.	iceb_u_nofwords.c
Получение числа прописью
дробная часть до 6 знаков полсе запятой !!!!
*/
#include <math.h>
#include "iceb_util.h"

const char *iceb_u_nofwords(double number)
{
static class iceb_u_str str_voz;
char drob_str[2048];
class iceb_u_str drob_str_char("");
class iceb_u_str naim_drob("");
double cel_h=0.;
double drob_h=0.;


str_voz.new_plus(iceb_u_propis(number,1));

drob_h=modf(number,&cel_h);

if(fabs(drob_h) > 0.00000009)
 {
  if(cel_h == 1.0)
   str_voz.plus(gettext(" целая и "));
  else  
   str_voz.plus(gettext(" целых и "));
 }
//printw("%s-%.15f\n",__FUNCTION__,drob_h);

sprintf(drob_str,"%.10g",drob_h);
iceb_u_polen(drob_str,&drob_str_char,2,'.');

memset(drob_str,'\0',sizeof(drob_str));

int kolznak=strlen(drob_str_char.ravno());
switch(kolznak)
 {
  case 0: /*нет дробной части*/

   break;

  case 1:

   strcpy(drob_str,iceb_u_propis(drob_h*10.,0));
   drob_h=iceb_u_okrug(drob_h,0.1); /*округляем чтобы правильно выполнилось первое условие*/

   if(drob_h == 0.1)
    naim_drob.new_plus(gettext("десятая"));
   else    
    naim_drob.new_plus(gettext("десятых"));
   break;

  case 2:

   strcpy(drob_str,iceb_u_propis(drob_h*100.,0));
   drob_h=iceb_u_okrug(drob_h,0.01); /*округляем чтобы правильно выполнилось первое условие*/

   if(drob_h == 0.01)
    naim_drob.new_plus(gettext("сотая"));
   else    
    naim_drob.new_plus(gettext("сотых"));
   break;

  case 3:

   strcpy(drob_str,iceb_u_propis(drob_h*1000.,0));
   drob_h=iceb_u_okrug(drob_h,0.001); /*округляем чтобы правильно выполнилось первое условие*/

   if(drob_h == 0.001)
    naim_drob.new_plus(gettext("тысячная"));
   else    
    naim_drob.new_plus(gettext("тысячных"));
   break;


  case 4:

   strcpy(drob_str,iceb_u_propis(drob_h*10000.,0));
   drob_h=iceb_u_okrug(drob_h,0.0001); /*округляем чтобы правильно выполнилось первое условие*/
   if(drob_h == 0.0001) 
     naim_drob.new_plus(gettext("десятитысячная"));
   else    
     naim_drob.new_plus(gettext("десятитысячных"));
   break;


  case 5:

   strcpy(drob_str,iceb_u_propis(drob_h*100000.,0));
   drob_h=iceb_u_okrug(drob_h,0.000001); /*округляем чтобы правильно выполнилось первое условие*/

   if(drob_h == 0.00001)
    naim_drob.new_plus(gettext("стотысячная"));
   else    
    naim_drob.new_plus(gettext("стотысячных"));
   break;


  case 6:

   strcpy(drob_str,iceb_u_propis(drob_h*1000000.,0));
   drob_h=iceb_u_okrug(drob_h,0.0000001); /*округляем чтобы правильно выполнилось первое условие*/

   if(drob_h == 0.000001)
    naim_drob.new_plus(gettext("милионная"));
   else    
    naim_drob.new_plus(gettext("милионных"));
   break;
  
  default:
   naim_drob.new_plus(gettext("не предусмотренная величина"));
   break;   
 }
str_voz.plus(drob_str);
str_voz.plus(" ",naim_drob.ravno());
//printw("%s-%s\n",__FUNCTION__,str_voz.ravno());

return(str_voz.ravno());
}
