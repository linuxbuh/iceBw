/* $Id: vzrkbzw.c,v 1.12 2013/08/13 06:09:58 sasa Exp $ */
/*28.12.2012	22.07.1997	Белых А.И.	vzrkbzw.c
Взять реквизиты организаций из базы для заполнения платежных 
поручений
*/
#include        "buhg_g.h"
#include        "dok4w.h"

extern class REC rec;
extern SQL_baza bd;

int vzrkbzw(const char *kodo,GtkWidget *wpredok)
{
SQL_str         row;
char		strsql[512];

SQLCURSOR cur;

char kontr00[64];
memset(kontr00,'\0',sizeof(kontr00));

if(kodo[0] != '\0') /*для типового платёжного при его создании пустой*/
 {    
  sprintf(strsql,"select naikon,kod,naiban,mfo,nomsh,adresb,pnaim from \
  Kontragent where kodkon='%s'",kodo);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s\n%s",gettext("Не найден код контрагента в общем списке!"),kodo);
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
  else
   {
    rec.kodor1.new_plus(kodo);
    if(row[6][0] == '\0')
     {
      if(iceb_u_pole(row[0],&rec.naior1,1,'(') != 0)    
       rec.naior1.new_plus(row[0]);
     }
    else  
       rec.naior1.new_plus(row[6]);
    rec.kod1.new_plus(row[1]);
    rec.naiban1.new_plus(row[2]);
    rec.mfo1.new_plus(row[3]);
    rec.nsh1.new_plus(row[4]);
    rec.gorod1.new_plus(row[5]);
   }
 }



iceb_poldan("Код контрагента для платёжного поручения",kontr00,"nastdok.alx",wpredok);
if(kontr00[0] == '\0')
  strcpy(kontr00,"00");
  
sprintf(strsql,"select naikon,kod,naiban,mfo,nomsh,adresb,pnaim from \
Kontragent where kodkon='%s'",kontr00);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  char strok[1024];
  sprintf(strok,"%s-%s %s !",__FUNCTION__,gettext("Не найден код контрагента"),kontr00);
  iceb_menu_soob(strok,wpredok);
  return(1);
 }
else
 {
  rec.kodor.new_plus("00");     
  rec.naior.new_plus("");

  if(row[6][0] == '\0')          
   {

    if(iceb_u_polen(row[0],&rec.naior,1,'(') != 0)    
      rec.naior.new_plus(row[0]);
   }
  else
    rec.naior.new_plus(row[6]);


  rec.kod.new_plus(row[1]);
  rec.naiban.new_plus(row[2]);
  rec.mfo.new_plus(row[3]);
  rec.nsh.new_plus(row[4]);
  rec.gorod.new_plus(row[5]);
 }

return(0);
}
