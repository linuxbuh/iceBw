/*$Id: i_xmu.c,v 1.187 2014/07/31 07:08:25 sasa Exp $*/
/*23.05.2018	03.05.2004	Белых А.И.	i_xmu.c
Материольный учёт
*/
#define DVERSIQ "05.03.2020"
#include        "buhg_g.h"

void    xmu_nsi(void);
void    rabdok(void);
void    rasoth(void);
int    sklkar_m(iceb_u_str*,iceb_u_str*,GtkWidget*);
void  l_doksmat(short dp,short mp,short gp,GtkWidget *wpredok);
void  vvodnd(void);
void rasoskr_oiv();
void poimatdokw(GtkWidget *wpredok);
void impmatdokw(void);
void sproznw(void);
void plc_mu(GtkWidget *wpredok);
void ras_dtmc(void);
void muinvw(void);
void dvtmcf1w(void);
void dvtmcf2w(void);
void dvtmcf3w(void);
void zagotkl(void);
void rasspdokw(void);
void rppvow(void);
void rasndsw(void);
void prognozw(void);
void rozn_oth(int,int,const char*);
void poigod_mu(GtkWidget*);
void doocsumw(void);
void kriostw(void);
void matuddokw(void);
void opssw(void);
void l_spec(GtkWidget*);
void spis_p_kontrw(void);
void ost_skl_shetw(GtkWidget *wpredok);
void mudmcw();
void mumow();
void saldomuw(GtkWidget*);
int l_nalog(int,GtkWidget*);
void i_xmu_saldo();
void i_xmu_start();
void i_xmu_inst();
void eksmuw();
void musspiw();
int l_musnrm(GtkWidget *wpredok);

extern SQL_baza	bd;
extern char		*host;
extern char		*parol;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern iceb_u_str shrift_rek_raz;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/
class iceb_u_str kat_for_nal_nak("");
extern class iceb_u_spisok sp_fio_klad; /*выбранные фамилии кладовщиков по складам*/
extern class iceb_u_int sp_kod_sklad; /*список кодов складов для фамилий кладовщиков*/

int main(int argc,char **argv)
{
iceb_kod_podsystem=ICEB_PS_MU;
int		kom=0;
iceb_u_str skl;
iceb_u_str n_kar;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);

//printf("Уровень вхождения циклов=%d\n",gtk_main_level());
i_xmu_start();

nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(iceb_get_pnk("00",0,NULL));
iceb_menu_mv_str2.new_plus(gettext("Материальный учёт"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с документами"));//1
menu_str.plus(gettext("Ввод нормативно-справочной информации"));//2
menu_str.plus(gettext("Распечатать отчёты"));//3
menu_str.plus(gettext("Просмотр карточки материалла"));//4
menu_str.plus(gettext("Инструменты"));//5



while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Материальный учёт"),fioop.ravno(),iceb_get_pnk("00",0,NULL),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        i_xmu_start();
        sp_fio_klad.free_class();        
        sp_kod_sklad.free_class();
       }
      break;


    case 1:
      rabdok();
      break;

    case 2:
      xmu_nsi();
      break;

    case 3:
      rasoth();
      break;


    case 4:
      if(sklkar_m(&skl,&n_kar,NULL) != 0)
        break;
      l_zkartmat(skl.ravno_atoi(),n_kar.ravno_atoi(),NULL);
      break;
      
    case 5:
      i_xmu_inst();        
      break;


   }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}

/***********************/
/*Работа с документами*/
/***********************/

void rabdok(void)
{
char       strsql[512];
iceb_u_str stroka;
int nomer=0;

short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Работа с документами"));

stroka.plus(gettext("Работа с документами"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Ввод нового документа"));//0
punkt_m.plus(gettext("Поиск введённого документа"));//1
punkt_m.plus(gettext("Просмотр всех введённых документов"));//2

sprintf(strsql,"%s %02d.%02d.%d%s",gettext("Просмотр документов начиная с"),
ddd,mmm,ggg,
gettext("г."));

punkt_m.plus(strsql);//3

punkt_m.plus(gettext("Импорт документов"));//4
punkt_m.plus(gettext("Списание розничной торговли"));//5
punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//6
while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      vvodnd();
      break;

    case 1:
      poimatdokw(NULL);
      break;

    case 2:
      l_doksmat(0,0,0,NULL);
      break;

    case 3:
      l_doksmat(ddd,mmm,ggg,NULL);
      break;

    case 4:
      impmatdokw();
      break;

    case 5:
      sproznw();
      break;

    case 6:
      iceb_l_blok(NULL);
      break;



   }
   
 }

}
/*********************************************/
/*ввод и корректировка концовок документов*/
/****************************************/
void i_xmu_vkd()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Ввод концовок документов"));

zagolovok.plus(gettext("Ввод и корректировка концовок документов"));

punkt_m.plus(gettext("Ввод и корректировка концовки счёта"));//0
punkt_m.plus(gettext("Ввод и корректировка концовки накладной (до подписи)"));//1
punkt_m.plus(gettext("Ввод и корректировка концовки накладной (после подписи)"));//2

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

