/*$Id:$*/
/*26.02.2017	26.02.2017	Белых А.И.	musspiw.c
Материальный отчёт
*/
#include "buhg_g.h"
#include "musspiw.h"

int   musspiw_m(class musspiw_rr *rek_ras);
int musspiw_r(class spis_oth *oth,class musspiw_rr *datark,GtkWidget *wpredok);




void musspiw()
{
static class musspiw_rr data;

class spis_oth oth;

if(musspiw_m(&data) != 0)
 return;

if(musspiw_r(&oth,&data,NULL) != 0)
   return;


iceb_rabfil(&oth,NULL);

}
