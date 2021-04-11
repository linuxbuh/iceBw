/*$ld:$*/
/*02.07.2019	02.07.2019	Белых А.И.	rspvk.cpp
Расчёт сальдо по всем контрагентам
*/

#include "buhg_g.h"
#include "rspvk.h"

class rspvk_rr data;

int rspvk_m(class rspvk_rr *data);
int rspvk_r(class spis_oth *sp,class rspvk_rr *data_rr,GtkWidget *wpredok);

class iceb_u_str rspvk_rr::datan;
class iceb_u_str rspvk_rr::datak;
class iceb_u_str rspvk_rr::shet;
class iceb_u_str rspvk_rr::kodgr;
class iceb_u_str rspvk_rr::kontr;
short rspvk_rr::metka_r;
short rspvk_rr::par;


void rspvk(GtkWidget *wpredok)
{
static class rspvk_rr data;
class spis_oth sp;

if(rspvk_m(&data) != 0)
 return;
 
if(rspvk_r(&sp,&data,wpredok) != 0)
 return;



iceb_rabfil(&sp,wpredok);


}
