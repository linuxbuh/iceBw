/*$Id: l_kv_start.c,v 1.1 2013/09/26 09:48:37 sasa Exp $*/
/*05.09.2013	05.09.2013	Белых А.И.	l_kv_start.c
Работа со справочником курсов валют
*/
#include "buhg_g.h"

void l_kv(const char *kod_val,GtkWidget *wpredok);
int l_kv_m(class iceb_u_str *kod_val,GtkWidget *wpredok);

void l_kv_start(GtkWidget *wpredok)
{
class iceb_u_str kod_val("");

if(l_kv_m(&kod_val,wpredok) != 0)
 return;

l_kv(kod_val.ravno(),wpredok);

}