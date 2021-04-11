/*$Id: zarindexvw.c,v 1.7 2013/08/13 06:10:03 sasa Exp $*/
/*16.04.2017	30.01.2010	Белых А.И.	zarindexvw.c
Расчёт индексации на не вовремя выплаченную зарплату
*/
#include        "buhg_g.h"
#include "read_nast_indw.h"

double zarindexv_kof(short mes,short god);


extern class index_ua tabl_ind; /*Таблица для индексации зарплаты*/
extern double   okrg; /*Округление*/
extern struct ZAR	zar;
extern SQL_baza bd;

void zarindexvw(int tn, //Табельный номер*/
short mp,short gp,int podr,
const char *nah_only,
GtkWidget *wpredok)
{
char strsql[1024];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str shet("");

sprintf(strsql,"\n%s-Расчёт индексации на не вовремя выплаченную зарплату\n\
------------------------------------------------------------------\n",__FUNCTION__); 
zar_pr_insw(strsql,wpredok);

if(tabl_ind.kninvz <= 0)
 {
  sprintf(strsql,"В файле настройки zarindexv.alx не введён код индексации!\n");
  zar_pr_insw(strsql,wpredok);
  return;
 }

if(iceb_u_proverka(nah_only,tabl_ind.kninvz,0,0) != 0)
 {
  sprintf(strsql,"Код %d исключён из расчёта\n",tabl_ind.kninvz);
  zar_pr_insw(strsql,wpredok);
  return;
 }
 
if(tabl_ind.kodpzns <= 0)
 {
  sprintf(strsql,"В файле настройки zarnast.alx не введён код перечисления на карт-счёт!\n");
  zar_pr_insw(strsql,wpredok);
 }

if(tabl_ind.kodpzvk <= 0)
 {
  sprintf(strsql,"В файле настройки zarnast.alx не введён код получения зарплаты в кассе!\n");
  zar_pr_insw(strsql,wpredok);
 }
if(tabl_ind.kodpzvk <= 0 && tabl_ind.kodpzns <= 0)
 return;

/*Проверяем есть ли индексация в списке начислений*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and knah=%d",tn,tabl_ind.kninvz);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"Не введён код индексации %d в список начислений!\n",tabl_ind.kninvz);
  zar_pr_insw(strsql,wpredok);
  return;
 }
shet.new_plus(row[0]);

/*Определяем есть ли выплаты в счёт предыдущих периодов */
sprintf(strsql,"select knah,suma,godn,mesn from Zarp where datz >= '%04d-%02d-01' \
and datz <= '%04d-%02d-31' and tabn=%d and prn='2' and (knah=%d or knah=%d) and suma <> 0.",
gp,mp,gp,mp,tn,tabl_ind.kodpzns,tabl_ind.kodpzvk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;

sprintf(strsql,"Таблица коэффициентов:%d\n",tabl_ind.koefv.kolih());
zar_pr_insw(strsql,wpredok);
for(int nom=0; nom < tabl_ind.koefv.kolih(); nom++)
   {
    sprintf(strsql,"%02d.%d|%f\n",tabl_ind.miv.ravno(nom),tabl_ind.giv.ravno(nom),tabl_ind.koefv.ravno(nom));
    zar_pr_insw(strsql,wpredok);
    
   }
 
short dpr=1,mpr=mp,gpr=gp; /*Дата зарплаты котороя выплачена в срок*/

iceb_u_dpm(&dpr,&mpr,&gpr,4); /*уменьшаем на месяц*/
short d,m,g;

class iceb_u_spisok mes_god;
class iceb_u_int mesv;
class iceb_u_int godv;
class iceb_u_double sumav;
int nomer_dat=0;

while(cur.read_cursor(&row) != 0)
 {
  m=atoi(row[3]);
  g=atoi(row[2]);  

  if(iceb_u_sravmydat(1,m,g,1,mpr,gpr) >= 0) 
   continue;
  sprintf(strsql,"%02d.%04d",m,g);
//  printw("*%02d.%04d\n",m,g);
  
  if((nomer_dat=mes_god.find(strsql)) < 0)
   {
    mes_god.plus(strsql);
    mesv.plus(m,-1);    
    godv.plus(g,-1);
   }
  sumav.plus(atof(row[1])*-1,nomer_dat);
 }

double koef=0.;
double ikoef=1;
double sum=0.;
struct ZARP     zp;
d=1;
iceb_u_dpm(&d,&mp,&gp,5);
zp.dz=d;
zp.nomz=0;
zp.podr=podr;
short mi=0;
short gi=0;

zp.tabnom=tn;
zp.prn=1;
zp.knu=tabl_ind.kninvz;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;

for(int nomer=0 ; nomer < mes_god.kolih(); nomer++)
 {
  sprintf(strsql,"Дата в счёт которой выплачено %s сумма %.2f\n",mes_god.ravno(nomer),sumav.ravno(nomer));
  zar_pr_insw(strsql,wpredok);

  d=1;
  mi=m=mesv.ravno(nomer);
  gi=g=godv.ravno(nomer);
  iceb_u_dpm(&d,&m,&g,3);  
  ikoef=1.;
  while(iceb_u_sravmydat(1,m,g,1,mpr,gpr) <= 0)  
   {
    koef=zarindexv_kof(m,g);
    ikoef*=koef;

    sprintf(strsql,"Дата %d.%d коэффициент за месяц %f Итоговый коэффициент %f\n",m,g,koef,ikoef);
    zar_pr_insw(strsql,wpredok);

    iceb_u_dpm(&d,&m,&g,3);
   }

  if(ikoef == 0.)
   {
    sprintf(strsql,"Итоговый коэффициент равен нолю!\n");
    zar_pr_insw(strsql,wpredok);
    continue;
   }
  sum=sumav.ravno(nomer)*(ikoef*100-100.)/100.;
  sum=iceb_u_okrug(sum,okrg);
  
  sprintf(strsql,"%.2f*(%f*100.-100.)/100.=%.2f\n",sumav.ravno(nomer),ikoef,sum);
  zar_pr_insw(strsql,wpredok);
  if(sum <= 0.)
   {
    sprintf(strsql,"Cумма меньше или равна нолю!\n");
    zar_pr_insw(strsql,wpredok);
    continue;      
   }  
//  zp.mesn=mi; zp.godn=gi;

//  strsql[0]='\0';
//  strcpy(zp.shet,shet.ravno());
//  zapzarp(zp.dz,mp,gp,tn,1,tabl_ind.kninvz,sum,shet.ravno(),mi,gi,0,0,strsql,podr,"",zp);

  zp.mesn=mi; zp.godn=gi;
  zp.nomz=0;
  zp.podr=podr;
  strcpy(zp.shet,shet.ravno());

  zapzarpw(&zp,sum,d,mi,gi,0,shet.ravno(),"",0,podr,"",wpredok);
   
 }

}

/****************************/
/*Получени коэффициента за месяц*/
/********************************/

double zarindexv_kof(short mes,short god)
{

//if(ff_prot != NULL)
// sprintf(strsql,"Ищем коэффициент для %d.%d\n",mes,god);
 
for(int nom=0; nom < tabl_ind.koefv.kolih(); nom++)
 {
   
  if(mes == tabl_ind.miv.ravno(nom) && god == tabl_ind.giv.ravno(nom))
   {
//    if(ff_prot != NULL)
//     sprintf(strsql,"Нашли %d.%d %f\n",tabl_ind.miv.ravno(nom),tabl_ind.giv.ravno(nom),tabl_ind.koefv.ravno(nom));
    return(tabl_ind.koefv.ravno(nom)/100.);
   }
 }
return(0.);
}