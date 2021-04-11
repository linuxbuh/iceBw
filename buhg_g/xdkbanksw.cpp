/*$Id: xdkbanksw.c,v 1.1 2013/08/13 06:23:37 sasa Exp $*/
/*18.07.2013	18.07.2013	Белых А.И.	xdkbanksw.c
Выполнение проводок для платёжного поручения - перечисление средсв на картсчета работников
*/
#include "buhg_g.h"
#include "xdkbanksw.h"

int xdkbanksw_m(class xdkbanksw_rr *rek_ras,GtkWidget *wpredok);
int xdkbanksw_r(class xdkbanksw_rr *datark,GtkWidget *wpredok);




void xdkbanksw(const char *tabl,short dd,short md,short gd,const char *nomdok,const char *kodop,const char *kodor,GtkWidget *wpredok)
{
static class xdkbanksw_rr data;

data.nomdok.new_plus(nomdok);
data.dd=dd;
data.md=md;
data.gd=gd;
data.kodop.new_plus(kodop);
data.kodop.new_plus(kodor);
data.tabl.new_plus(tabl);
if(xdkbanksw_m(&data,wpredok) != 0)
 return;


xdkbanksw_r(&data,wpredok);

}
