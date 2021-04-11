/*$Id: i_start.c,v 1.53 2014/08/31 06:19:19 sasa Exp $*/
/*19.06.2019	10.05.2004	Белых А.И.	i_start.c
Стартовая программа для запуска всех подсистем
*/
#define DVERSIQ "20.06.2019"
#include        <pwd.h>
#include        "buhg_g.h"

void i_start_obd(int argc,char *argv[]);
void razrab(void);
void i_start_sb();
void i_start_instrum(int argc,char **argv);
void optimbazw(int argc,char **argv);

extern char		*imabaz;
extern SQL_baza	bd;
extern char		*host;
extern char		*parol;
const char *name_system={NAME_SYSTEM};
const char *version={VERSION};
extern class iceb_u_str kodirovka_iceb;

extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
class iceb_u_str naim_kontr00("");

int main(int argc,char **argv)
{
char strsql[1024];
char bros[2048];
int		kom=0;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);

if(setenv("PAROLICEB",parol,1) != 0)
  printf("\n%s!\n",gettext("Ошибка установки переменной пароля"));

iceb_nastsys();//читаем шрифты
naim_kontr00.new_plus(iceb_get_pnk("00",0,NULL));
iceb_perzap(1,NULL);

iceb_menu_mv_str1.new_plus(iceb_get_pnk("00",0,NULL));
iceb_menu_mv_str2.new_plus(gettext("Стартовая программа"));

nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));
sql_closebaz(&bd);

//printf("Уровень вхождения циклов=%d\n",gtk_main_level());

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Главная бухгалтерская книга"));
menu_str.plus(gettext("Материальный учет"));
menu_str.plus(gettext("Платёжные документы"));
menu_str.plus(gettext("Зарaботная плата"));
menu_str.plus(gettext("Учёт основных средств"));
menu_str.plus(gettext("Учёт услуг"));
menu_str.plus(gettext("Учёт кассовых ордеров"));
menu_str.plus(gettext("Учёт командировочных расходов"));
menu_str.plus(gettext("Учёт путевых листов"));
menu_str.plus(gettext("Реестр налоговых накладных"));
menu_str.plus(gettext("Учёт доверенностей"));
menu_str.plus(gettext("Инструменты"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Бухгалтерский учёт"),fioop.ravno(),naim_kontr00.ravno(),nom_op,&menu_str,"#0080C9");
  switch(kom)
   {
    case 0:
      i_start_sb();
      break;

    case 1:
      sprintf(strsql,"i_xbu -h %s -b %s",host,imabaz);
      if(system(strsql) != 0)
       {
        sprintf(bros,"%s-%s",gettext("Не выполнена команда"),strsql);
        iceb_menu_soob(bros,NULL);        
       }
      break;

    case 2:
      sprintf(strsql,"i_xmu -h %s -b %s",host,imabaz);
      if(system(strsql) != 0)
       {
        sprintf(bros,"%s-%s",gettext("Не выполнена команда"),strsql);
        iceb_menu_soob(bros,NULL);        
       }
      break;

    case 3:
      sprintf(strsql,"i_xdk -h %s -b %s",host,imabaz);
      if(system(strsql) != 0)
       {
        sprintf(bros,"%s-%s",gettext("Не выполнена команда"),strsql);
        iceb_menu_soob(bros,NULL);        
       }
      break;

    case 4:
      sprintf(strsql,"i_xzp -h %s -b %s",host,imabaz);
      if(system(strsql) != 0)
       {
        sprintf(bros,"%s-%s",gettext("Не выполнена команда"),strsql);
        iceb_menu_soob(bros,NULL);        
       }
      break;

    case 5:
      sprintf(strsql,"i_xuos -h %s -b %s",host,imabaz);
      if(system(strsql) != 0)
       {
        sprintf(bros,"%s-%s",gettext("Не выполнена команда"),strsql);
        iceb_menu_soob(bros,NULL);        
       }
      break;

    case 6:
      sprintf(strsql,"i_xusl -h %s -b %s",host,imabaz);
      if(system(strsql) != 0)
       {
        sprintf(bros,"%s-%s",gettext("Не выполнена команда"),strsql);
        iceb_menu_soob(bros,NULL);        
       }
      break;

    case 7:
      sprintf(strsql,"i_xkas -h %s -b %s",host,imabaz);
      if(system(strsql) != 0)
       {
        sprintf(bros,"%s-%s",gettext("Не выполнена команда"),strsql);
        iceb_menu_soob(bros,NULL);        
       }
      break;

    case 8:
      sprintf(strsql,"i_xukr -h %s -b %s",host,imabaz);
      if(system(strsql) != 0)
       {
        sprintf(bros,"%s-%s",gettext("Не выполнена команда"),strsql);
        iceb_menu_soob(bros,NULL);        
       }
      break;
      break;

    case 9:
      sprintf(strsql,"i_xupl -h %s -b %s",host,imabaz);
      if(system(strsql) != 0)
       {
        sprintf(bros,"%s-%s",gettext("Не выполнена команда"),strsql);
        iceb_menu_soob(bros,NULL);        
       }
      break;


    case 10:
      sprintf(strsql,"i_xrnn -h %s -b %s",host,imabaz);
      if(system(strsql) != 0)
       {
        sprintf(bros,"%s-%s",gettext("Не выполнена команда"),strsql);
        iceb_menu_soob(bros,NULL);        
       }
      break;


    case 11:
      sprintf(strsql,"i_xud -h %s -b %s",host,imabaz);
      if(system(strsql) != 0)
       {
        sprintf(bros,"%s-%s",gettext("Не выполнена команда"),strsql);
        iceb_menu_soob(bros,NULL);        
       }
      break;

    case 12:
      i_start_instrum(argc,argv);
      break;
   }
 }


//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());
//kon:;

    
   
return( 0 );
}

