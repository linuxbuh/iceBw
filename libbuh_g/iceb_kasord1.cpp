/*$Id: iceb_kasord1.c,v 1.23 2013/11/05 10:51:28 sasa Exp $*/
/*16.06.2018	03.10.2000	Белых А.И.	iceb_kasord1.c
Распечатка приходного кассового ордера
*/
#include        <errno.h>
#include	"iceb_libbuh.h"

extern SQL_baza	bd;

void iceb_kasord1_0(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,double suma,const char *fio,const char *osnov,const char *dopol,GtkWidget *wpredok);
void iceb_kasord1_1(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,double suma,const char *fio,const char *osnov,const char *dopol,const char*,FILE *ff_f,GtkWidget *wpredok);
void iceb_kasord1_2(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,double suma,const char *fio,const char *osnov,const char *dopol,const char*,FILE *ff_f,GtkWidget *wpredok);

void	iceb_kasord1(const char *imaf,
const char *nomd,
short dd,short md,short gd,
const char *shet,
const char *shetkas,
double suma,const char *fio,const char *osnov,const char *dopol,
const char *kod_cel_naz,
GtkWidget *wpredok)
{
if(iceb_u_sravmydat(dd,md,gd,16,6,2009) < 0)
  iceb_kasord1_0(imaf,nomd,dd,md,gd,shet,shetkas,suma,fio,osnov,dopol,wpredok);
if(iceb_u_sravmydat(dd,md,gd,16,6,2009) && iceb_u_sravmydat(dd,md,gd,5,1,2018) < 0)
  iceb_kasord1_1(imaf,nomd,dd,md,gd,shet,shetkas,suma,fio,osnov,dopol,kod_cel_naz,NULL,wpredok);
else
  iceb_kasord1_2(imaf,nomd,dd,md,gd,shet,shetkas,suma,fio,osnov,dopol,kod_cel_naz,NULL,wpredok);
}
/*****************************************/
/*****************************************/

void iceb_kasord1_2(const char *imaf,
const char *nomd,
short dd,short md,short gd,
const char *shet,
const char *shetkas,
double suma,const char *fio,const char *osnov,const char *dopol,
const char *kod_cel_naz,
FILE *ff_f,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[1024];
SQL_str		row;
SQLCURSOR       cur;
class iceb_u_str naim("");
class iceb_u_str kodedrp("");
class iceb_u_str strf("");
short		kols;
class iceb_u_str sumac("");
class iceb_u_str naim_mes("");
FILE		*ff=ff_f;
class iceb_fioruk_rk glbuh;
char glbuh_str[512];
char kassir_str[512];

iceb_fioruk(2,&glbuh,wpredok); /*узнаём фамилию главного бухгалтера*/
/*записываем со смещением в право*/
memset(glbuh_str,'\0',sizeof(glbuh_str));
sprintf(glbuh_str,"%*.*s",
iceb_u_kolbait(35,glbuh.famil_inic.ravno()),
iceb_u_kolbait(35,glbuh.famil_inic.ravno()),
glbuh.famil_inic.ravno());

for(int nom=0; nom < (int)strlen(glbuh_str); nom++)
 if(glbuh_str[nom] == ' ')
  glbuh_str[nom]='_';

memset(kassir_str,'\0',sizeof(kassir_str));
  
/*Чтобы узнать фамилию кассира нужно узнать номер кассы*/
/*************
sprintf(strsql,"select kassa from Kasord where nomd='%s' and datd='%04d-%02d-%02d' and tp=1",
nomd,gd,md,dd);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  sprintf(strsql,"select fio from Kas where kod=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   sprintf(kassir_str,"%*.*s",iceb_u_kolbait(40,row[0]),iceb_u_kolbait(40,row[0]),row[0]); //чтобы получить со смещением вправо
 }
******************/
strcpy(strsql,iceb_getfioop(wpredok));
sprintf(kassir_str,"%*.*s",iceb_u_kolbait(40,strsql),iceb_u_kolbait(40,strsql),strsql); /*чтобы получить со смещением вправо*/

for(int nom=0; nom < (int)strlen(kassir_str); nom++)
 if(kassir_str[nom] == ' ')
  kassir_str[nom]='_';

iceb_u_preobr(suma,&sumac,0);
//sprintf(strsql," (%.2f)",suma);
//sumac.plus(strsql);
/*Читаем реквизиты организации*/

sprintf(strsql,"select naikon,kod from Kontragent where kodkon='00'");

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  naim.new_plus(row[0]);
  kodedrp.new_plus(row[1]);
 }

if(imaf[0] != '\0')
 {
  if((ff = fopen(imaf,"w")) == NULL)
    {
     iceb_er_op_fil(imaf,"",errno,wpredok);
     return;
    }
 }

