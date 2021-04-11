/* $Id: boln1w.c,v 1.12 2013/09/26 09:46:46 sasa Exp $ */
/*19.06.2019    01.02.2011      Белых А.И.      boln1w.c
Перерасчёт подоходного налога с учётом больничных
*/
#include        <math.h>
#include        "buhg_g.h"


extern short    kodpen;  /*Код пенсионных отчислений*/
extern float    procpen[2]; /*Процент отчисления в пенсионный фонд*/
extern float    procsoc; /*Процент отчисления в соц-страх*/
extern double   okrg;
extern short    kodpn;   /*Код подоходного налога*/
extern short kodpn_sbol;
extern class iceb_u_str kodpn_all;
extern short   *kodbl; /*Код больничного*/
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short    kodsocstr;  /*Код отчисления на соц-страх*/
extern short	*kodsocstrnv; //Коды не входящие в расчёт соцстраха
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short    *kodmp;   /*Коды материальной помощи*/
extern short	*kodnvpen; /*Коды не входящие в расчёт пенсионного отчисления*/

extern short	*kodsocstrnv; //Коды не входящие в расчёт соцстраха
extern short	*kodbzrnv; //Коды не входящие в расчёт отчислений на безработицу
extern SQL_baza bd;

double boln1w(int tn, //Табельный номер
short mp,short gp,
int podr,
const char *uder_only,
GtkWidget *wpredok)
{
int             i,i1;
short           den;
double          sn;
class iceb_u_str shet("");
SQL_str         row;
class SQLCURSOR cur;
char		strsql[1024];
short		kolzap=0;
struct ZARP     zp;
double bbb=0.;
int kolstr=0;
/*
printw("boln-%d %d.%d %d\n",tn,mp,gp,sovm);
refresh();
*/
if(kodbl == NULL)
  return(0);

iceb_u_dpm(&den,&mp,&gp,5);

/***********
zp.dz=den;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
*************/
zp.tabnom=tn;
zp.prn=1;
zp.knu=kodpn;
if(kodpn_sbol != 0)
 zp.knu=kodpn_sbol;
zp.dz=den;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

class iceb_u_int MES;
class iceb_u_int GOD;
class iceb_u_double SUM;
class iceb_u_int KNAH;

/*ищем больничные не в счёт месяца расчёта*/
sprintf(strsql,"select knah,suma,godn,mesn from Zarp where tabn=%d and datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' \
and prn='1' and mesn != %d and suma <> 0.",tn,gp,mp,gp,mp,mp);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(0.);
 }

if(kolstr == 0)
 {
  return(0.);
 }
double nah_suma_bol=0.;
short mes_bol;
short god_bol;
int knah=0;
while(cur.read_cursor(&row) != 0)
 {
  knah=atoi(row[0]);
  if(provkodw(kodbl,knah) >= 0)
   {
    mes_bol=atoi(row[3]);    
    nah_suma_bol=atof(row[1]);
    god_bol=atoi(row[2]);
    
    if((i1=MES.find(mes_bol)) == -1)
     {

      MES.plus(mes_bol,-1);
      GOD.plus(god_bol,-1);
      SUM.plus(nah_suma_bol,-1);
      KNAH.plus(knah,-1);
      kolzap++;
     }      
    else
     {
      SUM.plus(nah_suma_bol,i1);
     }
   }

 }


if(kolzap == 0)
  return(0.);
  

sn=0.;
short	mes=0,god=0;
double sumbol=0.;
for(i=0;i<kolzap;i++)
 {
  mes=MES.ravno(i);
  god=GOD.ravno(i);
  sumbol=SUM.ravno(i);
//  printw("mes=%d sumbol=%.2f\n",mes,sumbol);
//  OSTANOV();
      
    sprintf(strsql,"\n%s-Перерасчёт подоходного налога для больничного\n\
---------------------------------------------------\n",__FUNCTION__);
   zar_pr_insw(strsql,wpredok);

   sprintf(strsql,"%s-Дата перерасчёта:%d.%d Сумма больничного:%.2f\n",__FUNCTION__,mes,god,sumbol);
   zar_pr_insw(strsql,wpredok);


  if(iceb_u_sravmydat(1,mes,god,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
   {
//    boln1_pa(tn,podr,mp,gp,mes,god,sumbol,KNAH.ravno(i),uder_only,ff_prot,wpredok);
    continue;
   }

  double suma_podoh=0.;
  /*Смотрим какая там сумма подоходного уже посчитана*/
  sprintf(strsql,"select knah,suma from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31' and tabn=%d and prn='2' and suma <> 0. and godn=%d and mesn=%d",god,mes,god,mes,tn,god,mes);

  if(cur.make_cursor(&bd,strsql) < 0)
   {
    iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
    return(0.);
   }
  while(cur.read_cursor(&row) != 0)
   {
    if(iceb_u_proverka(kodpn_all.ravno(),row[0],0,1) == 0)
      suma_podoh+=atof(row[1]);
       
   }  
  sprintf(strsql,"%s-Сумма подоходного %.2f\n",__FUNCTION__,suma_podoh);
  zar_pr_insw(strsql,wpredok);
   
  double suma_esv=0.;
  double suma_esvb=0.;
//  zaresvw(tn,mes,god,podr,&suma_esv,&suma_esvb,1,sumbol,uder_only,wpredok);

  //расчёт подоходного налога
  bbb=ras_podoh1w(tn,mes,god,podr,suma_esv,suma_esvb,1,sumbol,uder_only,wpredok);

  sprintf(strsql,"%s-Сумма подоходного пересчитанная %.2f\n\
Разница %.2f-%.2f=%.2f\n",__FUNCTION__,bbb,bbb,suma_podoh,bbb-suma_podoh);
  zar_pr_insw(strsql,wpredok);
  bbb-=suma_podoh;      

  sprintf(strsql,"select shet from Uder where kod=%d",zp.knu);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s-%s %d",__FUNCTION__,gettext("Не найден код удержания"),zp.knu);
    iceb_menu_soob(strsql,wpredok);
   }
  else
    shet.new_plus(row[0]);

  zp.mesn=mes; zp.godn=god;
  strcpy(zp.shet,shet.ravno());
  if(iceb_u_proverka(uder_only,zp.knu,0,0) != 0)
   {
    sprintf(strsql,"%s-Код %d исключён из расчёта\n",__FUNCTION__,zp.knu);
    zar_pr_insw(strsql,wpredok);
   }
  else
    zapzarpw(&zp,bbb,den,mp,gp,0,shet.ravno(),gettext("Перерасчёт больничного"),0,podr,"",wpredok);

   
 }
return(sn);
}
