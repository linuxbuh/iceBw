/*$Id: iceb_u_vremzap.c,v 1.12 2013/08/13 06:10:17 sasa Exp $*/
/*17.01.2013	19.09.2003	Белых А.И.	iceb_u_vremzap.c
Получение символьной строки "Дата время"
*/
#include   <stdio.h>
#include   <string.h>
#include   <time.h>
#include   <stdlib.h>

const char *iceb_u_vremzap(char *vrem)
{
static char vremzap[64];
struct  tm      *bf;

memset(vremzap,'\0',sizeof(vremzap));
if(vrem == NULL)
 {
  strcpy(vremzap,"NULL");
  return(vremzap);
 } 

time_t      tim=atol(vrem);
memset(vremzap,'\0',sizeof(vremzap));

if(tim == 0)
 return(vremzap);

bf=localtime(&tim);

sprintf(vremzap,"%02d.%02d.%04d %02d:%02d:%02d",
bf->tm_mday,
bf->tm_mon+1,
bf->tm_year+1900,
bf->tm_hour,
bf->tm_min,
bf->tm_sec);

return(vremzap);
}
/****************************************/
/****************************************/
const char *iceb_u_vremzap(time_t vrem)
{
static char vremzap[64];
memset(vremzap,'\0',sizeof(vremzap));
struct  tm      *bf;
time_t      tim=vrem;
memset(vremzap,'\0',sizeof(vremzap));

if(tim == 0)
 return(vremzap);

bf=localtime(&tim);

sprintf(vremzap,"%02d.%02d.%04d %02d:%02d:%02d",
bf->tm_mday,
bf->tm_mon+1,
bf->tm_year+1900,
bf->tm_hour,
bf->tm_min,
bf->tm_sec);

return(vremzap);
}
