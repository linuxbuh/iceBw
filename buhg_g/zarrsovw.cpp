/*$Id:$*/
/*14.01.2019	03.10.2015	Белых А.И.	zarrsovw.c
Распечатка свода отработанного времени
*/
#include <math.h>
#include "buhg_g.h"
#include "zarrsovw.h"

void zarrsovw(short mn,short gn,
short mk,short gk,
int tabnom,
class iceb_zarrsov *svod_otvr,FILE *ff,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str fio("");

int kolmes=iceb_u_period(1,mn,gn,31,mk,gk,1);

fprintf(ff,"\n%s\n",gettext("Свод отработанного времени"));

sprintf(strsql,"select fio from Kartb where tabn=%d",tabnom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 fio.new_plus(row[0]);
fprintf(ff,"%s:%d %s\n",gettext("Работник"),tabnom,fio.ravno());

class iceb_u_str liniq("--------");
for(int nom=0; nom <= svod_otvr->kod.kolih(); nom++)
 liniq.plus("---------------------");

fprintf(ff,"%s\n",liniq.ravno());
 
fprintf(ff,"\
%-*.*s|",
iceb_u_kolbait(7,gettext("Дата")),
iceb_u_kolbait(7,gettext("Дата")),
gettext("Дата"));
int max_dl_str=0;

for(int nom=0; nom < svod_otvr->kod.kolih(); nom++)
 {
  sprintf(strsql,"select naik from Tabel where kod=%d",svod_otvr->kod.ravno(nom));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   fio.new_plus(row[0]);
  else
   fio.new_plus("");

  sprintf(strsql,"%d %s",svod_otvr->kod.ravno(nom),fio.ravno());

  fprintf(ff,"%-*.*s|",
  iceb_u_kolbait(20,strsql),
  iceb_u_kolbait(20,strsql),
  strsql);

  if(max_dl_str < iceb_u_strlen(strsql))
   max_dl_str=iceb_u_strlen(strsql);
 }

fprintf(ff,"%-*.*s|\n",
iceb_u_kolbait(20,gettext("Итого")),
iceb_u_kolbait(20,gettext("Итого")),
gettext("Итого"));

/*Печать продолжения наименования*/
double kolstrd=0.;
modf(max_dl_str/14,&kolstrd);
int kol_str=kolstrd;
//printw("\n%s-%d %d %d\n",__FUNCTION__,kol_str,max_dl_str,svod_otvr->kod.kolih());
//OSTANOV();

for(int nom_str=0; nom_str < kol_str;nom_str++)
 {
  fprintf(ff,"%7s|","");
  for(int nom=0; nom < svod_otvr->kod.kolih(); nom++)
   {
    sprintf(strsql,"select naik from Tabel where kod=%d",svod_otvr->kod.ravno(nom));
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     fio.new_plus(row[0]);
    else
     fio.new_plus("");

    sprintf(strsql,"%d %s",svod_otvr->kod.ravno(nom),fio.ravno());

    fprintf(ff,"%-*.*s|",
    iceb_u_kolbait(20,iceb_u_adrsimv((nom_str+1)*20,strsql)),
    iceb_u_kolbait(20,iceb_u_adrsimv((nom_str+1)*20,strsql)),
    iceb_u_adrsimv((nom_str+1)*20,strsql));
   }  
  fprintf(ff,"%20s|\n","");
 }
//fprintf(ff,"%20s|\n","");
/*Строка подчёркивания после наименования*/
fprintf(ff,"%7s|","");

for(int nom=0; nom <= svod_otvr->kod.kolih(); nom++)
 {
  fprintf(ff,"--------------------|");
 }
fprintf(ff,"\n");

fprintf(ff,"%7s|","");
for(int nom=0; nom <= svod_otvr->kod.kolih(); nom++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|%-*.*s|",
  iceb_u_kolbait(6,gettext("Рабочие")),
  iceb_u_kolbait(6,gettext("Рабочие")),
  gettext("Рабочие"),
  iceb_u_kolbait(6,gettext("Календарные")),
  iceb_u_kolbait(6,gettext("Календарные")),
  gettext("Календарные"),
  iceb_u_kolbait(6,gettext("Часы")),
  iceb_u_kolbait(6,gettext("Часы")),
  gettext("Часы"));
 }
fprintf(ff,"\n");

fprintf(ff,"%s\n",liniq.ravno());
int nom_vs=0;
short d=1;
short m=mn;
short g=gn;

for(int nom_mes=0; nom_mes < kolmes; nom_mes++)
 {

//  fprintf(ff,"%s|",svod_otvr->mes_god.ravno(nom_mes));
  fprintf(ff,"%02d.%04d|",m,g);

  double itogo_str[3];
  memset(itogo_str,'\0',sizeof(itogo_str));
  for(int nom_kod=0; nom_kod < svod_otvr->kod.kolih(); nom_kod++)
   {
//    sprintf(strsql,"%d|%s",svod_otvr->kod.ravno(nom_kod),svod_otvr->mes_god.ravno(nom_mes));
    sprintf(strsql,"%d|%02d.%04d",svod_otvr->kod.ravno(nom_kod),m,g);

    if((nom_vs=svod_otvr->kod_mes_god.find(strsql)) >= 0)
     {
      fprintf(ff,"%6d|%6d|%6.6g|",svod_otvr->kolrd.ravno(nom_vs),svod_otvr->kolkd.ravno(nom_vs),svod_otvr->kolhs.ravno(nom_vs));
      itogo_str[0]+=svod_otvr->kolrd.ravno(nom_vs);
      itogo_str[1]+=svod_otvr->kolkd.ravno(nom_vs);
      itogo_str[2]+=svod_otvr->kolhs.ravno(nom_vs);
     }
    else
     fprintf(ff,"%6s|%6s|%6s|","","","");
   }
  fprintf(ff,"%6.f|%6.f|%6.6g|\n",itogo_str[0],itogo_str[1],itogo_str[2]);
  iceb_u_dpm(&d,&m,&g,3);
 }

fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*.*s|",
iceb_u_kolbait(7,gettext("Итого")),
iceb_u_kolbait(7,gettext("Итого")),
gettext("Итого"));

m=mn;
g=gn;

for(int nom_kod=0; nom_kod < svod_otvr->kod.kolih(); nom_kod++)
 {
  double itogo_str[3];
  memset(itogo_str,'\0',sizeof(itogo_str));
  for(int nom_mes=0; nom_mes < kolmes; nom_mes++)
   {
//    sprintf(strsql,"%d|%s",svod_otvr->kod.ravno(nom_kod),svod_otvr->mes_god.ravno(nom_mes));
    sprintf(strsql,"%d|%02d.%04d",svod_otvr->kod.ravno(nom_kod),m,g);
    if((nom_vs=svod_otvr->kod_mes_god.find(strsql)) >= 0)
     {
      itogo_str[0]+=svod_otvr->kolrd.ravno(nom_vs);
      itogo_str[1]+=svod_otvr->kolkd.ravno(nom_vs);
      itogo_str[2]+=svod_otvr->kolhs.ravno(nom_vs);
     }
   }  
  fprintf(ff,"%6.f|%6.f|%6.6g|",itogo_str[0],itogo_str[1],itogo_str[2]);
  iceb_u_dpm(&d,&m,&g,3);
  
 }

fprintf(ff,"%6.d|%6.d|%6.6g|\n",svod_otvr->kolrd.suma(),svod_otvr->kolkd.suma(),svod_otvr->kolhs.suma());
//fprintf(ff,"%10.2f|\n",svod_otvr->suma.suma());


}
