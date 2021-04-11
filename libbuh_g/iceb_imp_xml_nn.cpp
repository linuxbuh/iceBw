/*$Id:$*/
/*02.02.2015	25.01.2014	Белых А.И.	iceb_imp_xml_nn.c
Импорт налоговой накладной в формате xml 
*/
#include "iceb_libbuh.h"

#define S_DATA_VNN "<HFILL>" /*Дата выписки налоговой накладной*/
#define E_DATA_VNN "</HFILL>"

#define S_NOM_NN   "<HNUM>"  /*Номер налоговой накладной*/
#define E_NOM_NN   "</HNUM>"

#define S_KOD_VD   "<HNUM1>"   /*Код вида деятельности*/
#define E_KOD_VD   "</HNUM1>"   /*Код вида деятельности*/

#define S_NOM_FIL   "<HNUM2>"   /*Номер филиала*/
#define E_NOM_FIL   "</HNUM2>"   /*Номер филиала*/

#define S_PRODAVEC   "<HNAMESEL>"   /*Продавец*/
#define E_PRODAVEC   "</HNAMESEL>"   /*Продавец*/

#define S_EDRPUO_PROD   "<TIN>"   /*Код ЕДРПУО Продавца*/
#define E_EDRPUO_PROD   "</TIN>" 

#define S_POKUPATEL   "<HNAMEBUY>"   /*Покупатель*/
#define E_POKUPATEL   "/HNAMEBUY"   /*Покупатель*/

#define S_INN_PROD    "<HKSEL>"     /*Индивидуальный налоговый номер продавца*/
#define E_INN_PROD    "</HKSEL>"     /*Индивидуальный налоговый номер продавца*/

#define S_INN_POKUP    "<HKBUY>"     /*Индивидуальный налоговый номер покупателя*/
#define E_INN_POKUP    "</HKBUY>"     /*Индивидуальный налоговый номер покупателя*/

#define S_ADRES_PROD   "<HLOCSEL>"  /*Адрес продавца*/
#define E_ADRES_PROD   "</HLOCSEL>"  /*Адрес продавца*/

#define S_ADRES_POKUP   "<HLOCBUY>"  /*Адрес покупателя*/
#define E_ADRES_POKUP   "</HLOCBUY>"  /*Адрес покупателя*/

#define S_TELEF_PROD    "<HTELSEL>" /*Телефон продавца*/
#define E_TELEF_PROD    "</HTELSEL>" /*Телефон продавца*/

#define S_TELEF_POKUP   "<HTELBUY>" /*Телефон покупателя*/
#define E_TELEF_POKUP   "</HTELBUY>" /*Телефон покупателя*/

#define S_NAIM_TUV      "<RXXXXG3S" /*Наименование товара/услуги*/
#define E_NAIM_TUV      "</RXXXXG3S>" /*Наименование товара/услуги*/

#define S_EIZ      "<RXXXXG4S" /*Единица измерения*/
#define E_EIZ      "</RXXXXG4S>" /*Единица измерения*/

#define S_KEIZ      "<RXXXXG105_2S" /*Код единицы измерения*/
#define E_KEIZ      "</RXXXXG105_2S>" 

#define S_KOLIH      "<RXXXXG5" /*Количество*/
#define E_KOLIH      "</RXXXXG5>" /*Количество*/


#define S_CENA      "<RXXXXG6" /*Цена*/
#define E_CENA      "</RXXXXG6>" /*Цена*/


#define S_SUMA_20      "<RXXXXG7" /*Основная ставка*/
#define E_SUMA_20      "</RXXXXG7>" /*Основная ставка*/

#define S_SUMA_7      "<RXXXXG109" /*ставка 7 процентов*/
#define E_SUMA_7      "</RXXXXG109>"

#define S_SUMA_0_NTU      "<RXXXXG8" /*ставка 0 процентов на територии Украины*/
#define E_SUMA_0_NTU      "</RXXXXG8>"

#define S_SUMA_0_EKS      "<RXXXXG9" /*ставка 0 процентов экспорт*/
#define E_SUMA_0_EKS      "</RXXXXG9>"

