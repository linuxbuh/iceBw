/*$Id: i_xud.c,v 1.21 2014/08/31 06:19:19 sasa Exp $*/
/*23.05.2018	07.04.2009	Белых А.И.	i_xud.c
Учёт доверенностей
*/

#define DVERSIQ "25.07.2019"

#include        "buhg_g.h"

void i_xud_rabdok();
int l_dovers(short dp,short mp,short gp,GtkWidget *wpredok);
void poixuddokw(GtkWidget *wpredok);
void i_xud_sg();
void xud_start();
void i_xud_inst();
int poigod_ud(GtkWidget *wpredok);
void udgodsw(GtkWidget *wpredok);

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
iceb_kod_podsystem=ICEB_PS_UD;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);
xud_start();

nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(iceb_get_pnk("00",0,NULL));
iceb_menu_mv_str2.new_plus(gettext("Учёт доверенностей"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с документами"));
menu_str.plus(gettext("Работа с файлом настройки"));
menu_str.plus(gettext("Работа с файлом настройки формы печати"));
menu_str.plus(gettext("Инструменты"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Учёт доверенностей"),fioop.ravno(),iceb_get_pnk("00",0,NULL),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        xud_start();       
       }
      break;

    case 1:
      i_xud_rabdok();
      break;

    case 2:
      iceb_f_redfil("nastud.alx",0,NULL);
      break;

    case 3:
      iceb_f_redfil("xudrasdok.alx",0,NULL);
      break;

    case 4:
      i_xud_inst();
      break;

  }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}



/****************************/
/*Работа с документами*/
/****************************/

void i_xud_rabdok()
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

punkt_m.plus(gettext("Ввод нового документа"));//0
punkt_m.plus(gettext("Поиск введённого документа"));//1
punkt_m.plus(gettext("Просмотр всех введённых документов"));//2

sprintf(strsql,"%s %02d.%02d.%d",gettext("Просмотр документов с"),dt,mt,gt);
punkt_m.plus(strsql); //3
punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//4


while(nomer >= 0)
 {
  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  iceb_u_str datad("");
  iceb_u_str nomdok("");
  switch (nomer)
   {
    case -1:
      break;

    case 0:
      if(udvhdw(&datad,&nomdok,NULL) == 0)
       l_dover(datad.ravno(),nomdok.ravno(),NULL);
      break;

    case 1:
      poixuddokw(NULL);
      break;

    case 2:
      l_dovers(0,0,0,NULL);
      break;

    case 3:
      l_dovers(dt,mt,gt,NULL);
      break;

    case 4:
//      iceb_f_redfil("dovblok.alx",0,NULL);
      iceb_l_blok(NULL);
      break;


   }
 }
}
/****************************/
/*Изменение стартового года*/
/*****************************/
void i_xud_sg()
{
char strsql[512];
short dt=0,mt=0,gt=0;
iceb_u_poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_u_str god("");

iceb_poldan("Стартовый год",&god,"nastud.alx",NULL);

if(god.ravno_atoi() == gt)
  return;

if(god.ravno_atoi() > gt)
 {
  sprintf(strsql,"%s! %d > %d",gettext("Стартовый год больше текущего года"),god.ravno_atoi(),gt);
  iceb_menu_soob(strsql,NULL);
  return;
 }

sprintf(strsql,"update Alx set str='Стартовый год|%d\n',ktoz=%d,vrem=%ld where fil='nastud.alx' and str like 'Стартовый год|%%'",gt,ICEB_KOD_OP_PROG,time(NULL));
iceb_sql_zapis(strsql,1,0,NULL);

}


/******************************/
/*Старт программы*/
/********************************/
void xud_start()
{
if(iceb_nastsys() != 0)
 iceb_exit(1);

if(iceb_perzap(1,NULL))
 iceb_exit(1);

iceb_blokps(); /*автоматическая блокировка подсистем*/
i_xud_sg(); /*установка стартового года*/
}
/**********************************/
/*Удаление за год*/
/***********************************/
void i_xud_udgods()
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
      udgodsw(NULL);
      break;       

    case 1:
      poigod_ud(NULL);
      break;
   }
   
 }
}
/**********************************/
/*инструменты*/
/***********************************/
void i_xud_inst()
{
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Инструменты"));

zagolovok.plus(gettext("Инструменты"));

punkt_m.plus(gettext("Работа с файлами распечаток"));//0
punkt_m.plus(gettext("Удаление документов за год"));//1
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
      i_xud_udgods();
      break; 

    case 2:
      iceb_font_selection(NULL);
      break;
   }
   
 }
}
