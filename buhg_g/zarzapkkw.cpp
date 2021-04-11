/*$Id:$*/
/*08.06.2016	08.06.2016	Белых А.И.	zarzapkkw.c
Запись кода контрагента в общий список контрагентов из подсистемы "Заработная плата"
если вернули 0 то есть
             1 нет
*/

#include "buhg_g.h"

int zarzapkkw(const char *kod_kontr,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

//Проверяем есть ли в общем списке
sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",iceb_u_sqlfiltr(kod_kontr));
if(iceb_sql_readkey(strsql,wpredok) >= 1)
 return(0);

//Узнаём фамилию и другие реквизиты


class iceb_u_str fio("");

class iceb_u_str tabn;
tabn.plus(iceb_u_adrsimv(iceb_u_strlen(ICEB_ZAR_PKTN),kod_kontr));

class iceb_u_str adres("");

class iceb_u_str telef("");

class iceb_u_str inn("");

sprintf(strsql,"select fio,adres,telef,inn from Kartb where tabn=%s",tabn.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  fio.new_plus(row[0]);

  adres.new_plus(row[1]);

  telef.new_plus(row[2]);

  inn.new_plus(row[3]);
 }

sprintf(strsql,"replace into Kontragent \
(kodkon,naikon,adres,innn,telef,ktoz,vrem) \
values ('%s','%s','%s','%s','%s',%d,%ld)",iceb_u_sqlfiltr(kod_kontr),fio.ravno_filtr(),adres.ravno_filtr(),inn.ravno(),telef.ravno_filtr(),iceb_getuid(wpredok),time(NULL));

if(iceb_sql_zapis(strsql,0,0,wpredok) != 0)
  return(1);
 

return(0);
}
