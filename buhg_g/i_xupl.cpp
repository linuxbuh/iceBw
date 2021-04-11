/*$Id:$*/
/*23.05.2018	29.02.2008	Белых А.И.	i_xupl.c
Подсистема "Учёт путевых листов"
*/

#define DVERSIQ "30.06.2020"
#include        "buhg_g.h"

int uplnastw();
void i_xupl_nsi(GtkWidget *wpredok);
void i_xupl_dok(GtkWidget *wpredok);
void l_uplspdok(short ds,short ms,short gs,GtkWidget *wpredok);
void i_xupl_ro(GtkWidget *wpredok);
void poiupldokw(GtkWidget *wpredok);
void upl_plc_r(GtkWidget *wpredok);
void upl_pvod();
void upl_pavt();
void upl_rpavt();
void upl_shvod();
void upl_shkg();
void upl_ovdt();
void saldouplw();
void i_xbu_saldo();
void i_xupl_start();
void i_xupl_inst();
void uplgodsw(GtkWidget *wpredok);
int poigod_upl(GtkWidget *wpredok);

extern SQL_baza	bd;
extern char		*host ;
const char *name_system={NAME_SYSTEM};
const char  *version={VERSION};
extern char		*parol;
extern iceb_u_str shrift_rek_raz;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
int		kom=0;
iceb_kod_podsystem=ICEB_PS_UPL;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);
i_xupl_start();


nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(iceb_get_pnk("00",0,NULL));
iceb_menu_mv_str2.new_plus(gettext("Учёт путевых листов"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с документами"));
menu_str.plus(gettext("Ввод нормативно-справочной информации"));
menu_str.plus(gettext("Распечатка отчётов"));
menu_str.plus(gettext("Инструменты"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Учёт путевых листов"),fioop.ravno(),iceb_get_pnk("00",0,NULL),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        i_xupl_start();
       }
      break;

    case 1:
      i_xupl_dok(NULL);
      break;


    case 2:
      i_xupl_nsi(NULL);
      break;

    case 3:
      i_xupl_ro(NULL);
      break;
      
    case 4:
      i_xupl_inst();
      break;



  }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}

/***********************/
/*Ввод нормативно-справочной информации*/
/**************************************/
void i_xupl_nsi(GtkWidget *wpredok)
{
iceb_u_str stroka;
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

stroka.plus(gettext("Ввод и корректировка НСИ"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Справочник подразделений"));//0
punkt_m.plus(gettext("Справочник марок топлива"));//1
punkt_m.plus(gettext("Справочник объектов учёта остатков топлива"));//2
punkt_m.plus(gettext("Справочник видов норм списания топлива"));//4
punkt_m.plus(gettext("Справочник автомобилей"));//4
punkt_m.plus(gettext("Справочник стартовых сальдо"));//5
punkt_m.plus(gettext("Файл настройки"));//6
punkt_m.plus(gettext("Справочник средних цен на топливо и коэфф. перевода в Кг"));//7



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
      l_uplpod(0,&kod,&naim,NULL);
      break;

    case 1:
      l_uplmt(0,&kod,&naim,NULL);
      break;

    case 2:
      l_uplout(0,&kod,&naim,0,NULL);
      break;

    case 3:
      l_uplnst(0,&kod,&naim,NULL);
      break;

    case 4:
      l_uplavt0(0,&kod,&naim,NULL);
      break;


    case 5:
      l_uplsal(NULL);
      break;


    case 6:
      if(iceb_f_redfil("uplnast.alx",0,NULL) == 0)
       uplnastw();
      break;

    case 7:
      l_uplscnt(NULL);
      break;




   }
   
 }
}
/***********************/
/*Работа в документами*/
/**************************************/
void i_xupl_dok(GtkWidget *wpredok)
{
char       strsql[512];
iceb_u_str stroka;
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

titl.plus(gettext("Работа с документами"));

stroka.plus(gettext("Работа с документами"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Ввод нового путевого листа"));//0
punkt_m.plus(gettext("Поиск введённого документа"));//1
punkt_m.plus(gettext("Просмотр всех введённых документов"));//2

sprintf(strsql,"%s %02d.%02d.%d%s",gettext("Просмотр документов начиная с"),
dt,mt,gt,
gettext("г."));
punkt_m.plus(strsql);//3

punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//4



while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);



  iceb_u_str data_dok("");
  iceb_u_str kod_pod("");
  iceb_u_str nomdok("");
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      if(l_upldok_v1(&data_dok,&kod_pod,&nomdok,NULL) == 0)
       l_upldok3(data_dok.ravno(),nomdok.ravno(),kod_pod.ravno_atoi(),NULL);
      break;

    case 1:
      poiupldokw(NULL);
      break;
      
    case 2:
      l_uplspdok(0,0,0,NULL);
      break;


    case 3:
      l_uplspdok(dt,mt,gt,NULL);
      break;

    case 4:
      iceb_l_blok(NULL);
      break;


   }
   
 }
}
/*********************/
/*Распечатка отчётов*/
/*********************/
void i_xupl_ro(GtkWidget *wpredok)
{
iceb_u_str stroka;
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

titl.plus(gettext("Распечатка отчётов"));

stroka.plus(gettext("Распечатка отчётов"));

zagolovok.plus(stroka.ravno());

punkt_m.plus(gettext("Распечатка реестра путевых листов по водителям"));//0
punkt_m.plus(gettext("Распечатка реестра путевых листов по автомобилям"));//1
punkt_m.plus(gettext("Распечатка оборотной ведомости движения топлива по автомобилям"));//2
punkt_m.plus(gettext("Распечатка ведомости списания топлива по водителям и счетам"));//3
punkt_m.plus(gettext("Распечатка ведомости списания топлива по счетам в киллограммах"));//4
punkt_m.plus(gettext("Распечатка оборотной ведомости движения топлива"));//5


while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);



  switch (nomer)
   {
    case -1:
      return;

    case 0:
      upl_pvod();
      break;

    case 1:
      upl_rpavt();
      break;

    case 2:
      upl_pavt();
      break;

    case 3:
      upl_shvod();
      break;

    case 4:
      upl_shkg();
      break;

    case 5:
      upl_ovdt();
      break;


   }
   
 }
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

iceb_poldan("Стартовый год",&god,"uplnast.alx",NULL);

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
  iceb_menu_soob(gettext("Необходимо перенести стартовые остатки топлива!"),NULL);
  return;
 }


}
/*******************************/
/*старт программы*/
/*******************************/
void i_xupl_start()
{
if(iceb_nastsys() != 0)
 iceb_exit(1);

if(uplnastw() != 0)
 iceb_exit(1);

if(iceb_perzap(1,NULL))
 iceb_exit(1);

iceb_blokps(); /*автоматическая блокировка подсистем*/
i_xbu_saldo(); /*проверка переноса стартовых остатков топлива*/
}
/**********************************/
/*Удаление за год*/
/***********************************/
void i_xupl_udgods()
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
      uplgodsw(NULL);
      break;       

    case 1:
      poigod_upl(NULL);
      break;
   }
   
 }
}
/**********************************/
/*инструменты*/
/***********************************/
void i_xupl_inst()
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
punkt_m.plus(gettext("Удаление документов за год"));//3
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
      upl_plc_r(NULL);
      break;


    case 2:
      saldouplw();
      break;

    case 3:
      i_xupl_udgods();
      break;
       
    case 4:
      iceb_font_selection(NULL);
      break;
   }
   
 }
}
