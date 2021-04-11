/* $Id: zapzarn1w.c,v 1.6 2013/05/17 14:58:30 sasa Exp $ */
/*13.07.2015	18.09.1998	Белых А.И.	zapzarn1w.c
Запись структуры zar3 в базу
*/
#include        "buhg_g.h"

extern SQL_baza bd;

void zapzarn1w(int tnn, //Табельный номер
short tipn, //1-начисление(ден дов) 2- удержание(прин. в расч.)
short kd, //Код начисления
short dn,short ms,short gd, //Дата до которой действует начисление
const char *sh, //Счет
GtkWidget *wpredok)
{
time_t		vrem;
char		strsql[512];

time(&vrem);
sprintf(strsql,"replace into Zarn1 \
values (%d,%d,%d,'%04d-%02d-%02d','%s')",
tnn,tipn,kd,gd,ms,dn,sh);
/*
  printw("\nstrsql=%s\n",strsql);
  refresh();
  OSTANOV();
*/

if(sql_zap(&bd,strsql) != 0)
 {
  char bros[512];
  sprintf(bros,"%s-Ошибка ввода записи !",__FUNCTION__);
  iceb_msql_error(&bd,bros,strsql,wpredok);
 }

}