#define S_SUMA_ZVIL      "<RXXXXG10" /*звільнення від ПДВ*/
#define E_SUMA_ZVIL      "</RXXXXG10>"

gsize iceb_imp_xml_nn_dt(gsize poz_start,gsize razmer,gchar *masiv,const char *teg_end,class iceb_u_str *sod_tega);
gsize iceb_imp_xml_nn_dtr(gsize poz_start,gsize razmer,gchar *masiv,const char *teg_end,class iceb_u_str *sod_tega,int *nomer_str);
int iceb_imp_xml_nn_zap(int nomer_str,const char *zapros,const char *tablica,GtkWidget *wpredok);

extern SQL_baza bd;

int iceb_imp_xml_nn(const char *imaf_xml,class iceb_imp_xml_nn_r *rk,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
class iceb_u_str stroka("");
gchar *masiv;
GError *fil_error=NULL;
gsize razmer=0;
char strsql[2048];
SQL_str row;
class SQLCURSOR cur;
int kolstr;
char imaf_tmp[64];
sprintf(imaf_tmp,"impnn%d.tmp",getpid());

if(iceb_cp(imaf_xml,imaf_tmp,0,wpredok) != 0)
  return(1);
if(iceb_perecod(3,imaf_tmp,wpredok) != 0)
 return(1);

if(g_file_get_contents(imaf_tmp,&masiv,&razmer,&fil_error) == FALSE)
 {
  stroka.new_plus(__FUNCTION__);
  stroka.plus("-");
  stroka.plus(fil_error->code);
  stroka.plus(" ",fil_error->message);
  stroka.ps_plus(imaf_xml);
    
  g_error_free (fil_error);

  iceb_menu_soob(stroka.ravno(),wpredok);
  
  unlink(imaf_tmp);

  return(1);
 }
  
unlink(imaf_tmp);

if(rk->ima_tablic.getdlinna() <= 1)
 {
  
  stroka.new_plus(__FUNCTION__);
  stroka.plus("-",gettext("Не задано имя таблицы!"));
  iceb_menu_soob(stroka.ravno(),wpredok);
  return(1);    
 }

char zaprostmp[1024];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
nz int not null default 0,\
nmu char(255) not null default '',\
eiz char(32) not null default '',\
keiz char(32) not null default '',\
kol double(16,6) not null default 0.,\
cena double(16,6) not null default 0.,\
s20 double(16,6) not null default 0.,\
s7 double(16,6) not null default 0.,\
s0ty double(16,6) not null default 0.,\
s0ek double(16,6) not null default 0.,\
szv double(16,6) not null default 0.,\
unique(nz)) ENGINE = MYISAM",rk->ima_tablic.ravno());

/*****************
 0 nz - номер записи
 1 nmu - наименование товара/услуги
 2 eiz - единица измерения
 3 keiz- код единицы измерения
 4 kol - количество
 5 cena - цена
 6 s20  - сумма по ставке 20
 7 s7   - сумма по ставке 7
 8 s0ty - сумма по ставке 0 митна територия украины
 9 s0ek - сумма по ставке 0 експорт
10 szv  - освобождено от НДС
************************/
gsize voz=0;

if(rk->tabtmp.create_tab(rk->ima_tablic.ravno(),zaprostmp,wpredok) != 0)
 {
  return(1);
 }  

