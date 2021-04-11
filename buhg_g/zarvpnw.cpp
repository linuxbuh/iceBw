/*$Id: zarvpnw.c,v 1.8 2013/08/13 06:10:04 sasa Exp $*/
/*13.07.2015	18.07.2008	Белых А.И.	zarvpnw.c
Подпрограмма возврата подоходного налога
*/
#include "buhg_g.h"

extern short kodpn;   /*Код подоходного налога*/
extern short kodvpn; /*Код возврата подоходного налога*/
extern SQL_baza bd;

void zarvpnw(int tabn,
short mes,short god,
int podr,
const char *uder_only,
GtkWidget *wpredok)
{
if(kodvpn == 0)
 return;
 
char strsql[1024];
short		d;

iceb_u_dpm(&d,&mes,&god,5);


sprintf(strsql,"\n%s-Возврат подоходного налога\n\
---------------------------------------------------\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);

if(iceb_u_proverka(uder_only,kodvpn,0,0) != 0)
 {
  sprintf(strsql,"%s-Код %d исключён из расчёта\n",__FUNCTION__,kodvpn);
  zar_pr_insw(strsql,wpredok);
  return;
 }
 
SQL_str row;
class SQLCURSOR cur;

/*Проверяем есть ли код возврата подоходного налога*/

sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
knah=%d",tabn,kodvpn);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 {
  sprintf(strsql,"%s-Не введено начисление возврата подоходного налога %d !\n",__FUNCTION__,kodvpn);
  zar_pr_insw(strsql,wpredok);
  return;
 }
class iceb_u_str shet(row[0]);

int kolstr=0;
/*Читаем сумму подоходного налога*/
sprintf(strsql,"select suma from Zarp where tabn=%d and \
datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and \
prn='2' and knah=%d and suma <> 0.",
tabn,god,mes,god,mes,kodpn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
if(kolstr == 0)
 {
  sprintf(strsql,"%s-Не найдено записей с подоходным налогом!\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return;
 }

double suma_podoh=0.;
while(cur.read_cursor(&row) != 0)
 suma_podoh+=atof(row[0]);
 
sprintf(strsql,"%s-Сумма подоходного налога=%.2f\n",__FUNCTION__,suma_podoh);
zar_pr_insw(strsql,wpredok);

struct ZARP     zp;

zp.tabnom=tabn;
zp.prn=1;
zp.knu=kodvpn;
zp.dz=d;
zp.mz=mes;
zp.gz=god;
zp.mesn=mes; zp.godn=god;
zp.nomz=0;
zp.podr=podr;
strncpy(zp.shet,shet.ravno(),sizeof(zp.shet)-1);

zapzarpw(&zp,suma_podoh*-1,d,mes,god,0,shet.ravno(),"",0,podr,"",wpredok);
 

}