/**********************/
/*Смена базы*/
/************************/
void i_start_sb()
{
struct  passwd  *ktor; /*Кто работает*/

ktor=getpwuid(getuid());

if(sql_openbaz(&bd,imabaz,host,ktor->pw_name,parol) != 0)
    iceb_eropbaz(imabaz,ktor->pw_uid,ktor->pw_name,0);

iceb_start_rf();/*читаем файл с командами и выполняем их*/

iceb_smenabaz(1,NULL); /*закрываем толко что откытую и открываем новую*/

naim_kontr00.new_plus(iceb_get_pnk("00",0,NULL));
sql_closebaz(&bd);

}

/*******************/
/*проверка физицеской целосности*/
/*********************/

void i_start_check_repair(int argc,char **argv)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;


titl.plus(gettext("Инструменты"));

zagolovok.plus(gettext("Инструменты"));
zagolovok.plus("\n",gettext("База"));
zagolovok.plus(":",iceb_get_namebase());

punkt_m.plus(gettext("Проверка физической целосности таблиц базы данных"));//0
punkt_m.plus(gettext("Восстановление таблиц базы данных"));//1

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,NULL);

switch(nomer)
 {
  case 0:

    iceb_start(argc,argv);

    check_tablew(imabaz,NULL);

    sql_closebaz(&bd);

    break;

  case 1:

    iceb_start(argc,argv);

    repair_tablew(imabaz,NULL);

    sql_closebaz(&bd);
    break;
   
  default:
   break;
 }
}
/*********************************/
/*меню личных настроек оператора*/
/*********************************/
void i_start_lno(int argc,char **argv)
{
iceb_start(argc,argv);

iceb_lnom();
sql_closebaz(&bd);
}

/*******************/
/*инструменты*/
/*********************/

void i_start_instrum(int argc,char **argv)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;


titl.plus(gettext("Инструменты"));

zagolovok.plus(gettext("Инструменты"));

punkt_m.plus(gettext("Выгрузить дамп текущей базы"));//0
punkt_m.plus(gettext("Администрирование баз данных"));//1
punkt_m.plus(gettext("Оптимизация базы данных"));//2
punkt_m.plus(gettext("Проверка физической целосности таблиц базы данных"));//3
punkt_m.plus(gettext("Личные настройки оператора"));//4

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,NULL);

switch(nomer)
 {

  case 0:
    iceb_start(argc,argv);
    if(iceb_prov_vdb(imabaz,NULL) != 0)
     {
      sql_closebaz(&bd);
      break;
     }

    sql_closebaz(&bd);

    iceb_dumpbazout(imabaz,iceb_u_getlogin(),parol,host,NULL);

    break;

  case 1:
    if(system("i_admin") != 0)
     {
      char bros[512];
      sprintf(bros,"%s-%s",gettext("Не выполнена команда"),"i_admin");
      iceb_menu_soob(bros,NULL);        
     }
     
    break;

  case 2:
    optimbazw(argc,argv);
    break;

  case 3:
    i_start_check_repair(argc,argv);
    break;

  case 4:
    i_start_lno(argc,argv);
    break;
   
  default:
   break;
 }
}