int nomer_str=0;
for(gsize nom=0; nom < razmer; nom++)
 {
  
  if(masiv[nom] == '<') /*возможное начало тега*/
   {


    if(iceb_u_SRAV(&masiv[nom],S_DATA_VNN,1) == 0) /*Дата выписки налоговой накладной*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_DATA_VNN),razmer,masiv,E_DATA_VNN,&rk->data_vnn)) > 0)
       {
        short d=0,m=0,g=0;
        iceb_u_rsdat(&d,&m,&g,rk->data_vnn.ravno(),1);
        sprintf(strsql,"%02d.%02d.%4d",d,m,g);
        rk->data_vnn.new_plus(strsql);
        nom=voz;
        continue;
       } 
    if(iceb_u_SRAV(&masiv[nom],S_NOM_NN,1) == 0) /*Номер налоговой накладной*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_NOM_NN),razmer,masiv,E_NOM_NN,&rk->nom_nn)) > 0)
       {
        nom=voz;
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_KOD_VD,1) == 0) /*Код вида деятельности*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_KOD_VD),razmer,masiv,E_KOD_VD,&rk->kod_vd)) > 0)
       {
        nom=voz;
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_NOM_FIL,1) == 0) /*Номер филиала*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_NOM_FIL),razmer,masiv,E_NOM_FIL,&rk->nom_fil)) > 0)
       {
        nom=voz;
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_PRODAVEC,1) == 0) /*Продавец*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_PRODAVEC),razmer,masiv,E_PRODAVEC,&rk->naim_prod)) > 0)
       {
        nom=voz;
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_POKUPATEL,1) == 0) /*Покупатель*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_POKUPATEL),razmer,masiv,E_POKUPATEL,&rk->naim_pokup)) > 0)
       {
        printf("%s-pokup-%s\n",__FUNCTION__,rk->naim_pokup.ravno());
        nom=voz;
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_INN_PROD,1) == 0) /*Индивидуальный налоговый номер продавца*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_INN_PROD),razmer,masiv,E_INN_PROD,&rk->inn_prod)) > 0)
       {
        nom=voz;
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_INN_POKUP,1) == 0) /*Индивидуальный налоговый номер покупателя*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_INN_POKUP),razmer,masiv,E_INN_POKUP,&rk->inn_pokup)) > 0)
       {
        nom=voz;
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_ADRES_PROD,1) == 0) /*Адрес продавца*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_ADRES_PROD),razmer,masiv,E_ADRES_PROD,&rk->adres_prod)) > 0)
       {
        nom=voz;
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_ADRES_POKUP,1) == 0) /*Адрес покупателя*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_ADRES_POKUP),razmer,masiv,E_ADRES_POKUP,&rk->adres_pokup)) > 0)
       {
        nom=voz;
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_TELEF_PROD,1) == 0) /*Телефон продавца*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_TELEF_PROD),razmer,masiv,E_TELEF_PROD,&rk->telef_prod)) > 0)
       {
        nom=voz;
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_TELEF_POKUP,1) == 0) /*Телефон покупателя*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_TELEF_POKUP),razmer,masiv,E_TELEF_POKUP,&rk->telef_pokup)) > 0)
       {
        nom=voz;
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_EDRPUO_PROD,1) == 0) /*ЕДРПУО продавца*/
      if((voz=iceb_imp_xml_nn_dt(nom+strlen(S_EDRPUO_PROD),razmer,masiv,E_EDRPUO_PROD,&rk->edrpuo_prod)) > 0)
       {
        nom=voz;
        continue;
       } 

     /*строки документа*/
     
    if(iceb_u_SRAV(&masiv[nom],S_NAIM_TUV,1) == 0) /*наименование товаров/услуг*/
      if((voz=iceb_imp_xml_nn_dtr(nom+strlen(S_NAIM_TUV),razmer,masiv,E_NAIM_TUV,&stroka,&nomer_str)) > 0)
       {
        nom=voz;
        sprintf(strsql,"update %s set nmu='%s' where nz=%d",rk->ima_tablic.ravno(),stroka.ravno_filtr(),nomer_str);
        iceb_imp_xml_nn_zap(nomer_str,strsql,rk->ima_tablic.ravno(),wpredok);
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_EIZ,1) == 0) /*единица измерения*/
      if((voz=iceb_imp_xml_nn_dtr(nom+strlen(S_EIZ),razmer,masiv,E_EIZ,&stroka,&nomer_str)) > 0)
       {
        nom=voz;
        sprintf(strsql,"update %s set eiz='%s' where nz=%d",rk->ima_tablic.ravno(),stroka.ravno_filtr(),nomer_str);
        iceb_imp_xml_nn_zap(nomer_str,strsql,rk->ima_tablic.ravno(),wpredok);
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_KEIZ,1) == 0) /*Код единицы измерения*/
      if((voz=iceb_imp_xml_nn_dtr(nom+strlen(S_KEIZ),razmer,masiv,E_KEIZ,&stroka,&nomer_str)) > 0)
       {
        nom=voz;
        sprintf(strsql,"update %s set keiz='%s' where nz=%d",rk->ima_tablic.ravno(),stroka.ravno_filtr(),nomer_str);
        iceb_imp_xml_nn_zap(nomer_str,strsql,rk->ima_tablic.ravno(),wpredok);
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_KOLIH,1) == 0) /*Количество*/
      if((voz=iceb_imp_xml_nn_dtr(nom+strlen(S_KOLIH),razmer,masiv,E_KOLIH,&stroka,&nomer_str)) > 0)
       {
        nom=voz;
        sprintf(strsql,"update %s set kol=%.10g where nz=%d",rk->ima_tablic.ravno(),stroka.ravno_atof(),nomer_str);
        iceb_imp_xml_nn_zap(nomer_str,strsql,rk->ima_tablic.ravno(),wpredok);
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_CENA,1) == 0) /*Цена*/
      if((voz=iceb_imp_xml_nn_dtr(nom+strlen(S_CENA),razmer,masiv,E_CENA,&stroka,&nomer_str)) > 0)
       {
        nom=voz;
        sprintf(strsql,"update %s set cena=%.10g where nz=%d",rk->ima_tablic.ravno(),stroka.ravno_atof(),nomer_str);
        iceb_imp_xml_nn_zap(nomer_str,strsql,rk->ima_tablic.ravno(),wpredok);
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_SUMA_20,1) == 0) /*Основная ставка*/
      if((voz=iceb_imp_xml_nn_dtr(nom+strlen(S_SUMA_20),razmer,masiv,E_SUMA_20,&stroka,&nomer_str)) > 0)
       {
        nom=voz;
        sprintf(strsql,"update %s set s20=%.10g where nz=%d",rk->ima_tablic.ravno(),stroka.ravno_atof(),nomer_str);
        iceb_imp_xml_nn_zap(nomer_str,strsql,rk->ima_tablic.ravno(),wpredok);
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_SUMA_7,1) == 0) /*7%*/
      if((voz=iceb_imp_xml_nn_dtr(nom+strlen(S_SUMA_7),razmer,masiv,E_SUMA_7,&stroka,&nomer_str)) > 0)
       {
        nom=voz;
        sprintf(strsql,"update %s set s7=%.10g where nz=%d",rk->ima_tablic.ravno(),stroka.ravno_atof(),nomer_str);
        iceb_imp_xml_nn_zap(nomer_str,strsql,rk->ima_tablic.ravno(),wpredok);
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_SUMA_0_NTU,1) == 0) /*0% на територии Украины*/
      if((voz=iceb_imp_xml_nn_dtr(nom+strlen(S_SUMA_0_NTU),razmer,masiv,E_SUMA_0_NTU,&stroka,&nomer_str)) > 0)
       {
        nom=voz;
        sprintf(strsql,"update %s set s0ty=%.10g where nz=%d",rk->ima_tablic.ravno(),stroka.ravno_atof(),nomer_str);
        iceb_imp_xml_nn_zap(nomer_str,strsql,rk->ima_tablic.ravno(),wpredok);
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_SUMA_0_EKS,1) == 0) /*0% експорт*/
      if((voz=iceb_imp_xml_nn_dtr(nom+strlen(S_SUMA_0_EKS),razmer,masiv,E_SUMA_0_EKS,&stroka,&nomer_str)) > 0)
       {
        nom=voz;
        sprintf(strsql,"update %s set s0ek=%.10g where nz=%d",rk->ima_tablic.ravno(),stroka.ravno_atof(),nomer_str);
        iceb_imp_xml_nn_zap(nomer_str,strsql,rk->ima_tablic.ravno(),wpredok);
        continue;
       } 

    if(iceb_u_SRAV(&masiv[nom],S_SUMA_ZVIL,1) == 0) /*звільно від ПДВ*/
      if((voz=iceb_imp_xml_nn_dtr(nom+strlen(S_SUMA_ZVIL),razmer,masiv,E_SUMA_ZVIL,&stroka,&nomer_str)) > 0)
       {
        nom=voz;
        sprintf(strsql,"update %s set szv=%.10g where nz=%d",rk->ima_tablic.ravno(),stroka.ravno_atof(),nomer_str);
        iceb_imp_xml_nn_zap(nomer_str,strsql,rk->ima_tablic.ravno(),wpredok);
        continue;
       } 
   }
 }



