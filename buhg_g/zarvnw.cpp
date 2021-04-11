/*$Id: zarvnw.c,v 1.1 2014/08/31 06:19:22 sasa Exp $*/
/*24.04.2018	12.08.2014	Белых А.И.	zarvnw.c
Расчёт военного налога
*/
#include "buhg_g.h"

extern char *shetb; /*Бюджетные счета начислений*/
extern short kodvn; /*код военного сбора*/
extern short kodvs_b; /*код военного сбора с больничног*/
extern class iceb_u_str kodvn_nvr; /*коды не входящие в расчёт военного сбора*/
extern short *kodbl;  /*Коды начисления больничного*/
extern SQL_baza bd;

void zarvnw(int tabnom,short mes,short god,int podr,GtkWidget *wpredok)
{
char strsql[2048];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str shet("");
class iceb_u_str shet_b("");

sprintf(strsql,"\n%s-Расчёт военного сбора. Код военного сборая %d Код военного сбора с больничного %d\n\
-----------------------------------------------\n",__FUNCTION__,kodvn,kodvs_b);

zar_pr_insw(strsql,wpredok);

/*Проверяем есть ли код удержания в списке*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and knah=%d",tabnom,kodvn); 
//printw("%s\n",strsql);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
 {
  
  
  sprintf(strsql,"%s-Коды не входящие в расчёт-%s\n",__FUNCTION__,kodvn_nvr.ravno());
  zar_pr_insw(strsql,wpredok);
  
  shet.new_plus(row[0]);
 }
else 
 {
  sprintf(strsql,"%s-Не введён код военного налога для работника\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  
  return;
 } 

sprintf(strsql,"select knah,suma,shet from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and tabn=%d and prn='1' and suma <> 0.",
god,mes,1,god,mes,31,tabnom);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s-Не найдено ни одного начисления\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
 }

double suma=0.;
double sumab=0.;
double suma_bol=0.;
double suma_bolb=0.;

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(kodvn_nvr.ravno(),row[0],0,1) == 0)
   {
    sprintf(strsql,"%s-Не вошло в расчёт Код=%s Сумма=%s\n",__FUNCTION__,row[0],row[1]);
    zar_pr_insw(strsql,wpredok);
    
    continue;
   }
  suma+=atof(row[1]);
  if(iceb_u_proverka(shetb,row[2],0,1) == 0)
   sumab+=atof(row[1]);
  if(provkodw(kodbl,row[0]) >= 0)
   {
    suma_bol+=atof(row[1]);   
    if(iceb_u_proverka(shetb,row[2],0,1) == 0)
     suma_bolb+=atof(row[1]);   
   }
 }
if(kodvs_b != 0)
 suma-=suma_bol; 

double suman=suma;

sprintf(strsql,"%s-Cумма с которой берётся налог %.2f Cумма больничных %.2f\n",__FUNCTION__,suma,suma_bol);
zar_pr_insw(strsql,wpredok);

if(sumab != 0.)
 {
  /*узнаём бюджетный и хозрасчётный счёт*/
  sprintf(strsql,"select shet,shetb from Uder where kod=%d",kodvn);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    shet.new_plus(row[0]);
    shet_b.new_plus(row[1]);
    if(shet_b.getdlinna() <= 1)
     {
      sprintf(strsql,"%s-Не введён бюджетный счёт для кода удержания %d\n",__FUNCTION__,kodvn);
      zar_pr_insw(strsql,wpredok);
     }
   }  

  sprintf(strsql,"%s-Бюджетная сумма с которой берётся налог%.2f\n",__FUNCTION__,sumab);
  zar_pr_insw(strsql,wpredok);
 
 }

const float proc_vn=1.5;
double suma_nal_b=0.;
if(sumab != 0.)
 {
  suma_nal_b=sumab*proc_vn/100.;
  suma_nal_b=iceb_u_okrug(suma_nal_b,0.01);
  sprintf(strsql,"%s-Бюджетный налог %.2f*%.2f/100.=%.2f\n",__FUNCTION__,sumab,proc_vn,suma_nal_b);
  zar_pr_insw(strsql,wpredok);
 }
#if 0
###########################33
double suma_nal=suman*proc_vn/100.-suma_nal_b;
suma_nal=iceb_u_okrug(suma_nal,0.01); /*удержание должно быть отрицательныйм*/

sprintf(strsql,"%s-%.2f*%.2f/100.-%.2f=%.2f\n",__FUNCTION__,suman,proc_vn,suma_nal_b,suma_nal);
zar_pr_insw(strsql,wpredok);
#############################3
#endif
double suma_nal=0.;

if(suman - sumab > 0.009)
 {
  suma_nal=(suman-sumab)*proc_vn/100.;
  suma_nal=iceb_u_okrug(suma_nal,0.01);
  sprintf(strsql,"%s-%.2f-%.2f*%.2f/100.=%.2f\n",__FUNCTION__,suman,sumab,proc_vn,suma_nal);
  zar_pr_insw(strsql,wpredok);
 }



