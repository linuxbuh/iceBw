/*$Id: zaruozw.c,v 1.9 2013/08/13 06:10:04 sasa Exp $*/
/*13.07.2015	25.04.2007	Белых А.И.	zaruozw.c
Расчёт удержание однодневного зароботка
*/
#include "buhg_g.h"

extern short kod_ud_dnev_zar; //Код удержания дневного зароботка
extern short *kodnah_nvr_dnev_zar; //Коды начислений не входящие в расчёт удержания дневного зароботка
extern short *kodtab_nvr_dnev_zar; //Коды табеля не входящие в расчёт удержания однодневного зароботка
extern struct ZAR zar;
extern double   okrg;
extern char *shetb; /*Бюджетные счета начислений*/
extern SQL_baza bd;

void zaruozw(int tabnom,short mr,short gr,int podr,const char *uder_only,GtkWidget *wpredok)
{

if(kod_ud_dnev_zar == 0)
 return;
char strsql[1024];

sprintf(strsql,"\n%s-Расчёт удержания однодневного зароботка %d %02d.%d\n\
---------------------------------------------------------\n",__FUNCTION__,kod_ud_dnev_zar,mr,gr); 
zar_pr_insw(strsql,wpredok);

if(iceb_u_proverka(uder_only,kod_ud_dnev_zar,0,0) != 0)
 {
  sprintf(strsql,"%s-Код %d исключён из расчёта\n",__FUNCTION__,kod_ud_dnev_zar);
  zar_pr_insw(strsql,wpredok);
  return;
 }

SQL_str row;
class SQLCURSOR cur;

class iceb_u_str shet_uder("");
class iceb_u_str shet_uderb("");

sprintf(strsql,"select shet,shetb from Uder where kod=%d",kod_ud_dnev_zar);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  shet_uder.new_plus(row[0]);
  shet_uderb.new_plus(row[1]);
 }
else
 {
  sprintf(strsql,"%s-Не найден код удержания %d в списке удержаний !",__FUNCTION__,kod_ud_dnev_zar);
  zar_pr_insw(strsql,wpredok);
  return;
 }

if(shet_uder.getdlinna() <= 1)
 {
  sprintf(strsql,"Не введён счёт в справочнике удержаний для удержания %d\n",kod_ud_dnev_zar);
  zar_pr_insw(strsql,wpredok);
  return;
 }
double suma_nah=0.;
double suma_nahb=0.;

sprintf(strsql,"select knah,suma from Zarp where \
datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and prn='1' and mesn=%d and \
suma <> 0. order by prn,knah asc",gr,mr,gr,mr,tabnom,mr);

if(cur.make_cursor(&bd,strsql) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {

  if(provkodw(kodnah_nvr_dnev_zar,row[0]) >= 0)
   continue;
  if(iceb_u_proverka(shetb,row[2],0,1) == 0)
   suma_nahb+=atof(row[1]);

  suma_nah+=atof(row[1]);

 }

sprintf(strsql,"%s-Сумма взятая в расчёт=%.2f/%.2f\n",__FUNCTION__,suma_nah,suma_nahb);
zar_pr_insw(strsql,wpredok);

if(suma_nah <= 0.)
  return;

sprintf(strsql,"select kodt,dnei from Ztab where god=%d and mes=%d and tabn=%d",gr,mr,tabnom);
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
int kolih_dnei=0;
while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(kodtab_nvr_dnev_zar,row[0]) >= 0)
   continue;
  kolih_dnei+=atoi(row[1]);
 }

sprintf(strsql,"%s-Количество дней взятое в расчёт=%d\n",__FUNCTION__,kolih_dnei);
zar_pr_insw(strsql,wpredok);

if(kolih_dnei == 0)
 return;

double suma_udb=0.;

if(suma_nahb != 0.)
 {
  suma_udb=suma_nahb/kolih_dnei; 
  suma_udb=iceb_u_okrug(suma_udb,okrg);
  sprintf(strsql,"%s-Бюджет-%.2f/%d=%.2f\n",__FUNCTION__,suma_nahb,kolih_dnei,suma_udb);
  zar_pr_insw(strsql,wpredok);
  
 } 
double suma_ud=suma_nah/kolih_dnei-suma_udb;
suma_ud=iceb_u_okrug(suma_ud,okrg);

sprintf(strsql,"%s-%.2f/%d-%.2f=%.2f\n",__FUNCTION__,suma_nah,kolih_dnei,suma_udb,suma_ud);
zar_pr_insw(strsql,wpredok);
suma_ud*=-1;
suma_udb*=-1;

short d=1;
iceb_u_dpm(&d,&mr,&gr,5);

struct ZARP     zp;


zp.tabnom=tabnom;
zp.prn=2;
zp.knu=kod_ud_dnev_zar;
zp.dz=d;
zp.mz=mr;
zp.gz=gr;
zp.mesn=mr; zp.godn=gr;
zp.nomz=0;
zp.podr=podr;
strncpy(zp.shet,shet_uder.ravno(),sizeof(zp.shet)-1);

zapzarpw(&zp,suma_ud,d,mr,gr,0,shet_uder.ravno(),"",0,podr,"",wpredok);

if(shet_uderb.getdlinna() > 1)
 {
  strcpy(zp.shet,shet_uderb.ravno());
  zapzarpw(&zp,suma_udb,d,mr,gr,0,shet_uderb.ravno(),"",0,podr,"",wpredok);
 } 

}
