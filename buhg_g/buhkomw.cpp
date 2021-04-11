/*$Id: buhkomw.c,v 1.8 2013/08/13 06:09:32 sasa Exp $*/
/*07.06.2005	07.06.2005	Белых А.И.	buhkomw.c
Отчёт по комментариям в проводках главной книги
*/
#include "buhg_g.h"
#include "buhkomw.h"

int buhkomw_m(class buhkomw_rr *rek_ras);
int buhkomw_r(class buhkomw_rr *datark,GtkWidget *wpredok);


void buhkomw()
{
static class buhkomw_rr data;

if(buhkomw_m(&data) != 0)
 return;

if(buhkomw_r(&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
