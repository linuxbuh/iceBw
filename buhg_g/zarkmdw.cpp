/*$Id: zarkmd.c,v 5.19 2013/09/26 09:43:45 sasa Exp $*/
/*03.04.2020	05.05.2004	Белых А.И.	zarkmdw.c
Расчёт командировочных 
*/
#include        "buhg_g.h"

extern short    *kodkomand; //Коды командировочных расходов
extern short   *knvr_komand; //Коды начислений не входящие в расчёт командировочных
extern short   kodt_b_komand; //Код бюджетного табеля командировочных
extern short   *ktnvr_komand; //Коды табеля не входящие в расчёт командировочных
extern short kod_komp_z_kom; /*Код компенсации за командировку*/
extern double  okrg; /*Округление*/
extern char    *shetb; /*Бюджетные счета начислений*/
extern class iceb_u_str kod_prem_z_kvrt; /*коды премии за квартал*/

extern SQL_baza bd;

void zarkmdw(int tn, //Табельный номер*/
short mp,short gp,
int podr,
const char *nah_only,
int metka_r, /*0-расчёт с использованием дней 1-часов*/
FILE *ffo, /*файл с распечаткой*/
GtkWidget *wpredok)
{
char strsql[1024];
int kolstr;
short kodt_komand=0; //Код табеля командировочных
short metka_vk=0; //0-хозрасчёт 1-бюджет
double itogo_nah=0.; /*Общая сумма всех командировочных начислений*/
float itogo_dnei=0; /*Общая сумма дней в командировке*/
float itogo_hasov=0; /*Общая сумма часов в командировке*/
double itogo_nah_zpred2mes=0.; /*Итого начислено за передыдущих два месяца*/
float itogo_dnei_zpred2mes=0; /*Итого отработанных дней за предыдущих два месяца*/
float itogo_hasov_zpred2mes=0; /*Итого отработанных часов за предыдущих два месяца*/
class iceb_u_str shet("");

sprintf(strsql,"\n%s-Начисление командировочных\n\
------------------------------------------------\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);

  
if(kodkomand == NULL)
 {
  sprintf(strsql,"%s-Не введены коды командировочных начислений!\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return;

 }

if(metka_r == 0)
 sprintf(strsql,"%s-Расчёт с использованием отработанных дней\n",__FUNCTION__);
else
 sprintf(strsql,"%s-Расчёт с использованием отработанных часов\n",__FUNCTION__);

zar_pr_insw(strsql,wpredok);

SQL_str row;
SQLCURSOR cur;   
int kodkomand_sp=0;

sprintf(strsql,"%s-kodkomand[0]=%d\n",__FUNCTION__,kodkomand[0]);
zar_pr_insw(strsql,wpredok);


short d=0;

iceb_u_dpm(&d,&mp,&gp,5);

class ZARP     zp;
zp.tabnom=tn;
zp.prn=1;
zp.knu=0;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; 
zp.godn=gp;
zp.nomz=0;
zp.podr=podr;



short dn,mn,gn;
short dk=1,mk=mp,gk=gp;


class iceb_u_str kod_prem_z_god("");
iceb_poldan("Коды премии за год",&kod_prem_z_god,"zarnast.alx",0,wpredok);

iceb_u_dpm(&dk,&mk,&gk,4); //уменьшить дату на месяц
/*определяем количество рабочих дней в этом месяце*/
int kol_rd[2];
memset(kol_rd,'\0',sizeof(kol_rd));
float kol_rh[2];
memset(kol_rh,'\0',sizeof(kol_rh));

int kol_rd_v_mr=0; /*Количество рабочих дней в месяце расчёта*/
float kol_rh_v_mr=0.; /*Количество рабочих часов в месяце расчёта*/

sprintf(strsql,"select krd,krh from Zarskrdh where data='%04d-%02d-01'",gp,mp);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  kol_rd_v_mr=atoi(row[0]);
  kol_rh_v_mr=atof(row[1]);
 }
sprintf(strsql,"%s-Количество рабочих дней в %02d.%04d=%d Часов %f\n",__FUNCTION__,mp,gp,kol_rd_v_mr,kol_rh_v_mr);
zar_pr_insw(strsql,wpredok);

sprintf(strsql,"select krd,krh from Zarskrdh where data='%04d-%02d-01'",gk,mk);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  kol_rd[0]=atoi(row[0]);
  kol_rh[0]=atof(row[1]);
 }

sprintf(strsql,"%s-Количество рабочих дней в %02d.%04d=%d Часов %f\n",__FUNCTION__,mk,gk,kol_rd[0],kol_rh[0]);
zar_pr_insw(strsql,wpredok);

 
 
dn=dk; mn=mk; gn=gk;

iceb_u_dpm(&dk,&mk,&gk,5); //получить последний день месяца

iceb_u_dpm(&dn,&mn,&gn,4); //уменьшить дату на месяц

sprintf(strsql,"select krd,krh from Zarskrdh where data='%04d-%02d-01'",gn,mn);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  kol_rd[1]=atoi(row[0]);
  kol_rh[1]=atof(row[1]);
 }
