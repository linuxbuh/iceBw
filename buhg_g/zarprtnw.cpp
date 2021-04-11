/*$Id: zarprtnw.c,v 1.16 2013/08/13 06:10:03 sasa Exp $*/
/*29.03.2017	07.02.2008	Белых А.И.	zarprtnw.c
Проверка есть ли табельный номер в файле и входит ли дата в диапазон действия настройки
Если вернули 1- табельный номер есть
             0- нет
*/
#include "buhg_g.h"

extern SQL_baza bd;

int zarprtnw(short mr,short gr,int tabn,int metka_s,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

if(metka_s == 1)
  sprintf(strsql,"%s-Проверяем справочник пенсионеров\n",__FUNCTION__);
if(metka_s == 2)
  sprintf(strsql,"%s-Проверяем справочник инвалидов\n",__FUNCTION__);
if(metka_s == 3)
  sprintf(strsql,"%s-Проверяем справочник работающих по договорам подряда\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);

sprintf(strsql,"select dnd,dkd,kom from Zargr where kg=%d and tn=%d and dnd <= '%04d-%02d-01' order by dnd desc limit 1",metka_s,tabn,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 {
  sprintf(strsql,"%s-В справочнике записи нет\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return(0);
 }
sprintf(strsql,"%s-Обнаружена запись dnd=%s dkd=%s kom=%s\n",__FUNCTION__,row[0],row[1],row[2]);
zar_pr_insw(strsql,wpredok);


if(iceb_u_SRAV(row[1],"0000-00-00",0)  == 0)
 {
  sprintf(strsql,"%s-Подходит\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return(1);
 }
short d=0,m=0,g=0;
iceb_u_rsdat(&d,&m,&g,row[1],2);
if(iceb_u_sravmydat(1,mr,gr,1,m,g) > 0)
 return(0);

sprintf(strsql,"%s-Подходит\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);

return(1);

}
