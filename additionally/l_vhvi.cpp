/*$Id: l_vhvi.c,v 1.5 2013/05/17 14:58:48 sasa Exp $*/
/*16.04.2017	13.12.2005	Белых А.И.	l_vhvi.c
Корневая программа работы с просмотром входов-выходов
*/
#include "i_rest.h"
#include "l_vhvi.h"

int l_vhvi_p(class l_vhvi_rp *datap,GtkWidget *wpredok);
int  l_vhvi_r(class l_vhvi_rp *datap,GtkWidget *wpredok);

void l_vhvi()
{

class l_vhvi_rp data;


for(;;)
 {
  if(l_vhvi_p(&data,NULL) == 0)
   return;


  l_vhvi_r(&data,NULL);
 }
}
