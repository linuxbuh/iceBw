/*$Id: iceb_getnkontr.c,v 1.1 2013/08/13 06:25:27 sasa Exp $*/
/*01.10.2010	01.10.2010	Белых А.И.	iceb_getnkontr.c
Получение свободного номера контрагента
*/
#include "iceb_libbuh.h"

int iceb_getnkontr(int nom_start,GtkWidget *wpredok)
{
char strsql[112];
if(nom_start <= 0)
 nom_start=1;
for(int nom=nom_start; ; nom++)
 {
  sprintf(strsql,"select kodkon from Kontragent where kodkon='%d'",nom);
  if(iceb_sql_readkey(strsql,wpredok) == 0)
   return(nom);
 }
return(0);

}