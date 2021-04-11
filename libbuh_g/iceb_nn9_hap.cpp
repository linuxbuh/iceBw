/*$Id:$*/
/*22.03.2017	16.03.2016	Белых А.И.	iceb_nn9_hap.c
Распечатка шапки налоговой накладной
*/
#include "iceb_libbuh.h"

void iceb_nn9_hap(int nomstr,int *kolstr,FILE *ff,GtkWidget *wpredok)
{
int kol=iceb_insfil("nn9_hap.alx",ff,wpredok);
if(kolstr != NULL)
 *kolstr+=kol;
}
