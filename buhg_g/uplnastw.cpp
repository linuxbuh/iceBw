/*$Id: uplnastw.c,v 1.14 2013/11/05 10:51:25 sasa Exp $*/
/*13.07.2015	01.03.2008	Белых А.И. 	uplnastw.c
Чтение настроек для подсистемы "Учет путевых листов"
*/
#include        <errno.h>
#include	"buhg_g.h"


short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		startgodupl=0; //Стартовый год для "Учета путевых листов"
short		startgodb=0; /*Стартовый год просмотров главной книги*/
extern SQL_baza bd;
short		startgod; /*Стартовый год просмотров для материального учёта*/
short metka_pros_mat_s_ost=0; //0-всё показывать 1-только материалы с остатками
double		okrg1;  /*Округление суммы*/
double		okrcn;  /*Округление цены*/
short		vtara; /*Код группы возвратная тара*/
//char            *mtsh=NULL;  /*Перечень материальных счетов*/
class iceb_u_str spmatshets(""); /*Список материальных счетов*/
char		*kodopsp=NULL; //Коды операций сторнирования для приходов
char		*kodopsr=NULL; //Коды операций сторнирования для расходов
class iceb_u_str shrt("");    /*Счета розничной торговли*/

int uplnastw(void)
{
class iceb_u_str st1("");


startgodupl=0;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"uplnast.alx"};

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

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  iceb_u_polen(row_alx[0],&st1,1,'|');

  if(iceb_u_SRAV(st1.ravno(),"Стартовый год",0) == 0)
   {
    iceb_u_polen(row_alx[0],&startgodupl,2,'|');
//    startgodupl=(short)iceb_u_atof(st1); 
    continue;
   }
 }

iceb_poldan("Многопорядковый план счетов",&st1,"nastrb.alx",NULL);
if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
 vplsh=1;

startgodb=0;
if(iceb_poldan("Стартовый год",&st1,"nastrb.alx",NULL) == 0)
 {
  startgodb=st1.ravno_atoi();
 }

metka_pros_mat_s_ost=0;
okrg1=0.;
startgod=0;
okrcn=0.;
vtara=0; /*Код группы возвратная тара*/
spmatshets.new_plus("");
/************
if(mtsh != NULL)
 {
  delete [] mtsh;
  mtsh=NULL;
 }
***************/
if(kodopsp != NULL)
 {
  delete [] kodopsp;
  kodopsp=NULL;
 }
if(kodopsr != NULL)
 {
  delete [] kodopsp;
  kodopsp=NULL;
 }
shrt.new_plus("");
const char *imaf_alx1={"matnast.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx1);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx1);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_SRAV(st1.ravno(),"Счета розничной торговли",0) == 0)
    {
     iceb_u_polen(row_alx[0],&shrt,2,'|');
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Коды операций сторнирования для расходов",0) == 0)
   {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     int i=strlen(st1.ravno())+1;
     kodopsr=new char[i];
     strcpy(kodopsr,st1.ravno());
     continue;
   }
  if(iceb_u_SRAV(st1.ravno(),"Коды операций сторнирования для приходов",0) == 0)
   {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     int i=strlen(st1.ravno())+1;
     kodopsp=new char[i];
     strcpy(kodopsp,st1.ravno());
     continue;
   }
/*********************
  if(iceb_u_SRAV(st1.ravno(),"Перечень материальных счетов",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     int i=strlen(st1.ravno())+1;
     mtsh=new char[i];
     strcpy(mtsh,st1.ravno());
     continue;
    }
*****************/
  if(iceb_u_SRAV(st1.ravno(),"Перечень материальных счетов",0) == 0)
    {
     iceb_u_polen(row_alx[0],&spmatshets,2,'|');
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Код группы возвратная тара",0) == 0)
    {
     iceb_u_polen(row_alx[0],&vtara,2,'|');
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],&startgod,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Показывать только материалы имеющие остаток",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
        metka_pros_mat_s_ost=1;
     else
        metka_pros_mat_s_ost=0;
     
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Округление цены",0) == 0)
    {
     iceb_u_polen(row_alx[0],&okrcn,2,'|');
//     okrcn=iceb_u_atof(st1);
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Округление суммы",0) == 0)
    {
     iceb_u_polen(row_alx[0],&okrg1,2,'|');
//     okrg1=iceb_u_atof(st1);
     continue;
    }

 }
return(0);

}

