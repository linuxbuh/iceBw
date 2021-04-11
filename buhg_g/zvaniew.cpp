/* $Id: zvaniew.c,v 1.16 2013/09/26 09:47:01 sasa Exp $ */
/*13.07.2015    02.12.1992      Белых А.И.      zvaniew.c
Подпрограмма начисления за звание
*/
#include        "buhg_g.h"

extern SQL_baza bd;

void zvaniew(int tn,//Табельный номер
short mp,short gp, //Дата расчёта
int podr, //Код подразделения
const char *nah_only,
GtkWidget *wpredok)
{
int             knzz; /*Код начисления за звание*/
double          nah=0.;
short           d;
class iceb_u_str shet("");
char		strsql[512];
int		zvan;
class ZARP     zp;
SQL_str row;
class SQLCURSOR cur;
nah=0.;

sprintf(strsql,"\n%s-Расчёт начисления за завание\n\
----------------------------------------------------------\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok); 

if(iceb_poldan("Код начисления за звание",strsql,"zarnast.alx",wpredok) != 0)
 {
  sprintf(strsql,"%s-Не найдена настройка \"Код начисления за звание\"!\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok); 
  return;
 }

knzz=atoi(strsql);
if(knzz == 0)
 {
  sprintf(strsql,"%s-Код начисления за звание равен нулю!\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok); 
  return;
 }
if(iceb_u_proverka(nah_only,knzz,0,0) != 0)
 {
  sprintf(strsql,"%s-Код начисления за звание исключён из расчёта %d",__FUNCTION__,knzz);
  zar_pr_insw(strsql,wpredok); 
  return;
 }

sprintf(strsql,"Код начисления за звание=%d\n",knzz);
zar_pr_insw(strsql,wpredok); 
  
/*Проверяем есть ли код начисления в списке*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
knah=%d",tn,knzz); 
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"Не нашли код начисления за звание %d\n",knzz);
  zar_pr_insw(strsql,wpredok); 
  return;
 }
shet.new_plus(row[0]);
//Читаем код звания
sprintf(strsql,"select zvan from Kartb where tabn=%d",tn); 
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"Не найден код звания для %d\n",tn);
  zar_pr_insw(strsql,wpredok); 
  return;
 }
zvan=atoi(row[0]);
sprintf(strsql,"Код звания=%d\n",zvan);
zar_pr_insw(strsql,wpredok); 


nah=0.;

sprintf(strsql,"select sdzz from Zvan where kod=%d",zvan);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 nah=atof(row[0]);

sprintf(strsql,"Сумма доплаты за звание=%.2f\n",nah);
zar_pr_insw(strsql,wpredok); 
 
iceb_u_dpm(&d,&mp,&gp,5);

zp.tabnom=tn;
zp.prn=1;
zp.knu=knzz;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
strncpy(zp.shet,shet.ravno(),sizeof(zp.shet)-1);

zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
}
