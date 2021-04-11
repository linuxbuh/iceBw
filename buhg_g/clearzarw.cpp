/*$Id: clearzarw.c,v 1.10 2013/08/13 06:09:33 sasa Exp $*/
/*10.07.2015	06.11.2008	Белых А.И.	clearzarw.c
Удаление записей начислений и удержаний в подсистеме "Зароботная плата"
*/
#include "buhg_g.h"

int clearzarw_r(int kom,short mu,short gu,GtkWidget *wpredok);


extern short	ddd,mmm,ggg;

void clearzarw()
{
char strsql[512];

SQL_str		row;
SQLCURSOR       cur;





/*проверяем полномочия оператора*/
sprintf(strsql,"select zpb,zpn from icebuser where login='%s'",iceb_u_getlogin());
if(iceb_sql_readkey(strsql,&row,&cur,NULL) != 1)
 {
  sprintf(strsql,"%s %s %s icebuser",gettext("Не найден логин"),iceb_u_getlogin(),gettext("в таблице"));
  iceb_menu_soob(strsql,NULL);
  return;
 }
if(atoi(row[0]) != 0 || atoi(row[1]) != 0)
 {
  iceb_menu_soob(gettext("У вас нет полномочий для этой операции!"),NULL);
  return;
 }



class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(iceb_get_pnk("00",0,NULL),40);
zagolovok.ps_plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Удаление записей за месяц"));//0
punkt_m.plus(gettext("Удаление записей за год"));//1
punkt_m.plus(gettext("Удаление записей с нулевыми значениями"));//2

int kom=0;
if((kom=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,NULL)) < 0)
  return;

class iceb_u_str dataud("");
short du=0,mu=0,gu=0;


switch (kom)
 {

  case 0:

    sprintf(strsql,"%s (%s)",gettext("Введите дату"),gettext("м.г"));
    if(iceb_menu_vvod1(strsql,&dataud,8,"",NULL) != 0)
     return;
    
    if(iceb_u_rsdat1(&mu,&gu,dataud.ravno()) != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата!"),NULL);
      return;
     }       
    break; 

  case 1:
    if(iceb_menu_vvod1(gettext("Введите год"),&dataud,5,"",NULL) != 0)
     return;
    
    du=0;
    mu=0;
    gu=dataud.ravno_atoi();
    break; 

  case 2:
    du=1;
    mu=mmm;
    gu=ggg;
    iceb_u_dpm(&du,&mu,&gu,4);
    sprintf(strsql,"%d.%d",mu,gu);
    dataud.new_plus(strsql);    
    if(iceb_menu_vvod1(gettext("Введите дату до которой нужно удалить"),&dataud,8,"",NULL) != 0)
     return;

    if(iceb_u_rsdat1(&mu,&gu,strsql) != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата!"),NULL);
      return;
     }

    break;
 }


if(kom == 0)
 {
  if(iceb_pbpds(mu,gu,NULL) != 0)
   return;


  sprintf(strsql,gettext("Удалить записи за %d.%dг. ? Вы уверены ?"),mu,gu);
 }

if(kom == 1)
 {
  for(int ii=0; ii < 12; ii++)
   if(iceb_pbpds(ii+1,gu,NULL) != 0)
    return;
  sprintf(strsql,gettext("Удалить записи %d год ? Вы уверены ?"),gu);
 }
if(kom == 2)
 {
  sprintf(strsql,gettext("Удалить записи с нулевым значением ? Вы уверены ?"));
 }

if(iceb_menu_danet(strsql,2,NULL) == 2)
  return;

if(kom == 0 || kom == 1)
 {
  if(iceb_parol(0,NULL) != 0)
   return;
 }

clearzarw_r(kom,mu,gu,NULL);






}
