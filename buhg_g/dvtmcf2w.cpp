/*$Id: dvtmcf2w.c,v 1.8 2013/08/13 06:09:34 sasa Exp $*/
/*19.06.2010	22.11.2004	Белых А.И.	dvtmcf2w.c
Расчёт движения товарно-материальных ценностей форма 2
*/
#include "buhg_g.h"
#include "dvtmcf2.h"

int   dvtmcf2_m(class dvtmcf2_rr *rek_ras);
int dvtmcf2_r(class dvtmcf2_rr *datark,GtkWidget *wpredok);

class iceb_u_str dvtmcf2_rr::datan;
class iceb_u_str dvtmcf2_rr::datak;
class iceb_u_str dvtmcf2_rr::shet;
class iceb_u_str dvtmcf2_rr::sklad;
class iceb_u_str dvtmcf2_rr::grupa;
class iceb_u_str dvtmcf2_rr::kodmat;
class iceb_u_str dvtmcf2_rr::nds;
class iceb_u_str dvtmcf2_rr::kontr;
class iceb_u_str dvtmcf2_rr::kodop;
class iceb_u_str dvtmcf2_rr::nomdok;
int dvtmcf2_rr::pr;
int        dvtmcf2_rr::metka_sort;
int        dvtmcf2_rr::metka_cena;

void dvtmcf2w()
{
class dvtmcf2_rr data;
if(data.datan.getdlinna() == 0)
 data.clear_data();
 
if(dvtmcf2_m(&data) != 0)
 return;

if(dvtmcf2_r(&data,NULL) == 0)
 iceb_rabfil(&data.oth,NULL);

}
