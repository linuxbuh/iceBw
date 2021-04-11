/*$Id:$*/
/*22.03.2017	16.03.2016	Белых А.И.	iceb_nn9_start.c
Печать шапки налоговой накладной  приказ Министерства финансов Украины от 31.12.2015 N 1307
Если вернули 0 - шапку сформировали
             1 - нет
             
*/
#include <errno.h>
#include "iceb_libbuh.h"

int iceb_nn9_start_v(class iceb_u_str *zved_pn,class iceb_u_str *snoz,class iceb_u_str *nevid,class iceb_u_str *tippr,GtkWidget *wpredok);

extern SQL_baza bd;

int iceb_nn9_start(const char *imaf,
short dd,short md,short gd, //дата документа
short dnn,short mnn,short gnn, //Дата налоговой накладной
const char *nomnn, //Номер налоговой накладной
const char *kpos,
int tipz,  /*1-приход 2-расход*/
const char *kodkontr00,
FILE **ff,
GtkWidget *wpredok)
{
int nomer_st=0;
class iceb_u_str stroka("");
char strsql[1024];
char data_vipiski[512];
SQL_str row,row1;
SQLCURSOR cur,cur1;
class iceb_u_str nmo(""),nmo1("");  /*Наименование организации*/
class iceb_u_str inn(""),inn1("");  /*Индивидуальный налоговый номер*/
class iceb_u_str kodfil(""),kodfil1(""); /*код филиала*/
class SQLCURSOR cur_alx;
SQL_str row_alx;
int kolstr=0;
class iceb_u_str nnn1("");
class iceb_u_str nnn2("");
class iceb_u_str bros("");

static iceb_u_str zved_pn("");
static iceb_u_str snoz("");
static iceb_u_str nevid("");
static iceb_u_str tippr("");


if(iceb_nn9_start_v(&zved_pn,&snoz,&nevid,&tippr,wpredok) != 0)
 return(1);



/*Читаем реквизиты организации свои */

sprintf(strsql,"select * from Kontragent where kodkon='%s'",kodkontr00);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s %s !",
  gettext("Не найден код контрагента"),"00");
  iceb_menu_soob(strsql,wpredok);
 }
else
 {
  if(tipz == 2)
   {
    if(row[16][0] == '\0')
     {
      if(iceb_u_polen(row[1],&nmo,1,'(') != 0)
        nmo.new_plus(row[1]);
     }
    else
      nmo.new_plus(row[16]);


    inn.new_plus(row[8]);
    
    sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' and zapis like 'Код филиала%%'",kodkontr00);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) >= 1)
     {
      iceb_u_polen(row1[0],&kodfil,2,'|');
     }        
   }

  if(tipz == 1)
   {
    if(row[16][0] == '\0')
      nmo1.new_plus(row[1]);
    else
      nmo1.new_plus(row[16]);

    inn1.new_plus(row[8]);
    sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' and zapis like 'Код филиала|%%'",kodkontr00);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) >= 1)
     {
      iceb_u_polen(row1[0],&kodfil1,2,'|');
     }        
   }

 } 


/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/
if(iceb_u_atof(kpos) != 0. || kpos[0] != '0')
 {
  sprintf(strsql,"select * from Kontragent where kodkon='%s'",kpos);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kpos);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    if(tipz == 2)
     {
      if(row[16][0] == '\0')
       {
        if(iceb_u_polen(row[1],&nmo1,1,'(') != 0)
          nmo1.new_plus(row[1]);
       }
      else
        nmo1.new_plus(row[16]);

        
      inn1.new_plus(row[8]);

      sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' and zapis like 'Код филиала%%'",kpos);
      if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) >= 1)
       {
        iceb_u_polen(row1[0],&kodfil1,2,'|');
       }        
     }
    if(tipz == 1)
     {
      if(row[16][0] == '\0')
        nmo.new_plus(row[1]);
      else
        nmo.new_plus(row[16]);

      inn.new_plus(row[8]);

      sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' and zapis like 'Код филиала%%'",kpos);
      if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) >= 1)
       {
        iceb_u_polen(row1[0],&kodfil,2,'|');
       }        
     }
    
   }
  if(inn1.ravno()[0] == '\0')
   {
    sprintf(strsql,gettext("Для контрагента %s не введён индивидуальный налоговый номер !"),kpos);
    iceb_menu_soob(strsql,wpredok);
   }
 }

sprintf(strsql,"select str from Alx where fil='nn9_start.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"nn9_start.alx");
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }


if((*ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,__FUNCTION__,errno,wpredok);
  return(1);
 }

fprintf(*ff,"\x1B\x33%c",20); /*Уменьшение межстрочного интервала*/
//fprintf(*ff,"\x1B\x33%c",24); /*Уменьшение межстрочного интервала*/
fprintf(*ff,"\x1B\%c%c",'[',10); /*Уменьшение высоты символа*/
memset(strsql,'\0',sizeof(strsql));
if(dnn != 0)
 {
  iceb_mesc(mnn,1,strsql);
  sprintf(data_vipiski,"%02d%02d%04d",dnn,mnn,gnn);
 }
