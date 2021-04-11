/*$Id: zarkvrt_zapvtablw.c,v 1.2 2013/08/13 06:10:03 sasa Exp $*/
/*17.05.2018	30.10.2012	Белых А.И.	zarkvrt_zapvtablw.c
*/

/**********************************************/
/*Запись во временную таблицу*/
/****************************************/

#include "buhg_g.h"

void zarkvrt_zapvtablw(const char *imatmptab,
const char *inn,
int kod_doh,
const char *kod_lgoti,
double dohod_nah,
double dohod_vip,
double podoh_nah,
double podoh_vip,
const char *datanr,
const char *datakr,
const char *fio,       /*фамилия или наименование контрагента*/
const char *tab_kontr, /*табелыный номeр или код контрагента*/
FILE *ff_prot,
GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select inn from %s where inn='%s' and pd=%d and kl='%s'",imatmptab,inn,kod_doh,kod_lgoti);

if(iceb_sql_readkey(strsql,wpredok) == 0) /*нет такой записи*/
 {
   sprintf(strsql,"insert into %s values('%s',%d,%.2f,%.2f,%.2f,%.2f,'%s','%s','%s','%s','%s')",
   imatmptab,
   inn,
   kod_doh,
   dohod_nah,
   dohod_vip,
   podoh_nah,
   podoh_vip,
   datanr,
   datakr,
   kod_lgoti,
   fio,
   tab_kontr);
 }
else /*запись существует*/
 {
   sprintf(strsql,"update %s set \
ndoh=ndoh+%.2f,\
vdoh=vdoh+%.2f,\
npod=npod+%.2f,\
vpod=vpod+%.2f,\
kl='%s' \
where inn='%s' and pd=%d and kl='%s'",
   imatmptab,
   dohod_nah,
   dohod_vip,
   podoh_nah,
   podoh_vip,
   kod_lgoti,
   inn,kod_doh,kod_lgoti);
 }

if(ff_prot != NULL)
  fprintf(ff_prot,"%s-%s\n",__FUNCTION__,strsql);
     
iceb_sql_zapis(strsql,1,0,wpredok);


}
