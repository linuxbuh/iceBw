/*$Id:$*/
/*15.02.2016	06.02.2016	Белых А.И.	iceb_get_kkfname.c
Получить код коднтрагента для заданного наименования организации
Если вернули 0 - был в справочнике
             1 - записали в справочник
            -1 - не записали

*/
#include "iceb_libbuh.h"

int iceb_get_kkfname(const char *naim, /*наименование контрагента*/
const char *kodkon, /*рекомендованный код контрагента*/
class iceb_u_str *kkontr, /*код контрагента под которым записано наименование в общем сприске контрагентов*/
int *nom_kod_kontr, /*номер кода контрагента с которого нужно начать проверять список контрагентов*/
GtkWidget *wpredok)
{
class iceb_u_str naim1(naim);
class iceb_u_str kodkon1(kodkon);
char strsql[2048];
SQL_str row;
class SQLCURSOR cur;

if(kodkon[0] != '\0')
 {
  int kolbait=strlen(kodkon);
  if(kodkon[kolbait-1] == '.')
   {
    sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kodkon1.ravno_filtr());
    if(iceb_sql_readkey(strsql,wpredok) == 1) /*есть в справочнике*/
     {
      kkontr->new_plus(kodkon1.ravno());
      return(0);
     }
   }  
 }

/*Проверяем есть ли такое короткое наименование в списке контрагентов*/
sprintf(strsql,"select kodkon from Kontragent where naikon='%s'",naim1.ravno_filtr());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  kkontr->new_plus(row[0]);
  return(0);
 }
else
 {
  /*Проверяем есть ли такое полное наименование в списке контрагентов*/
  sprintf(strsql,"select kodkon from Kontragent where pnaim='%s'",naim1.ravno_filtr());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0) /*теоретически может быть больше одной записи*/
   {
    kkontr->new_plus(row[0]);
    return(0);
   }
  else
   {

    /*проверяем свободен ли рекомендованный код контрагента*/
    class iceb_lock_tables klk("LOCK TABLES Kontragent WRITE,icebuser READ");
    
    if(kodkon1.getdlinna() > 1) /*если рекомендованый код есть*/
     {
      sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kodkon1.ravno_filtr());
      if(iceb_sql_readkey(strsql,wpredok) >= 1) /*код занят другим наименованием*/
       {
        kkontr->new_plus(iceb_getnkontr(*nom_kod_kontr,wpredok));
        *nom_kod_kontr=kkontr->ravno_atoi()+1;
       }
      else /*код свободен*/
       kkontr->new_plus(kodkon1.ravno());    
     }
    else
     {
      /*рекомендованный код контрагента не указан*/
      kkontr->new_plus(iceb_getnkontr(*nom_kod_kontr,wpredok));
      *nom_kod_kontr=kkontr->ravno_atoi()+1;
     }

    /*записываем контрагента в справочник контрагентов*/
    sprintf(strsql,"insert into Kontragent (kodkon,naikon,pnaim,ktoz,vrem) values ('%s','%s','%s',%d,%ld)",
    kkontr->ravno_filtr(),naim1.ravno_filtr(),naim1.ravno_filtr(),iceb_getuid(wpredok),time(NULL));

    if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
      return(-1);
    else
      return(1);
   }
 }


return(0);
}
