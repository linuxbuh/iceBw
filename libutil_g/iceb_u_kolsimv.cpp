/*$Id: iceb_u_kolsimv.c,v 1.2 2013/08/13 06:10:17 sasa Exp $*/
/*12.09.2011	05.09.2011	Белых А.И.	iceb_u_kolsimv.c
Определение сколько байт занимают символы во входной строке которые могут целиком поместитьтся в массив заданного размера
Возвращает количество символов
*/
#include "iceb_util.h"
int iceb_u_kolsimv(int kolbait_str,const char *stroka)
{
int voz=0;
int kol_bait=0;
int kolih_simv=iceb_u_strlen(stroka);
//printw("%s-stroka=%s kolih_simv=%d kolbait_str=%d\n",__FUNCTION__,stroka,kolih_simv,kolbait_str);
for(int nom=1; nom <= kolih_simv; nom++)
 {
  kol_bait=iceb_u_kolbait(nom,stroka);
  if(kol_bait >= kolbait_str-1)
   break;
  voz++;
 }
return(voz);

}