g_free(masiv);


class iceb_u_str repl(gettext("Код ЕГРПОУ продавца"));
repl.plus(":",rk->edrpuo_prod.ravno());

repl.ps_plus(gettext("Наименование продавца"));
repl.plus(":",rk->naim_prod.ravno());

repl.ps_plus(gettext("Индивидуальный налоговый номер продавца"));
repl.plus(":",rk->inn_prod.ravno());

repl.ps_plus(gettext("Адрес продавца"));
repl.plus(":",rk->adres_prod.ravno());

repl.ps_plus(gettext("Телефон продавца"));
repl.plus(":",rk->telef_prod.ravno());

repl.ps_plus(gettext("Дата выписки налоговой накладной"));
repl.plus(":",rk->data_vnn.ravno());

repl.ps_plus(gettext("Номер налоговой накладной"));
repl.plus(":",rk->nom_nn.ravno());

repl.ps_plus(gettext("Код вида деятельности"));
repl.plus(":",rk->kod_vd.ravno());

repl.ps_plus(gettext("Номер филиала"));
repl.plus(":",rk->nom_fil.ravno());


repl.ps_plus(gettext("Наименование покупателя"));
repl.plus(":",rk->naim_pokup.ravno());
repl.plus("\n");

iceb_printw(repl.ravno(),buffer,view);

