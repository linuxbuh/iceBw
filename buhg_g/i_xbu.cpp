/*$Id: i_xbu.c,v 1.185 2014/08/31 06:19:19 sasa Exp $*/
/*02.07.2019	12.08.2003	Белых А.И.	i_xbu.c
Главная книга
*/
#define  DVERSIQ "13.03.2020"
#include "buhg_g.h"
#include "go.h"
#include "pvbanks.h"

void    xbug_nsi(void);
void    xbug_rsp(void);
void	glmenu(int*);
int     buhnast_g(void);
void    l_saldo_sh(GtkWidget *wpredok);
void    l_saldo_shk(GtkWidget *wpredok);
void    vibshet(int);
void    poiprovw();
int     plc_gk(void);
void    xburo(void);
void    glkniw(void);
void    glkni_nbs_m(void);
void    rspvk(GtkWidget *wpredok);
int     go_m(class go_rr*);
int     go_ss_nbs(class go_rr*);
int     go_srv_nbs(class go_rr*);
int     go_srk(class go_rr*,int);
int     vibor_ss(void);
void    vibkontr(void);
int     pvbank_m(class pvbank_rr *rek_ras);
int     pvbanks_m(class pvbanks_rr *rek_ras);
int     pvbanks_r(class pvbanks_rr *rek_ras);
void    pvbank_r(class pvbank_rr *rek_ras);
void    clprov_m(void);
void    buhkomw(void);
void buhhahw(void);
void buhsdkrw(void);
void buhrpzw(void);
void buhrpznpw(void);
void buhgspw(void);
void buhvdzw(void);
void buhspkw(void);
void buhpsdw(void);
int gosssw_r(class go_rr *datark,GtkWidget *wpredok);
int gosrsnw_r(class go_rr *datark,int metkasort,GtkWidget *wpredok);
void saldogkw(GtkWidget*);
void i_xbu_saldo();
void i_xbu_inst();
void xbu_start();
void buh_rnr_shw();
void imp_prov(GtkWidget *wpredok);
void l_kv_start(GtkWidget *wpredok);
int shetsrs_m(class go_rr *rek_r,GtkWidget *wpredok);

extern SQL_baza	bd;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern iceb_u_str shrift_rek_raz;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
int		kom=0;
iceb_kod_podsystem=ICEB_PS_GK;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);

xbu_start();
 

nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(iceb_get_pnk("00",0,NULL));
iceb_menu_mv_str2.new_plus(gettext("Главная бухгалтерская книга"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с проводками"));//1
menu_str.plus(gettext("Ввод нормативно-справочной информации"));//2
menu_str.plus(gettext("Распечатать отчёты"));//3
menu_str.plus(gettext("Инструменты"));//4

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Главная бухгалтерская книга"),fioop.ravno(),iceb_get_pnk("00",0,NULL),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        xbu_start();
       }
      break;

    case 1:
      xbug_rsp();
      break;

    case 2:
      xbug_nsi();
      break;

    case 3:
      xburo();
      break;

    case 4:
      i_xbu_inst();
      break;



   }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}

/****************************************/
/*Ввод нормативно-справочной информации*/
/****************************************/

void    xbug_nsi(void)
{
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

zagolovok.plus(gettext("Ввод и корректировка НСИ"));

punkt_m.plus(gettext("Ввод и корректировка плана счетов"));//0
punkt_m.plus(gettext("Ввод и корректировка списка контрагентов"));//1
punkt_m.plus(gettext("Ввод и корректировка списка контрагентов для счёта"));//2
punkt_m.plus(gettext("Ввод и корректировка стартового сальдо по счетам"));//3
punkt_m.plus(gettext("Ввод и корректировка стартового сальдо по контрагентам"));//4
punkt_m.plus(gettext("Работа с файлом настройки"));//5
punkt_m.plus(gettext("Ввод и корректировка списка групп контрагентов"));//6
punkt_m.plus(gettext("Ввод и корректировка списка суббалансов"));//7
punkt_m.plus(gettext("Настройка списка банков для перечисления средств на картсчета"));//8
punkt_m.plus(gettext("Справочник валют"));//9
punkt_m.plus(gettext("Справочник курсов валют"));//10


while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);



  class iceb_u_str kod("");
  class iceb_u_str naikod("");
  class iceb_u_str shetsk("");
  class iceb_u_str repl;
  OPSHET rekshet;
  
  switch (nomer)
   {
    case -1:
      return;
      
    case 0:
      iceb_l_plansh(0,&kod,&naikod,NULL);
      break;

    case 1:
          
      iceb_l_kontr(0,&kod,&naikod,NULL);
      break;

    case 2:

      repl.new_plus(gettext("Введите номер счета с развернутым сальдо"));
      if(iceb_menu_vvod1(repl.ravno(),&shetsk,20,"",NULL) != 0)
        break;
      if(shetsk.ravno()[0] == '\0')
       {
        if(iceb_l_plansh(1,&kod,&naikod,NULL) != 0)
         break;
        shetsk.new_plus(kod.ravno());
        naikod.new_plus("");
       }        

      if(iceb_prsh1(shetsk.ravno(),&rekshet,NULL) != 0)
        break;
      if(rekshet.saldo != 3)
       {
        iceb_menu_soob(gettext("Счёт имеет свернутое сальдо !"),NULL);
        break;      
       }       
      iceb_l_kontrsh(0,shetsk.ravno(),&kod,&naikod,NULL);
        
      break;

    case 3:
      l_saldo_sh(NULL);
      break;

    case 4:
      l_saldo_shk(NULL);
      break;

    case 5:
      if(iceb_f_redfil("nastrb.alx",0,NULL) == 0)
        buhnast_g();
      break;

    case 6:
      iceb_l_gkontr(0,&kod,&naikod,NULL);
      break;
    case 7:
      iceb_l_subbal(0,&kod,&naikod,NULL);
      break;


    case 8:
      iceb_f_redfil("zarbanks.alx",0,NULL);
      break;

    case 9:
      l_sval(0,&kod,&naikod,NULL);
      break;

    case 10:
      l_kv_start(NULL);
      break;
   }
   
 }

}
/****************************************/
/*Работа с проводками*/
/****************************************/

