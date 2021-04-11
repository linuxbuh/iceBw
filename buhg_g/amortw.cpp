/* $Id: amortw.c,v 1.17 2013/09/26 09:46:45 sasa Exp $ */
/*29.12.2016    08.07.1996      Белых А.И.      amortw.c
Расчёт амортзационных отчислений
для заданного инвентарного номера за заданный месяц
Определить числиться ли инвентарны номер и в каком подразделении
он находиться на заданную дату. Если не числится расчёт не проводим
Если числится смотрим настройку и делаем расчёт
В месяце убытия амортизация считается в подразделении
где числился. В подразделении где прибыл амотизация считается со
следующего месяца
Если вернули 0 - расчитали
	     1 - нет
*/
#include        "buhg_g.h"

void amortbo(int in,int podr,int kodotl,double kof,short gr,double bs,double iz,short mra,class poiinpdw_data *rekin,FILE *ff_prot,GtkWidget *wpredok);

void amortw_udzap(int in,short mr,short gr,short mkr,GtkWidget *wpredok);

extern float      nemi; /*Необлагаемый минимум*/
extern short metkabo;  //Если 1 то организация бюджетная
extern SQL_baza bd;

int amortw(int in,
short mr,short gr,
short mra, //0-расчёт в потоке 1-нет 2-молча
short kvrt,  //Квартал
const char *grupa,
FILE *ff_prot,
GtkWidget *wpredok)
{
int             i,i1;
double          bs=0.,iz=0.;
double		amort[4];
double		kof;
int		podr;
SQL_str         row;
SQLCURSOR curr;
char		strsql[1024];
time_t		vrem;
int		kodotl;
class bsizw_data bal_st;
class poiinpdw_data rekin;
short mkr=mr;

//printf("1%s\n",__FUNCTION__);

if(ff_prot != NULL)
 {
  fprintf(ff_prot,"%s\n",__FUNCTION__);
 }

podr=0;
if(metkabo == 1)
  mr=1;
  

memset(&amort,'\0',sizeof(amort));
 
if(kvrt != 0)
 {
  if(kvrt == 1)
    mr=1;
  if(kvrt == 2)
    mr=4;
  if(kvrt == 3)
    mr=7;
  if(kvrt == 4)
    mr=10;
  mkr=mr+2;
 }
/********
if(mra == 0)
  printw("in-%d %d.%d\n",in,mr,gr);
***********/
if(ff_prot != NULL)
  fprintf(ff_prot,"Расчёт амортизации для налогового учёта.\nin-%d %d.%d\n",in,mr,gr);


if(metkabo == 1)
 {
  if((i=poiinw(in,1,1,gr+1,&podr,&kodotl,wpredok)) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s %d %s %d.%d ! %s %d.\n",
    gettext("Инвентарный номер"),in,
    gettext("не числится по дате"),1,gr+1,
    gettext("Подпрограмма вернула"),i);
    if(mra == 1)
     {
      sprintf(strsql,"%s %d %s %d.%d !\n %s %d",
      gettext("Инвентарный номер"),in,
      gettext("не числится по дате"),1,gr+1,
      gettext("Подпрограмма вернула"),i);

      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }   
 }
else
 if((i=poiinw(in,1,mr,gr,&podr,&kodotl,wpredok)) != 0)
  {

   if(ff_prot != NULL)
     fprintf(ff_prot,"%s %d %s %d.%d ! %s %d.\n",
   gettext("Инвентарный номер"),in,
   gettext("не числится по дате"),mr,gr,
   gettext("Подпрограмма вернула"),i);

    if(mra == 1)
     {
      sprintf(strsql,"%s %d\n %s %d.%d !\n %s %d",
      gettext("Инвентарный номер"),in,
      gettext("не числится по дате"),mr,gr,
      gettext("Подпрограмма вернула"),i);
      iceb_menu_soob(strsql,wpredok);
     }
   return(1);
  }   


if(metkabo == 1)
 {
  if((i=poiinpdw(in,1,gr+1,&rekin,wpredok)) != 0)
   {

   if(ff_prot != NULL)
     fprintf(ff_prot,"%s %d %s %d.%d ! %s %d\n",
    gettext("Инвентарный номер"),in,
    gettext("не найдено настройки на дату"),1,gr+1,
    gettext("Подпрограмма вернула"),i);
    if(mra == 1)
     {
      sprintf(strsql,"%s %d\n%s %d.%d !\n %s %d",
      gettext("Инвентарный номер"),in,
      gettext("не найдено настройки на дату"),1,gr+1,
      gettext("Подпрограмма вернула"),i);
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }  
 }
else
if((i=poiinpdw(in,mr,gr,&rekin,wpredok)) != 0)
 {

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s %d.%d ! %s %d\n",
  gettext("Инвентарный номер"),in,
  gettext("не найдено настройки на дату"),mr,gr,
  gettext("Подпрогамма вернула"),i);

  if(mra == 1)
   {
    sprintf(strsql,"%s %d\n %s %d.%d !\n %s %d",
    gettext("Инвентарный номер"),in,
    gettext("не найдено настройки на дату"),mr,gr,
    gettext("Подпрогамма вернула"),i);
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }  
if(grupa[0] != '\0')
if(iceb_u_proverka(grupa,rekin.hna.ravno(),0,0) != 0 || uosprovgrw(rekin.hna.ravno(),wpredok) != 1)
 {

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s (%s)\n",
  gettext("Инвентарный номер в группе"),rekin.hna.ravno(),
  gettext("амортизация не считается"));

  if(mra == 1)
   {
    sprintf(strsql,"%s %s (%s)",
    gettext("Инвентарный номер в группе"),rekin.hna.ravno(),
    gettext("амортизация не считается"));
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }

if(rekin.soso == 1 || rekin.soso == 2  )
 {

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s.\n",
  gettext("Инвентарный номер"),in,
  gettext("не эксплуатируется"));

  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s.",
    gettext("Инвентарный номер"),in,
    gettext("не эксплуатируется"));
    iceb_menu_soob(strsql,wpredok);
   }

  amortw_udzap(in,mr,gr,mkr,wpredok);
  return(1);
 }

