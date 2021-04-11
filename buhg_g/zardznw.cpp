/*$Id: zardznw.c,v 1.9 2013/09/26 09:47:00 sasa Exp $*/
/*04.07.2016	02.12.2009	Белых А.И.	zardrnw.c
Расчёт доплаты до средней зарплаты в командировке с учётом надбавки за награду
*/
#include        "buhg_g.h"

extern short    *kodkomand; //Коды командировочных расходов
extern short *kvvrdzn; /*Коды входящие в расчёт доплаты до средней зарплаты в командировке с учётом надбавки за награду*/
extern short koddzn; /*Код доплаты до средней зарплаты в командировке с учётом надбавки за награду*/
extern short   *ktnvr_komand; //Коды табеля не входящие в расчёт командировочных
extern double  okrg; /*Округление*/
extern SQL_baza bd;

void zardznw(int tn, //Табельный номер*/
short mp,short gp,
int podr,
const char *nah_only,
GtkWidget *wpredok)
{
char strsql[1024];
class iceb_u_str shet("");
SQL_str row;
SQLCURSOR cur;   

sprintf(strsql,"\n%s-Расчёт доплаты до средней зарплаты в командировке с учётом награды\n\
------------------------------------------------\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);

if(kodkomand == NULL)
 {
  sprintf(strsql,"Не введены коды командировочных расходов !\n");  
  zar_pr_insw(strsql,wpredok);
  return;

 }

  
if(kvvrdzn == NULL)
 {
  sprintf(strsql,"%s-Не введены коды входящие в расчёт для расчёта доплаты до средней зарплаты в командировке !\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return;

 }

if(koddzn == 0)
 {
  sprintf(strsql,"%s-Не введён код доплаты до средней заплаты в командировке с учётом надбавки за награду !\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return;

 }

sprintf(strsql,"%s-Код доплаты %d\n",__FUNCTION__,koddzn);
zar_pr_insw(strsql,wpredok);

if(iceb_u_proverka(nah_only,koddzn,0,0) != 0)
 {
  sprintf(strsql,"%s-Код %d исключён из расчёта\n",__FUNCTION__,koddzn);
  zar_pr_insw(strsql,wpredok);
  return;  
 }


//Проверяем есть ли начисление в карточке
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
knah=%d",tn,koddzn);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 {
  sprintf(strsql,"%s-Не введён код начисления %d !\n",__FUNCTION__,koddzn);
  zar_pr_insw(strsql,wpredok);
  return;
 }    
shet.new_plus(row[0]);

class iceb_u_str kod_tabel;

for(int shetkk=1; shetkk <= kodkomand[0]; shetkk++)
 {

  sprintf(strsql,"%s-Ищем код табеля для начисления %d\n",__FUNCTION__,kodkomand[shetkk]);
  zar_pr_insw(strsql,wpredok);
  //Читаем код табеля командировочных
  sprintf(strsql,"select kodt from Nash where kod=%d",kodkomand[shetkk]);
  zar_pr_insw(strsql,wpredok);
  zar_pr_insw("\n",wpredok);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s-Не найден код начисления %d для расчёта командировочных !\n",__FUNCTION__,kodkomand[shetkk]);
    zar_pr_insw(strsql,wpredok);
    continue;
   }   

  sprintf(strsql,"%s-Код табеля %s\n",__FUNCTION__,row[0]);   
  zar_pr_insw(strsql,wpredok);

  kod_tabel.z_plus(row[0]);
 } 

short dnei=0;
int kolstr;

    //Проверяем введён ли табель
sprintf(strsql,"select kodt,dnei from Ztab where tabn=%d and god=%d  and mes=%d",tn,gp,mp);

zar_pr_insw(strsql,wpredok);
zar_pr_insw("\n",wpredok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return;
 }
 
if(kolstr > 0)
 while(cur.read_cursor(&row) != 0)
  {
   if(iceb_u_proverka(kod_tabel.ravno(),row[0],0,1) != 0)
      continue;

   sprintf(strsql,"%s-%3s %3s\n",__FUNCTION__,row[0],row[1]);
   zar_pr_insw(strsql,wpredok);
      dnei+=atoi(row[1]);
  }
   
sprintf(strsql,"%s-Количество дней командировочных=%d\n",__FUNCTION__,dnei);
zar_pr_insw(strsql,wpredok);
    
if(dnei == 0)
 return;



short dn,mn,gn;
short dk=1,mk=mp,gk=gp;
iceb_u_dpm(&dk,&mk,&gk,4); //уменьшить дату на месяц

dn=dk; mn=mk; gn=gk;

iceb_u_dpm(&dk,&mk,&gk,5); //получить последний день месяца

iceb_u_dpm(&dn,&mn,&gn,4); //уменьшить дату на месяц

//Смотрим начисления за 2 последних месяца
sprintf(strsql,"select datz,knah,suma,shet from Zarp where datz >= '%d-%d-%d' \
and datz <= '%d-%d-%d' and tabn=%d and prn='1' and suma != 0.",
gn,mn,dn,gk,mk,dk,tn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
double suma=0.;

sprintf(strsql,"%s-Начисления за два предыдущих месяца\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);

while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(kvvrdzn,row[1]) < 0)
   continue;

  suma+=atof(row[2]);
  sprintf(strsql,"%s-%s %s %s %s\n",__FUNCTION__,row[0],row[1],row[2],row[3]);
  zar_pr_insw(strsql,wpredok);

 }

sprintf(strsql,"%s-Табель за два предыдущих месяца\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);
 
//Смотрим отработанное время

sprintf(strsql,"select kodt,dnei from Ztab where tabn=%d and \
((god=%d and mes=%d) or (god=%d and mes=%d))",tn,gn,mn,gk,mk);

zar_pr_insw(strsql,wpredok);
zar_pr_insw("\n",wpredok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);

int den=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(ktnvr_komand,row[0]) >=  0)
   continue;
  sprintf(strsql,"%s-%3s %3s\n",__FUNCTION__,row[0],row[1]);
  zar_pr_insw(strsql,wpredok);
  
  den+=atoi(row[1]);

 }


double sum_kr=0.;
if(den != 0 && dnei != 0 && suma != 0.)
 {
  sum_kr=suma/den*dnei;
  sum_kr=iceb_u_okrug(sum_kr,okrg);
 }

sprintf(strsql,"%s-Сумма доплаты за предыдущих два месяца %.2f/%d*%d=%.2f\n",__FUNCTION__,suma,den,dnei,sum_kr);
zar_pr_insw(strsql,wpredok);


//Смотрим начисления за месяц расчёта
sprintf(strsql,"select datz,knah,suma,shet from Zarp where datz >= '%d-%d-%d' \
and datz <= '%d-%d-%d' and tabn=%d and prn='1' and suma != 0.",
gp,mp,1,gp,mp,31,tn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
double suma_mr=0.;

sprintf(strsql,"%s-Начисления за месяц расчёта\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);

while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(kvvrdzn,row[1]) < 0)
   continue;

  suma_mr+=atof(row[2]);
  sprintf(strsql,"%s-%s %s %s %s\n",__FUNCTION__,row[0],row[1],row[2],row[3]);
  zar_pr_insw(strsql,wpredok);

 }

sprintf(strsql,"%s-Табель за месяц расчёта\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);
 
//Смотрим отработанное время

sprintf(strsql,"select kodt,dnei from Ztab where tabn=%d and god=%d and mes=%d",tn,gp,mp);

zar_pr_insw(strsql,wpredok);
zar_pr_insw("\n",wpredok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);

int den_mr=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(ktnvr_komand,row[0]) >=  0)
   continue;
  sprintf(strsql,"%s-%2s %s\n",__FUNCTION__,row[0],row[1]);
  zar_pr_insw(strsql,wpredok);
  
  den_mr+=atoi(row[1]);

 }


