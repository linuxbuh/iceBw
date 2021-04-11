/*$Id: check_tablew.c,v 1.2 2013/08/13 06:09:33 sasa Exp $*/
/*06.11.2012	06.11.2012	Белых А.И.	check_tablew.c
Оптимизация таблиц базы данных
*/
#include "buhg_g.h"
int check_tablew_r(class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,const char *namebase,GtkWidget *wpredok);


void check_tablew(const char *namebase,GtkWidget *wpredok)
{

class iceb_u_spisok imaf;
class iceb_u_spisok naimf;


if(check_tablew_r(&imaf,&naimf,namebase,wpredok) == 0)
 iceb_rabfil(&imaf,&naimf,wpredok);


}
