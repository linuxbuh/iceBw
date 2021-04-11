/*$Id: balans_f1w.c,v 1.17 2013/09/26 09:46:46 sasa Exp $*/
/*30.03.2016	15.03.2005	Белых А.И.	balans_f1w.c
Расчёт баланса по форме 1 (Додаток до Пооложення (стандарту) бухгалтерского обліку 2
*/
#include <errno.h>
#include "buhg_g.h"
#include "glktmpw.h"

void balans_f1_r(int metka_ras,int metkabd,const char *sheta,class iceb_u_spisok *masf,class iceb_u_double *ssd,class iceb_u_double *ssk,class iceb_u_double *deb_end,class iceb_u_double *kre_end,int nomer_kod,double *suma_kod1,double *suma_kod2,class iceb_u_spisok *kod);
void balans_f1_r1(int metkabd,const char *sheta,class iceb_u_spisok *masf,class iceb_u_double *ssd,class iceb_u_double *ssk,class iceb_u_double *deb_end,class iceb_u_double *kre_end,double *k1,double *k2,int nomer_kod);
void balans_f1_r2(const char *kodi,double *k1,double *k2,int nomer_kod,double *suma_kod1,double *suma_kod2,class iceb_u_spisok *kod);

extern char     *sbshet; //Список бюджетных счетов
extern SQL_baza bd;

void  balans_f1w(int metkabd, //0-хозрасчёт 1-бюджет
short dk,short mk,short gk,
char *imaf,
class GLKTMP *prom,
GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
SQLCURSOR cur;
int kolstr=0;
SQL_str row_alx;
class SQLCURSOR cur_alx;
class iceb_u_str naim_org("");
class iceb_u_str adres("");

//читаем реквизиты организации
sprintf(strsql,"select naikon,adres,pnaim from Kontragent where kodkon='00'");

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(row[2][0] != '\0')
   naim_org.new_plus(row[2]);
  else
   naim_org.new_plus(row[0]);

  adres.new_plus(row[1]);
  
   
 }

class iceb_u_str bros("");
sprintf(strsql,"select str from Alx where fil='glk_balans_n.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"glk_balans_n.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }


class iceb_u_spisok kod;
class iceb_u_spisok sheta;

class iceb_u_str stroka;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],&bros,1,'|') != 0)
   continue;
  if(bros.ravno()[0] == '\0')
   continue;
  kod.plus(bros.ravno());
  iceb_u_polen(row_alx[0],&bros,2,'|');
  /*sheta.plus(bros);*/
  iceb_fplus(1,bros.ravno(),&sheta,&cur_alx);

 }
double suma_kod1[kod.kolih()]; //Суммы по кодам
double suma_kod2[kod.kolih()]; //Суммы по кодам
memset(&suma_kod1,'\0',sizeof(suma_kod1));
memset(&suma_kod2,'\0',sizeof(suma_kod2));

for(int ii=0; ii < kod.kolih(); ii++) //Сначала считаем настройки со счетами
  balans_f1_r(0,metkabd,sheta.ravno(ii),&prom->masf,&prom->ssd,&prom->ssk,&prom->deb_end,&prom->kre_end,ii,suma_kod1,suma_kod2,&kod);
for(int ii=0; ii < kod.kolih(); ii++) //Потом считаем настройки с номерами строк
  balans_f1_r(1,metkabd,sheta.ravno(ii),&prom->masf,&prom->ssd,&prom->ssk,&prom->deb_end,&prom->kre_end,ii,suma_kod1,suma_kod2,&kod);

sprintf(strsql,"select str from Alx where fil='glk_balans.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"glk_balans.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }

