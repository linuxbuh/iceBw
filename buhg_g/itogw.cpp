/*$Id: itogw.c,v 1.13 2013/08/13 06:09:35 sasa Exp $*/
/*22.07.2019	18.02.2003	Белых А.И.	itodw.c
Вывод итога в ведомости на выдачу зарплаты
*/

#include        <errno.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void itogw(FILE *ff,double itogo,GtkWidget *wpredok)
{
class iceb_u_str stroka("");

iceb_u_prnb(itogo,&stroka);

fprintf(ff,"\
------------------------------------------------------------------------------\n\
%*s:%15s\n",iceb_u_kolbait(43,gettext("Итого")),gettext("Итого"),stroka.ravno());

class iceb_fioruk_rk fio_prov;
iceb_fioruk(4,&fio_prov,wpredok);

fprintf(ff,"\n\
            Відомість склав___________________%s\n\
                      (підпис, прізвище, ініціали)\n\
\n\
            Відомість перевірив_______________%s\n\
                      (підпис, прізвище, ініціали)\n",
iceb_getfioop(wpredok),
fio_prov.famil_inic.ravno());

/************************
int kolstr=0;
SQL_str row_alx;
class SQLCURSOR cur_alx;
char strsql[512];
sprintf(strsql,"select str from Alx where fil='zarspv1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarspv1.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }


int nomstr=0;
class iceb_fioruk_rk fio_prov;
iceb_fioruk(4,&fio_prov,wpredok);

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomstr++;
  stroka.new_plus(row_alx[0]);
  switch(nomstr)
   {
    case 2:
     iceb_u_vstav(&stroka,iceb_getfioop(wpredok),47,70,1);
     break;
    case 5:
     iceb_u_vstav(&stroka,fio_prov.famil_inic.ravno(),47,70,1);
     break;
   }
  fprintf(ff,"%s",stroka.ravno());
  
 }
********************/

iceb_podpis(ff,wpredok);

}
