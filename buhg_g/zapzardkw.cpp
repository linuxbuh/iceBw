/*$Id: zapzardkw.c,v 1.8 2013/05/17 14:58:30 sasa Exp $*/
/*24.11.2016	31.05.2006	Белых А.И.	zapzardkw.c
Запись шапки документа для подсистемы "Зароботная плата".
*/
#include "buhg_g.h"


int zapzardkw(short dd,short md,short gd,
const char *nomdok,
int podr,
const char *koment,
short prn,
const char *datsd,
GtkWidget *wpredok)
{
char strsql[512];
time_t vrem;
time(&vrem);
class iceb_u_str koment1(koment);
short dsd=0,msd=0,gsd=0;
iceb_u_rsdat(&dsd,&msd,&gsd,datsd,1);

sprintf(strsql,"insert into Zardok values (%d,'%04d-%02d-%02d','%s',%d,'%s',%d,%ld,%d,'%s')",
gsd,gd,md,dd,nomdok,podr,koment1.ravno_filtr(),iceb_getuid(wpredok),vrem,prn,iceb_u_datzap(datsd));

return(iceb_sql_zapis(strsql,0,0,wpredok));

}