if(rk->data_vnn.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата выписки налоговой накладной!"),wpredok);
  return(1);
 }

if(rk->edrpuo_prod.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не найден код ЕГРПОУ продавца!"),wpredok);
  return(1);
 }

if(rk->inn_prod.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не найден индивидуальный налоговый номер продавца!"),wpredok);
  return(1);
 }


if(rk->naim_prod.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не найдено наименование продавца!"),wpredok);
  return(1);
 }

/*проверяем код ЕДРПОУ если он такойже как у контрагета с кодом 00 то это расходная накладная а экспортировать нужно только приходные*/
voz=0;
sprintf(strsql,"select kodkon from Kontragent where kod='%s' order by kodkon asc",rk->edrpuo_prod.ravno_filtr());
  
printf("%s-%s\n",__FUNCTION__,strsql);
if((voz=iceb_sql_readkey(strsql,&row,&cur,wpredok)) < 0)
 {
  return(1);
 }

if(voz > 0)
 {
  if(iceb_u_SRAV(row[0],"00",0) == 0)
   {
    printf("%s-%s\n",__FUNCTION__,row[0]);
    iceb_menu_soob(gettext("Это расходный документ!"),wpredok);
    return(1);
   }
  rk->kontr_prod.new_plus(row[0]);
 }
else
 {
  /*проверяем по наименованию*/
  sprintf(strsql,"select kodkon from Kontragent where naikon='%s'",rk->naim_prod.ravno_filtr());    
  printf("%s-%s\n",__FUNCTION__,strsql);
  
  if(iceb_sql_readkey(strsql,wpredok) == 1)
   rk->kontr_prod.new_plus(row[0]);
  else
   {
    class iceb_lock_tables klk("LOCK TABLES Kontragent WRITE,icebuser READ");

    rk->kontr_prod.new_plus(iceb_getnkontr(1,wpredok)); /*получаем свободный номер контрагента*/

    /*записываем контрагента в справочник контрагентов*/
    sprintf(strsql,"insert into Kontragent (kodkon,naikon,kod,innn,telef,pnaim,ktoz,vrem) values ('%s','%s','%s','%s','%s','%s',%d,%ld)",
    rk->kontr_prod.ravno_filtr(),
    rk->naim_prod.ravno_filtr(),
    rk->edrpuo_prod.ravno_filtr(),
    rk->inn_prod.ravno_filtr(),
    rk->telef_prod.ravno_filtr(),
    rk->naim_prod.ravno_filtr(),
    iceb_getuid(wpredok),time(NULL));
  
    printf("%s-%s\n",__FUNCTION__,strsql);

    if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
     return(1);          
   }
 }

