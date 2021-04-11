/*$Id: doocsumw.c,v 1.9 2013/08/13 06:09:33 sasa Exp $*/
/*05.09.2012	01.05.2005	Белых А.И.	doocsumw.c
Расчёт сумм выполненых дооценок
*/


#include "buhg_g.h"
#include "doocsumw.h"

int doocsumw_m(class doocsumw_rr *rek_ras);
int doocsumw_r(class doocsumw_rr *datark,GtkWidget *wpredok);

iceb_u_str doocsumw_rr::datan;
iceb_u_str doocsumw_rr::datak;
iceb_u_str doocsumw_rr::sklad;
iceb_u_str doocsumw_rr::kodop;

void doocsumw()
{
class doocsumw_rr data;

if(doocsumw_m(&data) != 0)
 return;

if(doocsumw_r(&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
