/*$Id: i_xrnn.c,v 1.54 2014/08/31 06:19:19 sasa Exp $*/
/*23.05.2018	14.04.2008	Белых А.И.	i_xrnn.c
Реестр налоговых накладных
*/

#define DVERSIQ "20.06.2019"

#include        "buhg_g.h"

int ree_nastw();
void l_xrnnp(short dnp,short mnp,short gnp,GtkWidget *wpredok);
void l_xrnnv(short dnp,short mnp,short gnp,GtkWidget *wpredok);
void xrnn_rasp();
void xrnn_uzzp(GtkWidget*);
void i_xrnn_rsd();
void i_xrnn_nsi();
void i_xrnn_rs();
void rnn_rkontrw();
void i_xrnn_sg();
void i_xrnn_start();
void i_xrnn_inst();
int poigod_rnn(GtkWidget *wpredok);

extern SQL_baza	bd;
extern char		*host ;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern char		*parol;
extern iceb_u_str shrift_rek_raz;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern short dp,mp,gp; //Дата начала просмотра реестров
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
int		kom=0;
iceb_kod_podsystem=ICEB_PS_RNN;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);
i_xrnn_start();


nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(iceb_get_pnk("00",0,NULL));
iceb_menu_mv_str2.new_plus(gettext("Реестр налоговых накладных"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с рееcтрами"));
menu_str.plus(gettext("Распечатать отчёты"));
menu_str.plus(gettext("Ввод нормативно-справочной информации"));
menu_str.plus(gettext("Инструменты"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Реестр налоговых накладных"),fioop.ravno(),iceb_get_pnk("00",0,NULL),nom_op,&menu_str,"white");
  class iceb_u_str kod("");
  class iceb_u_str naim("");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        i_xrnn_start();
       }
      break;
    case 1:
      i_xrnn_rsd();
      break;

    case 2:
      i_xrnn_rs();
      break;

    case 3:
      i_xrnn_nsi();
      break;

    case 4:
      i_xrnn_inst();
      break;
  }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}
/********************************************************/
/*Ввод и корректировка нормативно-справочной информации*/
/********************************************************/
void i_xrnn_nsi()
{
int nomer=0;
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));

zagolovok.plus(gettext("Ввод и корректировка НСИ"));

punkt_m.plus(gettext("Ввод и корректировка списка групп для приходных накладных"));
punkt_m.plus(gettext("Ввод и корректировка списка групп для расходных накладных"));
punkt_m.plus(gettext("Настройка определения вида приходных накладных"));
punkt_m.plus(gettext("Настройка определения вида расходных накладных"));
punkt_m.plus(gettext("Работа с файлом настройки"));

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  class iceb_u_str kod("");
  class iceb_u_str naim("");  

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      l_xrnngpn(0,&kod,&naim,NULL);
      break;

    case 1:
      l_xrnngvn(0,&kod,&naim,NULL);
      break;

    case 2:
      iceb_f_redfil("rnnovdp.alx",0,NULL);
      break;

    case 3:
      iceb_f_redfil("rnnovdr.alx",0,NULL);
      break;

    case 4:
      if(iceb_f_redfil("rnn_nast.alx",0,NULL) == 0)
        ree_nastw();
      break;

   }
 }   
}
/*******************************/
/*Работа с реестрами*/
/*******************************/
void i_xrnn_rsd()
{
char bros[512];
int nomer=0;
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;
short dt,mt,gt;

iceb_u_poltekdat(&dt,&mt,&gt);
if(dt <= 20) /*реестр сдаётся до 20 числа*/
  iceb_u_dpm(&dt,&mt,&gt,4); /*Уменьшить на месяц*/
titl.plus(gettext("Работа с рееcтрами"));


zagolovok.plus(gettext("Работа с рееcтрами"));

punkt_m.plus(gettext("Реестр полученных налоговых накладных"));

sprintf(bros,"%s %02d.%04d %s",gettext("Реестр полученных налоговых накладных"),mt,gt,gettext("г."));
punkt_m.plus(bros);

punkt_m.plus(gettext("Реестр выданных налоговых накладных"));

sprintf(bros,"%s %02d.%04d %s",gettext("Реестр выданных налоговых накладных"),mt,gt,gettext("г."));
punkt_m.plus(bros);

punkt_m.plus(gettext("Установить дату начала просмотра реестров"));
punkt_m.plus(gettext("Устанвливать/снимать блокировку дат"));

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  class iceb_u_str kod("");

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      l_xrnnp(dp,mp,gp,NULL);
      break;

    case 1:
      l_xrnnp(1,mt,gt,NULL);
      break;

    case 2:
      l_xrnnv(dp,mp,gp,NULL);
      break;

    case 3:
      l_xrnnv(1,mt,gt,NULL);
      break;

    case 4:
      if(dp != 0)
       {
        char bros[32];
        sprintf(bros,"%d.%d.%d",dp,mp,gp);
        kod.new_plus(bros);
       }
      if(iceb_menu_vvod1(gettext("Введите дату начала просмотра (д.м.г)"),&kod,11,"",NULL) == 0)
       {
        short d=0,m=0,g=0;
        if(iceb_u_rsdat(&d,&m,&g,kod.ravno(),1) != 0)
         {
          iceb_menu_soob(gettext("Неправильно ввели дату!"),NULL);
         }
        else
         {
          dp=d;
          mp=m;
          gp=g;
         }
       }
      break;


    case 5:
//      iceb_f_redfil("rnn_blok.alx",0,NULL);
      iceb_l_blok(NULL);
      break;

   }
 }


}
/*******************/
/*распечатка отчётов*/
/*********************/

