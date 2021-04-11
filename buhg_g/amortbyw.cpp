/* $Id: amortbyw.c,v 1.16 2013/09/26 09:46:45 sasa Exp $ */
/*10.07.2015    06.12.2000      Белых А.И.      amortbyw.c
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

extern float      nemi; /*Необлагаемый минимум*/
void amortbyw_udzap(int in,short mr,short gr,GtkWidget *wpredok);

int amortbyw(int in,
short mr,short gr,
short mra, //0-расчёт в потоке 1-нет 2-молча
const char *grup_bu, //Группа бухгалтерского учёта
FILE *ff_prot,
GtkWidget *wpredok)
{
int             i;
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

if(ff_prot != NULL)
 fprintf(ff_prot,"%s\n",__FUNCTION__);

int vrabu=uosprovarw(grup_bu,1,wpredok); //Вариант расчёта амотизации бухгалтерского учёта

if(vrabu == -1)
 {
  sprintf(strsql,"%s\n%s:%d %s:%s",gettext("Не определён вариант расчёта"),gettext("Инвентарный номер"),in,gettext("Группа"),grup_bu);
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s\n",strsql);
  
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

podr=0;


 
memset(&amort,'\0',sizeof(amort));
 

if(ff_prot != NULL)
  fprintf(ff_prot,"Расчёт амортизационных отчислений для бухгалтерского учёта\nin-%d %d.%d\n",in,mr,gr);


if((i=poiinw(in,1,mr,gr,&podr,&kodotl,wpredok)) != 0)
 {
     
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s %d.%d ! %s %d\n",
  gettext("Инвентарный номер"),in,
  gettext("не числится по дате"),mr,gr,
  gettext("Подпрограмма вернула"),i);

  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s %d.%d ! %s %d",
    gettext("Инвентарный номер"),in,
    gettext("не числится по дате"),mr,gr,
    gettext("Подпрогамма вернула"),i);
    iceb_menu_soob(strsql,wpredok);   
   }

  return(1);
 }   


//if((i=poiinpdw(in,mr,gr,uos.shetu,uos.hzt,uos.hau,uos.hna,&uos.popkf,&uos.soso,uos.nomz,&m1,&g1,uos.hnaby,&uos.popkfby)) != 0)
if((i=poiinpdw(in,mr,gr,&rekin,wpredok)) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %d %s %d.%d ! %s %d\n",
  gettext("Инвентарный номер"),in,
  gettext("не найдено настройки на дату"),mr,gr,
  gettext("Подпрограмма вернула"),i);

  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s %d.%d ! %s %d",
    gettext("Инвентарный номер"),in,
    gettext("не найдено настройки на дату"),mr,gr,
    gettext("Подпрограмма вернула"),i);
    iceb_menu_soob(strsql,wpredok);
   }

  return(1);
 }  

//sizby=bsby=izby=iz1by=0.;
//Должны знать начальную стоимость даже если не считается амортизация
//bsiz(in,podr,1,mr,gr,&sbs,&siz,&bs,&iz,&iz1,&sbsby,&sizby,&bsby,&izby,&iz1by,ff_prot);
bsizw(in,podr,1,1,gr+1,&bal_st,ff_prot,wpredok);


double bsby=bal_st.sbsby+bal_st.bsby-uosgetlsw(in,1,mr,gr,1,wpredok);
double izby=bal_st.sizby+bal_st.izby+bal_st.iz1by;

double balst=bsby;
if(rekin.soso == 1 ||  rekin.soso == 3)
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
  amortbyw_udzap(in,mr,gr,wpredok);
  /*Записываем с нулём*/
  sprintf(strsql,"insert into Uosamor1 \
  values (%d,'%04d-%02d-01',%d,'%s','%s',%.2f,%d,%ld,%d)",in,gr,mr,podr,rekin.hzt.ravno(),rekin.hnaby.ravno(),0.,iceb_getuid(wpredok),time(NULL),kodotl);

  iceb_sql_zapis(strsql,1,0,wpredok);
  return(1);
 }



