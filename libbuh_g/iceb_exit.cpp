/*$Id: iceb_exit.c,v 1.6 2013/05/17 14:58:34 sasa Exp $*/
/*07.05.2005	14.12.2004	Белых А.И.	iceb_exit.c
Завершение работы программы
*/
#include "iceb_libbuh.h"

extern SQL_baza	bd;

int iceb_exit(int metka)
{

sql_closebaz(&bd);
    
exit(metka);

}