sprintf(imaf,"b_f1_%d.lst",getpid());
FILE *ff_bal;
if((ff_bal = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
fprintf(ff_bal,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

int nomer_koda=0;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  stroka.new_plus(row_alx[0]);

  if(iceb_u_SRAV("Підприємство",stroka.ravno(),1) == 0)
   {
    iceb_u_vstav(&stroka,naim_org.ravno(),14,66,1);
   }
  if(iceb_u_SRAV("Адреса",stroka.ravno(),1) == 0)
   {
    iceb_u_vstav(&stroka,adres.ravno(),8,66,1);
   }
  if(iceb_u_strstrm(stroka.ravno(),"на_______") == 1)
   {
    sprintf(strsql,"%02d.%02d.%d р.",dk,mk,gk);
    iceb_u_vstav(&stroka,strsql,29,68,1);
   }
  

  if(iceb_u_polen(stroka.ravno(),&bros,3,'|') == 0)
  if(iceb_u_polen(stroka.ravno(),strsql,sizeof(strsql),4,'|') == 0)
   if(bros.ravno()[0] != '\0' && strsql[0] == '\0')
    {
     bros.ud_lp(); //удаляем лидирующие пробелы
     if((nomer_koda=kod.find(bros.ravno())) < 0)
      {
       sprintf(strsql,gettext("Не найден код строки %s в настройках !"),bros.ravno());
       iceb_u_vstav(&stroka,strsql,73,184,1);
//       iceb_menu_soob(strsql,wpredok);
      }
     else
      {
       sprintf(strsql,"%12.2f",suma_kod1[nomer_koda]);
       iceb_u_vstav(&stroka,strsql,73,84,1);
       sprintf(strsql,"%12.2f",suma_kod2[nomer_koda]);
       iceb_u_vstav(&stroka,strsql,86,97,1);

      }
    }  
  fprintf(ff_bal,"%s",stroka.ravno());
 }
iceb_podpis(ff_bal,wpredok);

fclose(ff_bal);

}

/**********************************/
/*Расчёт*/
/**********************************/

void balans_f1_r(int metka_ras,int metkabd,const char *sheta,
class iceb_u_spisok *masf,
class iceb_u_double *ssd,
class iceb_u_double *ssk,
class iceb_u_double *deb_end,
class iceb_u_double *kre_end,
int nomer_kod,double *suma_kod1,double *suma_kod2,class iceb_u_spisok *kod)
{
double k1=0.;
double k2=0.;
if(sheta[0] == '\0')
 return;

if(iceb_u_pole2(sheta,':') == 0) //если нет ":" значит суммируются уже посчитанные колонки
 {
  if(metka_ras == 0)
   return;
  balans_f1_r2(sheta,&k1,&k2,nomer_kod,suma_kod1,suma_kod2,kod);

  suma_kod1[nomer_kod]+=k1;
  suma_kod2[nomer_kod]+=k2;

  return;
 }

if(metka_ras == 1)
 return;

int kolpol=iceb_u_pole2(sheta,',');
if(kolpol == 0)
 {
  balans_f1_r1(metkabd,sheta,masf,ssd,ssk,deb_end,kre_end,&k1,&k2, nomer_kod);
  suma_kod1[nomer_kod]+=k1;
  suma_kod2[nomer_kod]+=k2;
  return;
 }
 
class iceb_u_str shet_metka("");
for(int i=0; i < kolpol; i++)
 {
  iceb_u_polen(sheta,&shet_metka,i+1,',');
  if(shet_metka.ravno()[0] == '\0')
   continue;
  balans_f1_r1(metkabd,shet_metka.ravno(),masf,ssd,ssk,deb_end,kre_end,&k1,&k2, nomer_kod);
 }

suma_kod1[nomer_kod]+=k1;
suma_kod2[nomer_kod]+=k2;
}
/***********************/
/*                      */
/*************************/
void balans_f1_r1(int metkabd,const char *sheta,
class iceb_u_spisok *masf,
class iceb_u_double *ssd,
class iceb_u_double *ssk,
class iceb_u_double *deb_end,
class iceb_u_double *kre_end,
double *k1,double *k2,
int nomer_kod)
{
class iceb_u_str shet("");
class iceb_u_str metka("");
class iceb_u_str znak("");
iceb_u_polen(sheta,&znak,1,':');
iceb_u_polen(sheta,&shet,2,':');
iceb_u_polen(sheta,&metka,3,':');
int nom_shet;
for(nom_shet=0; nom_shet <  masf->kolih(); nom_shet++)
 {
  if(iceb_u_SRAV(shet.ravno(), masf->ravno(nom_shet),1) != 0)
   continue;

  if(sbshet != NULL)
   {
    if(metkabd == 0) //хозрасчёт
     if(iceb_u_proverka(sbshet, masf->ravno(nom_shet),1,0) == 0)
       continue;

    if(metkabd == 1) //бюджет
     if(iceb_u_proverka(sbshet, masf->ravno(nom_shet),1,0) != 0)
       continue;
   }

  if(metka.ravno()[0] == 'k') //кредит
   {
    if(znak.ravno()[0] == '+')
     {
      *k1+= ssk->ravno(nom_shet);
      *k2+= kre_end->ravno(nom_shet);
     }
    if(znak.ravno()[0] == '-')
     {
      *k1-= ssk->ravno(nom_shet);
      *k2-= kre_end->ravno(nom_shet);
     }

   }
  if(metka.ravno()[0] == 'd') //дебит
   {
    if(znak.ravno()[0] == '+')
     {
      *k1+= ssd->ravno(nom_shet);
      *k2+= deb_end->ravno(nom_shet);
     }
    if(znak.ravno()[0] == '-')
     {
      *k1-= ssd->ravno(nom_shet);
      *k2-= deb_end->ravno(nom_shet);
     }
   }

    
 }

}
/**************************************/
/*Подсчёт итогов по кодам */
/***************************************/
void balans_f1_r2(const char *kodip,double *k1,double *k2,
int nomer_kod,double *suma_kod1,double *suma_kod2,class iceb_u_spisok *kod)
{
*k1=*k2=0.;
int nom_kod=0;
short metka=1;
class iceb_u_str kodi(kodip);

int kolpol=iceb_u_pole2(kodi.ravno(),',');
if(kolpol == 0)
 {
  if(kodi.ravno_atoi() < 0)
   metka=-1;  

  kodi.udsimv("+-");
      
  if((nom_kod=kod->find(kodi.ravno())) < 0)
   return;

  *k1+=suma_kod1[nom_kod]*metka;
  *k2+=suma_kod2[nom_kod]*metka;

 }
class iceb_u_str kodd("");

for(int i=0; i < kolpol; i++)
 {

  if(iceb_u_polen(kodi.ravno(),&kodd,i+1,',') != 0)
   break;
  if(kodd.ravno()[0] == '\0')
   continue;
  metka=1;
  if(kodd.ravno_atoi() < 0)
   metka=-1;

  kodd.udsimv("+-");

  if((nom_kod=kod->find(kodd.ravno())) < 0)
   continue;
  *k1+=suma_kod1[nom_kod]*metka;
  *k2+=suma_kod2[nom_kod]*metka;
 }
}


