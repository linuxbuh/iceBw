/*$Id:$*/
/*05.04.2016	05.04.2016	Белых А.И.	iceb_get_kodstavki.c
Получение кода ставки для налоговой накладной
*/
#include <string.h>

const char *iceb_get_kodstavki(int lnds)
{
static char kodstavki[32];
memset(kodstavki,'\0',sizeof(kodstavki));

if(lnds == 0)
 strcpy(kodstavki,"20");
if(lnds == 1)
 strcpy(kodstavki,"901");
if(lnds == 2)
 strcpy(kodstavki,"902");
if(lnds == 3)
 strcpy(kodstavki,"903");
if(lnds == 4)
 strcpy(kodstavki,"7");
  
return(kodstavki);
}
