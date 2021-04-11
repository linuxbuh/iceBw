/* $Id: matnastw.c,v 1.19 2014/07/31 07:08:26 sasa Exp $ */
/*08.08.2015    14.04.1997      Белых А.И.     matnastw.c
Чтение настpойки для материального учета 
*/
#include	"buhg_g.h"

class iceb_u_str spmatshets(""); /*Список материальных счетов*/
double		okrcn;  /*Округление цены*/
double		okrg1;  /*Округление суммы*/
short           mfnn; /*Метка формирования номера накладной
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                       */
short		cnsnds; /*0-Цена материалла без ндс 1- с ндс*/
short		srtnk; /*0-не включена 1-включена сортировка записей в накладной*/
short		vtara; /*Код группы возвратная тара*/
class iceb_u_str bnds1(""); /*0% НДС реализация на територии Украины.*/
class iceb_u_str bnds2(""); /*0% НДС экспорт.*/
class iceb_u_str bnds3(""); /*Освобождение от НДС*/
class iceb_u_str nds_mp(""); /*НДС на мед. препараты*/
short           obzap; /*0-не объединять записи 1-обединять*/
double		kursue;/*Курс условной единицы*/
class iceb_u_str shrt("");    /*Счета розничной торговли*/
short		konost;   /*Контроль остатка 0-включен 1-выключен*/
short		startgod; /*Стартовый год просмотров для материального учёта*/
short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		startgodb; /*Стартовый год просмотров главной книги*/
short		mborvd;    /*0-многопользовательская работа в документе разрешена 1- запрещена*/
char		*kodopsrvc=NULL; //Код операции расчёта средневзвешенной цены
char		*kodopsp=NULL; //Коды операций сторнирования для приходов
char		*kodopsr=NULL; //Коды операций сторнирования для расходов
short metka_pros_mat_s_ost=0; //0-всё показывать 1-только материалы с остатками
extern SQL_baza bd;

int             matnastw(void)
{
class iceb_u_str bros("");
class iceb_u_str st1("");
short           i;

//printw("\n%s\n",
//gettext("Чтение нормативно-справочной информации"));
 
mborvd=vplsh=konost=0;
kursue=0.;
obzap=0;
vtara=0;
srtnk=0;
metka_pros_mat_s_ost=0;

bnds1.new_plus("");
bnds2.new_plus("");
bnds3.new_plus("");
nds_mp.new_plus("");

shrt.new_plus("");
spmatshets.new_plus("");
if(kodopsrvc != NULL)
 {
  delete [] kodopsrvc;
  kodopsrvc=NULL;
 }
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
  
startgod=0;
cnsnds=0;
okrg1=okrcn=0.;
mfnn=(-1);

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='matnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"matnast.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;


  iceb_u_polen(row_alx[0],&st1,1,'|');

  if(iceb_u_SRAV(st1.ravno(),"Контроль остатка на выписке",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
        konost=0;
     else
        konost=1;
     
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
  if(iceb_u_SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     startgod=st1.ravno_atoi(); 
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Блокировка многопользовательской работы в документе",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
        mborvd=1;
     else
        mborvd=0;
     
     continue;
    }
    
  if(iceb_u_SRAV(st1.ravno(),"Счета розничной торговли",0) == 0)
    {
     iceb_u_polen(row_alx[0],&shrt,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Код операции расчёта средневзвешенной цены",0) == 0)
    {
     iceb_u_polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     kodopsrvc=new char[i];
     strcpy(kodopsrvc,bros.ravno());
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"0% НДС реализация на територии Украины",0) == 0)
    {
     iceb_u_polen(row_alx[0],&bnds1,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"0% НДС экспорт",0) == 0)
    {
     iceb_u_polen(row_alx[0],&bnds2,2,'|');
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Освобождение от НДС",0) == 0)
   {
    iceb_u_polen(row_alx[0],&bnds3,2,'|');
    continue;
   }

  if(iceb_u_SRAV(st1.ravno(),"НДС на мед. препараты",0) == 0)
   {
    iceb_u_polen(row_alx[0],&nds_mp,2,'|');
    continue;
   }

  if(iceb_u_SRAV(st1.ravno(),"Цена материалла с НДС",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
       cnsnds=1;
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Объединение одинаковых материалов в накладной",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
       obzap=1;
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Перечень материальных счетов",0) == 0)
    {
     iceb_u_polen(row_alx[0],&spmatshets,2,'|');
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Коды операций сторнирования для приходов",0) == 0)
   {
     iceb_u_polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     kodopsp=new char[i];
     strcpy(kodopsp,bros.ravno());
     continue;
   }
  if(iceb_u_SRAV(st1.ravno(),"Округление цены",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     okrcn=st1.ravno_atof();
     continue;
    }

  if(iceb_u_SRAV(st1.ravno(),"Округление суммы",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     okrg1=st1.ravno_atof();
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Курс УЕ",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     kursue=st1.ravno_atof();
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Код группы возвратная тара",0) == 0)
    {
     iceb_u_polen(row_alx[0],&st1,2,'|');
     vtara=st1.ravno_atoi();
     continue;
    }
  if(iceb_u_SRAV(st1.ravno(),"Сортировка записей в накладной",0) == 0)
   {
    srtnk=0;
    iceb_u_polen(row_alx[0],&st1,2,'|');
      
    if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
       srtnk=1;
     continue;
    
   }
  if(iceb_u_SRAV(st1.ravno(),"Раздельная нумерация накладных",0) == 0)
    {
     mfnn=0;
     
     iceb_u_polen(row_alx[0],&st1,2,'|');
     if(st1.ravno()[0] == '\0' || st1.ravno()[0] == '\n')
      mfnn=(-1);
      
     if(iceb_u_SRAV(st1.ravno(),"Вкл",1) == 0)
       mfnn=1;
     continue;
    }
 }

iceb_poldan("Многопорядковый план счетов",&bros,"nastrb.alx",NULL);
if(iceb_u_SRAV(bros.ravno(),"Вкл",1) == 0)
 vplsh=1;

startgodb=0;
if(iceb_poldan("Стартовый год",&bros,"nastrb.alx",NULL) == 0)
 {
  startgodb=bros.ravno_atoi();
 }
iceb_u_str reploh;

if(okrcn == 0.)
 {
  reploh.plus_ps("Не введено \"Округление цены\" !");
 }

if(okrg1 == 0.)
 {
  reploh.plus("Не введено \"Округление суммы\" !");
 }

if(reploh.getdlinna() > 1)
 iceb_menu_soob(&reploh,NULL);
 
return(0);
}
