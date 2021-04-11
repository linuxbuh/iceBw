/* $Id: sumprzw.c,v 1.11 2013/09/26 09:46:56 sasa Exp $ */
/*12.07.2015	29.05.1998	Белых А.И.	sumprzw.c
Определение суммы выполненных проводок для определенного табельного
номера.
*/
#include        "buhg_g.h"

extern double   okrg; /*Округление*/
extern class iceb_u_str	shrpz; /*Счет расчётов по зарплате*/
extern class iceb_u_str shrpzbt; /*Счет расчётов по зарплате бюджет*/
extern class iceb_u_spisok dop_zar_sheta; //Дополнительные зарплатные счёта
extern SQL_baza bd;

double sumprzw(short m,short g,const char *nn,int podr,
double *sumd,double *sumk,
GtkWidget *wpredok)
{
double		sum;
long		kolstr;
SQL_str         row;
char		strsql[512];
double		deb;

*sumd=0.;
*sumk=0.;
sum=0.;

/*Суммиреум выполненные проводки*/
sprintf(strsql,"select sh,shk,deb,kre from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd>='%d-%02d-01' and \
datd <= '%d-%02d-31'",
ICEB_MP_ZARP,podr,nn,g,m,g,m);
/*
printw("\n%s\n",strsql);
OSTANOV();  
*/
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_TABLEACCESS_DENIED_ERROR) //Запрет команды select  
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

if(kolstr == 0)
 {
  return(0.);
 }

while(cur.read_cursor(&row) != 0)
 {
  deb=atof(row[2]);  

  if(iceb_u_SRAV(shrpz.ravno(),row[1],0) == 0 || iceb_u_SRAV(shrpzbt.ravno(),row[1],0) == 0 || dop_zar_sheta.find(row[1]) == 0)
    *sumd=*sumd+deb;
  if(iceb_u_SRAV(shrpz.ravno(),row[0],0) == 0 || iceb_u_SRAV(shrpzbt.ravno(),row[0],0) == 0 || dop_zar_sheta.find(row[1]) == 0) 
    *sumk=*sumk+deb;
  
  sum+=deb;

 }

sum=iceb_u_okrug(sum,okrg);
*sumd=iceb_u_okrug(*sumd,okrg);
*sumk=iceb_u_okrug(*sumk,okrg);

return(sum);
}
