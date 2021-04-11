/*$Id: iceb_vixod.c,v 1.6 2013/08/13 06:10:14 sasa Exp $*/
/*19.07.2013    17.12.2010      Белых А.И.     iceb_vixod.c
Проверка даты на выходной день
Возвращием 0 - если дата выходная
	   1 - нет
*/
#include	"iceb_libbuh.h"

extern SQL_baza bd;




int iceb_vixod(short d,short m,short g,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select data from Zarsvd where data='%04d-%02d-%02d'",g,m,d);
if(iceb_sql_readkey(strsql,wpredok) == 1)
 return(0);

return(1);


}
