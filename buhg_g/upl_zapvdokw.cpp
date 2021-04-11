/*$Id: upl_zapvdokw.c,v 1.6 2013/05/17 14:58:28 sasa Exp $*/
/*13.07.2015	23.06.2005	Белых А.И.	upl_zapvdokw.c
Запись в путевой лист
*/
#include "buhg_g.h"

int upl_zapvdokw(short dd,short md,short gd,
int kp,
const char *nomd,
int ka,
int kv,
int kodsp,
int tz,
const char *kodtp,
double kolih,
const char *kom,
int kap,
int nz, /*Номер записи шапки документа*/
GtkWidget *wpredok)
{
char strsql[512];
time_t vrem;
time(&vrem);
int nzap=0; /*Номер записи в документе*/
int koh=0;

for(;;)
 {
  sprintf(strsql,"insert into Upldok1 values('%d-%d-%d',%d,'%s',%d,%d,%d,%d,%d,'%s',%.3f,'%s',\
%d,%d,%ld,%d)",
  gd,md,dd,kp,
  nomd,
  nzap,
  ka,
  kv,
  kodsp,
  tz,
  kodtp,
  kolih,
  kom,
  kap,
  iceb_getuid(wpredok),vrem,
  nz);

  //printw("\nupl_zapvdok=%s\n",strsql);
  //OSTANOV();

  if((koh=iceb_sql_zapis(strsql,1,1,wpredok)) != 0)
   {
    if(koh == ER_DUP_ENTRY)
     {
      nzap++;
      continue;
     }
    return(1);
   }
  break;
 }
return(0);
}
