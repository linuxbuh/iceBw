/*$Id: saldozrw.c,v 1.7 2013/05/17 14:58:27 sasa Exp $*/
/*12.07.2015	26.02.2009	Белых А.И.	saldozrw.c
Перенос сальдо для подсистемы "Заработная плата"
*/
#include "buhg_g.h"
#include "saldo.h"

int saldozrw_r(short godn,short godk,GtkWidget *wpredok);

void saldozrw()
{
class saldo_m_rek data;

if(saldo_m(&data,NULL) != 0)
 return;

if(iceb_pbpds(1,data.datak.ravno_atoi(),NULL) != 0)
 return;

saldozrw_r(data.datan.ravno_atoi(),data.datak.ravno_atoi(),NULL);
 
}
