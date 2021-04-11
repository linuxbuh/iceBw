/*$Id: iceb_u_fgets.c,v 1.5 2014/06/30 06:35:58 sasa Exp $*/
/*05.02.2016	21.10.2013	Белых А.И.	iceb_tu_fgets.c
Чтерие строки из файла
*/

#include "iceb_util.h"

int iceb_u_fgets(class iceb_u_str *str,FILE *ff)
{
char stroka[1024];
str->new_plus("");
int dlinna=0;
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  dlinna=strlen(stroka);

  if(stroka[dlinna-1] == '\n' && stroka[dlinna-2] == '\r')  /*избавляемся от возврата каретки*/
   {
    stroka[dlinna-1]='\0';
    stroka[dlinna-2]='\n';
    dlinna--;
   }
 
  if(stroka[dlinna-1] == '\n' && stroka[dlinna-2] == '\\')  /*знак продолжения в следующей строке*/
   {
    stroka[dlinna-1]='\0';
    stroka[dlinna-2]='\0';
    str->plus(stroka);
    continue;
   }   

  if(stroka[dlinna-1] == '\n' && stroka[dlinna-2] == ' ' && stroka[dlinna-3] == '\\')  /*знак продолжения в следующей строке*/
   {
    stroka[dlinna-1]='\0';
    stroka[dlinna-2]='\0';
    stroka[dlinna-3]='\0';
    str->plus(stroka);
    continue;
   }   


  str->plus(stroka);
  
  if(stroka[dlinna-1] == '\n') 
   return(0);
 }
if(str->getdlinna() <= 1)
 return(1);
else
 return(0);
}
