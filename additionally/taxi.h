/*$Id: taxi.h,v 1.20 2013/08/13 06:10:25 sasa Exp $*/
/*30.05.2009	24.09.2003	Белых А.И.	taxi.h
*/

#include <iceb_libbuh.h>
#define   NAME_SYSTEM "taxi"
#define   CONFIG_PATH "/etc/iceB"

double    taxi_saldo(const char *);
double    taxi_saldo(const char *,short);
int   l_taxisk(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_vod(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_taxikz(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);


