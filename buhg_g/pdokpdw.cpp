/*$Id: pdokpdw.c,v 1.9 2013/08/13 06:09:48 sasa Exp $*/
/*12.07.2015	12.01.2010	Белых А.И.	pdokpdw.c
Просмотр платёжных поручений
*/
#include <errno.h>
#include "buhg_g.h"
#include "dok4w.h"

extern SQL_baza bd;

int pdokpdw(const char *metka,
const char *datadok,
const char *nomdok,
GtkWidget *wpredok)
{
short dd,md,gd;
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[512];
char tablica[24];
FILE *ff;
char imaf[56];
class iceb_u_str naim("");

iceb_u_rsdat(&dd,&md,&gd,datadok,1);

strcpy(tablica,"Pltp");

if(iceb_u_SRAV(ICEB_MP_PTRE,metka,0) == 0)
 {
  strcpy(tablica,"Pltt");
 }

sprintf(strsql,"select * from %s where datd='%04d-%02d-%02d' and nomd='%s'",tablica,gd,md,dd,nomdok);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s\n%d.%d.%d %s %s",gettext("Не найден документ!"),
  dd,md,gd,nomdok,metka);
  iceb_menu_soob(strsql,wpredok);
  return(1);

 }

sprintf(imaf,"pdokpd%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }
if(iceb_u_SRAV(ICEB_MP_PPOR,metka,0) == 0)
 fprintf(ff,"%s\n",gettext("Платёжное поручение"));
if(iceb_u_SRAV(ICEB_MP_PTRE,metka,0) == 0)
 fprintf(ff,"%s\n",gettext("Платёжное требование"));

fprintf(ff,"%s:%d.%d.%d %s:%s\n",
gettext("Дата документа"),dd,md,gd,
gettext("Номер документа"),nomdok);

fprintf(ff,"%s:%s\n",gettext("Записал"),iceb_kszap(row[12],wpredok));
fprintf(ff,"%s:%s\n",gettext("Дата записи"),iceb_u_vremzap(row[13]));


naim.new_plus("");
if(iceb_u_SRAV(ICEB_MP_PPOR,metka,0) == 0)
  sprintf(strsql,"select naik from Opltp where kod='%s'",row[2]);
if(iceb_u_SRAV(ICEB_MP_PTRE,metka,0) == 0)
  sprintf(strsql,"select naik from Opltt where kod='%s'",row[2]);

if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Код операции"),row[2],naim.ravno());

class iceb_u_str kontr;
class iceb_u_str naim_kontr;
iceb_u_polen(row[8],&kontr,1,'#');
iceb_u_polen(row[8],&naim_kontr,2,'#');

fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),kontr.ravno(),naim_kontr.ravno());

fprintf(ff,"%s:%s\n",gettext("Сумма платежа"),row[3]);

fprintf(ff,"\n%s\n-------------------------------------------------------\n",
gettext("Назначение платежа"));

sprintf(strsql,"select zapis from %sz where datd='%04d-%02d-%02d' and \
nomd='%s' and mz=0 order by nz asc",tablica,gd,md,dd,nomdok);
/*printw("\n%s",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(4);
 }

short i=0;

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  i++;
  fprintf(ff,"%s\n",row[0]);
 }
iceb_podpis(ff,wpredok);
fclose(ff);

iceb_prosf(imaf,wpredok);
sleep(1);
unlink(imaf);


return(0);
}
