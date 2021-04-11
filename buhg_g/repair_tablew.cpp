/*$Id: repair_tablew.c,v 1.2 2013/08/13 06:09:51 sasa Exp $*/
/*21.11.2012	13.11.2012	Белых А.И.	repair_tablew.c
Оптимизация таблиц базы данных
*/
#include "buhg_g.h"
int repair_tablew_r(class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,const char *namebase,GtkWidget *wpredok);

void repair_tablew(const char *namebase,GtkWidget *wpredok)
{

class iceb_u_spisok imaf;
class iceb_u_spisok naimf;


if(repair_tablew_r(&imaf,&naimf,namebase,wpredok) == 0)
 iceb_rabfil(&imaf,&naimf,wpredok);


}