sprintf(strsql,"%s-Количество рабочих дней в %02d.%04d=%d Часов %f\n",__FUNCTION__,mn,gn,kol_rd[1],kol_rh[1]);
zar_pr_insw(strsql,wpredok);

sprintf(strsql,"%s-Коды квартальной премии-%s\n",__FUNCTION__,kod_prem_z_kvrt.ravno());
zar_pr_insw(strsql,wpredok);

sprintf(strsql,"%s-Коды премии за год-%s\n",__FUNCTION__,kod_prem_z_god.ravno());
zar_pr_insw(strsql,wpredok);

sprintf(strsql,"%s-Определяем количество отработанных дней за 2 предыдущих месяца\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);
 
//Смотрим отработанное время

sprintf(strsql,"select kodt,dnei,has from Ztab where tabn=%d and ((god=%d and mes=%d) or (god=%d and mes=%d))",tn,gn,mn,gk,mk);

zar_pr_insw(strsql,wpredok);
zar_pr_insw("\n",wpredok);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(ktnvr_komand,atoi(row[0])) >= 0)
   continue;

  sprintf(strsql,"%s %s %s\n",row[0],row[1],row[2]);
  zar_pr_insw(strsql,wpredok);
  
  itogo_dnei_zpred2mes+=atof(row[1]);
  itogo_hasov_zpred2mes+=atof(row[2]);

 }
if(metka_r == 0)
 {
  if(itogo_dnei_zpred2mes == 0)
   {
    sprintf(strsql,"%s-Количество отработанных дней за предыдущих два месяца равно нолю!\n",__FUNCTION__);
    zar_pr_insw(strsql,wpredok);
    return;
   }
  else
   {
    sprintf(strsql,"%s-Количество отработанных дней за предыдущих два месяца=%f!\n",__FUNCTION__,itogo_dnei_zpred2mes);
    zar_pr_insw(strsql,wpredok);
   } 
 }
else
 {
  if(itogo_hasov_zpred2mes == 0)
   {
    sprintf(strsql,"%s-Количество отработанных часоа за предыдущих два месяца равно нолю!\n",__FUNCTION__);
    zar_pr_insw(strsql,wpredok);
    return;
   }
  else
   {
    sprintf(strsql,"%s-Количество отработанных часов за предыдущих два месяца=%f!\n",__FUNCTION__,itogo_hasov_zpred2mes);
    zar_pr_insw(strsql,wpredok);
   } 
 }
