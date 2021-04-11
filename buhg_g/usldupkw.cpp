/*$Id: usldupkw.c,v 1.5 2013/08/13 06:09:53 sasa Exp $*/
/*08.02.2015	08.02.2015	Белых А.И.	usldupkw.c
Отчёт списания/получения услуг по контрагентам
*/
#include "buhg_g.h"
#include "usldupkw.h"

int usldupkw_m(class usldupkw_rr *rek_poi,GtkWidget *wpredok);
int usldupkw_r(class usldupkw_rr *datark,GtkWidget *wpredok);

void usldupkw()
{

static class usldupkw_rr data;

if(usldupkw_m(&data,NULL) != 0)
 return;

if(usldupkw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,NULL);


}

