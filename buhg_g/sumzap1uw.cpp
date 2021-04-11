/* $Id: sumzap1uw.c,v 1.10 2013/09/26 09:46:56 sasa Exp $ */
/*12.07.2015	18.01.2001	Белых А.И.	sumzap1uw.c
Получение суммы по документа учета услуг
*/
#include        "buhg_g.h"

extern double	okrg1;  /*Округление суммы*/
extern SQL_baza bd;

void sumzap1uw(int d,int m,int g, //Дата
const char *nn,  //Номер документа
int podr,  //код подразделения
double *suma,  //сума по документа
double *sumabn, //в том числе без НДС
int lnds, //Метка льгот по НДС
int tp,  //1-приход 2-расход
GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row;
char		strsql[512];
double		bb;

*sumabn=0.;
*suma=0.;

/*Узнаем сумму всех записей*/
sprintf(strsql,"select cena,kolih from Usldokum1 where datd='%d-%d-%d' and \
podr=%d and nomd='%s' and tp=%d",
g,m,d,podr,nn,tp);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  
  bb=atof(row[0])*atof(row[1]);
  bb=iceb_u_okrug(bb,okrg1);

  if(lnds > 0)
    *sumabn+=bb;
  else
    *suma+=bb;

 }

}
