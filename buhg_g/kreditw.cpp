/* $Id: kreditw.c,v 1.18 2014/02/28 05:20:59 sasa Exp $ */
/*19.06.2019    01.12.1992      Белых А.И.      kreditw.c
Подпрограмма расчёта удержаний за кредит
Переменные данные лежат в файле кредит
*/
#include        "buhg_g.h"

extern struct ZAR zar;
extern SQL_baza bd;

void kreditw(int tn,short mp,short gp,int podr,const char *uder_only,GtkWidget *wpredok)
{
int             ku=0;   /*Код начисления*/
double          nah=0.;
short		d,m,g;
SQL_str         row;
char		strsql[1024];
class iceb_u_str shet("");
class ZARP	zp;
class SQLCURSOR cur;

sprintf(strsql,"\n%s-Расчёт удержания по кредиту\n----------------------------------------------\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);

sprintf(strsql,"select ku,sp,mu,dd from Zarrud where tn=%d",tn);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  char soob[2048];
  sprintf(soob,"%s\nНе найдено настройки для удержания!\n",strsql);
  zar_pr_insw(soob,wpredok);
  return;
 }

ku=atoi(row[0]);


if(iceb_u_proverka(uder_only,ku,0,0) != 0)
 {
  sprintf(strsql,"%s-Код %d исключён из расчёта\n",__FUNCTION__,ku);
  zar_pr_insw(strsql,wpredok);
  return;
 }

d=0;
iceb_u_rsdat(&d,&m,&g,row[3],2);
if(d != 0 && mp > m && gp >= g)
 {
  sprintf(strsql,"%s-Закончисля срок действия. Расчёт не производим.\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return;
 }
if(atoi(row[2]) == 1)
 {
  double procent=atof(row[1]);
  sprintf(strsql,"%s-Процент от суммы начисленной %f\n",__FUNCTION__,procent);
  zar_pr_insw(strsql,wpredok);
  //Узнаём всю сумму начиления
  sprintf(strsql,"select suma from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and prn='1' and suma <> 0.",
  gp,mp,gp,mp,tn);
  int kolstr=0;  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
   }

  double suma=0.;

  if(kolstr > 0)
   while(cur.read_cursor(&row) != 0)
    suma+=atof(row[0]);

  sprintf(strsql,"%s-Сумма всех начислений:%.2f процент=%f\n",__FUNCTION__,suma,procent);
  zar_pr_insw(strsql,wpredok);

  if(suma != 0.)   
   {
    nah=suma*procent/100.;
    nah=iceb_u_okrug(nah,0.01);
     
   }
      
 }
else
 {
  sprintf(strsql,"%s-Стабильная сумма\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  nah=atof(row[1]);
  
 }


/*printw("tn=%d tnn=%d nah=%.2f ku=%d\n",tn,tnn,nah,ku);*/
if(nah == 0.)
 {
  sprintf(strsql,"%s-Начисленная сумма равна нолю\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
   
  return;
 }

sprintf(strsql,"%s-Код удержания %d. Сумма удержания %.2f\n",__FUNCTION__,ku,nah);
zar_pr_insw(strsql,wpredok);
 
SQLCURSOR curr;
/*Проверяем есть ли код начисления в списке*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and \
knah=%d",tn,ku); 
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  sprintf(strsql,"%s-Не введён код удержания %d в список удержаний !\n",__FUNCTION__,ku);
  zar_pr_insw(strsql,wpredok);
  return;
 }
shet.new_plus(row[0]);

nah=nah*(-1);

iceb_u_dpm(&d,&mp,&gp,5);

zp.tabnom=tn;
zp.prn=2;
zp.knu=ku;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
strcpy(zp.shet,shet.ravno());

zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
}