switch (nomer)
 {
  case -1:
    return;

  case 0:
    iceb_f_redfil("matshetend.alx",0,NULL);
    return;

  case 1:
    iceb_f_redfil("matnakend_dp.alx",0,NULL);
    return;

  case 2:
    iceb_f_redfil("matnakend.alx",0,NULL);
    return;
 }
}

/****************************/
/*Ввод нормативно-справочной информации*/
/***************************************/
void    xmu_nsi(void)
{
iceb_u_str stroka;
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

stroka.plus(gettext("Ввод и корректировка НСИ"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Ввод и корректировка списка групп товаров и материалов"));//0
punkt_m.plus(gettext("Ввод и корректировка списка единиц измерения"));//1
punkt_m.plus(gettext("Ввод и корректировка списка товаров и материалов"));//2
punkt_m.plus(gettext("Ввод и корректировка списка операций приходов"));//3
punkt_m.plus(gettext("Ввод и корректировка списка операций расходов"));//4
punkt_m.plus(gettext("Ввод и корректировка списка складов"));//5
punkt_m.plus(gettext("Ввод и корректировка списка форм оплаты"));//6
punkt_m.plus(gettext("Работа с файлом настройки"));//7
punkt_m.plus(gettext("Ввод и корректировка списка контрагентов"));//8
punkt_m.plus(gettext("Ввод и корректировка плана счетов"));//9
punkt_m.plus(gettext("Работа со списком корректировок на сумму документа"));//10
punkt_m.plus(gettext("Настройка автоматического выполнения проводок"));//11
punkt_m.plus(gettext("Ввод и корректировка спецификаций на изделия"));//12
punkt_m.plus(gettext("Ввод и корректировка концовок документов"));//13
punkt_m.plus(gettext("Ввод и корректировка НДС"));//14
punkt_m.plus(gettext("Ввод и корректировка норм списания материалов"));//15

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
      iceb_l_grupmat(0,&kod,&naikod,NULL);
      break;

    case 1:
      iceb_l_ei(0,&kod,&naikod,NULL);
      break;

    case 2:
      l_mater(0,&kod,&naikod,0,0,NULL);
      break;

    case 3:
      iceb_l_opmup(0,&kod,&naikod,NULL);
      break;

    case 4:
      iceb_l_opmur(0,&kod,&naikod,NULL);
      break;

    case 5:
      iceb_l_sklad(0,&kod,&naikod,NULL);
      break;

    case 6:
      iceb_l_forop(0,&kod,&naikod,NULL);
      break;

    case 7:
      if(iceb_f_redfil("matnast.alx",0,NULL) == 0)
        matnastw();
      break;

    case 8:
      iceb_l_kontr(0,&kod,&naikod,NULL);
      break;

    case 9:
      iceb_l_plansh(0,&kod,&naikod,NULL);
      break;

    case 10 :
      iceb_f_redfil("makkor.alx",0,NULL);
      break;

    case 11 :
      menuvnpw(1,0,NULL);
      break;

    case 12 :
      l_spec(NULL);
      break;

    case 13:
      i_xmu_vkd();
      break;


    case 14:
      l_nalog(0,NULL);
      break;

    case 15:
      l_musnrm(NULL);
      break;

   }
   
 }

}
/*****************************/
/*Распечатка отчётов*/
/*****************************/

void rasoth()
{

iceb_u_str stroka;
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка отчётов"));

stroka.plus(gettext("Распечатка отчётов"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Распечатать остатки на карточках"));//0
punkt_m.plus(gettext("Распечатать движение товарно-материальных ценностей"));//1
punkt_m.plus(gettext("Распечатать заготовку для отчёта"));//2
punkt_m.plus(gettext("Распечатать список документов"));//3
punkt_m.plus(gettext("Распечатать реестр проводок по видам операций"));//4
punkt_m.plus(gettext("Распечатать распределение НДС по видам операций"));//5
punkt_m.plus(gettext("Распечатать прогноз реализации товаров"));//6
punkt_m.plus(gettext("Распечатать розничную реализацию по торговим местам"));//7
punkt_m.plus(gettext("Распечатать свод розничной реализации"));//8
punkt_m.plus(gettext("Распечатать суммы выполненных автоматических дооценок"));//10
punkt_m.plus(gettext("Распечатать материалы имеющие критический остаток"));//11

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;
    case 0:
      rasoskr_oiv();
      break;

    case 1:
      ras_dtmc();
      break;

    case 2:
      zagotkl();
      break;

    case 3:
      rasspdokw();
      break;

    case 4:
      rppvow();
      break;

    case 5:
      rasndsw();
      break;

    case 6:
      prognozw();
      break;

    case 7:
      rozn_oth(0,0,"");
      break;

    case 8:
      rozn_oth(1,0,"");
      break;



    case 9:
      doocsumw();
      break;

    case 10:
      kriostw();
      break;


   }
   
 }

}
/*****************************/
/*Ввод нового документа*/
/**************************/

void vvodnd()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Ввод нового документа"));

zagolovok.plus(gettext("Ввод нового документа"));