void    xbug_rsp(void)
{
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Работа с проводками"));

zagolovok.plus(gettext("Работа с проводками"));

punkt_m.plus(gettext("Ввод проводок"));//0
punkt_m.plus(gettext("Поиск проводок"));//1
punkt_m.plus(gettext("Установка или снятие блокировки проводок"));//2
punkt_m.plus(gettext("Ввод проводок по группам контрагентов"));//3
punkt_m.plus(gettext("Импорт проводок"));//4

while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      vibshet(0);
      break;

    case 1:
      poiprovw();
      break;


    case 2:
      iceb_l_blok(NULL);
      break;

    case 3:
      vibshet(1);
      break;

    case 4:
      imp_prov(NULL);
      break;
   }
   
 }

}
/****************************************/
/*Рапечатка шахматок*/
/****************************************/

void    xbug_rhax()
{
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка шахматок"));

zagolovok.plus(gettext("Распечатка шахматок"));

punkt_m.plus(gettext("Расчёт по вариантам"));//0
punkt_m.plus(gettext("Расчёт шахматки по счёту"));//1
punkt_m.plus(gettext("Расчёт по заданным спискам дебетовых и кредитовых счетов"));//2
punkt_m.plus(gettext("Расчёт распределения административных затрат на доходы"));//3
punkt_m.plus(gettext("Расчёт видов затрат по элементам"));//4
punkt_m.plus(gettext("Расчёт по спискам групп контрагентов"));//5
punkt_m.plus(gettext("Расчёт валовых доходов и затрат"));//6

while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      buhhahw();
      break;

    case 1:
      buh_rnr_shw();
      break;
      
    case 2:
      buhsdkrw();
      break;

    case 3:
      buhrpznpw();
      break;
  
    case 4:
      buhrpzw();
      break;

    case 5:
      buhgspw();
      break;

    case 6:
      buhvdzw();
      break;


   }
   
 }

}
/****************************************/
/*Расчёт сальдо*/
/****************************************/

void    xbug_rsal()
{
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Расчёт сальдо"));

zagolovok.plus(gettext("Расчёт сальдо"));

punkt_m.plus(gettext("Расчёт сальдо по всем контрагентам"));//0
punkt_m.plus(gettext("Расчёт сальдо по контрагенту"));//1

while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      rspvk(NULL);
      break;

    case 1:
      iceb_salorok("",NULL);
      break;


   }
   
 }

}
/**********************************/
/*Распечатать отчёты              */
/**********************************/

