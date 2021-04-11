/*$Id:$*/
/*23.10.2019	22.10.2019	Белых А.И.	zvb_end_lstw.cpp
Коцовка распечатки перечисления на карт счета в банк
*/
#include "buhg_g.h"

void zvb_end_lstw(const char *imaf,double sumai,FILE *ff,GtkWidget *wpredok)
{
class iceb_fioruk_rk ruk;
class iceb_fioruk_rk glbuh;

iceb_fioruk(1,&ruk,wpredok);
iceb_fioruk(2,&glbuh,wpredok);

fprintf(ff,"ICEB_LST_END\n");
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(96,"Разом"),"Разом",sumai);


fprintf(ff,"\n\
%-*s_____________________ %s\n\n\
%-*s_____________________ %s\n",
iceb_u_kolbait(20,gettext("Руководитель")),
gettext("Руководитель"),
ruk.fio.ravno(),
iceb_u_kolbait(20,gettext("Главный бухгалтер")),
gettext("Главный бухгалтер"),
glbuh.fio.ravno());

iceb_podpis(ff,wpredok);
fclose(ff);

class iceb_rnl_c rh;
iceb_ustpeh(imaf,3,&rh.orient,wpredok);
iceb_rnl(imaf,&rh,&zvb_head_lstw,wpredok);

}
