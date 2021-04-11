/*$Id: iceb_u_sravtime.c,v 1.4 2013/05/17 14:58:43 sasa Exp $*/
/*12.01.2006	12.01.2006	Белых А.И.	iceb_u_sravtime.c
Сравнение времени
Если вернули 0 - равны
             больше 0 - первое больше второго
             меньше 0 - первое меньше второго
*/

int iceb_u_sravtime(short has1,short min1,short sek1,short has2,short min2,short sek2)
{
if(has1 > has2)
 return(1);
if(has1 < has2)
 return(-1);

if(min1 > min2)
 return(1);
if(min1 < min2)
 return(-1);

if(sek1 > sek2)
 return(1);
if(sek1 < sek2)
 return(-1);

return(0);

}
