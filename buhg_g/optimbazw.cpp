/*$Id: optimbazw.c,v 1.8 2014/05/07 10:58:06 sasa Exp $*/
/*06.11.2012	06.11.2012	Белых А.И.	optimbazw.c
Оптимизация таблиц базы данных
*/
#include "buhg_g.h"
int optimbazw_r(GtkWidget *wpredok);

extern SQL_baza bd;

void optimbazw(int argc,char **argv)
{
iceb_start(argc,argv);


iceb_optimbazw(iceb_get_namebase(),NULL);
sql_closebaz(&bd);
}
