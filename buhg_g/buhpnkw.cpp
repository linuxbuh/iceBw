/*$Id: buhpnkw.c,v 1.3 2013/09/05 07:24:32 sasa Exp $*/
/*16.04.2017	04.03.2013	Белых А.И.	buhpnkw.c
Получение наименования контрагента для проводки 
Если код контрагента в проводке не введён и проводка сделана из какойнибудь подсистемы то пытаемся найти
Для зарплаты ищем по номеру документа (в номере документа есть табельный номер)
Для остальных подсистем по дате номеру документа и номеру подразделения ищем в шапке документа
номер контрагента

*/
#include "buhg_g.h"

int buhpnkw(class iceb_u_str *naimkontr, /*сюда записываем полученное наименование контрагента*/
char *kodkontr, /*код контрагента из проводки если он там есть*/
int metka_pod, /*метка подсистемы смотри iceb_t_mpods.c */
int tipz, /*1-приходный 2-расходный*/
char *datdok, /*дата документа в SQL формате*/
char *nomdok,
int podr,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

naimkontr->new_plus("");

if(kodkontr[0] != '\0')
 {
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodkontr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    /*в проводке может быть табельный номер и его нет в списке контрагентов*/
    if(iceb_u_SRAV(ICEB_ZAR_PKTN,kodkontr,1) == 0)
     {
      char tabnom[128];
      sprintf(tabnom,"%s",iceb_u_adrsimv(iceb_u_strlen(ICEB_ZAR_PKTN),kodkontr));
      sprintf(strsql,"select fio from Kartb where tabn=%s",tabnom);
      if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
       {
        naimkontr->new_plus(row[0]);
        return(0);
       }
  
     }
    sprintf(strsql,"%s-%s %s",__FUNCTION__,gettext("Не найден код контрагента"),kodkontr);
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
  else
   naimkontr->new_plus(row[0]);

  return(0);

 }
else
 {
  class iceb_u_str kod_kontr("");   
  short d=0,m=0,g=0;
  switch(metka_pod)
   {
    case 0: /*проводка без признака подсистемы*/
      return(0);

    case 1: /*материальный учёт*/
     sprintf(strsql,"select kontr from Dokummat where datd='%s' and sklad=%d and nomd='%s' and tip=%d",datdok,podr,nomdok,tipz);
     if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
       kod_kontr.new_plus(row[0]);
     break;

    case 2: /*заработная плата*/
     iceb_u_polen(nomdok,&kod_kontr,2,'-');     
     sprintf(strsql,"select fio from Kartb where tabn=%d",kod_kontr.ravno_atoi());
     if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      {
       naimkontr->new_plus(row[0]);
       return(0);
      }
     return(1);       

    case 3: /*учёт основных средств*/
     sprintf(strsql,"select kontr from Uosdok where datd='%s' and nomd='%s' and tipz=%d",datdok,nomdok,tipz);
     if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      kod_kontr.new_plus(row[0]);
     break;

    case 4: /*Платёжные поручения*/
     sprintf(strsql,"select polu from Pltp where datd='%s' and nomd='%s'",datdok,nomdok);
     if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
      return(1);
     iceb_u_polen(row[0],&kod_kontr,1,'#');
     
     break;

    case 5: /*Платёжные требования*/
     sprintf(strsql,"select plat from Pltt where datd='%s' and nomd='%s'",datdok,nomdok);
     if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
      return(1);
     iceb_u_polen(row[0],&kod_kontr,1,'#');
     
     break;
 
    case 6: /*учёт услуг*/
     sprintf(strsql,"select kontr from Usldokum where datd='%s' and podr=%d and nomd='%s' and tp=%d",datdok,podr,nomdok,tipz);
     if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      kod_kontr.new_plus(row[0]);
     break;

    case 7: /*учёт кассовых ордеров*/
     iceb_u_rsdat(&d,&m,&g,datdok,2);
     sprintf(strsql,"select kontr from Kasord1 where kassa=%d and god=%d and tp=%d and nomd='%s'",podr,g,tipz,nomdok);
     if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      kod_kontr.new_plus(row[0]);
     break;

    case 8: /*учёт командировочных расходов*/
     iceb_u_rsdat(&d,&m,&g,datdok,2);
     sprintf(strsql,"select kont from Ukrdok where god=%d and nomd='%s'",g,nomdok);
     if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      kod_kontr.new_plus(row[0]);
     break;
   };

  if(kod_kontr.getdlinna() <= 1)
   return(0);
      
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kod_kontr.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    naimkontr->new_plus(kod_kontr.ravno());
    naimkontr->plus(" ",row[0]);
   }
  else
   return(1);
 }

return(0);

}
