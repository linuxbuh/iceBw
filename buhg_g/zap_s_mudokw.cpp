/*$Id: zap_s_mudokw.c,v 1.15 2014/07/31 07:08:27 sasa Exp $*/
/*13.07.2015	15.02.2005	Белых А.И.	zap_s_mudok.c
Запись шапки документа материального учёта
Возвращаем 0-если записали
           1-если не записали
*/
#include "buhg_g.h"

extern SQL_baza	bd;

int zap_s_mudokw(int tipz,
short dd,short md,short gd,
int sklad,
const char *kontr,
const char *nomdok,
const char *nomnalnak,
const char *kodop,
int podt,
int prov,
const char *nomvstdok,
short dpnn,short mpnn,short gpnn,
int metka_opl,
const char *k00,
float pnds, /*процент НДС*/
GtkWidget *wpredok)
{
char strsql[512];
time_t tmm;

//float pnds=iceb_pnds(dd,md,gd,wpredok);

time(&tmm);

sprintf(strsql,"insert into Dokummat \
values (%d,'%04d-%02d-%02d',%d,'%s','%s','%s','%s',%d,%d,%d,%ld,'%s','%04d-%d-%d',%.2f,%d,'%s')",
tipz,gd,md,dd,sklad,kontr,nomdok,nomnalnak,kodop,podt,prov,iceb_getuid(wpredok),tmm,
nomvstdok,gpnn,mpnn,dpnn,pnds,metka_opl,k00);
//printf("zap_s_mudokw=%s\n",strsql);

if(sql_zap(&bd,strsql) != 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,wpredok);
  return(1);
 }

return(0);
}