else
 {
  iceb_mesc(md,1,strsql);
  sprintf(data_vipiski,"%02d%02d%04d",dd,md,gd);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
     continue;

  nomer_st+=1;
  stroka.new_plus(row_alx[0]);
  
  switch(nomer_st)
   {  
    case 2: /*зведена податкова накладна*/
      iceb_u_vstav(&stroka,zved_pn.ravno(),54,56,1);
      break;
    case 4: /*складена на операції, звільнені від оподаткування*/
      iceb_u_vstav(&stroka,snoz.ravno(),54,56,1);
      break;

    case 6: /*Не підлягае наданню отримувачу/покупчю*/
      iceb_u_vstav(&stroka,nevid.ravno(),54,57,1);
      break;

    case 7: /*тип причини*/
      iceb_u_vstav(&stroka,tippr.ravno(),53,60,2);
      break;



    case 9: /*Дата складання - номер налоговой накладной*/
      iceb_u_vstav(&stroka,data_vipiski,79,96,2 );
      
      if(iceb_u_polen(nomnn,&nnn1,1,'/') != 0)
        nnn1.new_plus(nomnn);
      else
        iceb_u_polen(nomnn,&nnn2,2,'/');
        
      sprintf(strsql,"%*s",iceb_u_kolbait(7,nnn1.ravno()),nnn1.ravno());
      iceb_u_vstav(&stroka,strsql,100,115,2 );
      if(nnn2.getdlinna() > 1)
       {
        sprintf(strsql,"%*s",iceb_u_kolbait(4,nnn2.ravno()),nnn2.ravno());
        iceb_u_vstav(&stroka,strsql,118,120,1 );
       }      
      break;


    case 15: /*первая строка наименования продавца и покупателя*/
      sprintf(strsql,"%*.*s",iceb_u_kolbait(86,nmo.ravno()),iceb_u_kolbait(86,nmo.ravno()),nmo.ravno());
      iceb_u_vstav(&stroka,strsql,2,88,1 );
      sprintf(strsql,"%*.*s",iceb_u_kolbait(86,nmo1.ravno()),iceb_u_kolbait(86,nmo1.ravno()),nmo1.ravno());
      iceb_u_vstav(&stroka,strsql,93,165,1 );
      break;
      
    case 16:  /*вторая строка наименования продавца и покупателя*/
      if(iceb_u_strlen(nmo.ravno()) > 86)
       {
        sprintf(strsql,"%*.*s",iceb_u_kolbait(86,iceb_u_adrsimv(86,nmo.ravno())),iceb_u_kolbait(86,iceb_u_adrsimv(86,nmo.ravno())),iceb_u_adrsimv(86,nmo.ravno()));
        iceb_u_vstav(&stroka,strsql,2,88,1 );
       }
      if(iceb_u_strlen(nmo1.ravno()) > 86)
       {
        sprintf(strsql,"%*.*s",iceb_u_kolbait(86,iceb_u_adrsimv(86,nmo1.ravno())),iceb_u_kolbait(86,iceb_u_adrsimv(86,nmo1.ravno())),iceb_u_adrsimv(86,nmo1.ravno()));
        iceb_u_vstav(&stroka,strsql,93,178,1 );
       }
      break;

    case 17:  /*третя строка ниаменования продвца и покупателя*/
      if(iceb_u_strlen(nmo.ravno()) > 172)
       {
        sprintf(strsql,"%*.*s",iceb_u_kolbait(86,iceb_u_adrsimv(172,nmo.ravno())),iceb_u_kolbait(86,iceb_u_adrsimv(172,nmo.ravno())),iceb_u_adrsimv(172,nmo.ravno()));
        iceb_u_vstav(&stroka,strsql,2,88,1 );
       }
      if(iceb_u_strlen(nmo1.ravno()) > 172)
       {
        sprintf(strsql,"%*.*s",iceb_u_kolbait(86,iceb_u_adrsimv(172,nmo1.ravno())),iceb_u_kolbait(86,iceb_u_adrsimv(172,nmo1.ravno())),iceb_u_adrsimv(172,nmo1.ravno()));
        iceb_u_vstav(&stroka,strsql,93,178,1 );
       }
      break;
      
    case 20:  /*индивидуальный налоговый номер*/
      sprintf(strsql,"%*s",iceb_u_kolbait(12,inn.ravno()),inn.ravno()); /*Для печати со смещением вправо*/
      iceb_u_vstav(&stroka,strsql,6,30,2 );

      iceb_u_vstav(&stroka,kodfil.ravno(),38,46,2 );
      
      sprintf(strsql,"%*s",iceb_u_kolbait(12,inn1.ravno()),inn1.ravno()); /*Для печати со смещением вправо*/
      iceb_u_vstav(&stroka,strsql,97,121,2 );

      iceb_u_vstav(&stroka,kodfil1.ravno(),129,137,2 );
      break;
   }
  fprintf(*ff,"%s",stroka.ravno());
 }

return(0);

}
