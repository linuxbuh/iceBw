/* $Id: dolhn.c,v 5.27 2013/09/26 09:43:32 sasa Exp $ */
/*19.06.2019    06.01.1993      Белых А.И.      dolhnnw.c
Расчёт начислений
*/
#include        <ctype.h>
#include        <errno.h>
#include        "buhg_g.h"

extern double   okrg;
extern float    hasov; /*Количество рабочих часов в текущем месяце*/
//extern SQL_baza bd;

double dolhnnw(int tn, //Табельный номер
short mp,short gp, //Дата
int kp,  //Код подразделения
const char *nah_only,
GtkWidget *wpredok)
{
double		koefpv;
short           d;
int             kodokl=0; /*Код должносного оклада*/
double          oklad;  /*Оклад*/
double          nah;
int             met; /*0-оклад 1-часовая ставка 2-не пересчитываемый оклад*/
char		strsql[1024];
float		dneii,has;
float		kolrd=0.; /*Количество рабочих дней в месяце*/
class iceb_u_str shet("");
float		has1;   /*Количество часов в одном рабочем дне*/
float		kof;   /*Коэффициент оплаты ставки*/
float		mesnoh;  /*Месячная норма отработанных часов*/
class SQLCURSOR cur;
SQL_str row;
class ZARP     zp;
int		kodtab=0;
float dnei=0.,hasov=0.;
int metka_sp=0;
koefpv=oklad=nah=0.;

redkalw(mp,gp,&dnei,&hasov,wpredok);

sprintf(strsql,"\n%s-Расчёт должностных окладов\n---------------------------\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);


kolrd=dnei;
met=0;

sprintf(strsql,"%s-Просматриваем таблицу должностных окладов\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);

//if(dolhn1(tn,&oklad,&has1,&kof,&met,&kodokl,&metka,&metka1,&shet,&metka_no,mp,gp) != 0)
//  return(koefpv);

sprintf(strsql,"select * from Zarsdo where tn=%d and dt <= '%04d-%02d-01' order by dt desc limit 1",tn,gp,mp);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s-Нет настроек по табельному номеру %d на дату %02d.%04d\n",__FUNCTION__,tn,mp,gp);
  zar_pr_insw(strsql,wpredok);
  return(koefpv);
 }

sprintf(strsql,"%s-%6s %s %3s %10s %s %5s %5s %6s %s\n",__FUNCTION__,row[0],iceb_u_datzap_mg(row[1]),row[2],row[3],row[4],row[5],row[6],row[7],row[8]);
zar_pr_insw(strsql,wpredok);

oklad=atof(row[3]);
metka_sp=atoi(row[4]);
shet.new_plus(row[7]);
kof=atof(row[6]);

has1=atof(row[5]);

kodokl=atoi(row[2]);

if(iceb_u_proverka(nah_only,kodokl,0,0) != 0)
 {
  sprintf(strsql,"%s-Код должностного оклада исключён из расчёта %d\n",__FUNCTION__,kodokl);
  zar_pr_insw(strsql,wpredok);
  return(koefpv);
  
 }

if(shet.ravno()[0] == '*')
 {
  //читаем счёт в карточке
  sprintf(strsql,"select shet from Kartb where tabn=%d",tn);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   shet.new_plus(row[0]);
  else
   shet.new_plus("");
 }

if(shet.ravno()[0] != '\0')
 {
  OPSHET opshet;
  if(iceb_prsh1(shet.ravno(),&opshet,wpredok) != 0)
    shet.new_plus("");
 }

sprintf(strsql,"\n%s-Табельный номер:%d dolhn-dnei=%.2f hasov=%.2f oklad=%.2f has1=%.2f kof=%.2f met=%d kodok=%d\n",
__FUNCTION__,tn,dnei,hasov,oklad,has1,kof,met,kodokl);
zar_pr_insw(strsql,wpredok);
  

if(metka_sp == 5 || metka_sp == 6)
  oklad=okladtrw(mp,gp,(int)oklad,wpredok); /*oklad содержит тарифный разряд*/

if(kodokl == 0)
 {
  sprintf(strsql,"%s-%s\n",__FUNCTION__,"Не введён код оклада !!!");
  zar_pr_insw(strsql,wpredok);
  return(koefpv);
 }



/*Проверяем есть ли код начисления в списке*/

sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and knah=%d",tn,kodokl); 
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s-Не нашли код %d в списке начислений для %d табельного номера !\n",__FUNCTION__,kodokl,tn);
  zar_pr_insw(strsql,wpredok);
  return(koefpv);
 }
if(shet.ravno()[0] == '\0')
  shet.new_plus(row[0]);

/*Читаем код табеля*/
sprintf(strsql,"select kodt from Nash where kod=%d",kodokl);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s-Нет кода начисления %d в списке начислений !\n",__FUNCTION__,kodokl);
  zar_pr_insw(strsql,wpredok);
  return(koefpv);
 }
kodtab=atoi(row[0]);

sprintf(strsql,"%s-Код табеля для автоматизированного расчёта %d\n",__FUNCTION__,kodtab);
zar_pr_insw(strsql,wpredok);

