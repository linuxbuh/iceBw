/*$Id: i_xusl.c,v 1.89 2014/02/28 05:20:59 sasa Exp $*/
/*23.05.2018	17.07.2005	Белых А.И.	i_xusl.c
Главнаяя программа подсистемы учёт услуг
*/
#define  DVERSIQ "21.01.2020"
#include        "buhg_g.h"

void    xusl_nsi(void);
void usl_rabdok();
void l_usldoks(short ds,short ms,short gs,GtkWidget *wpredok);
void poiusldokw(GtkWidget *wpredok);
void uddusl_r(GtkWidget *wpredok);
void impusldokw(void);
void rasothusl(void);
void dvuslf2w(void);
void rasspdokuw(void);
int uslnastw(void);
void rppvouw(void);
void poigod_usl(GtkWidget *wpredok);
void xuslplcw(GtkWidget *wpredok);
void opss_uw(int);
void i_xusl_sg();
void i_xusl_start();
void i_xusl_inst();
void usldupkw();

extern SQL_baza	bd;
extern char		*host;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern char		*parol;
extern iceb_u_str shrift_rek_raz;
extern short startgodus;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/
class iceb_u_str kat_for_nal_nak("");

int main(int argc,char **argv)
{
int		kom=0;
iceb_kod_podsystem=ICEB_PS_UU;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);
i_xusl_start();


nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(iceb_get_pnk("00",0,NULL));
iceb_menu_mv_str2.new_plus(gettext("Учёт услуг"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с документами"));
menu_str.plus(gettext("Ввод нормативно-справочной информации"));
menu_str.plus(gettext("Распечатка отчётов"));
menu_str.plus(gettext("Инструменты"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Учёт услуг"),fioop.ravno(),iceb_get_pnk("00",0,NULL),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        i_xusl_start();
       }
      break;


    case 1:
      usl_rabdok();
      break;

    case 2:
      xusl_nsi();
      break;

    case 3:
      rasothusl();
      break;

    case 4:
      i_xusl_inst();
      break;


  }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}

/****************************/
/*Ввод нормативно-справочной информации*/
/***************************************/
void    xusl_nsi(void)
{
iceb_u_str stroka;
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

stroka.plus(gettext("Ввод и корректировка НСИ"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Ввод и корректировка списка групп услуг"));//0
punkt_m.plus(gettext("Ввод и корректировка списка единиц измерения"));//1
punkt_m.plus(gettext("Ввод и корректировка списка услуг"));//2
punkt_m.plus(gettext("Ввод и корректировка списка операций приходов"));//3
punkt_m.plus(gettext("Ввод и корректировка списка операций расходов"));//4
punkt_m.plus(gettext("Ввод и корректировка списка подразделений"));//5
punkt_m.plus(gettext("Ввод и корректировка списка форм оплаты"));//6
punkt_m.plus(gettext("Работа с файлом настройки"));//7
punkt_m.plus(gettext("Ввод и корректировка списка контрагентов"));//8
punkt_m.plus(gettext("Ввод и корректировка плана счетов"));//9
punkt_m.plus(gettext("Работа со списком корректировок на сумму документа"));//10
punkt_m.plus(gettext("Настройка автоматического выполнения проводок"));//11
punkt_m.plus(gettext("Ввод и корректировка концовки счёта"));//12

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);



  iceb_u_str kod;
  iceb_u_str naikod;
  
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      l_uslgrup(0,&kod,&naikod,NULL);
      break;

    case 1:
      iceb_l_ei(0,&kod,&naikod,NULL);
      break;

    case 2:
      l_uslugi(0,&kod,&naikod,NULL);
      break;

    case 3:
      l_uslopp(0,&kod,&naikod,NULL);
      break;

    case 4:
      l_uslopr(0,&kod,&naikod,NULL);
      break;

    case 5:
      l_uslpodr(0,&kod,&naikod,NULL);
      break;

    case 6:
      iceb_l_forop(0,&kod,&naikod,NULL);
      break;

    case 7:
      if(iceb_f_redfil("uslnast.alx",0,NULL) == 0)
        uslnastw();
      break;

    case 8:
      iceb_l_kontr(0,&kod,&naikod,NULL);
      break;

    case 9:
      iceb_l_plansh(0,&kod,&naikod,NULL);
      break;

    case 10 :
      iceb_f_redfil("uslkor.alx",0,NULL);
      break;

    case 11 :
      menuvnpw(2,0,NULL);

      break;

    case 12:
      iceb_f_redfil("usl_shet_fak_end.alx",0,NULL);
      break;


   }
   
 }

}
/****************************************/
/*Работа с документами*/
/******************************/