sprintf(strsql,"\n%s-Определяем количество отработанных дней в расчётном месяце\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);
 
//Смотрим отработанное время

sprintf(strsql,"select kodt,dnei,has from Ztab where tabn=%d and god=%d and mes=%d",tn,gp,mp);

zar_pr_insw(strsql,wpredok);
zar_pr_insw("\n",wpredok);

float kol_od_vrm=0; /*количество отработанных дней в расчётном месяце*/
float kol_od_has=0.; /*количество отработанных часоа в расчётном месяце*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(ktnvr_komand,atoi(row[0])) >= 0)
   continue;

  sprintf(strsql,"%s %s %s\n",row[0],row[1],row[2]);
  zar_pr_insw(strsql,wpredok);
  
  kol_od_vrm+=atof(row[1]);
  kol_od_has+=atof(row[2]);
 }

 
sprintf(strsql,"%s-Количество отработанных дней в расчётном месяце=%f Часов %f\n",__FUNCTION__,kol_od_vrm,kol_od_has);
zar_pr_insw(strsql,wpredok);

if(metka_r == 1) /*расчёт с учётом отработанных часов*/
 kol_od_vrm=kol_od_has;


/*Смотрим начисления за 2 предыдущих месяца*/
sprintf(strsql,"select knah,suma,shet from Zarp where datz >= '%d-%d-%d' \
and datz <= '%d-%d-%d' and tabn=%d and prn='1' and suma != 0.",
gn,mn,dn,gk,mk,dk,tn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

double suma=0.;

sprintf(strsql,"\n%s-Начисления взятые в расчёт за два предыдущих месяца\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);
itogo_nah_zpred2mes=0.;
double suma_nah=0;
while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(knvr_komand,atoi(row[0])) >= 0)
   continue;

  suma_nah=atof(row[1]);

  itogo_nah_zpred2mes+=suma_nah;

  
  if(iceb_u_proverka(kod_prem_z_kvrt.ravno(),row[0],0,1) == 0)
   {
    sprintf(strsql,"%s-%3s %10s %s\n",__FUNCTION__,row[0],row[1],row[2]);
    zar_pr_insw(strsql,wpredok);
   
    sprintf(strsql,"%s-Квартальная премия\n",__FUNCTION__);
    zar_pr_insw(strsql,wpredok);
    
    double suma_vr=0.;

    if(metka_r == 0) /*расчёт с использованием дней*/
     {
      if(kol_rd[0]+kol_rd[1] > 0)          
       suma_vr=(suma_nah/3*2)/(kol_rd[0]+kol_rd[1])*itogo_dnei_zpred2mes;
      suma+=suma_vr;
    
      sprintf(strsql,"%s-Берём в расчёт-(%.2f/3*2/(%d+%d)*%f=%.2f\n",__FUNCTION__,suma_nah,kol_rd[0],kol_rd[1],itogo_dnei_zpred2mes,suma_vr);
      zar_pr_insw(strsql,wpredok);
     }
    else /*расчёт с использованием часов*/
     {
      if(kol_rh[0]+kol_rh[1] > 0)          
       suma_vr=(suma_nah/3*2)/(kol_rh[0]+kol_rh[1])*itogo_hasov_zpred2mes;
      suma+=suma_vr;
    
      sprintf(strsql,"%s-Берём в расчёт-(%.2f/3*2/(%f+%f)*%f=%.2f\n",__FUNCTION__,suma_nah,kol_rh[0],kol_rh[1],itogo_hasov_zpred2mes,suma_vr);
      zar_pr_insw(strsql,wpredok);

     }     
    continue;
   }
 
  if(iceb_u_proverka(kod_prem_z_god.ravno(),row[0],0,1) == 0)
   {
    sprintf(strsql,"%s-%3s %10s %s\n",__FUNCTION__,row[0],row[1],row[2]);
    zar_pr_insw(strsql,wpredok);
   
    sprintf(strsql,"%s-Годовая премия\n",__FUNCTION__);
    zar_pr_insw(strsql,wpredok);
    
    double suma_vr=0.;

    if(metka_r == 0) /*расчёт с использованием дней*/
     {
      if(kol_rd[0]+kol_rd[1] > 0)          
       suma_vr=(suma_nah/12*2)/(kol_rd[0]+kol_rd[1])*itogo_dnei_zpred2mes;
      suma+=suma_vr;
    
      sprintf(strsql,"%s-Берём в расчёт-(%.2f/12*2/(%d+%d)*%f=%.2f\n",__FUNCTION__,suma_nah,kol_rd[0],kol_rd[1],itogo_dnei_zpred2mes,suma_vr);
      zar_pr_insw(strsql,wpredok);
     }
    else
     {
      if(kol_rh[0]+kol_rh[1] > 0)          
       suma_vr=(suma_nah/12*2)/(kol_rh[0]+kol_rh[1])*itogo_hasov_zpred2mes;
      suma+=suma_vr;
    
      sprintf(strsql,"%s-Берём в расчёт-(%.2f/12*2/(%f+%f)*%f=%.2f\n",__FUNCTION__,suma_nah,kol_rh[0],kol_rh[1],itogo_hasov_zpred2mes,suma_vr);
      zar_pr_insw(strsql,wpredok);
     }
    continue;
   }

  suma+=suma_nah;
  sprintf(strsql,"%s-%3s %10s %s\n",__FUNCTION__,row[0],row[1],row[2]);
  zar_pr_insw(strsql,wpredok);

 }
sprintf(strsql,"%s-Итого за предыдущих два месяца=%.2f\n",__FUNCTION__,suma);
zar_pr_insw(strsql,wpredok);

double suma_zrm=0.; /*сумма начислений взятых в расчёт за расчётный месяц*/

/*Смотрим начисления за расчётный месяц*/
sprintf(strsql,"select knah,suma,shet from Zarp where datz >= '%d-%d-%d' \
and datz <= '%d-%d-%d' and tabn=%d and prn='1' and suma != 0.",
gp,mp,1,gp,mp,31,tn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);


sprintf(strsql,"\n%s-Начисления взятые в расчёт за расчётный месяц\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);
while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(knvr_komand,atoi(row[0])) >= 0)
   continue;

  sprintf(strsql,"%s-%3s %10s %s\n",__FUNCTION__,row[0],row[1],row[2]);
  zar_pr_insw(strsql,wpredok);

  if(iceb_u_proverka(kod_prem_z_kvrt.ravno(),row[0],0,1) == 0)
   {
    sprintf(strsql,"%s-Квартальная премия\n",__FUNCTION__);
    zar_pr_insw(strsql,wpredok);
    
    double suma_vr=0.;
    if(metka_r == 0) /*расчёт с использованием дней*/
     {
      if(kol_rd[0]+kol_rd[1] > 0)          
       suma_vr=(atof(row[1])/3)/kol_rd_v_mr*kol_od_vrm;
      suma_zrm+=suma_vr;
    
      sprintf(strsql,"%s-Берём в расчёт-(%.2f/3/%d*%f=%.2f\n",__FUNCTION__,atof(row[1]),kol_rd_v_mr,kol_od_vrm,suma_vr);
      zar_pr_insw(strsql,wpredok);
     }
    else
     {
      if(kol_rh[0]+kol_rh[1] > 0)          
       suma_vr=(atof(row[1])/3)/kol_rh_v_mr*kol_od_vrm;
      suma_zrm+=suma_vr;
    
      sprintf(strsql,"%s-Берём в расчёт-(%.2f/3/%f*%f=%.2f\n",__FUNCTION__,atof(row[1]),kol_rh_v_mr,kol_od_vrm,suma_vr);
      zar_pr_insw(strsql,wpredok);
     }
   }
  else
   suma_zrm+=atof(row[1]);
 }

