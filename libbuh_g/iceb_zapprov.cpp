/* $Id: iceb_zapprov.c,v 1.15 2013/08/13 06:10:14 sasa Exp $ */
/*18.02.2009    26.11.1997      Белых А.И.      iceb_zapprov.c
Запись проводок
Если вернули 0 запись прошла успешно
             1 нет
*/
#include        "iceb_libbuh.h"

extern SQL_baza	bd;

int iceb_zapprov(int val, //0-балансовая проводка -1 не балансовая
int god,int mes,int den, //Дата
const char *sh, //Счет
const char *shk, //Счет коррепондент
const char *kor1, //Код контрагента счета
const char *kor2, //Код контрагента счета корреспондента
const char *kto,  //Кто записал
const char *nn,   //Номер документа
const char *kop,  //Код операции
double deb,  //Сумма в дебете
double kre,  //Сумма в кредите
const char *raz,  //Коментарий
short kp,    //Количество проводок
int pod,     //Подразделение
time_t vrem,   //Время записи
short gd,short md,short dd, //Дата документа
int tipz, //1-приходный докуемент 2-расходный документ 0- не определено
int kekv,//код экономической классификации расходов
GtkWidget *wpredok)
{
int             ktor; /*Кто работает*/
char		strsql[2048];
class iceb_u_str kontr1("");
class iceb_u_str kontr2("");


//printf("\niceb_zapprov-%d %d.%d.%d %s %s %s %s %s %s %s %d %.2f %.2f %s дата док.%d.%d.%d tipz=%d\n",
//val,god,mes,den,sh,shk,kor1,kor2,kto,nn,kop,pod,deb,kre,raz,dd,md,gd,tipz);

if(iceb_pbsh(mes,god,sh,shk,gettext("Невозможно записать проводку!"),wpredok) != 0)
 return(1);


kontr1.new_plus(kor1);
kontr2.new_plus(kor2);

if(kontr1.ravno()[0] == '\0')
 kontr1.new_plus(kor2);

if(kontr2.ravno()[0] == '\0')
 kontr2.new_plus(kor1);

deb=iceb_u_okrug(deb,0.01);
kre=iceb_u_okrug(kre,0.01);

ktor=iceb_getuid(wpredok);

sprintf(strsql,"insert into Prov \
values (%d,'%04d-%02d-%02d','%s','%s','%s','%s','%s',%d,'%s',%.2f,%.2f,%d,%ld,\
'%s','%04d-%02d-%02d',%d,%d)",
val,god,mes,den,sh,shk,kontr1.ravno(),kto,nn,pod,kop,deb,kre,ktor,vrem,raz,
gd,md,dd,tipz,kekv);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
   return(1);
  }
 else
  {
   iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok );
   return(1);
  }
 }
if(kp == 1)
  return(0);

sprintf(strsql,"insert into Prov \
values (%d,'%04d-%02d-%02d','%s','%s','%s','%s','%s',%d,'%s',%.2f,%.2f,%d,%ld,\
'%s','%04d-%02d-%02d',%d,%d)",
val,god,mes,den,shk,sh,kontr2.ravno(),kto,nn,pod,kop,kre,deb,ktor,vrem,raz,
gd,md,dd,tipz,kekv);

if(sql_zap(&bd,strsql) != 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }
return(0);
}
