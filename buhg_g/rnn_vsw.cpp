/*$Id: rnn_vsw.c,v 1.3 2013/08/13 06:09:51 sasa Exp $*/
/*19.02.2011	19.02.2011	Белых А.И.	rnn_vsw.c
Выбор сортировки
*/
#include "buhg_g.h"

int rnn_vsw()
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите сортировку для выданных накладных"));

punkt_m.plus(gettext("Сортировко по номеру налоговых накладных"));//0
punkt_m.plus(gettext("Сортировка по дневному номеру записи"));//1


int nomer=0;
return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL));

}
