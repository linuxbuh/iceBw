/*$Id: rasttnw.c,v 1.2 2014/01/31 12:13:00 sasa Exp $*/
/*30.01.2014	28.09.2013	Белых А.И.	rasttnw.c
Распечатка товарно-транспортной накладной
*/
#include "buhg_g.h"
//#include "rasttnw.h"
 

int rasttn1w(short dd,short md,short gd,const char *nomdok,int sklad,class spis_oth *oth,GtkWidget *wpredok);
int rasttn2w(short dd,short md,short gd,const char *nomdok,int sklad,class spis_oth *oth,GtkWidget *wpredok);

int rasttnw(short dd,short md,short gd,const char *nomdok,int sklad,class spis_oth *oth,GtkWidget *wpredok)
{
int voz=0;
if(iceb_u_sravmydat(dd,md,gd,14,1,2014) < 0)
 voz=rasttn1w(dd,md,gd,nomdok,sklad,oth,wpredok);
else
 voz=rasttn2w(dd,md,gd,nomdok,sklad,oth,wpredok);
 


return(voz);

}
