/*$Id: i_xdk.c,v 1.66 2014/06/30 06:35:54 sasa Exp $*/
/*23.05.2018	14.03.2006	Белых А.И.	i_xdk.c
Программа работы с платёжными документами и платёжными требованиями
*/
#define DVERSIQ "05.01.2020"
#include        "buhg_g.h"

void i_xdk_platdok(const char *tablica);
int nastdkw(void);
int vpld(iceb_u_str *datad,iceb_u_str *nomdok,const char *tablica,GtkWidget *wpredok);
void l_xdkdoks(short ds,short ms,short gs,const char *tablica,GtkWidget *wpredok);
void l_tippl(const char *tablica,GtkWidget *wpredok);
void xdksoznkom(char *imaf,GtkWidget *wpredok);
void poixdkdokw(const char *tablica,GtkWidget *wpredok);
void xdkgur(const char *tablica);
void xdksend(const char *tablica,GtkWidget *wpredok);
void xdkudgod(void);
void dok_doverw(void);
void xdkplcw(GtkWidget *wpredok);
void i_xdk_sg();
void i_xdk_start();
void i_xdk_inst();
int poigod_xdk(GtkWidget *wpredok);

extern SQL_baza	bd;
extern char		*host;
const char *name_system={NAME_SYSTEM};
const char *version={VERSION};
extern char		*parol;
extern iceb_u_str shrift_rek_raz;
char *imafkomtmp=NULL;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
iceb_kod_podsystem=ICEB_PS_PD;
int		kom=0;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);

i_xdk_start();

nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(iceb_get_pnk("00",0,NULL));
iceb_menu_mv_str2.new_plus(gettext("Платёжные документы"));

char bros[512];
sprintf(bros,"pl%d.tmp",getpid());
imafkomtmp=new char[strlen(bros)+1];
strcpy(imafkomtmp,bros);

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с платёжными поручениями"));//1
menu_str.plus(gettext("Работа с платёжными требованиями"));//2
menu_str.plus(gettext("Работа с файлом настройки"));//3
menu_str.plus(gettext("Выписка доверенностей"));//4
menu_str.plus(gettext("Инструменты"));//5

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Платёжные документы"),fioop.ravno(),iceb_get_pnk("00",0,NULL),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        i_xdk_start();
       }
      break;
    case 1:
      i_xdk_platdok("Pltp");
      break;

    case 2:
      i_xdk_platdok("Pltt");
      break;
    
    case 3:
      if(iceb_f_redfil("nastdok.alx",0,NULL) == 0)
      nastdkw();
      break;


    case 4:
      dok_doverw();
      break;

    case 5:
      i_xdk_inst();
      break;

  }
 }
//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}

/*****************************************/
/*Работа с платёжными документами*/
/********************************/

void i_xdk_platdok(const char *tablica)
{
char strsql[512];
iceb_u_str stroka;
int nomer=0;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
  titl.plus(gettext("Работа с платёжными поручениями"));
if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
  titl.plus(gettext("Работа с платёжными требованиями"));


if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
  stroka.plus(gettext("Работа с платёжными поручениями"));
if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
  stroka.plus(gettext("Работа с платёжными требованиями"));

zagolovok.plus(stroka.ravno());

if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
 {
  punkt_m.plus(gettext("Ввод нового платёжного поручения"));//0
  punkt_m.plus(gettext("Ввод с использованием типового платёжного поручения"));//1
  punkt_m.plus(gettext("Поиск платёжного поручения"));//2
  punkt_m.plus(gettext("Просмотр журнала платёжных поручений"));//3

  sprintf(strsql,"%s %d.%d.%d",gettext("Просмотр журнала платёжных поручений за"),dt,mt,gt);
  punkt_m.plus(strsql); //4
  
  punkt_m.plus(gettext("Распечатка журнала платёжных поручений"));//5

  punkt_m.plus(gettext("Ввод и корректировка контрагентов в файл"));//6

  punkt_m.plus(gettext("Ввод и корректировка контрагентов в базу"));//7

  punkt_m.plus(gettext("Ввод и корректировка операций"));//8

  punkt_m.plus(gettext("Переслать файл платёжек для передачи в банк"));//9

  punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//10


  punkt_m.plus(gettext("Настройка автоматического выполнения проводок"));//11

  punkt_m.plus(gettext("Настройка списка банков для передачи документов"));//12

  
 }

if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
 {
  punkt_m.plus(gettext("Ввод нового платёжного требования"));//0
  punkt_m.plus(gettext("Ввод с использованием типового платёжного требования"));//1
  punkt_m.plus(gettext("Поиск платёжного требования"));//2
  punkt_m.plus(gettext("Просмотр журнала платёжных требований"));//3

  sprintf(strsql,"%s %d.%d.%d",gettext("Просмотр журнала платёжных требований за"),dt,mt,gt);
  punkt_m.plus(strsql); //4
  
  punkt_m.plus(gettext("Распечатка журнала платёжных требований"));//5

  punkt_m.plus(gettext("Ввод и корректировка контрагентов в файл"));//6

  punkt_m.plus(gettext("Ввод и корректировка контрагентов в базу"));//7

  punkt_m.plus(gettext("Ввод и корректировка операций"));//8
//  punkt_m.plus(gettext("Ввод и корректировка типовых платёжных требований"));//9

  punkt_m.plus(gettext("Переслать файл платёжек для передачи в банк"));//9

  punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//10


  punkt_m.plus(gettext("Настройка автоматического выполнения проводок"));//11 

  punkt_m.plus(gettext("Настройка списка банков для передачи документов"));//12

  
 }
iceb_u_str datad;
iceb_u_str kod;
iceb_u_str naikod;
kod.plus("");
naikod.plus("");
datad.plus("");

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      break; //в конце удаляется промежуточный файл
    case 0:
      if(vpld(&datad,&kod,tablica,NULL) == 0)
       {
        xdkdok(tablica,NULL);
       }
      break;

    case 1:
      if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
        l_tippl("Ktnp",NULL);
      if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
        l_tippl("Ktnt",NULL);
      break;

    case 2:
      poixdkdokw(tablica,NULL);
      break;

    case 3:
      l_xdkdoks(0,0,0,tablica,NULL);
      break;

    case 4:
      l_xdkdoks(dt,mt,gt,tablica,NULL);
      break;

    case 5:
      xdkgur(tablica);
      break;

    case 6:
      iceb_f_redfil("platpor.alx",0,NULL);
      break;

    case 7:
      iceb_l_kontr(0,&kod,&naikod,NULL);
      break;

    case 8:
      l_xdkop(0,&kod,&naikod,tablica,NULL);
      break;
/**********************
    case 9:
      if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
        l_tippl("Ktnp",NULL);
      if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
        l_tippl("Ktnt",NULL);
      break;
***********************/

    case 9:
      xdksend(tablica,NULL);
      break;

    case 10:
      iceb_l_blok(NULL);
      break;

      
    case 11:
      if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
         iceb_f_redfil("avtprodok.alx",0,NULL);
      if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
         iceb_f_redfil("avtprodokt.alx",0,NULL);
      break;

    case 12:
      iceb_f_redfil("banki.alx",0,NULL);
      break;


   }
   
 }

