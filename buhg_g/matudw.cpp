/* $Id: matudw.c,v 1.6 2013/05/17 14:58:22 sasa Exp $ */
/*26.02.2017    16.04.1997      Белых А.И.      matudw.c
Удаление документа
1- удаляем записи в таблице списания материалов на изготовление деталей (только для расходных документов)
2- удаляем услуги
3- удаляем список оплат к документу
4-удаляем все записи в накладной
5-удаляем дополнительные записи к шапке документа
6-удаляем шапку накладной
*/
#include	"buhg_g.h"

extern SQL_baza  bd;

void matudw(const char *nomdok,short d,short m,short g,int skl,
int tipz,GtkWidget *wpredok)
{
char		strsql[512];
//printf("%s-%d.%d.%d\n",__FUNCTION__,d,m,g);

if(tipz == 2)
 {
  /*удаляем записи в таблице списания материалов на детали*/
  sprintf(strsql,"delete from Dokummat4 where datd='%04d-%02d-%02d' and skl=%d and nomd='%s'",
  g,m,d,skl,nomdok);
  if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
   return;
 }


//Удаляем услуги
sprintf(strsql,"delete from Dokummat3 where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
g,m,d,skl,nomdok);
if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return;

//Удаляем список оплат к документа
sprintf(strsql,"delete from Opldok where ps=1 and datd='%04d-%02d-%02d' and pd=%d and nomd='%s'",
g,m,d,skl,nomdok);
if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return;

/*Удаляем записи в накладной*/

sprintf(strsql,"delete from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' and tipz=%d",
g,m,d,skl,nomdok,tipz);

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return;


sprintf(strsql,"delete from Dokummat2 where god=%d and nomd='%s' and \
sklad=%d",
g,nomdok,skl);

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return;

/*Удаляем накладную*/

sprintf(strsql,"delete from Dokummat where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' and tip=%d",
g,m,d,skl,nomdok,tipz);

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return;

}
