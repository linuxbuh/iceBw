/*$Id: krasactw.c,v 1.11 2013/09/26 09:46:49 sasa Exp $*/
/*11.07.2015	20.07.2003	Белых А.И.	krasactw.c
Концовка акта приема - передачи
*/
#include	"buhg_g.h"


void krasactw(const char *shetz,const char *naimshet,
double kolihshet,double sumashet,
double kolihi,double sumai,
FILE *ff)
{
char strsql[512];

sprintf(strsql,"%s %s %s",gettext("Итого по счёту"),shetz,naimshet);
fprintf(ff,"%*s %10.10g %10.2f\n",iceb_u_kolbait(62,strsql),strsql,kolihshet,sumashet);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%*s %10.10g %10.2f\n",iceb_u_kolbait(62,gettext("Общий итог")),gettext("Общий итог"),kolihi,sumai);

fprintf(ff,"\n\n\
%s:\n\
%s:\n",
gettext("Председатель коммиссии"),
gettext("Члены коммисии"));

fprintf(ff,"\n%s:\n\n",gettext("Передал"));
fprintf(ff,"%s:\n\n",gettext("Принял"));

}
