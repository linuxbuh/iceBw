/*$Id: rnnrd5w.c,v 1.29 2014/07/31 07:08:26 sasa Exp $*/
/*19.01.2015	19.01.2015	Белых А.И.	rnnrd5w.c
Распечатка додатка 5
*/
#include "buhg_g.h"
#include "rnn_d5w.h"


int rnnrd5w_5(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,char *imaf_svod_naim,char *imaf_xml_dot,char *imaf_svod_dot,double proc_dot,class rnn_d5 *dod5,GtkWidget *wpredok);
int rnnrd5w_6(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,char *imaf_svod_naim,char *imaf_xml_dot,char *imaf_svod_dot,double proc_dot,class rnn_d5 *dod5,GtkWidget *wpredok);

int rnnrd5w(short dn,short mn,short gn,short dk,short mk,short gk,
char *imaf_xml,char *imaf_svod_naim,
char *imaf_xml_dot,char *imaf_svod_dot, /*Дополнительные отчёты*/
double proc_dot, /*Процент дотации*/
class rnn_d5 *dod5,
GtkWidget *wpredok)
{
if(iceb_u_sravmydat(dn,mn,gn,1,12,2014) < 0)
 return(rnnrd5w_5(dn,mn,gn,dk,mk,gk,imaf_xml,imaf_svod_naim,imaf_xml_dot,imaf_svod_dot,proc_dot,dod5,wpredok));
else
 return(rnnrd5w_6(dn,mn,gn,dk,mk,gk,imaf_xml,imaf_svod_naim,imaf_xml_dot,imaf_svod_dot,proc_dot,dod5,wpredok));


}
