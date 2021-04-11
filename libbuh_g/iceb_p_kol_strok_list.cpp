/*$Id: iceb_p_kol_strok_list.c,v 1.7 2013/08/13 06:10:12 sasa Exp $*/
/*29.12.2011	14.11.2004	Белых А.И.	iceb_p_kol_strok_list.c
Получить количество строк на стандартном листе
*/
#include "iceb_libbuh.h"

int  iceb_p_kol_strok_list(GtkWidget *wpredok)
{

int kol_strok=64;
char bros[512];

if(iceb_poldan("Количество строк на стандартном листе для системы с графическим интерфейсом",bros,"nastsys.alx",wpredok) == 0)
  kol_strok=atoi(bros);

return(kol_strok);

}
