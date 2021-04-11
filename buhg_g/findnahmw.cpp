/* $Id: findnahmw.c,v 1.13 2013/09/26 09:46:47 sasa Exp $ */
/*10.07.2015    10.12.1993      Белых А.И.      findnahmw.c
Поиск всех начислений на нужного человека за указанный
месяц и год
*/

#include        "buhg_g.h"


int findnagm_prov(short mt,short mr,int prnu,short mes,int knu,double suma,char *sheta,
short *kodi,double *suma_nfnz,double *suma_nfnz_b,double *suma_zarp_o,double *sumabu,
double *sumanb,double *suma_zarp_b,double *nah,double *suma_boln_rm);

extern short    *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/
extern short    *kuvr;/*Коды удержаний входящие в расчёт подоходного нолога*/
extern char	*shetb; /*Бюджетные счета*/
extern  short   *kodbl; /*Код больничного*/
extern short    *kuvvr_pens_sr; //Коды удержаний входящие в расчёт при отчислении с работника
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern SQL_baza bd;
/**************/
/*Чтение в базе*/
/***************/

double          findnahmw(short mr,short gr, //Дата расчёта
int tabn,
short mt, //1- подоходный налог
          //2 - пенсионные отчисления
          //3 - профсоюз
          //4 - отчисления в фонд безработицы
          //5 - соц-страх
short *kodi, //Дополнительные коды не входящие в расчёт*/
double *sumanb, //Сумма не бюджетная
double *sumabu, //Сумма бюджетная
double *suma_nfnz, //Общая сумма начисленная не денежными формами оплаты
double *suma_nfnz_b, //Бюджетная сумма начисленная не денежными формами оплаты
double *suma_zarp_o, //Сумма зарплатных начислений (на которые распространяется социальная льгота) общая
double *suma_zarp_b, //Сумма зарплатных начислений (на которые распространяется социальная льгота) бюджета
double *suma_boln_rm, /*Сумма больничных за расчётный месяц*/
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int kolstr;
double nah;

*suma_zarp_o=*suma_zarp_b=0.;
*sumanb=*sumabu=nah=0.;
*suma_nfnz=*suma_nfnz_b=0.;
*suma_boln_rm=0.;

sprintf(strsql,"select prn,knah,suma,shet,mesn from Zarp where datz >= '%04d-%02d-%02d' \
and datz <= '%04d-%02d-%02d' and tabn=%d",gr,mr,1,gr,mr,31,tabn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(0.);
 }
while(cur.read_cursor(&row) != 0)
 {
  findnagm_prov(mt,mr,atoi(row[0]),atoi(row[4]),atoi(row[1]),atof(row[2]),row[3],
  kodi,suma_nfnz,suma_nfnz_b,suma_zarp_o,
  sumabu,sumanb,suma_zarp_b,&nah,suma_boln_rm);

 }

return(nah);

}
/*******************************************/
/*Проверка общая для двух подпрограмм*/
/*****************************************/
//Если вернули 0-всё впорядке 1-прервать цикл
int findnagm_prov(short mt,short mr,int prnu,short mes,int knu,double suma,char *sheta,
short *kodi,
double *suma_nfnz,
double *suma_nfnz_b,
double *suma_zarp_o,
double *sumabu,
double *sumanb,
double *suma_zarp_b,
double *nah,
double *suma_boln_rm) /*Сумма больничных за расчётный месяц*/
{

if(prnu == 0)
 return(1);
 
//  printw("%.2f\n",suma);
char strsql[1024];

if(prnu == 1)
 {
//     if(ff_prot != NULL)
//        sprintf(strsql,"mr=%d mes[%d]=%d mt=%d knu=%d\n",mr,i,mes[i],mt,knu);

  /*Узнаём сумму больничных за расчётный месяц*/
   if(mes == mr)
     if(provkodw(kodbl,knu) >= 0)
      {
       sprintf(strsql,"%s-Больничный %d %.2f\n",__FUNCTION__,knu,suma);
       zar_pr_insw(strsql,NULL);
       *suma_boln_rm+=suma;
      }

  /*Если больничный не за текущий месяц то в общюю сумму не включ.*/
  /*Для расчёта профсоюзного взносов включается*/
  /*Для этих удержаний делается перерасчёт по этим больничным отдельно*/
  
  if(mt == 1 || mt == 2 || mt == 5)
   if(mes != mr)
     if( provkodw(kodbl,knu) >= 0)
      {
       sprintf(strsql,"%s-Не вошло в расчёт %d %.2f\n",__FUNCTION__,knu,suma);
       zar_pr_insw(strsql,NULL);
       return(0);
      }
  /*Коды начислений не входящие в расчёт подоходного налога*/
  if(mt == 1)
  if(provkodw(knvr,knu) >= 0)
   {
    sprintf(strsql,"%s-Не вошло в расчёт %d %.2f\n",__FUNCTION__,knu,suma);
    zar_pr_insw(strsql,NULL);
    return(0);
   }
   
  if(provkodw(kodi,knu) >= 0)
   {
    sprintf(strsql,"%s-Не вошло в расчёт %d %.2f\n",__FUNCTION__,knu,suma);
    zar_pr_insw(strsql,NULL);
    return(0);
   }
  *suma_nfnz+=knnfndsw(1,knu,sheta,suma,suma_nfnz_b,0);

/*********
  if(provkodw(knnf,knu) >= 0)
   {
    if(ff_prot != NULL)
     sprintf(strsql,"Не денежная форма оплаты (общая) %d %.2f\n",knu,suma);
    *suma_nfnz+=suma;
    if(iceb_u_proverka(shetb,sheta,0,1) == 0)
     {
      if(ff_prot != NULL)
       sprintf(strsql,"Не денежная форма оплаты (бюджет) %d %.2f\n",knu,suma);
      *suma_nfnz_b+=suma;  
     }
   }
************/  
  if(provkodw(kn_bsl,knu) < 0)
   {
    *suma_zarp_o+=suma;
   }

  if(shetb != NULL ) 
   {
    /*Узнаем какая часть суммы бюджетных счетов и всех остальных*/
    if(iceb_u_proverka(shetb,sheta,0,0) == 0)
     {
      *sumabu+=suma;
      if(provkodw(kn_bsl,knu) < 0)
        *suma_zarp_b+=suma;
     }
    else
        *sumanb+=suma;
    
/*************    
    int i1=0;
    if(pole1(shetb,sheta,',',0,&i1) == 0)
        *sumabu+=suma;
    else
        *sumanb+=suma;
**************/


   }

  *nah+=suma;
 
 }

if(prnu == 2)
 {
  /*Коды удержаний входящие в расчёт подоходного налога*/

  if(mt == 1)  /*Для пенсии и профсоюза входят*/
  if(provkodw(kuvr,knu) >= 0)
   {
    *nah+=suma; //Удержание отрицательное поэтому для того, чтобы уменьшить сумму мы его просто прибавляем
    sprintf(strsql,"%s-Вошло в расчёт удержание %d %.2f\n",__FUNCTION__,knu,suma);
    zar_pr_insw(strsql,NULL);
    return(0);
   }

  if(mt == 2)  //Коды удержаний входящие в расчёт для пенсионного отчисления с работника
  if(provkodw(kuvvr_pens_sr,knu) >= 0)
   {
    *nah+=suma; //Удержание отрицательное поэтому для того, чтобы уменьшить сумму мы его просто прибавляем
    sprintf(strsql,"%s-Вошло в расчёт удержание %d %.2f\n",__FUNCTION__,knu,suma);
    zar_pr_insw(strsql,NULL);
    return(0);
   }

 }
return(0);
}

