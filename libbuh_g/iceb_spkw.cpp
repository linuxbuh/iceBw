/*$Id: iceb_spkw.c,v 1.10 2013/08/13 06:10:13 sasa Exp $*/
/*28.09.2014	30.05.2007	Белых А.И.	iceb_spkw.c
Вывод в файл справки по конечному сальдо по контрагенту
*/
#include "iceb_libbuh.h"


void iceb_spkw(short dk,short mk,short gk,
const char *shet,
const char *naimsh,
const char *kontr,
const char *naikontr,
double suma,
FILE *ff_spr,
GtkWidget *wpredok)
{
fprintf(ff_spr,"%s\n\n",iceb_get_pnk("00",0,wpredok));

fprintf(ff_spr,"%20s %s\n\n\
%s %s %s\n\
%s %s %s\n\
%s %02d.%02d.%d %s %s %.2f %s\n",
"",
gettext("Справка"),
gettext("На счёте"),
shet,
naimsh,
gettext("по контрагенту"),
kontr,naikontr,
gettext("по состоянию на"),
dk,mk,gk,
gettext("г."),
gettext("числится сумма в размере"),
suma,
gettext("грн."));

class iceb_u_str suma_prop("");
iceb_u_preobr(suma,&suma_prop,0);

fprintf(ff_spr,"%s: %s\n",gettext("Сумма прописью"),suma_prop.ravno());

fprintf(ff_spr,"\n\
%-*s____________________\n\n\
%-*s____________________\n",
iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"));

iceb_podpis(ff_spr,wpredok);
}




