/* $Id: udamortw.c,v 1.7 2013/05/17 14:58:28 sasa Exp $ */
/*16.08.2011    04.07.1996      Белых А.И.      udamortw.c
Удаление амортизационных отчислений к документа
*/
#include        "buhg_g.h"


void udamortw(int in,GtkWidget *wpredok)
{
char		strsql[512];

sprintf(strsql,"delete from Uosamor where  innom=%d",in);

iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"delete from Uosamor1 where  innom=%d",in);

iceb_sql_zapis(strsql,1,0,wpredok);

}
