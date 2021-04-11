/* $Id: uosnastw.c,v 1.14 2013/09/26 09:46:57 sasa Exp $ */
/*13.07.2015    08.04.1996      Белых А.И.   	uosnastw.c
Чтение настpойки для учета основных сpедств
*/
#include        "buhg_g.h"

float           nemi; /*Необлагаемый минимум*/
short		startgoduos; /*Стартовый год*/
short		startgodb; /*Стартовый год просмотров главной книги*/
short		vplsh; /*0-двух-порядковый план счетов 1-многопорядковый*/
char		*bnds1=NULL; /*0% НДС реализация на територии Украины.*/
char            *bnds2=NULL; /*0% НДС экспорт.*/
char            *bnds3=NULL; /*0% Освобождение от НДС статья 5.*/
short		metkabo=0; //Если 1 то организация бюджетная
double okrg1=1.;
double okrcn=1.;
extern SQL_baza bd;

int uosnastw()
{
class iceb_u_str st1("");
class iceb_u_str bros("");
int		i;

metkabo=vplsh=0;
nemi=0.;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"uosnast.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }


if(bnds1 != NULL)
 {
  delete [] bnds1;
  bnds1=NULL;
 }

if(bnds2 != NULL)
 {
  delete [] bnds2;
  bnds2=NULL;
 }

if(bnds3 != NULL)
 {
  delete [] bnds3;
  bnds3=NULL;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
   
  iceb_u_polen(row_alx[0],&st1,1,'|');

  if(iceb_u_SRAV(st1.ravno(),"Бюджетная организация",0) == 0)
   {
    iceb_u_polen(row_alx[0],&st1,2,'|');
    if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
     metkabo=1;
    continue;
   }
  if(iceb_u_SRAV(st1.ravno(),"Необлагаемый минимум",0) == 0)
   {
    iceb_u_polen(row_alx[0],&nemi,2,'|');
//    nemi=(float)iceb_u_atof(st1);
    continue;
   }

  if(iceb_u_SRAV(st1.ravno(),"Стартовый год",0) == 0)
   {
    iceb_u_polen(row_alx[0],&startgoduos,2,'|');
//    startgoduos=(short)iceb_u_atof(st1);
    continue;
   }
  if(iceb_u_SRAV(st1.ravno(),"0% НДС реализация на територии Украины",0) == 0)
    {
     iceb_u_polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     bnds1=new char[i*sizeof(char)];
     strcpy(bnds1,bros.ravno());
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"0% НДС экспорт",0) == 0)
    {
     iceb_u_polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     bnds2=new char[i*sizeof(char)];
     strcpy(bnds2,bros.ravno());
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"0% Освобождение от НДС статья 5",0) == 0)
    {
     iceb_u_polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     bnds3=new char[i*sizeof(char)];
     strcpy(bnds3,bros.ravno());
     continue;
    }

 }


iceb_poldan("Многопорядковый план счетов",&st1,"nastrb.alx",NULL);
if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
 vplsh=1;

if(iceb_poldan("Округление суммы",&st1,"matnast.alx",NULL) == 0)
  okrg1=st1.ravno_atof();
if(iceb_poldan("Округление цены",&st1,"matnast.alx",NULL) == 0)
  okrcn=st1.ravno_atof();

return(0);
}
