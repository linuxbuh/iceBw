/* $Id: iceb_nomdok.c,v 1.7 2013/05/17 14:58:22 sasa Exp $ */
/*31.03.2006	22.01.1994      Белых А.И.      iceb_nomdok.c
Определение последнего номера платежного поручения
*/
#include  "iceb_libbuh.h"
extern SQL_baza bd;

int iceb_nomdok(short god,const char *tabl,GtkWidget *wpredok)
{
int             mpn;  /*Максимальный порядковый номер*/
int		i;
long		kolstr;
SQL_str         row;
char		strsql[512];

mpn=0;

sprintf(strsql,"select nomd from %s where datd >= '%d-01-01' and \
datd <= '%d-12-31'",tabl,god,god);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr == 0)
  return(1);


while(cur.read_cursor(&row) != 0)
 {
  i=atoi(row[0]);
  if(i > mpn)
   mpn=i;

 }

mpn++;

return(mpn);

}
