/*$ld:$*/
/*20.11.2019	22.10.2019	Белых А.И.	zvb_start_lstw.cpp
Шапка распечатки ведомости перечисления на карт счета
*/
#include "buhg_g.h"

void zvb_start_lstw(class zvb_menu_lst_r *rm,FILE *ff,GtkWidget *wpredok)
{
time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);


fprintf(ff,"\
              ЗВЕДЕНА ВІДОМІСТЬ,\n\
          працівників %s \n\
          по заробітній платі за %02d.%d р.,\n\
          перерахований за платіжним дорученням N%-2s від %02d.%02d.%d р.\n",
iceb_get_pnk("00",0,wpredok),
rm->mz,rm->gz,
rm->nomer_pp.ravno(),
rm->d_pp,rm->m_pp,rm->g_pp);

fprintf(ff,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

}