double sum_kr_mr=0.;
if(den != 0 && dnei != 0 && suma_mr != 0.)
 {
  sum_kr_mr=suma_mr/den_mr*dnei;
  sum_kr_mr=iceb_u_okrug(sum_kr_mr,okrg);
 }

sprintf(strsql,"%s-Сумма доплаты за текущий месяц %.2f/%d*%d=%.2f\n",
__FUNCTION__,suma_mr,den_mr,dnei,sum_kr_mr);
zar_pr_insw(strsql,wpredok);

double suma_dop=sum_kr;
if(sum_kr >= sum_kr_mr)
 {
  suma_dop=sum_kr;

  sprintf(strsql,"%s-Сумма %.2f >= %.2f берём большую\n",__FUNCTION__,sum_kr,sum_kr_mr);
  
  zar_pr_insw(strsql,wpredok);
 }
else
 {
  suma_dop=sum_kr_mr;

  sprintf(strsql,"%s-Сумма %.2f > %.2f берём большую\n",__FUNCTION__,sum_kr_mr,sum_kr);
  
  zar_pr_insw(strsql,wpredok);
 }  




short d=0;

iceb_u_dpm(&d,&mp,&gp,5);
/**********
struct ZARP     zp;
zp.dz=d;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

strcpy(zp.shet,shet);
zapzarp(d,mp,gp,tn,1,koddzn,sum_kr,shet,mp,gp,0,0,"",podr,"",zp);
***************/
class ZARP     zp;
zp.tabnom=tn;
zp.prn=1;
zp.knu=koddzn;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

strcpy(zp.shet,shet.ravno());
zapzarpw(&zp,suma_dop,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);

}




