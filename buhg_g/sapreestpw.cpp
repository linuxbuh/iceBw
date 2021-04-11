/*$Id: sapreestpw.c,v 1.8 2013/08/13 06:09:52 sasa Exp $*/
/*12.07.2015	04.10.2006	Белых А.И.	sapreestpw.c
Шапка реестра заработной платы по подразделениям
*/

#include "buhg_g.h"

void sapreestpw(short mr,short gr,
int metka, //0-не бюджет 1-бюджет
FILE *ffpodr)
{
fprintf(ffpodr,"%s\n\n",iceb_get_pnk("00",0,NULL));
if(metka == 1)
  fprintf(ffpodr,"%s.",gettext("Бюджет"));

fprintf(ffpodr,"%s %02d.%d%s\n",
gettext("Реестр платежных ведомостей за"),mr,gr,
gettext("г."));

fprintf(ffpodr,"\
--------------------------------------------------------\n");
fprintf(ffpodr,gettext("\
Код|     Наименование подразделения         |  Сумма   |\n"));
fprintf(ffpodr,"\
--------------------------------------------------------\n");
}
