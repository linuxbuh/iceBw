/*$Id: iceb_u_strspisok.c,v 1.2 2013/08/13 06:10:17 sasa Exp $*/
/*28.03.2012	28.03.2012	Белых А.И.	iceb_u_strspisok.c
Взятие данных из строки разделённых символом разделителем в список
*/

#include "iceb_util.h"

void iceb_u_strspisok(const char *stroka,int razdelitel,class iceb_u_spisok *spisok)
{
int kolih_razdel=iceb_u_pole2(stroka,razdelitel);
if(kolih_razdel == 0)
 {
  spisok->plus(stroka);
  return;
 } 
class iceb_u_str pole("");
for(int nom=1; nom <= kolih_razdel; nom++)
 {
  iceb_u_polen(stroka,&pole,nom,razdelitel);
  spisok->plus(pole.ravno());
 }
 
}