sprintf(strsql,"select str from Alx where fil='kasord1_1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s kasord1_1.alx",gettext("Не найдены настройки"));
  iceb_menu_soob(strsql,wpredok);
  return;
 }


iceb_mesc(md,1,&naim_mes);

kols=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  strf.new_plus(row_alx[0]);
  kols++;
  switch (kols)
   {
    case 7:
      iceb_u_vstav(&strf,kodedrp.ravno(),36,56,1);
      iceb_u_vstav(&strf,naim.ravno(),64,118,1);
      break;

    case 8:
      iceb_u_vstav(&strf,naim.ravno(),0,60,1);
      break;     

    case 11:
      iceb_u_vstav(&strf,nomd,29,53,1);
      break;     

    case 12:
      sprintf(strsql,"%02d",dd);
      iceb_u_vstav(&strf,strsql,5,6,1);

      sprintf(strsql,"%s %d року",naim_mes.ravno(),gd);
      iceb_u_vstav(&strf,strsql,9,53,1);

      iceb_u_vstav(&strf,nomd,97,118,1);
      break;     

    case 13:

      sprintf(strsql,"%02d",dd);
      iceb_u_vstav(&strf,strsql,69,75,1);
      sprintf(strsql,"%s %d року",naim_mes.ravno(),gd);
      iceb_u_vstav(&strf,strsql,73,118,1);

      break;     

    case 15:
      iceb_u_vstav(&strf,fio,76,118,1);
      break;

    case 17:
      iceb_u_vstav(&strf,osnov,73,116,1);
       
      break;

    case 18:
      if(iceb_u_strlen(osnov) > 44)
        iceb_u_vstav(&strf,iceb_u_adrsimv(44,osnov),63,118,1);
      break;

    case 20: 
      iceb_u_vstav(&strf,shet,4,20,1);
      iceb_u_vstav(&strf,shetkas,20,30,1);
      sprintf(strsql,"%.2f",suma);
      iceb_u_vstav(&strf,strsql,30,42,1);

      if(atoi(kod_cel_naz) != 0)
        iceb_u_vstav(&strf,kod_cel_naz,41,51,1);


      break;     

    case 21:
      iceb_u_vstav(&strf,fio,13,60,1);
      break;     

    case 22:
      iceb_u_vstav(&strf,osnov,9,59,1);
      iceb_u_vstav(&strf,sumac.ravno(),68,118,1);
      break;     


    case 23:
      if(iceb_u_strlen(osnov) > 51)
       iceb_u_vstav(&strf,iceb_u_adrsimv(51,osnov),0,59,1);
      if(iceb_u_strlen(sumac.ravno()) > 50)
        iceb_u_vstav(&strf,iceb_u_adrsimv(50,sumac.ravno()),63,118,1);
      break;     


    case 24:
      iceb_u_vstav(&strf,sumac.ravno(),5,59,1);
      break;

    case 25:
      if(iceb_u_strlen(sumac.ravno()) > 55)
       iceb_u_vstav(&strf,iceb_u_adrsimv(55,sumac.ravno()),0,59,1);
      break;     

    case 27:
      iceb_u_vstav(&strf,dopol,9,59,1);
      break;     

    case 28:
      if(iceb_u_strlen(dopol) > 51)
        iceb_u_vstav(&strf,iceb_u_adrsimv(51,dopol),0,59,1);
      break;     

    case 29:
      iceb_u_vstav(&strf,glbuh_str,19,59,1);
      iceb_u_vstav(&strf,glbuh_str,82,118,1);
      break;     

    case 31:
      iceb_u_vstav(&strf,kassir_str,14,59,1);
      iceb_u_vstav(&strf,kassir_str,69,118,1);
      break;     



      
   }
  fprintf(ff,"%s",strf.ravno());

 }


if(imaf[0] != '\0') 
 {
  fclose(ff);
  iceb_ustpeh(imaf,1,wpredok);
 }
}
/*****************************************/
/*****************************************/