void i_xrnn_rs()
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Распечатать реестр налоговых накладных"));//0
punkt_m.plus(gettext("Распечатать записи по контрагенту"));//0


int nomer=0;
while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch(nomer)
   {

    case 0:
     xrnn_rasp();
     break;

    case 1:
     rnn_rkontrw();
     break;
   }

 }
}
/****************************/
/*Изменение стартового года*/
/*****************************/
void i_xrnn_sg()
{
char strsql[512];
short dt=0,mt=0,gt=0;
iceb_u_poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_u_str datanp("");

iceb_poldan("Дата начала просмотра реестра",&datanp,"rnn_nast.alx",NULL);

short d,m,g;
iceb_u_rsdat(&d,&m,&g,datanp.ravno(),1);

if(gt == g)
 return;

if(iceb_u_sravmydat(d,m,g,dt,mt,gt) > 0)
 {
  sprintf(strsql,"%s! %02d.%02d.%04d > %02d.%02d.%04d",gettext("Дата начала просмотра больше текущей даты"),d,m,g,dt,mt,gt);
  iceb_menu_soob(strsql,NULL);
  return;
 }
extern short dp,mp,gp; //Дата начала просмотра реестров
dp=1; mp=1; gp=gt;
sprintf(strsql,"update Alx set str='Дата начала просмотра реестра|%02d.%02d.%04d\n',ktoz=%d,vrem=%ld where fil='rnn_nast.alx' and str like 'Дата начала просмотра реестра|%%'",1,1,gt,ICEB_KOD_OP_PROG,time(NULL));
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
/******************************/
/*Старт программы*/
/******************************/
void i_xrnn_start()
{
if(iceb_nastsys() != 0)
 iceb_exit(1);

if(ree_nastw() != 0)
 iceb_exit(1);

if(iceb_perzap(1,NULL))
 iceb_exit(1);

iceb_blokps(); /*автоматическая блокировка подсистем*/
i_xrnn_sg(); /*Установка даты начала просмотра реестра*/
}
/**********************************/
/*Удаление за год*/
/***********************************/
void i_xrnn_udgods()
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
      xrnn_uzzp(NULL);
      break;       

    case 1:
      poigod_rnn(NULL);
      break;
   }
   
 }
}

/**********************************/
/*инструменты*/
/***********************************/
void i_xrnn_inst()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Инструменты"));

zagolovok.plus(gettext("Инструменты"));

punkt_m.plus(gettext("Работа с файлами распечаток"));//0
punkt_m.plus(gettext("Удалить записи за период"));//1
//punkt_m.plus(gettext("Выбор шрифта"));//2


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
      i_xrnn_udgods();
      break;

    case 2:
      iceb_font_selection(NULL);
      break;
 
   }
   
 }
}
