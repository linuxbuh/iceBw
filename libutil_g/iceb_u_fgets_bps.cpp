/*$Id: iceb_u_fgets_bps.c,v 1.1 2014/06/30 06:35:58 sasa Exp $*/
/*26.08.2004	26.08.2004	Белых А.И.	iceb_u_fgets.c
Чтение строки из файла с удалением перевода строки
*/
#include <stdio.h>
#include <string.h>

int iceb_u_fgets_bps(char *stroka,int razmer,FILE *ff)
{
if(fgets(stroka,razmer,ff) == NULL)
 return(0);
int dlinna=strlen(stroka);
if(stroka[dlinna-1] == '\n')
  stroka[dlinna-1]='\0';
return(1); 
}
