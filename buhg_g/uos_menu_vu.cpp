/*$Id:$*/
/*23.11.2016	23.11.2016	Белых А.И.	uos_menu_vu.c
Выбор какой учёт нужен
*/
#include "buhg_g.h"


int uos_menu_vu(GtkWidget *wpredok)
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Выберите нужное"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Налоговый учёт"));//0
punkt_m.plus(gettext("Бухгалтерский учёт"));//1



return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok));
}
