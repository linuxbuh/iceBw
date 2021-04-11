/* $Id: iceb_prsh.c,v 1.12 2013/08/13 06:10:13 sasa Exp $ */
/*09.03.2016    16.11.1993      Белых А.И.      iceb_prsh.c
Подпрограмма проверки наличия счета
Если вернули 0 - счета нет
	     1 - счет есть и не имеет субсчетов
	     2 - счет есть и имеет субсчета
	     3 - счет является субсчетом
             4 - счёт заблокирован в плане счетов
*/
#include	"iceb_libbuh.h"

extern SQL_baza	bd;

int iceb_prsh(const char *ssh, //Номер счета
OPSHET *shetv, //Возвращаем тип сальдо
GtkWidget *wpredok)
{
char		strsql[1024];
SQL_str         row;
int		kolstr;
class iceb_u_str shet("");
short	vplsh=0; /*0-двух уровневый план счетов 1-многоуровневый*/

if(iceb_poldan("Многопорядковый план счетов",strsql,"nastrb.alx",wpredok) == 0)
 if(iceb_u_SRAV("Вкл",strsql,1) == 0)
  vplsh=1;

shet.new_plus(ssh);

class SQLCURSOR cur;
sprintf(strsql,"select * from Plansh where ns like '%s%%' \
order by ns asc",shet.ravno_filtr());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,NULL);
 }


if(kolstr == 0)
  return(0);

cur.read_cursor(&row);

if(atoi(row[10]) == 1) /*счёт заблокирован в плане счетов*/
 return(4);

shetv->naim.new_plus(row[1]);
shetv->tips=atoi(row[2]);
shetv->vids=atoi(row[3]);
shetv->saldo=atoi(row[6]);
shet.new_plus(row[0]);
shetv->stat=atoi(row[7]);
shetv->val=atoi(row[8]);

if(kolstr == 1 && iceb_u_SRAV(ssh,shet.ravno(),0) != 0)
  return(0);


if(kolstr == 1 && iceb_u_SRAV(ssh,shet.ravno(),0) == 0 && shetv->vids == 0)
   return(1);

/*Для двух уровневой системы счетов*/
if(vplsh == 0)
 if(kolstr >= 1 && iceb_u_SRAV(ssh,shet.ravno(),0) == 0 && shetv->vids == 1)
    return(3);

/*Многоуровневый план счетов*/
if(vplsh == 1)
 {
  if(kolstr == 1 && iceb_u_SRAV(ssh,shet.ravno(),0) == 0 && shetv->vids == 1)
      return(3);
  if(kolstr > 1 && iceb_u_SRAV(ssh,shet.ravno(),0) == 0 && shetv->vids == 1)
      return(2);
 }
 
if(kolstr > 1 && iceb_u_SRAV(ssh,shet.ravno(),0) == 0)
 return(2);


return(0);
}
