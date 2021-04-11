/*$Id: i_xkas.c,v 1.75 2014/07/31 07:08:25 sasa Exp $*/
/*26.02.2020	18.01.2006	Белых А.И.	i_xkas.c
Учёт кассовых ордеров (головная программа)
*/
#define DVERSIQ "13.03.2020"
#include        "buhg_g.h"


void    xkas_nsi(void);
int kasnastw(void);
void rab_kas_ord();
void ras_oth();
void l_kasdoks(short ds,short ms,short gs,GtkWidget *wpredok);
void poikasdokw(GtkWidget *wpredok);
void kasothw(int);
void l_kaskcnsl(GtkWidget*);
void kasothkcn(void);
void imp_kasordw(GtkWidget*);
int   l_kasvedz(GtkWidget *wpredok);
void xkasplcw(GtkWidget *wpredok);
void saldokasw();
void i_xkas_start();
void i_xkas_inst();
void udkasdokgw(GtkWidget *wpredok);
void kasgodsw_r(GtkWidget *wpredok);

short   	mfnn=1; /*Метка формирования номера накладной
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                      */
extern SQL_baza	bd;
extern char		*host;
const char *name_system={NAME_SYSTEM};
const char *version={VERSION};
extern char		*parol;
extern iceb_u_str shrift_rek_raz;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
int		kom=0;
iceb_kod_podsystem=ICEB_PS_UKO;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);
i_xkas_start();

nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(iceb_get_pnk("00",0,NULL));
iceb_menu_mv_str2.new_plus(gettext("Учёт кассовых ордеров"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с документами"));
menu_str.plus(gettext("Ввод нормативно-справочной информации"));
menu_str.plus(gettext("Распечатать отчёты"));
menu_str.plus(gettext("Работа с журналом платёжных ведомостей"));
menu_str.plus(gettext("Инструменты"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Учёт кассовых ордеров"),fioop.ravno(),iceb_get_pnk("00",0,NULL),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        i_xkas_start();
       }
      break;
    case 1:
      rab_kas_ord();
      break;

    case 2:
      xkas_nsi();
      break;

    case 3:
      ras_oth();
      break;

    case 4:
      l_kasvedz(NULL);
      break;

    case 5:
      i_xkas_inst();
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
void    xkas_nsi(void)
{
iceb_u_str stroka;
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

stroka.plus(gettext("Ввод и корректировка НСИ"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Ввод и корректировка списка касс"));//0
punkt_m.plus(gettext("Ввод и корректировка кодов целевого назначения"));//1
punkt_m.plus(gettext("Ввод и корректировка списка операций приходов"));//2
punkt_m.plus(gettext("Ввод и корректировка списка операций расходов"));//3
punkt_m.plus(gettext("Ввод и корректировка стартовых сальдо по кодам целевых затрат"));//4
punkt_m.plus(gettext("Работа с файлом настройки"));//5

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  iceb_u_str kod("");
  iceb_u_str naim("");
  
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      l_spis_kas(0,&kod,&naim,NULL);
      break;

    case 1:
      l_kaskcn(0,&kod,&naim,NULL);
      break;

    case 2:
      l_kasopp(0,&kod,&naim,NULL);
      break;

    case 3:
      l_kasopr(0,&kod,&naim,NULL);
      break;

    case 4:
      l_kaskcnsl(NULL);
      break;

    case 5:
      iceb_f_redfil("kasnast.alx",0,NULL);
      break;

   }
   
 }

}
/****************************/
/*Работа с кассовыми ордерами*/
/****************************/

void rab_kas_ord()
{
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);


iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
char strsql[512];
int nomer=0;
titl.plus(gettext("Работа с документами"));

zagolovok.plus(gettext("Работа с документами"));

punkt_m.plus(gettext("Ввод нового ордера на расход"));//0
punkt_m.plus(gettext("Ввод нового ордера на приход"));//1
punkt_m.plus(gettext("Поиск введённого документа"));//2
punkt_m.plus(gettext("Просмотр всех введённых документов"));//3

sprintf(strsql,"%s %02d.%02d.%d",gettext("Просмотр документов с"),dt,mt,gt);
punkt_m.plus(strsql); //4

punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//5
punkt_m.plus(gettext("Импорт кассовых ордеров из файла"));//6

class iceb_u_str datad("");
class iceb_u_str nomdok("");
class iceb_u_str kassa("");

while(nomer >= 0)
 {
  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  switch (nomer)
   {
    case -1:
      break;

    case 0:
      datad.new_plus("");
      nomdok.new_plus("");
      kassa.new_plus("");

      if(vkasord2(&datad,&nomdok,&kassa,NULL) == 0)
       l_kasdok(datad.ravno(),2,nomdok.ravno(),kassa.ravno(),NULL);

      break;

    case 1:
      datad.new_plus("");
      nomdok.new_plus("");
      kassa.new_plus("");
      if(vkasord1(&datad,&nomdok,&kassa,NULL) == 0)
       l_kasdok(datad.ravno(),1,nomdok.ravno(),kassa.ravno(),NULL);
      break;

    case 2:
      poikasdokw(NULL);
      break;

    case 3:
      l_kasdoks(0,0,gt,NULL);
      break;

    case 4:
      l_kasdoks(dt,mt,gt,NULL);
      break;

    case 5:
      iceb_l_blok(NULL);
      break;

    case 6:
      imp_kasordw(NULL);
      break;



   }
 }
}
/*******************************/
/*Получение отчётов*/
/**********************/

void ras_oth()
{
iceb_u_str stroka;
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка отчётов"));

stroka.plus(gettext("Распечатка отчётов"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Распечатать список документов"));//0
punkt_m.plus(gettext("Распечатать свод по операциям"));//1
punkt_m.plus(gettext("Распечатать целевую кассовую книгу"));//2

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;
    case 0:
      kasothw(0);
      break;

    case 1:
      kasothw(1);
      break;

    case 2:
      kasothkcn();
      break;


   }
   
 }

}
/********************************/
/*Старт программы*/
/********************************/
void i_xkas_start()
{
if(iceb_nastsys() != 0)
 iceb_exit(1);
if(kasnastw() != 0)
 iceb_exit(1);
if(iceb_perzap(1,NULL))
 iceb_exit(1);

iceb_blokps(); /*автоматическая блокировка подсистем*/
}
/**********************************/
/*Удаление за год*/
/***********************************/
void i_xkas_udgods()
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
      udkasdokgw(NULL);
      break;       
    case 1:
      kasgodsw_r(NULL);
      break;
   }
   
 }
}

/**********************************/
/*инструменты*/
/***********************************/
void i_xkas_inst()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Инструменты"));

zagolovok.plus(gettext("Инструменты"));

punkt_m.plus(gettext("Работа с файлами распечаток"));//0
punkt_m.plus(gettext("Проверка логической целосности базы данных"));//1
punkt_m.plus(gettext("Перенос сальдо"));//2
punkt_m.plus(gettext("Удаление ордеров за год"));//3
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
      xkasplcw(NULL);
      break;

    case 2:
      saldokasw();
      break;
    case 3:
      i_xkas_udgods();
      break;       

    case 4:
      iceb_font_selection(NULL);
      break;
   }
   
 }
}
