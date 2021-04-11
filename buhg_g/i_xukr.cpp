/*$Id: i_xukr.c,v 1.35 2013/12/31 11:49:12 sasa Exp $*/
/*25.02.2019	18.02.2008	Белых А.И.	i_xukr.c
Подсистема "Учёт командировочных расходов"
*/

#define DVERSIQ "20.06.2019"
#include        "buhg_g.h"


int ukrnastw();
void i_xukr_nsi(GtkWidget *wpredok);
void poiukrdokw(GtkWidget *wpredok);
void ukrplcw(GtkWidget *wpredok);
void i_xukr_rd();
void i_xukr_ro();
void ukrrpw();
void ukrkontr();
void ukrprik();
void ukrshkz();
void ukrspdokw();
void ukrspdok1w();
void i_xukr_sg();
void i_xkr_start();
void i_xukr_inst();
void poigod_ukr(GtkWidget *wpredok);
void ukrgodsw(GtkWidget *wpredok);
void xukroppw();

extern SQL_baza	bd;
extern char		*host ;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern char		*parol;
extern iceb_u_str shrift_rek_raz;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
int		kom=0;
iceb_kod_podsystem=ICEB_PS_UKR;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);
i_xkr_start();


nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(iceb_get_pnk("00",0,NULL));
iceb_menu_mv_str2.new_plus(gettext("Учёт командировочных расходов"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с документами"));
menu_str.plus(gettext("Ввод нормативно-справочной информации"));
menu_str.plus(gettext("Распечатка отчётов"));
menu_str.plus(gettext("Инструменты"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Учёт командировочных расходов"),fioop.ravno(),iceb_get_pnk("00",0,NULL),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        i_xkr_start();
       }
      break;

    case 1:
      i_xukr_rd();
      break;


    case 2:
      i_xukr_nsi(NULL);
      break;

    case 3:
      i_xukr_ro();
      break;
      
    case 4:
      i_xukr_inst();
      break;

  }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}

/**********************************/
/*Ввод нормативно-справочной информации*/
/**************************************/
void i_xukr_nsi(GtkWidget *wpredok)
{
char       strsql[512];
iceb_u_str stroka;
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

memset(strsql,'\0',sizeof(strsql));
stroka.plus(gettext("Ввод и корректировка НСИ"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Ввод и корректировка групп затрат на командировки"));//0
punkt_m.plus(gettext("Ввод и корректировка списка единиц измерения"));//1
punkt_m.plus(gettext("Ввод и корректировка командировочных расходов"));//2
punkt_m.plus(gettext("Ввод и корректировка видов командировочных расходов"));//3
punkt_m.plus(gettext("Ввод и корректировка списка контрагентов"));//4
punkt_m.plus(gettext("Работа с файлом настройки"));//5
punkt_m.plus(gettext("Настройка автоматического выполнения проводок"));//6

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);



  iceb_u_str kod("");
  iceb_u_str naim("");
  
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      l_ukrgrup(0,&kod,&naim,NULL);
      break;

    case 1:
      iceb_l_ei(0,&kod,&naim,NULL);
      break;

    case 2:
      l_ukrzat(0,&kod,&naim,NULL);
      break;

    case 3:
      l_ukrvk(0,&kod,&naim,NULL);
      break;

    case 4:
      iceb_l_kontr(0,&kod,&naim,NULL);
      break;

    case 5:
      if(iceb_f_redfil("ukrnast.alx",0,NULL) == 0)
       ukrnastw();
      break;

    case 6:
      iceb_f_redfil("avtprukr.alx",0,NULL);
      break;

   }
   
 }

}
/****************************/
/*Работа с документами*/
/**************************/

void i_xukr_rd()
{
char strsql[512];
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Работа с документами"));


zagolovok.plus(gettext("Работа с документами"));


punkt_m.plus(gettext("Ввод нового документа"));//0
punkt_m.plus(gettext("Поиск введённого документа"));//1
punkt_m.plus(gettext("Просмотр введённых документов"));//2

sprintf(strsql,"%s %02d.%02d.%d%s",gettext("Просмотр документов начиная с"),
dt,mt,gt,
gettext("г."));
punkt_m.plus(strsql);//3

punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//4

int nomer=0;
while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  class iceb_u_str datadok("");
  class iceb_u_str nomdok("");
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      if(ukrvhd(&datadok,&nomdok,NULL) == 0)
        l_ukrdok(datadok.ravno(),nomdok.ravno(),NULL);
        
      break;

    case 1:
      poiukrdokw(NULL);
      break;
      
    case 2:
      l_ukrspdok(0,0,0,NULL);      
      break;

    case 3:
      l_ukrspdok(dt,mt,gt,NULL);      
      break;

    case 4:
      iceb_l_blok(NULL);
      break;

   }
 }
}
/****************************/
/*Отчёты*/
/**************************/

void i_xukr_ro()
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка отчётов"));


zagolovok.plus(gettext("Распечака отчётов"));


punkt_m.plus(gettext("Распечатать реестр проводок по видам командировок"));//0
punkt_m.plus(gettext("Распечатать ведомость по контрагентам"));//1
punkt_m.plus(gettext("Распечатать приказы на командировки"));//2
punkt_m.plus(gettext("Распечатать свод по счетам-кодам расходов"));//3
punkt_m.plus(gettext("Распечатать список документов на командировки"));//4
punkt_m.plus(gettext("Распечатать список документов по датам приказов"));//5
punkt_m.plus(gettext("Распечатать свод расходов по постащикам"));//6

int nomer=0;
while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      ukrrpw();
      break;

    case 1:
      ukrkontr();
      break;

    case 2:
      ukrprik();
      break;

    case 3:
      ukrshkz();
      break;

    case 4:
      ukrspdokw();
      break;

    case 5:
      ukrspdok1w();
      break;
    case 6:
      xukroppw();
      break;
   }
 }
}
/****************************/
/*Изменение стартового года*/
/*****************************/
void i_xukr_sg()
{
char strsql[512];
short dt=0,mt=0,gt=0;
iceb_u_poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_u_str god("");

iceb_poldan("Стартовый год",&god,"ukrnast.alx",NULL);

if(god.ravno_atoi() == gt)
  return;

if(god.ravno_atoi() > gt)
 {
  sprintf(strsql,"%s! %d > %d",gettext("Стартовый год больше текущего года"),god.ravno_atoi(),gt);
  iceb_menu_soob(strsql,NULL);
  return;
 }
extern short startgod; /*Стартовый год*/
startgod=gt;

sprintf(strsql,"update Alx set str='Стартовый год|%d\n',ktoz=%d,vrem=%ld where fil='ukrnast.alx' and str like 'Стартовый год|%%'",gt,ICEB_KOD_OP_PROG,time(NULL));
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
/*Старт программы*/
/*****************************/
void i_xkr_start()
{
if(iceb_nastsys() != 0)
 iceb_exit(1);

if(ukrnastw() != 0)
 iceb_exit(1);

if(iceb_perzap(1,NULL))
 iceb_exit(1);

iceb_blokps(); /*автоматическая блокировка подсистем*/
i_xukr_sg(); /*Установка стартового года*/
}
/**********************************/
/*Удаление за год*/
/***********************************/
void i_xukr_udgods()
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
      ukrgodsw(NULL);
      break;       

    case 1:
      poigod_ukr(NULL);
      break;
   }
   
 }
}
/**********************************/
/*инструменты*/
/***********************************/
void i_xukr_inst()
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
      ukrplcw(NULL);
      break;

    case 2:
     i_xukr_udgods();
     break;
            
    case 3:
      iceb_font_selection(NULL);
      break;
   }
   
 }
}
