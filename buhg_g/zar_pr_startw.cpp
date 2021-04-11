/*$Id: zar_pr_startw.c,v 1.3 2013/08/13 06:10:01 sasa Exp $*/
/*13.07.2015	26.11.2011	Белых А.И.	zar_start_prw.c
Начало создания протокола хода расчёта зарплаты
*/
#include <errno.h>
#include "buhg_g.h"


int zar_pr_nomzap=0; /*нумерация строк в протоколе*/
short zar_pr_mes=0;
short zar_pr_god=0;
int zar_pr_tabn=0;
int zar_pr_tipz=-1;
FILE *zar_pr_ffp=NULL;

int zar_pr_startw(const char *imaf_prot,GtkWidget *wpredok)
{

if((zar_pr_ffp = fopen(imaf_prot,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prot,"",errno,wpredok);
  return(1);
 }
return(0);
}
/**************************************************/

void zar_pr_startw(int tipz,int tabn,short mr,short gr,GtkWidget *wpredok)
{
char strsql[1024];

zar_pr_nomzap=0;
zar_pr_tabn=tabn;
zar_pr_tipz=tipz;
zar_pr_mes=mr;
zar_pr_god=gr;

sprintf(strsql,"DELETE FROM Zarpr where tn=%d and tz=%d and dr='%04d-%02d-01'",tabn,tipz,gr,mr);
iceb_sql_zapis(strsql,1,0,wpredok);

if(tipz == 1)
 sprintf(strsql,"\n\n%s\nПротокол хода расчёта начислений\n",iceb_get_pnk("00",0,wpredok));
if(tipz == 2)
 sprintf(strsql,"\n\n%s\nПротокол хода расчёта удержаний\n",iceb_get_pnk("00",0,wpredok));
if(tipz == 3)
 sprintf(strsql,"\n\n%s\nПротокол хода расчёта начислений на фонд оплаты\n",iceb_get_pnk("00",0,wpredok));

zar_pr_insw(strsql,wpredok);

sprintf(strsql,"====================================================================\n");

zar_pr_insw(strsql,wpredok);

sprintf(strsql,"Месяц расчёта %02d.%d г.\n",mr,gr);
zar_pr_insw(strsql,wpredok);

time_t vrem=time(NULL);
struct tm *bf;
bf=localtime(&vrem);

sprintf(strsql,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

zar_pr_insw(strsql,wpredok);

sprintf(strsql,"Оператор:%d %s\n",iceb_getuid(wpredok),iceb_getfioop(wpredok));

zar_pr_insw(strsql,wpredok);

class iceb_u_str fio("");
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 fio.new_plus(row[0]);

sprintf(strsql,"\nТабельный номер %d %s\n",tabn,fio.ravno());

zar_pr_insw(strsql,wpredok);

}
