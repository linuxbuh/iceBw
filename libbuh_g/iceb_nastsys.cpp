/*$Id: iceb_nastsys.c,v 1.21 2013/12/31 11:49:19 sasa Exp $*/
/*24.05.2016	26.11.2004	Белых А.И.	iceb_nastsys.c
Чтение системных настроек
*/
#include <sys/stat.h>
#include <errno.h>
#include "iceb_libbuh.h"

extern SQL_baza bd;

int kol_strok_na_liste=64;
int kol_strok_na_liste_l=47; //Количесво строк на стандартном листе в ориентации ландшафт
int kol_strok_na_liste_u=76;//количество строк на листе с уменьшенным межстрочным расстоянием
int kol_strok_na_liste_l_u=56; //Количесво строк на стандартном листе в ориентации ландшафт с уменьшенным межстрочным расстоянием
int timeout_progress_bar=500;
extern int iceb_kod_podsystem;

int iceb_nastsys()
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
if(iceb_kod_podsystem != 0)
 if(iceb_prr(NULL) != 0)
  iceb_exit(0); /*Проверка можно ли работать оператору с этой подсистемой*/


sprintf(strsql,"select str from Alx where fil='nastsys.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob("Не найдены настройки nastsys.alx",NULL);
  return(1);
 }



char stroka2[1024];
kol_strok_na_liste_l=47;
kol_strok_na_liste=64;
kol_strok_na_liste_l_u=56;
kol_strok_na_liste=76;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  
  iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),1,'|');

  if(iceb_u_SRAV(stroka2,"Количество строк на стандартном листе для системы с графическим интерфейсом",0) == 0)
   {
    iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),2,'|');
    kol_strok_na_liste=atoi(stroka2);
    continue;
   }

  if(iceb_u_SRAV(stroka2,"Количество строк на стандартном листе в ориентации ландшафт для системы с графическим интерфейсом",0) == 0)
   {
     iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),2,'|');
     kol_strok_na_liste_l=atoi(stroka2);
     continue;
   }

  if(iceb_u_SRAV(stroka2,"Количество строк на А4 с уменьшенным межстрочным расстоянием для системы с графическим интерфейсом",0) == 0)
   {
    iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),2,'|');
    kol_strok_na_liste_u=atoi(stroka2);
    continue;
   }

  if(iceb_u_SRAV(stroka2,"Количество строк на А4 с уменьшенным межстрочным расстоянием в ориентации ландшафт для системы с графическим интерфейсом",0) == 0)
   {
     iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),2,'|');
     kol_strok_na_liste_l_u=atoi(stroka2);
     continue;
   }

  if(iceb_u_SRAV(stroka2,"Таймаут в милесекундах для движения шкалы степени выполнения",0) == 0)
    {
     iceb_u_polen(row_alx[0],stroka2,sizeof(stroka2),2,'|');
     timeout_progress_bar=atoi(stroka2);
     continue;
    }
 }

class iceb_u_str putnafil(g_get_home_dir());
putnafil.plus(G_DIR_SEPARATOR_S,ICEB_KAT_ALX);

umask(0000); 

if(mkdir(putnafil.ravno(),0775) != 0)
 {
//  printw("%d\n%d\n%d\n%d\n%d\n%d\n",EPERM,EFAULT,EACCES,ENAMETOOLONG,ENOENT,EEXIST);
  if(errno != EEXIST) //Уже есть такой каталог
   { 
    class iceb_u_str repl(__FUNCTION__);


    repl.ps_plus(gettext("Не могу создать каталог !"));
    sprintf(strsql,"%d %s",errno,strerror(errno));
    repl.ps_plus(strsql);

    repl.ps_plus(putnafil.ravno());

    iceb_menu_soob(repl.ravno(),NULL);

   }
 }

umask(ICEB_UMASK); /*Установка маски для записи и чтения группы*/

return(0);
 
}
