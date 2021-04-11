/*$Id: plc_gk.c,v 1.6 2014/02/28 05:21:00 sasa Exp $*/
/*14.02.2004	21.01.2004	Белых А.И.	plc_gk.c
Проверка логической целосности базы "Главная книга"
*/
#include "buhg_g.h"

int  m_plc_gk(class iceb_u_str *datan,class iceb_u_str *datak);
int  plc_gk_r(const char *datan,const char *datak);

void  plc_gk(void)
{
class iceb_u_str datan("");
class iceb_u_str datak("");


if(m_plc_gk(&datan,&datak) != 0)
 return;

plc_gk_r(datan.ravno(),datak.ravno());

}
