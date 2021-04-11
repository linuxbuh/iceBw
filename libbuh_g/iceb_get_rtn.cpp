/*$Id:$*/
/*16.03.2016	16.03.2016	Белых А.И.	iceb_get_rtn.c
Взять реквизиты табельного номера
 0-взяли
 1-нет
*/
#include "iceb_libbuh.h"

int iceb_get_rtn(int tabnom,class iceb_fioruk_rk *rek,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

rek->clear();
if(tabnom <= 0)
 return(1);
 
sprintf(strsql,"select fio,dolg,inn,telef from Kartb where tabn=%d",tabnom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s-Не знайден табельный номер %d !",__FUNCTION__,tabnom);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
else
 {
  char stroka[512];
  iceb_u_polen(row[0],&rek->famil,1,' ');
  iceb_u_polen(row[0],&rek->ima,2,' ');
  iceb_u_polen(row[0],&rek->oth,3,' ');
  
  sprintf(stroka,"%.*s.%.*s.",
  iceb_u_kolbait(1,rek->ima.ravno()),rek->ima.ravno(),   
  iceb_u_kolbait(1,rek->oth.ravno()),rek->oth.ravno());
  
  strcpy(rek->inic,stroka);

  rek->famil_inic.new_plus(rek->famil.ravno());
  rek->famil_inic.plus(" ",rek->inic);
          
  rek->fio.new_plus(row[0]);
  rek->dolg.new_plus(row[1]);
  rek->inn.new_plus(row[2]);
  
  strncpy(stroka,row[3],sizeof(stroka)-1);
  iceb_u_ud_simv(stroka,"-/.()+,");
       
  rek->telef.new_plus(stroka);
  
 } 

return(0);

}