//printw("in=%d podr=%d %d.%d\n",in,podr,mr,gr);
bs=iz=0.;

if(metkabo != 1)
  bsizw(in,podr,1,mr,gr,&bal_st,ff_prot,wpredok);
else
  bsizw(in,podr,1,1,gr+1,&bal_st,ff_prot,wpredok);

bs=bal_st.sbs+bal_st.bs-uosgetlsw(in,1,mr,gr,0,wpredok);
iz=bal_st.siz+bal_st.iz+bal_st.iz1;

if(ff_prot != NULL)
 fprintf(ff_prot,"bs=%f iz=%f\n",bs,iz);
 
if(bs <= iz)
 {

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s. bs=%.2f <= iz=%.2f\n",
  gettext("Инвентарный номер"),in,
  gettext("амортизирован полностью"),bs,iz);

  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s. %.2f <= %.2f",
    gettext("Инвентарный номер"),in,
    gettext("амортизирован полностью"),bs,iz);
    iceb_menu_soob(strsql,wpredok);  
   }
 }

/*Читаем коэффициент*/
sprintf(strsql,"select kof from Uosgrup where kod='%s'",rekin.hna.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {


  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %d - %s %s !\n",
  gettext("Инвентарный номер"),in,
  gettext("не нашли кода группы"),rekin.hna.ravno());

  if(mra == 1)
   {
    sprintf(strsql,"%s %d - %s %s !",
    gettext("Инвентарный номер"),in,
    gettext("не нашли кода группы"),rekin.hna.ravno());
   }
  return(1);
 }

kof=atof(row[0]);
if(ff_prot != NULL)
 fprintf(ff_prot,"Коэффициент=%f\n",kof);
 
/*
printw("bs=%f iz=%f popkf=%f kof=%f nemi=%f\n",bs,iz,uos.popkf,kof,nemi);
OSTANOV();    
*/

if(metkabo == 1)
 {
//  amortbo(in,podr,kodotl,kof,gr,bs,iz,rekin.popkf,rekin.hzt.ravno(),rekin.hna.ravno(),ff_prot,wpredok);
  amortbo(in,podr,kodotl,kof,gr,bs,iz,mra,&rekin,ff_prot,wpredok);
  return(0);
 }

if(uosprovarw(rekin.hna.ravno(),0,wpredok) == 1)
 {
  if(iceb_u_SRAV(rekin.hna.ravno(),"1",0) == 0 && bs-iz <= 100*nemi)
   {
    amort[1]=bs-iz;
   }
  else
   {      
    
    amort[0]=(bs-iz)*rekin.popkf*3*kof/100/12.;

    if(ff_prot != NULL)
      fprintf(ff_prot,"%s=(%.2f-%.2f)*%.2f/100/12*3*%.2f=%.2f\n",
    gettext("Амортизация"),
    bs,iz,kof,rekin.popkf,amort[0]);
   
    if(mra == 1)
     {
      sprintf(strsql,"%s\n(%.2f-%.2f)*%.2f/100/12*3*%.2f=%.2f",
      gettext("Амортизация"),
      bs,iz,kof,rekin.popkf,amort[0]);
      iceb_menu_soob(strsql,wpredok);
     }

    amort[0]=iceb_u_okrug(amort[0],0.01);
    amort[1]=amort[0]/3;
    amort[1]=iceb_u_okrug(amort[1],0.01);
    amort[2]=amort[1];
    amort[3]=amort[0]-amort[1]-amort[2];


   }
 }
else  /*Так как было раньше*/
 {

  amort[1]=bs*rekin.popkf*kof/100/12.;
  if(amort[1] > bs-iz)
    amort[1]=bs-iz;
  amort[0]=iceb_u_okrug(amort[0],0.01);
  if(mra == 1)
   {
    sprintf(strsql,"%s\n%.2f*%.2f/100/12.*%.2f=%.2f",
    gettext("Амортизация"),
    bs,kof,rekin.popkf,amort[1]);

    iceb_menu_soob(strsql,wpredok);
   }
 }

