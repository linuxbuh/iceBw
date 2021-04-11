/*$Id: zapzarpw.c,v 1.16 2013/09/26 09:46:59 sasa Exp $*/
/*13.07.2015	25.05.1999	Белых А.И.	zapzarpw.c
Запись строки начисления/удержания
Если вернули 0 - записали
             1 - запись уже есть
             2 - ошибка записи
             3 - у оператора доступ только на чтение
*/
#include        "buhg_g.h"

extern SQL_baza bd;

int zapzarpw(class ZARP *zp,
double suma,
short denz, //день записи
short mv,short gv,  //В счёт какого месяца
short kdn, //Количество дней
const char *shet, //Счет в запись
const char *koment, //Коментарий
short nz, //Номер записи
int podr, //подразделение
const char *nomdok, //номер документа
GtkWidget *wpredok)
{
time_t		vrem;
char		strsql[1024];

if(zp->dz != 0)
 {
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"delete from Zarp where datz='%04d-%02d-%02d' and \
tabn=%d and prn='%d' and knah=%d and godn=%d and mesn=%d and \
podr=%d and shet='%s' and nomz=%d",
  zp->gz,zp->mz,zp->dz,zp->tabnom,zp->prn,zp->knu,
  zp->godn,zp->mesn,zp->podr,zp->shet,zp->nomz);
/**************
  zar_pr_insw(__FUNCTION__,wpredok);
  zar_pr_insw("-",wpredok);
  zar_pr_insw(strsql,wpredok);
  zar_pr_insw("\n",wpredok);
***************/        
  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Доступ только на чтение
    return(3);
   else
    {
     iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
    }
   }
 }

time(&vrem);
suma=iceb_u_okrug(suma,0.01);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"insert into Zarp \
values ('%04d-%02d-%02d',%d,'%d',%d,%.2f,'%s',%d,%d,%d,%d,%d,%ld,'%s',%d,'%s')",
zp->gz,zp->mz,denz,zp->tabnom,zp->prn,zp->knu,suma,shet,gv,mv,kdn,nz,iceb_getuid(wpredok),vrem,koment,podr,nomdok);
/******************
zar_pr_insw(__FUNCTION__,wpredok);
zar_pr_insw("-",wpredok);
zar_pr_insw(strsql,wpredok);
zar_pr_insw("\n",wpredok);
*****************/
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
   {
//    printf("zapzarpw-запись уже есть-%s\n%d %s",strsql,sql_nerror(&bd),sql_error(&bd));
    return(1);
   }
  else
   {
    iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
    return(2);
   }
 }

return(0);

}