void iceb_kasord1_1(const char *imaf,
const char *nomd,
short dd,short md,short gd,
const char *shet,
const char *shetkas,
double suma,const char *fio,const char *osnov,const char *dopol,
const char *kod_cel_naz,
FILE *ff_f,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
SQL_str		row;
SQLCURSOR       cur;
class iceb_u_str naim("");
class iceb_u_str kodedrp("");
class iceb_u_str strf("");
short		kols;
class iceb_u_str sumac("");
class iceb_u_str naim_mes("");
FILE		*ff=ff_f;
class iceb_fioruk_rk glbuh;
char glbuh_str[512];
char kassir_str[512];

iceb_fioruk(2,&glbuh,wpredok); /*узнаём фамилию главного бухгалтера*/

/*записываем со смещением в право*/
memset(glbuh_str,'\0',sizeof(glbuh_str));
sprintf(glbuh_str,"%*.*s",
iceb_u_kolbait(35,glbuh.famil_inic.ravno()),
iceb_u_kolbait(35,glbuh.famil_inic.ravno()),
glbuh.famil_inic.ravno());

for(int nom=0; nom < (int)strlen(glbuh_str); nom++)
 if(glbuh_str[nom] == ' ')
  glbuh_str[nom]='_';

memset(kassir_str,'\0',sizeof(kassir_str));

/*Чтобы узнать фамилию кассира нужно узнать номер кассы*/
sprintf(strsql,"select kassa from Kasord where nomd='%s' and datd='%04d-%02d-%02d' and tp=1",
nomd,gd,md,dd);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  sprintf(strsql,"select fio from Kas where kod=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   sprintf(kassir_str,"%*.*s",iceb_u_kolbait(40,row[0]),iceb_u_kolbait(40,row[0]),row[0]); /*чтобы получить со смещением вправо*/
 }
for(int nom=0; nom < (int)strlen(kassir_str); nom++)
 if(kassir_str[nom] == ' ')
  kassir_str[nom]='_';


iceb_u_preobr(suma,&sumac,0);
sprintf(strsql," (%.2f)",suma);
sumac.plus(strsql);
/*Читаем реквизиты организации*/

sprintf(strsql,"select naikon,kod from Kontragent where kodkon='00'");

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  naim.new_plus(row[0]);
  kodedrp.new_plus(row[1]);
 }


if(imaf[0] != '\0')
 {
  if((ff = fopen(imaf,"w")) == NULL)
    {
     iceb_er_op_fil(imaf,"",errno,wpredok);
     return;
    }
 }

sprintf(strsql,"select str from Alx where fil='kasord1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob("Не найдены настройки kasord1.alx",wpredok);
  return;
 }


iceb_mesc(md,1,&naim_mes);

kols=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  strf.new_plus(row_alx[0]);
  kols++;
  switch (kols)
   {
    case 7:
      iceb_u_vstav(&strf,kodedrp.ravno(),28,53,1);
      break;

    case 8:
      iceb_u_vstav(&strf,naim.ravno(),0,53,1);
      iceb_u_vstav(&strf,naim.ravno(),58,111,1);
      break;     

    case 10:
      iceb_u_vstav(&strf,nomd,29,53,1);
      break;     

    case 11:
      sprintf(strsql,"%02d",dd);
      iceb_u_vstav(&strf,strsql,5,6,1);

      sprintf(strsql,"%s %d р.",naim_mes.ravno(),gd);
      iceb_u_vstav(&strf,strsql,9,53,1);

      iceb_u_vstav(&strf,nomd,91,111,1);
      break;     

    case 12:

      sprintf(strsql,"%02d",dd);
      iceb_u_vstav(&strf,strsql,63,64,1);
      sprintf(strsql,"%s %d р.",naim_mes.ravno(),gd);
      iceb_u_vstav(&strf,strsql,67,111,1);

      break;     

    case 14:
      iceb_u_vstav(&strf,fio,70,111,1);
      break;

    case 16:
      iceb_u_vstav(&strf,osnov,66,111,1);
       
      break;

    case 17:
      iceb_u_vstav(&strf,shet,1,14,1);
      iceb_u_vstav(&strf,shetkas,16,25,1);
      sprintf(strsql,"%.2f",suma);
      iceb_u_vstav(&strf,strsql,27,39,1);
      if(atoi(kod_cel_naz) != 0)
       iceb_u_vstav(&strf,kod_cel_naz,41,50,1);

      if(iceb_u_strlen(osnov) > 46)
        iceb_u_vstav(&strf,iceb_u_adrsimv(46,osnov),58,111,1);

      break;     

    case 18:
      iceb_u_vstav(&strf,fio,13,53,1);

      if(iceb_u_strlen(osnov) > 100)
        iceb_u_vstav(&strf,iceb_u_adrsimv(100,osnov),58,111,1);

      break;     

    case 19:
      iceb_u_vstav(&strf,osnov,9,53,1);
      iceb_u_vstav(&strf,sumac.ravno(),62,111,1);
      break;     

      break;     

    case 20:
      if(iceb_u_strlen(osnov) > 45)
       iceb_u_vstav(&strf,iceb_u_adrsimv(45,osnov),0,53,1);
      if(iceb_u_strlen(sumac.ravno()) > 50)
        iceb_u_vstav(&strf,iceb_u_adrsimv(50,sumac.ravno()),58,111,1);
      break;     

    case 21:
      if(iceb_u_strlen(osnov) > 99)
       iceb_u_vstav(&strf,iceb_u_adrsimv(99,osnov),0,53,1);
      break;     

    case 22:
      iceb_u_vstav(&strf,sumac.ravno(),5,53,1);
      break;     

    case 23:
      if(iceb_u_strlen(sumac.ravno()) > 49)
       iceb_u_vstav(&strf,iceb_u_adrsimv(49,sumac.ravno()),0,53,1);
      break;     

    case 24:
      iceb_u_vstav(&strf,dopol,9,53,1);
      break;     

    case 25:
      if(iceb_u_strlen(dopol) > 45)
        iceb_u_vstav(&strf,iceb_u_adrsimv(45,dopol),0,53,1);
      break;     


    case 27:
      iceb_u_vstav(&strf,glbuh_str,19,53,1);
      iceb_u_vstav(&strf,glbuh_str,76,110,1);
      break;     

    case 29:
      iceb_u_vstav(&strf,kassir_str,14,53,1);
      iceb_u_vstav(&strf,kassir_str,63,110,1);
      break;     


      
   }
  fprintf(ff,"%s",strf.ravno());

 }


