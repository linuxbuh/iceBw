/*$Id:$*/
/*22.03.2017	13.03.2013	Белых А.И.	musow.c
Автоматическое формирование документа на списание остатков по складу
для расходного документа
*/
#include "buhg_g.h"
extern short     mdd;  /*0-обычный документ 1-двойной*/
extern SQL_baza bd;

void musow(short dd,short md,short gd,const char *nomdok,int sklad,const char *kodkontr,const char *nomdokp,GtkWidget *wpredok)
{
char strsql[1024];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

int sklad2=0;


sprintf(strsql,"select * from Kart where sklad=%d",sklad);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной карточки !"),wpredok);
  return;
 }

if(mdd == 1)
 {
  iceb_u_polen(kodkontr,&sklad2,2,'-');  
 }

class ostatok ost;
int nomkart=0;

while(cur.read_cursor(&row) != 0)
 {
  ostkarw(1,1,gd,dd,md,gd,sklad,atoi(row[1]),&ost);
  if(ost.ostg[3] <= 0.)
    continue; 

  nomkart=0;
  
  if(mdd == 1) /*Двойной документ*/
   {
    int nomkart=0;
    /*проверяем может карточка с подходящими реквизитами уже есть*/
    sprintf(strsql,"select nomk from Kart where kodm=%s and \
sklad=%d and shetu='%s' and cena=%s and ei='%s' and nds=%s \
and cenap=%s and krat=%s and mnds=%s and dat_god='%s'",
    row[2],sklad2,row[5],row[6],row[4],row[9],row[7],row[8],row[3],row[18]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) > 0)
     {
      nomkart=atoi(row1[0]);
     }
    else
     {
      short denv=0,mesv=0,godv=0; /*дата ввода в эксплуатацию*/
      short deng=0,mesg=0,godg=0; /*конечная дата годности товара*/
      /*создаём новую карточку*/
      nomkart=nomkrw(sklad2,wpredok);
      iceb_u_rsdat(&denv,&mesv,&godv,row[14],2);
      iceb_u_rsdat(&deng,&mesg,&godg,row[18],2);
                        
      if(zapkarw(sklad2,nomkart,atoi(row[2]),atof(row[6]),row[4],row[5],atof(row[8]),
      atof(row[10]),atoi(row[11]),atof(row[9]),atoi(row[3]),atof(row[7]),denv,mesv,godv,row[15],row[16],row[17],deng,mesg,godg,0,wpredok) != 0)
        continue;

     }

    /*запись в приходный документ*/
    zapvdokw(dd,md,gd,sklad2,nomkart,atoi(row[2]),nomdokp,ost.ostg[3],atof(row[6]),row[4],atof(row[9]),atoi(row[3]),0,1,atoi(row[1]),"","",0,"",wpredok);
    
   }
  /*запись в расходный документ*/
  zapvdokw(dd,md,gd,sklad,atoi(row[1]),atoi(row[2]),nomdok,ost.ostg[3],atof(row[6]),row[4],atof(row[9]),atoi(row[3]),0,2,nomkart,"","",0,"",wpredok);
 }








}