unlink(imafkomtmp);

}
/****************************/
/*Изменение стартового года*/
/*****************************/
void i_xdk_sg()
{
char strsql[512];
short dt=0,mt=0,gt=0;
iceb_u_poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_u_str god("");

iceb_poldan("Стартовый год",&god,"nastdok.alx",NULL);

if(god.ravno_atoi() == gt)
  return;

if(god.ravno_atoi() > gt)
 {
  sprintf(strsql,"%s! %d > %d",gettext("Стартовый год больше текущего года"),god.ravno_atoi(),gt);
  iceb_menu_soob(strsql,NULL);
  return;
 }
extern short startgodd; /*Стартовый год платежных документов*/
startgodd=gt;

sprintf(strsql,"update Alx set str='Стартовый год|%d\n',ktoz=%d,vrem=%ld where fil='nastdok.alx' and str like 'Стартовый год|%%'",gt,ICEB_KOD_OP_PROG,time(NULL));
//iceb_sql_zapis(strsql,1,0,NULL);

if(sql_zap(&bd,strsql) != 0)
 {
  int kod_oh=sql_nerror(&bd);
  if(kod_oh == ER_DBACCESS_DENIED_ERROR) //Только чтение
   {
    return;
   }

  iceb_msql_error(&bd,"__FUNCTION__",strsql,NULL);
 }

}
/*****************************/
/*старт программы*/
/*******************************/
void i_xdk_start()
{
if(iceb_prr(NULL) != 0) /*проверяем разрешено ли оператору работать с подсистемой*/
 iceb_exit(0);

if(iceb_nastsys() != 0)
 iceb_exit(1);

if(nastdkw() != 0)
 iceb_exit(1);

if(iceb_perzap(1,NULL))
 iceb_exit(1);

iceb_blokps(); /*автоматическая блокировка подсистем*/
i_xdk_sg(); /*установка стартового года*/

}
/**********************************/
/*Удаление за год*/
/***********************************/
void i_xdk_udgods()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Инструменты"));

zagolovok.plus(gettext("Инструменты"));

punkt_m.plus(gettext("Удаление документов за год"));//0
punkt_m.plus(gettext("Просмотр списка годов за которые введены документы"));//1


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      xdkudgod();
      break;       
    case 1:
      poigod_xdk(NULL);
      break;
   }
   
 }
}
/**********************************/
/*инструменты*/
/***********************************/
void i_xdk_inst()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Инструменты"));

zagolovok.plus(gettext("Инструменты"));


punkt_m.plus(gettext("Работа с файлами распечаток"));//0
punkt_m.plus(gettext("Проверка логической целосности базы данных"));//1
punkt_m.plus(gettext("Удаление документов за год"));//2
//punkt_m.plus(gettext("Выбор шрифта"));//3


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
      xdkplcw(NULL);
      break;
       

    case 2:
      i_xdk_udgods();
      break;

    case 3:
      iceb_font_selection(NULL);
      break;

   }
   
 }
}
