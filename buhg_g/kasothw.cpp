/*$Id: kasothw.c,v 1.13 2014/06/30 06:35:55 sasa Exp $*/
/*17.07.2013	10.03.2006	Белых А.И.	kasothw.c
Отчёты в "Учёте кассовых ордеров
*/
#include "buhg_g.h"
#include "kasothw.h"

int kasothw_m(class kasothw_rr *rek_ras,GtkWidget *wpredok);
int kasothw_r(class kasothw_rr *datark,class spis_oth *oth,GtkWidget *wpredok);
int kasothw_r1(class kasothw_rr *datark,class spis_oth *oth,GtkWidget *wpredok);




void kasothw(int metka)
{
static class kasothw_rr data;

if(metka == 0)
 data.naim_oth.new_plus(gettext("Распечатка списка документов"));
if(metka == 1)
 data.naim_oth.new_plus(gettext("Распечатать свод по операциям"));

if(kasothw_m(&data,NULL) != 0)
 return;

class spis_oth oth;

if(metka == 0)
  if(kasothw_r(&data,&oth,NULL) != 0)
   return;

if(metka == 1)
  if(kasothw_r1(&data,&oth,NULL) != 0)
   return;

iceb_rabfil(&oth,NULL);

}
