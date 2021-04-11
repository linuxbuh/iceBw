/*$ld:$*/
/*22.10.2019	22.10.2019	Белых А.И.	zvb_head_lstw.cpp
Шапка распечатки ведомости перечисления на карт счета
*/
#include "buhg_g.h"

void zvb_head_lstw(class iceb_rnl_c *rd,int *kolstr,FILE *ff)
{
if(kolstr != NULL)
 *kolstr=*kolstr+4;

fprintf(ff,"\
                                                                                           %s N%d\n",gettext("Лист"),rd->nom_list);

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
N п/п | Т.н. |      Прізвище Імʼя Побатькові          |        N картрахунку        |Реєстрац.н|  Сума     |\n");
/*********
123456 123456 1234567890123456789012345678901234567890 12345678901234567890123456789 1234567890 1234567890 1234567890
*********/
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------\n");
}
