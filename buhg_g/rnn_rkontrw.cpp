/*$Id: rnn_rkontrw.c,v 1.3 2013/08/13 06:09:51 sasa Exp $*/
/*20.01.2012	20.01.2012	Белых А.И.	rnn_rkontrw.c
Распечтака отчёта по контрагенту - Реестр налоговых накладных
*/
#include "buhg_g.h"
#include "rnn_rkontrw.h"

int rnn_rkontrw_m(class rnn_rkontrw_rr *rek_ras,GtkWidget *wpredok);
int rnn_rkontrw_r(class rnn_rkontrw_rr *datark,GtkWidget *wpredok);




void rnn_rkontrw()
{
static class rnn_rkontrw_rr data;
data.imaf.free_class();
data.naimf.free_class();

data.naim_oth.new_plus(gettext("Распечатка записей по контрагенту"));

if(rnn_rkontrw_m(&data,NULL) != 0)
 return;

if(rnn_rkontrw_r(&data,NULL) != 0)
 return;


iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