sprintf(strsql,"%s-Итого за расчётный месяц=%.2f\n",__FUNCTION__,suma_zrm);
zar_pr_insw(strsql,wpredok);

class iceb_u_str fio("");
if(ffo != NULL)
 {
  sprintf(strsql,"select fio from Kartb where tabn=%d",tn);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
   fio.new_plus(row[0]);  
 }
for(int shetkk=1; shetkk <= kodkomand[0]; shetkk++)
 {
  zp.knu=kodkomand_sp=kodkomand[shetkk];
  if(iceb_u_proverka(nah_only,kodkomand_sp,0,0) != 0)
   {
    sprintf(strsql,"%s-Код начисления %d исключён из расчёта\n",__FUNCTION__,kodkomand_sp);
    zar_pr_insw(strsql,wpredok);
    continue;
   }
  
  sprintf(strsql,"%s-Код начисления командировочных расходов %d\n",__FUNCTION__,kodkomand_sp);
  zar_pr_insw(strsql,wpredok);
  sprintf(strsql,"%s-Ищем код табеля для начисления %d\n",__FUNCTION__,kodkomand_sp);
  zar_pr_insw(strsql,wpredok);

  sprintf(strsql,"select kodt from Nash where kod=%d",kodkomand_sp);
  zar_pr_insw(strsql,wpredok);
  zar_pr_insw("\n",wpredok);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s-Не найден код начисления %d для расчёта командировочных !\n",__FUNCTION__,kodkomand_sp);
    zar_pr_insw(strsql,wpredok);
    continue;
   }   

  sprintf(strsql,"%s-Код табеля %s\n",__FUNCTION__,row[0]);   
  zar_pr_insw(strsql,wpredok);

  kodt_komand=atoi(row[0]);
  if(kodt_komand == 0)
   {
    sprintf(strsql,"%s-Не введён код табеля для кода командировочных %d !\n",__FUNCTION__,kodkomand_sp);
    zar_pr_insw(strsql,wpredok);
    continue;
   }

  float dnei=0.;
  float hasov=0.;
  if(kodt_b_komand == kodt_komand)
   {
    sprintf(strsql,"%s-Бюджетный код табеля командировочных расходов %d\n",__FUNCTION__,kodt_b_komand);
    zar_pr_insw(strsql,wpredok);
     
    //Проверяем введён ли табель
    sprintf(strsql,"select dnei,has from Ztab where tabn=%d and god=%d \
and mes=%d and kodt=%d",tn,gp,mp,kodt_b_komand);
    zar_pr_insw(strsql,wpredok);
    zar_pr_insw("\n",wpredok);

    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    if(kolstr > 0) 
     while(cur.read_cursor(&row) != 0)
      {
       dnei+=atof(row[0]);
       hasov+=atof(row[1]);
       sprintf(strsql,"%s-%s %s\n",__FUNCTION__,row[0],row[1]);
       zar_pr_insw(strsql,wpredok);
      }

    if(metka_r == 0)
     if(dnei != 0)
      metka_vk=1; 

    if(metka_r == 1)
     if(hasov != 0)
      metka_vk=1; 
   }

  if(metka_vk == 0)
   {
    //Проверяем введён ли табель
    sprintf(strsql,"select dnei,has from Ztab where tabn=%d and god=%d \
 and mes=%d and kodt=%d",tn,gp,mp,kodt_komand);
    zar_pr_insw(strsql,wpredok);
    zar_pr_insw("\n",wpredok);

    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

    if(kolstr > 0)
     while(cur.read_cursor(&row) != 0)
      {
       sprintf(strsql,"%s-%s %s\n",__FUNCTION__,row[0],row[1]);
       zar_pr_insw(strsql,wpredok);
       dnei+=atof(row[0]);
       hasov+=atof(row[1]);
      }
   }
   
  sprintf(strsql,"%s-Итого дней=%f часов %f\n",__FUNCTION__,dnei,hasov);
  zar_pr_insw(strsql,wpredok);

  if(metka_r == 0)    
   if(dnei == 0)
    continue;   
  if(metka_r == 1)    
   if(hasov == 0)
    continue;   

  itogo_dnei+=dnei;
  itogo_hasov+=hasov;    
  shet.new_plus("");
  
  //Проверяем есть ли начисление в карточке
  sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and knah=%d",tn,kodkomand_sp);

  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
   {
    sprintf(strsql,"%s-Не нашли начисления в карточке работника!",__FUNCTION__);
    zar_pr_insw(strsql,wpredok);
    continue;
   }    

  shet.new_plus(row[0]);



  double sum_kr=0.; /*сумма командировочных из расчёта средней зарплаты за 2 предыдущих месяца*/
  double sum_kr_r=0.; /*сумма командировочных из расчёта средней зарплаты за расчётный месяц*/
  
  if(metka_r == 0)
   {
    if(itogo_dnei_zpred2mes != 0)
     {
      sum_kr=suma/itogo_dnei_zpred2mes*dnei;
      sum_kr=iceb_u_okrug(sum_kr,okrg);
     }

    if(kol_od_vrm != 0)
     {
      sum_kr_r=suma_zrm/kol_od_vrm*dnei;
      sum_kr_r=iceb_u_okrug(sum_kr_r,okrg);
     }
    sprintf(strsql,"%s-Командировочные из расчёта средней зарплаты за 2 предыдущих месяца %.2f/%f*%f=%.2f\n",
    __FUNCTION__,suma,itogo_dnei_zpred2mes,dnei,sum_kr);
    zar_pr_insw(strsql,wpredok);

    sprintf(strsql,"%s-Командировочные из расчёта средней зарплаты за месяц расчёта %.2f/%f*%f=%.2f\n",
    __FUNCTION__,suma_zrm,kol_od_vrm,dnei,sum_kr_r);
    zar_pr_insw(strsql,wpredok);


   }
  else
   {
    if(itogo_hasov_zpred2mes != 0)
     {
      sum_kr=suma/itogo_hasov_zpred2mes*hasov;
      sum_kr=iceb_u_okrug(sum_kr,okrg);
     }

    if(kol_od_has != 0)
     {
      sum_kr_r=suma_zrm/kol_od_has*hasov;
      sum_kr_r=iceb_u_okrug(sum_kr_r,okrg);
     }
    sprintf(strsql,"%s-Командировочные из расчёта средней зарплаты за 2 предыдущих месяца %.2f/%f*%f=%.2f\n",
    __FUNCTION__,suma,itogo_hasov_zpred2mes,hasov,sum_kr);
    zar_pr_insw(strsql,wpredok);

    sprintf(strsql,"%s-Командировочные из расчёта средней зарплаты за месяц расчёта %.2f/%f*%f=%.2f\n",
    __FUNCTION__,suma_zrm,kol_od_has,hasov,sum_kr_r);
    zar_pr_insw(strsql,wpredok);
   }

  
  if(ffo != NULL)
   fprintf(ffo,"%6d|%-*.*s|%10.2f|%10.2f|\n",
   tn,
   iceb_u_kolbait(30,fio.ravno()),
   iceb_u_kolbait(30,fio.ravno()),
   fio.ravno(),
   sum_kr,
   sum_kr_r);
     
  
  double suma_v=0.; /*выбранная сумма*/
  class iceb_u_str koment("");

  if(sum_kr >= sum_kr_r)
   {
    sprintf(strsql,"%s-Сумма %.2f >= %.2f\n",__FUNCTION__,sum_kr,sum_kr_r);
    zar_pr_insw(strsql,wpredok);
    koment.new_plus(gettext("Среднедневной заработок взят в расчёт"));

    zar_pr_insw(__FUNCTION__,wpredok);
    zar_pr_insw("-",wpredok);
    zar_pr_insw(koment.ravno(),wpredok);
    zar_pr_insw("\n",wpredok);

    suma_v=sum_kr;
   }
  else
   {
    sprintf(strsql,"Сумма %.2f < %.2f\n",sum_kr,sum_kr_r);
    zar_pr_insw(strsql,wpredok);
    koment.new_plus(gettext("Дневной заработок взят в расчёт"));

    zar_pr_insw(__FUNCTION__,wpredok);
    zar_pr_insw("-",wpredok);

    zar_pr_insw(koment.ravno(),wpredok);
    zar_pr_insw("\n",wpredok);

    suma_v=sum_kr_r;
   }

  strcpy(zp.shet,shet.ravno());

  if(ffo == NULL)
   {
    strcpy(zp.shet,shet.ravno());
    zapzarpw(&zp,suma_v,d,mp,gp,0,shet.ravno(),koment.ravno(),0,podr,"",wpredok);
   }