if(bsby <= izby)
 {

  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %d %s. bsby=%.2f <= izby=%.2f\n",
  gettext("Инвентарный номер"),in,
  gettext("амортизирован полностью"),bsby,izby);

  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s. %.2f <= %.2f",
    gettext("Инвентарный номер"),in,
    gettext("амортизирован полностью"),bsby,izby);
    iceb_menu_soob(strsql,wpredok);
   }
  bsby=izby=0.;
 }

/*Читаем коэффициент*/
sprintf(strsql,"select kof from Uosgrup1 where kod='%s'",rekin.hnaby.ravno());
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {

  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %d - %s %s !\n",
  gettext("Инвентарный номер"),in,
  gettext("не нашли кода группы"),rekin.hnaby.ravno());

  if(mra == 1)
   { 
    sprintf(strsql,"%s %d - %s %s !\n",
    gettext("Инвентарный номер"),in,
    gettext("не нашли кода группы"),rekin.hnaby.ravno());
    iceb_menu_soob(strsql,wpredok);
   }

  return(1);
 }

kof=atof(row[0]);
/*
printw("bsby=%f izby=%f popkfby=%f kof=%f nemi=%f\n",bsby,izby,uos.popkfby,kof,nemi);
OSTANOV();    
*/

if(vrabu == 1)
  amort[1]=(bsby-izby)*rekin.popkfby*kof/100/12.;
if(vrabu == 0)
 {
  amort[1]=bsby*rekin.popkfby*kof/100/12.;
  if(amort[1] > bsby -izby)
     amort[1]=bsby -izby;
 }
amort[0]=iceb_u_okrug(amort[0],0.01);
if(mra == 1)
 {

  if(vrabu == 1)
    sprintf(strsql,"(%.2f-%.2f)*%.2f/100/12*%.2f=%.2f",
    bsby,izby,kof,rekin.popkfby,amort[1]);
  if(vrabu == 0)
    sprintf(strsql,"%.2f*%.2f/100/12*%.2f=%.2f",
    bsby,kof,rekin.popkfby,amort[1]);
  
  iceb_menu_soob(strsql,wpredok);
 }

if(izby + amort[0] > bsby)
 {

  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %d %s.\n",
  gettext("Инвентарный номер"),in,
  gettext("остаточная амортизация"));

  if(mra == 1)
   {
    sprintf(strsql,"%s %d %s.\n",
    gettext("Инвентарный номер"),in,
    gettext("остаточная амортизация"));

    iceb_menu_soob(strsql,wpredok);
   }
 }

amortbyw_udzap(in,mr,gr,wpredok);

if(ff_prot != NULL)
  fprintf(ff_prot,"%s: %5.2f %s: %.2f\n",
gettext("Амортизация"),amort[1],
gettext("Балансовая стоимость"),bsby);

if(balst != 0.) //Если начальная балансовая стоимость равна 0. амортизацию не записывать
 { 
  time(&vrem);

  sprintf(strsql,"insert into Uosamor1 \
values (%d,'%04d-%02d-01',%d,'%s','%s',%.2f,%d,%ld,%d)",in,gr,mr,podr,rekin.hzt.ravno(),rekin.hnaby.ravno(),amort[1],iceb_getuid(wpredok),time(NULL),kodotl);

  iceb_sql_zapis(strsql,1,0,wpredok);
 }


return(0);
}
/**************************************/
/*Удаляем запись*/
/**************************/
void amortbyw_udzap(int in,short mr,short gr,GtkWidget *wpredok)
{
char strsql[512];
//sprintf(strsql,"delete from Uosamor1 where innom=%d and god=%d and mes=%d",in,gr,mr);
sprintf(strsql,"delete from Uosamor1 where innom=%d and da >= '%04d-%02d-01' and da <= '%04d-%02d-31'",in,gr,mr,gr,mr);

iceb_sql_zapis(strsql,1,0,wpredok);

}