if(mra == 1 && bs == 0.)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"%d-%s\n",in,
    gettext("Балансовая стоимость равна нолю !"));

  sprintf(strsql,"%d-%s",in,gettext("Балансовая стоимость равна нолю !"));
  iceb_menu_soob(strsql,wpredok);
 }

 
if(iz + amort[0] > bs)
 {

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s.\n",
  gettext("Инвентарный номер"),in,
  gettext("остаточная амортизация"));
  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s",
    gettext("Инвентарный номер"),in,
    gettext("остаточная амортизация"));
    iceb_menu_soob(strsql,wpredok);
   }
 }

 amortw_udzap(in,mr,gr,mkr,wpredok);

if(ff_prot != NULL)
 fprintf(ff_prot,"%s: %5.2f %s: %.2f\n",
gettext("Амортизация"),amort[1],
gettext("Балансовая стоимость"),bs);
time(&vrem);
i1=1;
 for(i=mr ; i <= mkr; i++)
  {
   sprintf(strsql,"insert into Uosamor \
values (%d,'%04d-%02d-01',%d,'%s','%s',%.2f,%d,%ld,%d)",
   in,gr,i,podr,rekin.hzt.ravno(),rekin.hna.ravno(),amort[i1++],iceb_getuid(wpredok),vrem,kodotl);

   iceb_sql_zapis(strsql,1,0,wpredok);
  }

return(0);
}
/*************************/
/*Амортизация для бюджетных организаций*/
/***************************************/
void amortbo(int in,
int podr,
int kodotl,
double kof,
short gr,
double bs,
double iz,
short mra, //0-расчёт в потоке 1-нет 2-молча
class poiinpdw_data *rekin,
FILE *ff_prot,
GtkWidget *wpredok)
{
time_t  vrem;
char	strsql[1024];

if(ff_prot != NULL)
 fprintf(ff_prot,"%s\n",__FUNCTION__);

SQL_str row;
class SQLCURSOR cur;
int kol_mes=12;

/*Если основное средство получено в течение года то нужно знать количество месяцев амортизации*/
sprintf(strsql,"select datd from Uosdok1 where innom=%d and podt=1 and tipz=1 order by datd desc limit 1",in);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  short dd=0,md=0,gd=0;
  iceb_u_rsdat(&dd,&md,&gd,row[0],2);
  if(gd == gr)
   {
    kol_mes=iceb_u_period(dd,md,gd,1,12,gr,1);
   }
 } 

double amort=0.;
if(kol_mes == 12)
 amort=bs*rekin->popkf/100*kof;
else
 amort=bs*rekin->popkf/100*kof/12*kol_mes;

if(mra == 1)
 {
  if(kol_mes == 12)
   sprintf(strsql,"%.2f*%.6g/100*%.6g=%.2f",bs,rekin->popkf,kof,amort);
  else
   sprintf(strsql,"%.2f*%.6g/100*%.6g/12*%d=%.2f",bs,rekin->popkf,kof,kol_mes,amort);
  
  iceb_menu_soob(strsql,wpredok);
 }

if(amort > bs-iz)
 {
  if(mra == 1)
   {
    sprintf(strsql,"%s %.2f > %.2f\n%s",gettext("Сумма амортизации больше остаточной стоимости"),amort,bs-iz,gettext("Берём в расчёт остаточную стоимость"));
  
    iceb_menu_soob(strsql,wpredok);
   }
  amort=bs-iz;
 }
amort=iceb_u_okrug(amort,1.); //Округляем до гривны

/*Удаляем старую запись если она есть*/
sprintf(strsql,"delete from Uosamor where innom=%d and da >= '%04d-01-01' and da <= '%04d-12-31'",in,gr,gr);
iceb_sql_zapis(strsql,1,0,wpredok);

time(&vrem);

sprintf(strsql,"insert into Uosamor \
values (%d,'%04d-12-01',%d,'%s','%s',%.2f,%d,%ld,%d)",
in,gr,podr,rekin->hzt.ravno(),rekin->hna.ravno(),amort,iceb_getuid(wpredok),vrem,kodotl);

if(ff_prot != NULL)
 fprintf(ff_prot,"%s\n",strsql);
 
iceb_sql_zapis(strsql,1,0,wpredok);

}

/**********************************/
/*Удаление*/
/*********************/
void amortw_udzap(int in,short mr,short gr,short mkr,GtkWidget *wpredok)
{
char strsql[512];
/*Удаляем старую запись если она есть*/
 for(int i=mr ; i <= mkr; i++)
  {
   sprintf(strsql,"delete from Uosamor where innom=%d and da='%04d-%02d-01'",in,gr,i);
   iceb_sql_zapis(strsql,1,0,wpredok);
  }
}