//    zapzarpw(&zp,suma_v,d,mp,gp,0,shet.ravno(),koment.ravno(),0,podr,"",wpreok);
 
  itogo_nah+=sum_kr;
  
 }

sprintf(strsql,"%s-Код компенсации за командировку=%d\n",__FUNCTION__,kod_komp_z_kom);
zar_pr_insw(strsql,wpredok);

if(kod_komp_z_kom == 0)
 return;

sprintf(strsql,"select kodt from Nash where kod=%d",kod_komp_z_kom);
zar_pr_insw(strsql,wpredok);
zar_pr_insw("\n",wpredok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s-Не найден код компенсации за командировку %d для расчёта командировочных !\n",
  __FUNCTION__,kod_komp_z_kom);
  zar_pr_insw(strsql,wpredok);
  return;
 }   

shet.new_plus("");

//Проверяем есть ли начисление в карточке
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and knah=%d",tn,kod_komp_z_kom);
zar_pr_insw(strsql,wpredok);
zar_pr_insw("\n",wpredok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 {
  sprintf(strsql,"%s-Не найдено код компенсации в списке начислений в карточке работкика!",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
 
  return;

 }  

sprintf(strsql,"%s-Есть начисление %d в карточке!\n",__FUNCTION__,kod_komp_z_kom);
zar_pr_insw(strsql,wpredok);
 
shet.new_plus(row[0]);

sprintf(strsql,"\n%s-Расчёт компенсации за командировку\n\
---------------------------------------------------------\n",__FUNCTION__); 
zar_pr_insw(strsql,wpredok);

if(itogo_nah == 0.)
 {
  sprintf(strsql,"%s-Сумма командировочных равна нолю!",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return; 
 }
if(metka_r == 0)
 if(itogo_dnei == 0)
  {
   sprintf(strsql,"%s-Количество дней в командировке равно нолю!\n",__FUNCTION__);
   zar_pr_insw(strsql,wpredok);
   return;
  }
if(metka_r == 1)
 if(itogo_hasov == 0)
  {
   sprintf(strsql,"%s-Количество часов в командировке равно нолю!\n",__FUNCTION__);
   zar_pr_insw(strsql,wpredok);
   return;
  }

double snah_kom=0.;
double snah_zp2mes=0.;
if(metka_r == 0)
 {
  snah_kom=itogo_nah/itogo_dnei;
  snah_zp2mes=itogo_nah_zpred2mes/itogo_dnei_zpred2mes;
 }
else
 {
  snah_kom=itogo_nah/itogo_hasov;
  snah_zp2mes=itogo_nah_zpred2mes/itogo_hasov_zpred2mes;
 }
if(metka_r == 0)
  sprintf(strsql,"%s-\
Общая сумма командировочных=%.2f\n\
Общее количество дней в командировке=%f\n\
Среднее начисление за один день=%.2f/%f=%f\n\
Начислено за предыдущих два месяца=%.2f\n\
Количество отработанных дней за предыдущих два месяца=%f\n\
Среднее начисление за 1 день за предыдущих два месяца=%f\n",
__FUNCTION__,
itogo_nah,
itogo_dnei,
itogo_nah,
itogo_dnei,
snah_kom,
itogo_nah_zpred2mes,
itogo_dnei_zpred2mes,
snah_zp2mes);
else
  sprintf(strsql,"%s-\
Общая сумма командировочных=%.2f\n\
Общее количество дней в командировке=%f\n\
Среднее начисление за один день=%.2f/%f=%f\n\
Начислено за предыдущих два месяца=%.2f\n\
Количество отработанных дней за предыдущих два месяца=%f\n\
Среднее начисление за 1 день за предыдущих два месяца=%f\n",
__FUNCTION__,
itogo_nah,
itogo_hasov,
itogo_nah,
itogo_hasov,
snah_kom,
itogo_nah_zpred2mes,
itogo_hasov_zpred2mes,
snah_zp2mes);

zar_pr_insw(strsql,wpredok);


if(snah_zp2mes < snah_kom)
 {
  sprintf(strsql,"%s-Средняя зарплата за предыдущих 2 месяца меньше среднего начисления за командировку!",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return;
 }
double raznica=snah_zp2mes-snah_kom;
double suma_komp_zkom=raznica*itogo_dnei;

sprintf(strsql,"%s-\
Разница между средними начислениями=%f-%f=%f\n\
Умножаем на количество дней в командировке=%f*%f=%f\n",
 __FUNCTION__,
 snah_zp2mes,
 snah_kom,
 raznica,
 raznica,
 itogo_dnei,
 suma_komp_zkom);  

zar_pr_insw(strsql,wpredok);

strcpy(zp.shet,shet.ravno());
zp.knu=kod_komp_z_kom;

if(ffo == NULL)
 {
  strcpy(zp.shet,shet.ravno());
  zp.knu=kod_komp_z_kom;
  zapzarpw(&zp,suma_komp_zkom,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
 }
// zapzarp(&zp,suma_komp_zkom,d,mp,gp,0,shet.ravno(),"",0,podr,"");

}
