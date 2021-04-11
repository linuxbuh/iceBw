/*$Id: iceb_u_poltekdat.c,v 1.4 2013/05/17 14:58:43 sasa Exp $*/
/*01.02.2015	16.07.2004	Белых А.И.	iceb_u_poltekdat.c
Получение текущего числа
*/
#include <time.h>
#include <stdio.h>
void iceb_u_poltekdat(short *dt,short *mt,short *gt)
{
time_t vrem;
time(&vrem);
struct tm *bf=localtime(&vrem);

*dt=bf->tm_mday;
*mt=bf->tm_mon+1;
*gt=bf->tm_year+1900;

}
/*************************************/
const char *iceb_u_poltekdat()
{
static char str[64];
time_t vrem;
time(&vrem);
struct tm *bf=localtime(&vrem);
sprintf(str,"%02d.%02d.%d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
return(str);
} 