void xburo(void)
{
static class go_rr  rek_ras;
//int metkasort=0;

int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатать отчёты"));

zagolovok.plus(gettext("Распечатать отчёты"));

punkt_m.plus(gettext("Расчёт оборотного баланса и главной книги"));//0
punkt_m.plus(gettext("Расчёт журнала-ордера по счёту"));//1
punkt_m.plus(gettext("Расчёт сальдо"));//2
punkt_m.plus(gettext("Расчёт оборотного баланса по забалансовым счетам"));//3
punkt_m.plus(gettext("Распечатать документы для перечисления на карт-счета"));//4
punkt_m.plus(gettext("Распечатать отчёт по комментариям"));//5
punkt_m.plus(gettext("Расчёт шахматки"));//6
punkt_m.plus(gettext("Расчёт акта сверки по группе контрагента"));//7
punkt_m.plus(gettext("Проверка согласованности данных в подсистемах"));//8


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      glkniw();
      break;
        
      break;

    case 1:
      if(go_m(&rek_ras) != 0)
       break;
      if(rek_ras.saldo != 3)  //Свернутое сальдо
       {
        if(rek_ras.stat == 0)
         {
          if(gosssw_r(&rek_ras,NULL) == 0)
            iceb_rabfil(&rek_ras.imaf,&rek_ras.naimf,NULL);

         }
        if(rek_ras.stat == 1)
          go_ss_nbs(&rek_ras);

       }
      else  // Развернутое сальдо
       {
        if(rek_ras.stat == 0)
         {
          if(shetsrs_m(&rek_ras,NULL) != 0)
           break;
          if(rek_ras.metka_oth == 0)
           {
              if(gosrsnw_r(&rek_ras,rek_ras.metkasort,NULL) == 0)
               iceb_rabfil(&rek_ras.imaf,&rek_ras.naimf,NULL);
           }
          if(rek_ras.metka_oth == 1)
              if(go_srk(&rek_ras,rek_ras.metkasort) == 0)
                   iceb_rabfil(&rek_ras.imaf,&rek_ras.naimf,NULL);
         }
        if(rek_ras.stat == 1)
            go_srv_nbs(&rek_ras);
       }
                    
      break;

    case 2:
      xbug_rsal();
      break;


    case 3:
      glkni_nbs_m();
      break;

    case 4:
      static class pvbanks_rr rek_rass;
      if(pvbanks_m(&rek_rass) == 0)
       pvbanks_r(&rek_rass);
      break;

    case 5:
      buhkomw();
      break;
  
    case 6:
      xbug_rhax();
      break;


    case 7:
      buhspkw();
      break;

    case 8:
      buhpsdw();
      break;
   }
   
 }
}
/*******************/
/*выбрать вид отчёта*/
/*********************/

int vibor_ss()
{
char bros[512];

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

sprintf(bros,"%s 1",gettext("Расчёт журнала-ордера форма"));
punkt_m.plus(bros);
sprintf(bros,"%s 2",gettext("Расчёт журнала-ордера форма"));
punkt_m.plus(bros);

int nomer=0;
return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL));

}
/**************************************/
/*Проверка переноса сальдо*/
/******************************/
void i_xbu_saldo()
{
char strsql[512];
short dt=0,mt=0,gt=0;
iceb_u_poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_u_str god("");

iceb_poldan("Стартовый год",&god,"nastrb.alx",NULL);

if(god.ravno_atoi() == gt)
  return;

if(god.ravno_atoi() > gt)
 {
  sprintf(strsql,"%s! %d > %d",gettext("Стартовый год больше текущего года"),god.ravno_atoi(),gt);
  iceb_menu_soob(strsql,NULL);
  return;
 }

if(god.ravno_atoi() < gt)
 {
  iceb_menu_soob(gettext("Необходимо перенести сальдо!"),NULL);
  return;
 }


}
/**********************************/
/*Удаление за год*/
/***********************************/
void i_xbu_udgods()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Инструменты"));

zagolovok.plus(gettext("Инструменты"));

punkt_m.plus(gettext("Удаление проводок"));//0
punkt_m.plus(gettext("Просмотр списка годов за которые введены проводки"));//1


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      clprov_m();
      break;       
    case 1:
      rbd(1);
      break;
   }
   
 }
}


/**********************************/
/*инструменты*/
/***********************************/
void i_xbu_inst()
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Инструменты"));

zagolovok.plus(gettext("Инструменты"));

punkt_m.plus(gettext("Работа с файлами распечаток"));//0
punkt_m.plus(gettext("Удалить проводки"));//1
punkt_m.plus(gettext("Проверка логической целосности базы данных"));//2
punkt_m.plus(gettext("Перенос сальдо"));//3
//punkt_m.plus(gettext("Выбор шрифта"));//4


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      iceb_dirlst("lst,txt,csv,xml,dbf",NULL);
      break;


    case 1:
      i_xbu_udgods();
      break;
 
    case 2:
      plc_gk();
      break;


    case 3:
      saldogkw(NULL);
      break;

    case 4:
      iceb_font_selection(NULL);
      break;
       
   }
   
 }
}
/**********************************/
/*старт программы*/
/************************************/
void xbu_start()
{
if(iceb_nastsys() != 0)
 iceb_exit(1);
if(buhnast_g() != 0)
 iceb_exit(1);
if(iceb_perzap(1,NULL))
 iceb_exit(1);
iceb_blokps(); /*автоматическая блокировка подсистем*/
i_xbu_saldo(); /*проверка стартового года*/

}