/*$Id: iceb_pvglkni.c,v 1.8 2013/05/17 14:58:36 sasa Exp $*/
/*11.09.2010	17.10.2008	Белых А.И.	iceb_pvglkni.c
Проверка блокировки подсистемы "Главная книга"
Если вернули 0 - не заблокировано
             1 - заблокировано
*/
#include "iceb_libbuh.h"


int iceb_pvglkni(short mes,short god,GtkWidget *wpredok)
{
if(iceb_pblok(mes,god,ICEB_PS_GK,wpredok) != 0)
 {
  char strsql[112];
  sprintf(strsql,gettext("На дату %d.%d г. проводки заблокированы!"),mes,god);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

return(0);

}

int iceb_pvglkni(const char *mesgod,GtkWidget *wpredok)
{
short d=0,m=0,g=0;
iceb_u_rsdat(&d,&m,&g,mesgod,1);
return(iceb_pvglkni(m,g,wpredok));
}
