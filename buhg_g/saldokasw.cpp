/*$Id: saldokasw.c,v 1.7 2013/08/13 06:09:52 sasa Exp $*/
/*12.07.2015	27.02.2009	Белых А.И.	saldokasw.c
Перенос сальдо для подсистемы "Материальный учёт"
*/
#include "buhg_g.h"

int saldokasw_r(short godp,GtkWidget *wpredok);
int saldokasw_m(class iceb_u_str *god,GtkWidget *wpredok);


void saldokasw()
{
char strsql[1024];
class iceb_u_str gods;
short dt,mt,gt;

if(iceb_prn(NULL) != 0) /*проверка возможности корректировки оператором настроечных файлов*/
 return;

iceb_u_poltekdat(&dt,&mt,&gt);

sprintf(strsql,"%d",gt);
gods.plus(strsql);

naz:;

if(saldokasw_m(&gods,NULL) != 0)
 return;

if(iceb_pbpds(1,gods.ravno_atoi(),NULL) != 0)
 goto naz;
   
saldokasw_r(gods.ravno_atoi(),NULL);

}
