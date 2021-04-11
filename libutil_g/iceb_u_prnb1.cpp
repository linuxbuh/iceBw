/*$Id: iceb_u_prnb1.c,v 1.4 2013/05/17 14:58:43 sasa Exp $*/
/*10.05.2006	13.02.1997	Белых А.И. iceb_u_prnb1.c
Вставка запятой в число
*/
#include <stdio.h>
#include <string.h>

void    iceb_u_prnb1(double nb, //Число для преобразования
char *nb1)  //Символьная строка с числом
{
short           i,i1;

sprintf(nb1,"%.2f",nb);
i1=strlen(nb1);

for(i=i1;i >=0 ;i--)
 {
  if(nb1[i] == '.')
   {
    nb1[i] = ',';
    break;
   } 
 }
} 