suma_nal*=-1; /*удержание должно быть отрицательныйм*/
suma_nal_b*=-1; /*удержание должно быть отрицательныйм*/

short d=31;

iceb_u_dpm(&d,&mes,&god,5);

struct ZARP     zp;

zp.tabnom=tabnom;
zp.prn=2;
zp.knu=kodvn;
zp.dz=d;
zp.mz=mes;
zp.gz=god;
zp.mesn=mes; zp.godn=god;
zp.nomz=0;
zp.podr=podr;
strcpy(zp.shet,shet.ravno());

sprintf(strsql,"%.2f%%",proc_vn);
class iceb_u_str koment(strsql);

zapzarpw(&zp,suma_nal,d,mes,god,0,shet.ravno(),koment.ravno(),0,podr,"",wpredok);

if(shetb != NULL)
if(shetb[0] != '\0')
if(shet_b.getdlinna() > 1)
 {
  strcpy(zp.shet,shet_b.ravno());
  zapzarpw(&zp,suma_nal_b,d,mes,god,0,shet_b.ravno(),koment.ravno(),0,podr,"",wpredok);
 }


if(kodvs_b == 0)
 {
  sprintf(strsql,"%s-Код военного сбора с больничного равен нолю!\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return;
 }

sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and knah=%d",tabnom,kodvs_b); 
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 {
  sprintf(strsql,"select shet,shetb from Uder where kod=%d",kodvs_b);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
   {
    sprintf(strsql,"%s-Не найден код военного сбора с больничного в справочнике удержаний!\n",__FUNCTION__);
    zar_pr_insw(strsql,wpredok);
    return;
   }
  else
   {
    if(row[0][0] == '\0')
     {
      sprintf(strsql,"%s-В справочнике удержаний для \"Военный сбор с больничных\" не введён счёт!\n",__FUNCTION__);
      zar_pr_insw(strsql,wpredok);
      return;
     }
    shet.new_plus(row[0]);
    shet_b.new_plus(row[1]);
   }
 } 
else
 {
  shet.new_plus(row[0]);

  if(shetb != NULL)/*если расчёт с бюджетом до счета нужно брать только в справочнике удержаний так как неизветно какой счёт был в последней записи удержания*/
  if(shetb[0] != '\0')
   {
    sprintf(strsql,"select shetb from Uder where kod=%d",kodvs_b);
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
     {
      sprintf(strsql,"%s-Не найден код военного сбора с больничного в справочнике удержаний!\n",__FUNCTION__);
      zar_pr_insw(strsql,wpredok);
      return;
     }
    else
     {
      if(row[0][0] == '\0')
       {
        sprintf(strsql,"%s-В справочнике удержаний для \"Военный сбор с больничных\" не введён бюджетный счёт!\n",__FUNCTION__);
        zar_pr_insw(strsql,wpredok);
       }
      shet_b.new_plus(row[0]);
     }
   }
 }

if(shet.getdlinna() <= 1)
 {
  sprintf(strsql,"%s-Для удержания \"Военный сбор с больничных\" не введён счёт!\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return;
 }

suma_nal_b=0.;

if(suma_bolb != 0.)
 {
  suma_nal_b=suma_bolb*proc_vn/100.;
  suma_nal_b=iceb_u_okrug(suma_nal_b,0.01);
  sprintf(strsql,"%s-Бюджетный налог с больничных %.2f*%.2f/100.=%.2f\n",__FUNCTION__,suma_bolb,proc_vn,suma_nal_b);
  zar_pr_insw(strsql,wpredok);
 }

suma_nal=0.;
if(suma_bolb < suma_bol)
 {
  suma_nal=(suma_bol-suma_bolb)*proc_vn/100.; 
  suma_nal=iceb_u_okrug(suma_nal,0.01)*-1;
 }

sprintf(strsql,"%s-Налог с больничных %.2f*%.2f/100.=%.2f\n",__FUNCTION__,suma_bol-suma_bolb,proc_vn,suma_nal);
zar_pr_insw(strsql,wpredok);

koment.plus(" ",gettext("С больничного"));

zp.knu=kodvs_b;
strcpy(zp.shet,shet.ravno());

zapzarpw(&zp,suma_nal,d,mes,god,0,shet.ravno(),koment.ravno(),0,podr,"",wpredok);

if(shetb != NULL)
if(shetb[0] != '\0')
if(shet_b.getdlinna() > 1)
 {
  strcpy(zp.shet,shet_b.ravno());
  koment.plus(" ",gettext("бюджет"));
  zapzarpw(&zp,suma_nal_b,d,mes,god,0,shet_b.ravno(),koment.ravno(),0,podr,"",wpredok);
 }



}
