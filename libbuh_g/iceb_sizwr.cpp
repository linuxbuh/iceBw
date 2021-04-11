/*$Id:$*/
/*22.01.2014	22.01.2014	Белых А.И.
Чтение размеров окна
*/
#include "iceb_libbuh.h"

int iceb_sizwr(const char *name_ok,int *gor,int *ver)
{

class iceb_u_str filname(g_get_home_dir());
filname.plus(G_DIR_SEPARATOR_S,ICEB_KAT_ALX);
filname.plus(G_DIR_SEPARATOR_S,ICEB_IMAF_SIZW);
class iceb_u_str razmer("");

if(iceb_u_poldan(name_ok,&razmer,filname.ravno()) != 0)
 return(3);

iceb_u_polen(razmer.ravno(),gor,1,' ');
iceb_u_polen(razmer.ravno(),ver,2,' ');
//printf("%s-gor=%d ver=%d\n",__FUNCTION__,*gor,*ver);

return(0);
 
}