punkt_m.plus(gettext("Ввод расходного документа"));//0
punkt_m.plus(gettext("Ввод приходного документа"));//1

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);



iceb_u_str kod;
iceb_u_str naikod;
iceb_u_str nomdok;
iceb_u_str sklad;
iceb_u_str datad;
switch (nomer)
 {
  case -1:
    return;

  case 0:
    datad.new_plus("");
    sklad.new_plus("");
    nomdok.new_plus("");
    if(iceb_l_opmur(1,&kod,&naikod,NULL) == 0)
      if(vdndw(2,&datad,&sklad,&nomdok,&kod,NULL) == 0)
           l_dokmat(&datad,1,&sklad,&nomdok,NULL);

    return;

  case 1:
    datad.new_plus("");
    sklad.new_plus("");
    nomdok.new_plus("");
    if(iceb_l_opmup(1,&kod,&naikod,NULL) == 0)
      if(vdndw(1,&datad,&sklad,&nomdok,&kod,NULL) == 0)
           l_dokmat(&datad,1,&sklad,&nomdok,NULL);

    return;


 }
}
/*****************************/
/*Распечатать остатки на картосках*/
/**************************/

void rasoskr_oiv()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Распечатка отчётов"));

zagolovok.plus(gettext("Распечатка отчётов"));

punkt_m.plus(gettext("Распечатать остатки на карточках"));//0
punkt_m.plus(gettext("Распечатать инвентаризационные ведомости"));//1
punkt_m.plus(gettext("Расчёт остатков по складам и счетам учёта"));//2

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

switch (nomer)
 {
  case -1:
    return;

  case 0:
    rasoskrw(NULL);
    return;

  case 1:
    muinvw();
    return;

  case 2:
    ost_skl_shetw(NULL);
    return;
 }
}
/******************************/
/*Меню выбора отчёта*/
/************************/
void ras_dtmc()
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка отчётов"));

iceb_u_str stroka;
stroka.plus(gettext("Распечатать движение товарно-материальных ценностей"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Распечатать движение товарно-материальных ценностей (форма 1)"));//0
punkt_m.plus(gettext("Распечатать движение товарно-материальных ценностей (форма 2)"));//1
punkt_m.plus(gettext("Распечатать движение товарно-материальных ценностей по счетам списания"));//2
punkt_m.plus(gettext("Расчёт списания/получения материаллов по контрагентам"));//3
punkt_m.plus(gettext("Расчёт движения по кодам материалов"));//4
punkt_m.plus(gettext("Расчёт материального отчёта"));//5
punkt_m.plus(gettext("Распечатать свод списания материалов по деталям"));//6

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;
    case 0:
      dvtmcf1w();
      break;
    case 1:
      dvtmcf2w();
      break;
    case 2:
      opssw();
      break;
    case 3:
      spis_p_kontrw();
      break;
    case 4:
      mudmcw();
      break;
    case 5:
      mumow();
      break;
    case 6:
      musspiw();
      break;
   }
   
 }

}
/**************************************/
/*Проверка переноса сальдо*/
/******************************/
void i_xmu_saldo()
{
char strsql[512];
short dt=0,mt=0,gt=0;
iceb_u_poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_u_str god("");

iceb_poldan("Стартовый год",&god,"matnast.alx",NULL);

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
  iceb_menu_soob(gettext("Необходимо перенести стартовые остатки по материалам!"),NULL);
  return;
 }


}
/***********************************/
/*Старт программы*/
/***********************/
void i_xmu_start()
{
if(iceb_nastsys() != 0)
 iceb_exit(1); 
if(matnastw() != 0)
 iceb_exit(1);
if(iceb_perzap(1,NULL))
 iceb_exit(1);

iceb_blokps(); /*автоматическая блокировка подсистем*/
i_xmu_saldo(); /*проверка переноса остатков по материалам*/

}
/**********************************/
/*Удаление за год*/
/***********************************/
void i_xmu_udgods()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Инструменты"));

zagolovok.plus(gettext("Инструменты"));

punkt_m.plus(gettext("Удаление ордеров за год"));//0
punkt_m.plus(gettext("Просмотр списка годов за которые введены кассовые ордера"));//1


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      matuddokw();
      break;       

    case 1:
      poigod_mu(NULL);
      break;
   }
   
 }
}
/*****************************/
/*Инструменты*/
/*******************************/
void i_xmu_inst()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Инструменты"));

zagolovok.plus(gettext("Инструменты"));

punkt_m.plus(gettext("Работа с файлами распечаток"));//0
punkt_m.plus(gettext("Удаление документов за год"));//1
punkt_m.plus(gettext("Проверка логической целосности базы данных"));//2
punkt_m.plus(gettext("Перенос стартовых остатков"));//3
punkt_m.plus(gettext("Экспорт остатков"));//4
punkt_m.plus(gettext("Выбор шрифта"));//5


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
      i_xmu_udgods();
      break;
 
    case 2:
      plc_mu(NULL);
      break;
       
    case 3:
      saldomuw(NULL);
      break;

    case 4:
     eksmuw();
     break;

    case 5:
      iceb_font_selection(NULL);
      break;
   }
   
 }
}
