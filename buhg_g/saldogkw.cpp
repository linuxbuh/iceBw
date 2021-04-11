/*$Id: saldogkw.c,v 1.8 2013/05/17 14:58:26 sasa Exp $*/
/*12.07.2015	25.02.2009	Белых А.И.	saldogkw.c
Перенос сальдо для подсистемы "Главная книга"
*/
#include "buhg_g.h"
#include "saldo.h"

int saldogk_r(short godn,short godk,GtkWidget *wpredok);


void saldogkw(GtkWidget *wpredok)
{
class saldo_m_rek data;

if(saldo_m(&data,wpredok) != 0)
 return;

if(iceb_pbpds(1,data.datak.ravno_atoi(),wpredok) != 0)
 return;

saldogk_r(data.datan.ravno_atoi(),data.datak.ravno_atoi(),wpredok);

}