/*Читаем табель*/

 sprintf(strsql,"select dnei,has,kolrd,kolrh,nomz from Ztab \
where god=%d and mes=%d and tabn=%d and kodt=%d order by nomz asc",
 gp,mp,tn,kodtab);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  dneii=has=0.;
  kolrd=0.;
  char soob[2048];
  sprintf(soob,"%s-%s\nНе найден код табеля %d указанного в начислении.\n",__FUNCTION__,strsql,kodtab);
  zar_pr_insw(soob,wpredok); }
else
 {
  dneii=atof(row[0]);
  has=atof(row[1]);
  if(atof(row[2]) != 0.)
   {
    kolrd=atof(row[2]);
    sprintf(strsql,"%s-Количество рабочих дней в месяце взяли из табеля=%.2f\n",__FUNCTION__,kolrd);
    zar_pr_insw(strsql,wpredok);   
   }
  if(atof(row[3]) != 0.)
   {
    has1=atof(row[3]);
    sprintf(strsql,"%s-Количество рабочих часов в месяце взяли из табеля=%.2f\n",__FUNCTION__,has1);
    zar_pr_insw(strsql,wpredok);
   }
 }

mesnoh=hasov;
if(has1 != 0.)
 {
  mesnoh=kolrd*has1;
  sprintf(strsql,"%s-mesnoh=%.2f*%.2f=%.2f\n",__FUNCTION__,kolrd,has1,mesnoh);
  zar_pr_insw(strsql,wpredok);
 }  
else
 {
  sprintf(strsql,"%s-mesnoh=%.2f\n",__FUNCTION__,mesnoh);
  zar_pr_insw(strsql,wpredok);
 }

nah=0.;
if(metka_sp == 2) /*не пересчитываемый оклад*/
 {
  nah=oklad;
  koefpv=1.;
 }

if((metka_sp == 1 || metka_sp == 6) && mesnoh > 0.00001)
 {
  nah=oklad/mesnoh*has;
  koefpv=has/mesnoh;
  if(metka_sp == 1)
   {
    sprintf(strsql,"%s-%s\n",__FUNCTION__,gettext("Оплата за месяц"));
    zar_pr_insw(strsql,wpredok);
   
   }   
  if(metka_sp == 6)
   {
    sprintf(strsql,"%s-%s\n",__FUNCTION__,gettext("Оплата по тарифу"));
    zar_pr_insw(strsql,wpredok);
   
   }  
  
  sprintf(strsql,"%s-Расчёт по по часам=%.2f/%.2f*%.2f=%.2f koefpv=%.2f/%.2f=%f\n",
  __FUNCTION__,oklad,mesnoh,has,nah,has,mesnoh,koefpv);
  zar_pr_insw(strsql,wpredok);
 }

if((metka_sp == 0 || metka_sp == 5) && kolrd > 0)
 {
  nah=oklad/kolrd*dneii;
  koefpv=dneii/kolrd;

  if(metka_sp == 0)
   {
    sprintf(strsql,"%s-%s\n",__FUNCTION__,gettext("Оплата за месяц"));
    zar_pr_insw(strsql,wpredok);
   
   }   
  if(metka_sp == 5)
   {
    sprintf(strsql,"%s-%s\n",__FUNCTION__,gettext("Оплата по тарифу"));
    zar_pr_insw(strsql,wpredok);
   
   }  


  sprintf(strsql,"%s-Расчёт по по дням=%.2f/%.2f*%.2f=%.2f koefpv=%f\n",__FUNCTION__,oklad,kolrd,dneii,nah,koefpv);
  zar_pr_insw(strsql,wpredok);
 }   

if(metka_sp == 3) /*расчёт по часовой ставке*/
 {
  nah=oklad*has;
  koefpv=1.;


  sprintf(strsql,"%s-Расчёт по часовой ставке. Начислено=%.2f*%.2f=%.2f koefpv=%f\n",__FUNCTION__,oklad,has,nah,koefpv);
  zar_pr_insw(strsql,wpredok);
 }

if(metka_sp == 4) /*расчёт по дневной ставке*/
 {
  nah=oklad*kolrd;
  koefpv=1.;
  sprintf(strsql,"%s-Расчёт по дневной ставке. Начислено=%.2f*%.2f=%.2f koefpv=%f\n",__FUNCTION__,oklad,kolrd,nah,koefpv);
  zar_pr_insw(strsql,wpredok);
 }
/********************** 
if(metka == 1)
 {
  dobule procb=0.;  
  double procent=zarpodpr(kp,&procb);

  sprintf(strsql,"%s-Учет процента выполнения плана подразделением %d, процент: %.2f %%\n",__FUNCTION__,kp,procent);   
  zar_pr_insw(strsql,wpredok);

  sprintf(strsql,"%s-%.2f*%.2f/100=",__FUNCTION__,nah,procent);
  zar_pr_insw(strsql,wpredok);

  koefpv*=procent/100.;
  nah=nah*procent/100.;
  sprintf(strsql,"%s-%.2f koefpv=%f\n",__FUNCTION__,nah,koefpv);
  zar_pr_insw(strsql,wpredok);

 } 
**********************/
if(nah > 0.)
  nah=iceb_u_okrug(nah,okrg);


iceb_u_dpm(&d,&mp,&gp,5);


zp.tabnom=tn;
zp.prn=1;
zp.knu=kodokl;

zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=kp;
strcpy(zp.shet,shet.ravno());

zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,kp,"",wpredok);



return(koefpv);
}