sprintf(strsql,"select * from %s",rk->ima_tablic.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найденo ни одной записи в документе!"),wpredok);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%2s %-*.*s %-*s %2s %10.10g %10.10g %10.10g %10.10g %10.10g %10.10g\n",
  row[0],
  iceb_u_kolbait(20,row[1]),
  iceb_u_kolbait(20,row[1]),
  row[1],
  iceb_u_kolbait(5,row[2]),
  row[2],
  row[3],atof(row[4]),atof(row[5]),atof(row[6]),atof(row[7]),atof(row[8]),atof(row[9]));

  iceb_printw(strsql,buffer,view);
    
 }



return(0);

}
/**********************/
gsize iceb_imp_xml_nn_dt(gsize poz_start,gsize razmer,gchar *masiv,const char *teg_end,class iceb_u_str *sod_tega) /*получение содержимого тега*/
{
sod_tega->new_plus("");
// printf("%s-poz_start=%ld teg_end=%s\n",__FUNCTION__,poz_start,teg_end);

for(gsize poz_end=poz_start; poz_end < razmer; poz_end++) /*ищем конец тега*/
 {
  if(iceb_u_SRAV(teg_end,&masiv[poz_end],1) == 0)
   {

    gsize dlinna_tega=poz_end-poz_start;

    if(dlinna_tega == 0)
     return(poz_end+strlen(teg_end));
     
    gchar *buffer=new gchar[dlinna_tega+1];
    memset(buffer,'\0',sizeof(gchar)*(dlinna_tega+1));
    for(gsize nt=0; nt < dlinna_tega; nt++)  
     {
      buffer[nt]=masiv[poz_start++];
     }

    sod_tega->new_plus(iceb_u_filtr_from_xml(buffer));
    delete [] buffer;

//    printf("%s-Найдена строка=%s dlinna_tega=%ld\n",__FUNCTION__,sod_tega->ravno(),dlinna_tega);

    return(poz_end+strlen(teg_end));
   }
 }
printf("%s-%s -> %s\n",__FUNCTION__,gettext("Не найден тег"),teg_end);
return(-1);
}

/*********************************************/
gsize iceb_imp_xml_nn_dtr(gsize poz_start,gsize razmer,gchar *masiv,const char *teg_end,class iceb_u_str *sod_tega,int *nomer_str) /*получение содержимого тега*/
{
class iceb_u_str buf("");
class iceb_u_str buf1("");

iceb_u_polen(&masiv[poz_start],&buf,2,'=');
iceb_u_polen(buf.ravno(),&buf1,2,'"');
*nomer_str=buf1.ravno_atoi();

for(; poz_start < razmer; poz_start++) /*ищем начало тега*/
 {
  if(masiv[poz_start] == '>')
   break;
 }
poz_start++;
for(gsize poz_end=poz_start; poz_end < razmer; poz_end++) /*ищем конец тега*/
 {
  if(iceb_u_SRAV(teg_end,&masiv[poz_end],1) == 0)
   {
    gsize dlinna_tega=poz_end-poz_start;

    if(dlinna_tega == 0)
     return(poz_end+strlen(teg_end));
     
    char *buffer=new char[dlinna_tega+1];
    memset(buffer,'\0',sizeof(char)*(dlinna_tega+1));
    for(gsize nt=0; nt < dlinna_tega; nt++)  
     {
      buffer[nt]=masiv[poz_start++];
     }

    sod_tega->new_plus(iceb_u_filtr_from_xml(buffer));
    delete [] buffer;

//    printf("%s-Найдена строка=%s dlinna_tega=%ld nomer_str=%d\n",__FUNCTION__,sod_tega->ravno(),dlinna_tega,*nomer_str);

    return(poz_end+strlen(teg_end));
   }
 }
printf("%s-%s -> %s\n",__FUNCTION__,gettext("Не найден тег"),teg_end);
return(-1);
}

/****************************************/
int iceb_imp_xml_nn_zap(int nomer_str,const char *zapros,const char *tablica,GtkWidget *wpredok)
{
char strsql[1024];

sprintf(strsql,"insert into %s (nz) values (%d)",tablica,nomer_str);
sql_zap(&bd,strsql);

return(iceb_sql_zapis(zapros,1,0,wpredok));


}
