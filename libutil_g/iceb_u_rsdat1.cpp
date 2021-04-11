/*$Id: iceb_u_rsdat1.c,v 1.6 2013/08/13 06:10:17 sasa Exp $*/
/*29.10.2012    11.11.1993      Белых А.И.      iceb_u_rsdat1.c
Подпрограмма расшифровки символьной строки с датой
Если вернули 0 - дата введена правильно
*/
#include	"iceb_util.h"
int iceb_u_rsdat1(short *m,short *g,const char *str)
{
char data[64];
memset(data,'\0',sizeof(data));
strncpy(data,str,sizeof(data)-1);
return(iceb_u_rsdat1(m,g,data));
}

/************************************/

int iceb_u_rsdat1(short *m,short *g,char *str)
{
int		i;
char *stroka=new char[strlen(str)+1];
strcpy(stroka,str);

for(i=0; stroka[i] != '\0'; i++)
  if(stroka[i] == ',')
    stroka[i]='.';

iceb_u_polen(stroka,m,1,'.');

iceb_u_polen(stroka,g,2,'.');

delete [] stroka;

if(*m > 12 || *m <= 0)
 return(2);

if(*g < 1980 || *g > 3000)
 return(3);

return(0);
}
