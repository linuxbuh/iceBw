/*$Id: uslnastw.c,v 1.14 2013/10/04 07:27:17 sasa Exp $*/
/*13.07.2015	11.02.2000	Белых А.И.	uslnast.c
Чтение настроек для услуг
Если вернули 0 файл прочитан
	     1 файл не прочитан
*/
#include	"buhg_g.h"

short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		startgodus=0; /*Стартовый год просмотров услуг*/
short		startgodb=0; /*Стартовый год Главной книги*/
short		startgod; //Стартовый год просмотров в материальном учёте

class iceb_u_str bndsu1(""); /*0% НДС реализация на територии Украины.*/
class iceb_u_str bndsu2(""); /*0% НДС экспорт.*/
class iceb_u_str bndsu3(""); /*0% Освобождение от НДС статья 5.*/
double		okrcn;  /*Округление цены*/
double		okrg1;  /*Округление суммы*/
short		cnsnds; /*0-Цена материалла без ндс 1- с ндс*/
short		cnsndsu; /*0-Цена услуг без ндс 1- с ндс*/
short           mfnn; /*Метка формирования номера накладной
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                       */
double		kursue=0.;/*Курс условной единицы*/
extern SQL_baza bd;

int		uslnastw()
{
class iceb_u_str st1("");
class iceb_u_str bros("");
short		voz;


startgod=0;
cnsnds=cnsndsu=0;
okrg1=okrcn=0.;
vplsh=startgodus=0;
mfnn=(-1);
kursue=0.;
bndsu1.new_plus("");
bndsu2.new_plus("");
bndsu3.new_plus("");

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"uslnast.alx"};

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
  if(iceb_u_SRAV(st1.ravno(),"Раздельная нумерация документов",0) == 0)
    {
     mfnn=0;
     
     iceb_u_polen(row_alx[0],&st1,2,'|');
     if(st1.ravno()[0] == '\0' || st1.ravno()[0] == '\n')
      mfnn=(-1);
      
     if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
       mfnn=1;
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],&startgodus,2,'|');
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"0% НДС реализация на територии Украины",0) == 0)
    {
     iceb_u_polen(row_alx[0],&bndsu1,2,'|');
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"0% НДС экспорт",0) == 0)
    {
     iceb_u_polen(row_alx[0],&bndsu2,2,'|');
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Освобождение от НДС",0) == 0)
    {
     iceb_u_polen(row_alx[0],&bndsu3,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Цена услуг с НДС",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
       cnsndsu=1;
     continue;
    }

 }
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
  iceb_u_polen(row_alx[0],&st1,1,'|');
  if(iceb_u_SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],&startgod,2,'|');
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Округление цены",0) == 0)
   {
    iceb_u_polen(row_alx[0],&okrcn,2,'|');
    continue;
   }

  if(iceb_u_SRAV(st1.ravno(),"Округление суммы",0) == 0)
    {
     iceb_u_polen(row_alx[0],&okrg1,2,'|');
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Цена материалла с НДС",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
       cnsnds=1;
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Курс УЕ",0) == 0)
    {
     iceb_u_polen(row_alx[0],&kursue,2,'|');
     continue;
    }
 }
vplsh=0;
iceb_poldan("Многопорядковый план счетов",&st1,"nastrb.alx",NULL);
if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
 vplsh=1;

startgodb=0;
if(iceb_poldan("Стартовый год",&st1,"nastrb.alx",NULL) == 0)
 {
  startgodb=st1.ravno_atoi();
 }

voz=0;
iceb_u_str repl;

if(okrcn == 0.)
 {
  voz++;
//  printw(gettext("Не введено \"Округление цены\" !\n"));
  repl.plus_ps("Не введено \"Округление цены\" !");
  
 }

if(okrg1 == 0.)
 {
  voz++;
//  printw(gettext("Не введено \"Округление суммы\" !\n"));
  repl.plus_ps("Не введено \"Округление суммы\" !");
 }

if(voz != 0)
 iceb_menu_soob(&repl,NULL);
 
return(0);

}
