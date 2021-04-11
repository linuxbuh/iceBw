/*$Id: pvbanks_kprovw.c,v 1.8 2013/08/13 06:09:49 sasa Exp $*/
/*12.07.2015	20.02.2008	Белых А.И.	pvbanks_kprovw.c
проверка контрагента
*/
#include "buhg_g.h"


int pvbanks_kprovw(class iceb_u_spisok *KONT,int snom,const char *pris,int metkadk,double *sum,class iceb_u_str *tabnom,
double *deb,double *kre,int kolih_simv,class iceb_u_str *fio,class iceb_u_str *bankshet,class iceb_u_str *inn,const char *grupk,
int kod_banka,
GtkWidget *wpredok)
{
char strsql[1024];
class iceb_u_str kontrag("");

tabnom->new_plus("");
inn->new_plus("");
bankshet->new_plus("");
fio->new_plus("");

kontrag.new_plus(KONT->ravno(snom));

if(grupk[0] != '\0')
 {
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select grup from Kontragent where kodkon='%s'",kontrag.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
     if(iceb_u_proverka(grupk,row[0],0,0) != 0)
       return(1);
   }
 }


if(iceb_u_strstrm(kontrag.ravno(),pris) == 0)
 return(1);
 
if(metkadk == 0)
  *sum=deb[snom]-kre[snom];
if(metkadk == 1)
  *sum=kre[snom]-deb[snom];

if(*sum <= 0.009)
 return(1);
/************** 
memset(tabnom,'\0',sizeof(tabnom));
int shethik=0;
int dlkodkon=strlen(kontrag);
for(int i=dlpris; i < dlkodkon; i++)
 tabnom[shethik++]=kontrag[i];
*******************/
tabnom->new_plus(iceb_u_adrsimv(kolih_simv,kontrag.ravno()));

SQL_str row1;
SQLCURSOR cur1;
  
//Читем в карточке фамилию и картсчет   
sprintf(strsql,"select fio,bankshet,inn,kb from Kartb where tabn=%s",tabnom->ravno());
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 {
  fio->new_plus(row1[0]);
  bankshet->new_plus(row1[1]);
  inn->new_plus(row1[2]);
 }
if(kod_banka != 0)
 {
  sprintf(strsql,"select nks from Zarkh where tn=%s and kb=%d",tabnom->ravno(),kod_banka);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sprintf(strsql,"Для работника %s %s нет картсчёта для кода банка %d!",tabnom->ravno(),fio->ravno(),kod_banka);
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
  bankshet->new_plus(row1[0]);
 }
   
if(bankshet->ravno()[0] == '\0')
 {
  sprintf(strsql,"%s\n%s:%s %s",
  gettext("Не введён счёт!"),
  gettext("Табельный номер"),tabnom->ravno(),fio->ravno());
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
return(0);
}