void usl_rabdok()
{
short godn;
short dd,md,gd;
iceb_u_str nomdok;
int podr;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

godn=startgodus;
if(godn == 0)
 godn=gt;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
char strsql[512];
int nomer=0;
titl.plus(gettext("Работа с документами"));

zagolovok.plus(gettext("Работа с документами"));

punkt_m.plus(gettext("Ввод нового документа на выполнение услуг"));//0
punkt_m.plus(gettext("Ввод нового документа на получение услуг"));//1
punkt_m.plus(gettext("Поиск введённого документа"));//2
punkt_m.plus(gettext("Просмотр всех введённых документов"));//3

sprintf(strsql,"%s %02d.%02d.%d",gettext("Просмотр документов с"),dt,mt,gt);
punkt_m.plus(strsql); //4

punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//5
punkt_m.plus(gettext("Импорт документов"));//6

while(nomer >= 0)
 {
  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  switch (nomer)
   {
    case -1:
      break;

    case 0:

      podr=0;
      nomdok.new_plus("");
      dd=md=gd=0;

      if(vdnuw(&dd,&md,&gd,2,&nomdok,&podr,NULL) == 0)
        l_usldok(dd,md,gd,2,podr,nomdok.ravno(),NULL);

      break;

    case 1:
      podr=0;
      nomdok.new_plus("");
      dd=md=gd=0;

      if(vdnuw(&dd,&md,&gd,1,&nomdok,&podr,NULL) == 0)
        l_usldok(dd,md,gd,1,podr,nomdok.ravno(),NULL);
      break;

    case 2:
      poiusldokw(NULL);
      break;

    case 3:
      l_usldoks(0,0,godn,NULL);
      break;

    case 4:
      l_usldoks(dt,mt,gt,NULL);
      break;

    case 5:
      iceb_l_blok(NULL);
      break;

    case 6:
      impusldokw();
      break;


   }
 }
}
/*****************************/
/*Распечатка отчётов*/
/*****************************/

void rasothusl()
{

iceb_u_str stroka;
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка отчётов"));

stroka.plus(gettext("Распечатка отчётов"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Распечатать движение услуг"));//0
punkt_m.plus(gettext("Распечатать список документов"));//1
punkt_m.plus(gettext("Распечатать реестр проводок по видам операций"));//2
punkt_m.plus(gettext("Распечатать движение услуг по счетам получения"));//3
punkt_m.plus(gettext("Распечатать движение услуг по счетам списания"));//4
punkt_m.plus(gettext("Распечатать списание/получение услуг по контрагентам"));//5

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;
    case 0:
      dvuslf2w();
      break;

    case 1:
      rasspdokuw();
      break;

    case 2:
      rppvouw();
      break;


    case 3:
      opss_uw(1);
      break;


    case 4:
      opss_uw(2);
      break;

    case 5:
     usldupkw();
     break;
   }
   
 }

}
/****************************/
/*Изменение стартового года*/
/*****************************/
void i_xusl_sg()
{
char strsql[512];
short dt=0,mt=0,gt=0;
iceb_u_poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_u_str god("");

iceb_poldan("Стартовый год",&god,"uslnast.alx",NULL);

if(god.ravno_atoi() == gt)
  return;

if(god.ravno_atoi() > gt)
 {
  sprintf(strsql,"%s! %d > %d",gettext("Стартовый год больше текущего года"),god.ravno_atoi(),gt);
  iceb_menu_soob(strsql,NULL);
  return;
 }
extern short startgodus; /*Стартовый год просмотров услуг*/
startgodus=gt;

sprintf(strsql,"update Alx set str='Стартовый год|%d\n',ktoz=%d,vrem=%ld where fil='uslnast.alx' and str like 'Стартовый год|%%'",gt,ICEB_KOD_OP_PROG,time(NULL));
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
/************************/
/*Старт программы*/
/**************************/
void i_xusl_start()
{
if(iceb_nastsys() != 0)
 iceb_exit(1);
if(uslnastw() != 0)
 iceb_exit(1);
if(iceb_perzap(1,NULL))
 iceb_exit(1);
iceb_blokps(); /*автоматическая блокировка подсистем*/
i_xusl_sg();  /*Установка стартового года*/
}
/**********************************/
/*Удаление за год*/
/***********************************/
void i_xusl_udgods()
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
      uddusl_r(NULL);
      break;
 
    case 1:
      poigod_usl(NULL);
      break;
   }
   
 }
}
/**********************************/
/*инструменты*/
/***********************************/
void i_xusl_inst()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Инструменты"));

zagolovok.plus(gettext("Инструменты"));

punkt_m.plus(gettext("Работа с файлами распечаток"));//0
punkt_m.plus(gettext("Проверка логической целосности базы данных"));//1
punkt_m.plus(gettext("Удаление всех документов за год"));//2
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
      xuslplcw(NULL);
      break;
       
    case 2:
      i_xusl_udgods();
      break;

    case 3:
      iceb_font_selection(NULL);
      break;
   }
   
 }
}