if(imaf[0] != '\0') 
 {
  fclose(ff);
  iceb_ustpeh(imaf,1,wpredok);
 }
}
/*******************************************/
/*******************************************/

void	iceb_kasord1_0(const char *imaf,
const char *nomd,
short dd,short md,short gd,
const char *shet,
const char *shetkas,
double suma,const char *fio,const char *osnov,const char *dopol,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
SQL_str		row;
SQLCURSOR       cur;
class iceb_u_str naim("");
class iceb_u_str kodedrp("");
FILE		*ff;
class iceb_u_str strf("");
short		kols;
class iceb_u_str sumac("");


/*printf("iceb_kasord1_0\n");*/

iceb_u_preobr(suma,&sumac,0);
sprintf(strsql," (%.2f)",suma);
sumac.plus(strsql);

/*Читаем реквизиты организации*/

sprintf(strsql,"select naikon,kod from Kontragent where kodkon='00'");

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  naim.new_plus(row[0]);
  kodedrp.new_plus(row[1]);
 }

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

sprintf(strsql,"select str from Alx where fil='kasorder.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob("Не найдены настройки kasorder.alx",wpredok);
  return;
 }


kols=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  strf.new_plus(row_alx[0]);
  kols++;
  switch (kols)
   {
    case 1:
     break;

    case 2:
      iceb_u_vstav(&strf,naim.ravno(),0,39,1);
      iceb_u_vstav(&strf,naim.ravno(),76,115,1);
      break;     

    case 6:
      iceb_u_vstav(&strf,kodedrp.ravno(),19,30,1);
      iceb_u_vstav(&strf,nomd,100,109,1);
      break;     

    case 10:
      iceb_u_vstav(&strf,fio,91,117,1);
      break;     

    case 11:
      iceb_u_vstav(&strf,&fio[118-91],77,115,1);
      break;

    case 13:
      iceb_u_vstav(&strf,osnov,87,117,1);
      break;     

    case 14:
      if(iceb_u_strlen(osnov) > 31)
       iceb_u_vstav(&strf,iceb_u_adrsimv(31,osnov),77,117,1);
      break;

    case 15:
      iceb_u_vstav(&strf,nomd,1,8,1);
      sprintf(strsql,"%02d.%02d.%d",dd,md,gd);
      iceb_u_vstav(&strf,strsql,9,19,1);
      iceb_u_vstav(&strf,shet,23,31,1);
      iceb_u_vstav(&strf,shetkas,34,39,1);
      sprintf(strsql,"%15.2f",suma);
      iceb_u_vstav(&strf,strsql,41,55,1);
      if(iceb_u_strlen(osnov) > 31+41)
       iceb_u_vstav(&strf,iceb_u_adrsimv(31+41,osnov),77,117,1);
      
      break;


    case 17:
      iceb_u_vstav(&strf,fio,14,70,1);
      iceb_u_vstav(&strf,sumac.ravno(),79,117,1);
      break;     

    case 18:
      iceb_u_vstav(&strf,osnov,10,71,1);
      iceb_u_vstav(&strf,&sumac.ravno()[118-79],77,115,1);
      break;     

    case 19:
      if(iceb_u_strlen(osnov) > 62)
       iceb_u_vstav(&strf,iceb_u_adrsimv(62,osnov),0,71,1);
      break;     

    case 20:
      iceb_u_vstav(&strf,sumac.ravno(),14,70,1);
      break;     

    case 21:
      sprintf(strsql,"%02d.%02d.%d%s",
      dd,md,gd,gettext("г."));
      iceb_u_vstav(&strf,strsql,84,101,1);
      break;     

    case 22:
      iceb_u_vstav(&strf,dopol,9,70,1);
      break;     
      
   }
  fprintf(ff,"%s",strf.ravno());

 }
 
fclose(ff);
iceb_ustpeh(imaf,1,wpredok);

}
