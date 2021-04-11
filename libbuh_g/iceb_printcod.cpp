/*$Id: iceb_printcod.c,v 1.7 2013/05/17 14:58:36 sasa Exp $*/
/*30.09.2014	24.03.2000	Белых А.И.  iceb_printcod.c
Распечатка списка кодов в файл
*/
#include	"iceb_libbuh.h"

extern SQL_baza	bd;

void iceb_printcod(FILE *ff,
const char *tabl,    //Имя таблицы
const char *naimkod, //Наименование поля по которому надо искать
const char *naimp,   //Наименование поля в таблице, которое надо найти
int metka,      //0-код цифровой 1-символьный
const char *spisok,  //Список параметров по которым надо искать
int *kolstr)   //Количество распечатанных строчек
{
SQL_str         row;
SQLCURSOR       cur;
char		strsql[512];
int		kolkod,i;
class iceb_u_str kod("");

*kolstr=0;
if(spisok[0] == '\0')
 return;

kolkod=iceb_u_pole2(spisok,',');

if(kolkod > 0)
 for(i=1; i <= kolkod; i++)
  {
   iceb_u_polen(spisok,&kod,i,',');
   if(kod.ravno()[0] == '\0')
    break;
   if(metka == 0)
    sprintf(strsql,"select %s from %s where %s=%s",
    naimp,tabl,naimkod,kod.ravno());
   if(metka == 1)
    sprintf(strsql,"select %s from %s where %s='%s'",
    naimp,tabl,naimkod,kod.ravno());

   if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     fprintf(ff,"%s-Код %s не найден !\n",__FUNCTION__,kod.ravno());
   else
     fprintf(ff,"%s %s\n",kod.ravno(),row[0]);
   *kolstr+=1;
  }

if(kolkod == 0 && spisok[0] != '\0')
 {
  kod.new_plus(spisok);
  if(metka == 0)
   sprintf(strsql,"select %s from %s where %s=%s",
   naimp,tabl,naimkod,kod.ravno());
  if(metka == 1)
   sprintf(strsql,"select %s from %s where %s='%s'",
   naimp,tabl,naimkod,kod.ravno());

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
    fprintf(ff,"%s-Код %s не найден !\n",__FUNCTION__,kod.ravno());
  else
    fprintf(ff,"%s %s\n",kod.ravno(),row[0]);
  *kolstr+=1;
 }
}
