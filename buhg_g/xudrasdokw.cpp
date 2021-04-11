/*$Id: xudrasdokw.c,v 1.17 2013/08/13 06:09:59 sasa Exp $*/
/*01.09.2015	02.04.2009	Белых А.И.	xudrasdokw.c
Распечатка доверенности
*/
#include <errno.h>
#include "buhg_g.h"

int xudrasdok_r(class iceb_u_spisok *prug,GtkWidget *wpredok);
extern SQL_baza bd;

int xudrasdokw(class iceb_u_str *datdov,class iceb_u_str *nomdov,
class iceb_u_spisok *imafo,
class iceb_u_spisok *naimf,
GtkWidget *wpredok)
{
FILE *ff;
char imaf[56];
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str komu("");
class iceb_u_str post("");
class iceb_u_str datad("");
class iceb_u_str nomnar("");
class iceb_u_str datanar("");
class iceb_u_str inn("");
class iceb_u_str nspnds("");
class iceb_u_str telef("");
class iceb_u_str adres_pol("");
class iceb_u_str adres_plat("");
class iceb_u_str mfo("");
class iceb_u_str nomshet("");
class iceb_u_str naim_bank("");
class iceb_u_str naim_pol("");
class iceb_u_str naim_plat("");
class iceb_u_str dolg("");
class iceb_u_str dokum("");
class iceb_u_str seriq("");
class iceb_u_str nomerdok("");
class iceb_u_str datavid("");
class iceb_u_str vidan("");
class iceb_u_str kod_edrp("");
class iceb_u_spisok prug;
/*Читаем шапку документа*/
sprintf(strsql,"select * from Uddok where datd='%s' and nomd='%s'",
datdov->ravno_sqldata(),nomdov->ravno());

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  komu.new_plus(row[3]);
  post.new_plus(row[4]);

  datad.new_plus(iceb_u_datzap(row[2]));
  nomnar.new_plus(row[8]);
  datanar.new_plus(iceb_u_datzap(row[7]));
  dokum.new_plus(row[5]);
  seriq.new_plus(row[9]);
  nomerdok.new_plus(row[10]);
  datavid.new_plus(iceb_u_datzap(row[11]));
  vidan.new_plus(row[12]);
  dolg.new_plus(row[13]);
 }
class iceb_u_str kod_pol("");
class iceb_u_str kod_plat("");

iceb_poldan("Код контрагента получателя",&kod_pol,"nastud.alx",wpredok);
iceb_poldan("Код контрагента плательщика",&kod_plat,"nastud.alx",wpredok);

if(kod_pol.getdlinna() <= 1)
 kod_pol.new_plus("00");


sprintf(strsql,"select * from Kontragent where kodkon='%s'",kod_pol.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  naim_pol.new_plus(row[1]);
  if(row[16][0] != '\0')
   naim_pol.new_plus(row[16]);
  kod_edrp.new_plus(row[5]);
  inn.new_plus(row[8]);
  nspnds.new_plus(row[9]);
  adres_pol.new_plus(row[3]);
  if(iceb_u_polen(row[10],&telef,1,' ') != 0)
   telef.new_plus(row[10]);
 }

sprintf(strsql,"select * from Kontragent where kodkon='%s'",kod_plat.ravno_filtr());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  naim_plat.new_plus(row[1]);
  if(row[16][0] != '\0')
   naim_plat.new_plus(row[16]);


  adres_plat.new_plus(row[3]);

  mfo.new_plus(row[6]);
  nomshet.new_plus(row[7]);
  naim_bank.new_plus(row[2]);
 }


sprintf(imaf,"dov%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }
iceb_u_startfil(ff);

fprintf(ff,"\x1B\x4D"); //12 знаков на дюйм

char stroka[1024];

xudrasdok_r(&prug,wpredok); /*читаем текст который должен быть в правом углу*/
int kolih_str=prug.kolih();

//fprintf(ff,"IПH - %s  NCB - %s тел.%-15s \n",inn.ravno(),nspnds.ravno(),telef.ravno());  
if(kolih_str == 0)
 fprintf(ff,"Одержувач:%-*.*s\n",
 iceb_u_kolbait(45,naim_pol.ravno()),
 iceb_u_kolbait(45,naim_pol.ravno()),
 naim_pol.ravno());
else
 fprintf(ff,"Одержувач:%-*.*s %*.*s",
 iceb_u_kolbait(45,naim_pol.ravno()),
 iceb_u_kolbait(45,naim_pol.ravno()),
 naim_pol.ravno(),
 iceb_u_kolbait(38,prug.ravno(0)),
 iceb_u_kolbait(38,prug.ravno(0)),
 prug.ravno(0));

if(iceb_u_strlen(naim_pol.ravno()) > 45)
 fprintf(ff,"%s\n",iceb_u_adrsimv(45,naim_pol.ravno()));
if(kolih_str < 1) 
 fprintf(ff,"Індивідуальний податковий номер:%-17s\n",inn.ravno());
else
 fprintf(ff,"Індивідуальний податковий номер:%-17s        %*.*s",
 inn.ravno(),
 iceb_u_kolbait(37,prug.ravno(1)),
 iceb_u_kolbait(37,prug.ravno(1)),
 prug.ravno(1));

if(kolih_str < 2)
 fprintf(ff,"Номер свідоцтва платника ПДВ:%-26s\n",nspnds.ravno());
else
 fprintf(ff,"Номер свідоцтва платника ПДВ:%-26s %*.*s",
 nspnds.ravno(),
 iceb_u_kolbait(37,prug.ravno(2)),
 iceb_u_kolbait(37,prug.ravno(2)),
 prug.ravno(2));





fprintf(ff,"Телефон одержувача:%-*s\n",iceb_u_kolbait(10,telef.ravno()),telef.ravno());
int dlinna_str=50;
sprintf(stroka,"Адреса одержувача: %s",adres_pol.ravno());

for(int ii=0; ii < iceb_u_strlen(stroka); ii+=dlinna_str)
 {
  fprintf(ff,"%-*.*s\n",
  iceb_u_kolbait(dlinna_str,iceb_u_adrsimv(ii,stroka)),
  iceb_u_kolbait(dlinna_str,iceb_u_adrsimv(ii,stroka)),
  iceb_u_adrsimv(ii,stroka));
 }
fprintf(ff,"Ідентифікаційний код ЄДРПОУ:%-*s          Код за УКУД\n",
iceb_u_kolbait(20,kod_edrp.ravno()),kod_edrp.ravno());

//fprintf(ff,"Платник:%s\n",naim_pol.ravno());
fprintf(ff,"Платник:%.*s\n",iceb_u_kolbait(85,naim_plat.ravno()),naim_plat.ravno());
if(iceb_u_strlen(naim_plat.ravno()) > 85)
 fprintf(ff,"%s\n",iceb_u_adrsimv(85,naim_plat.ravno()));

sprintf(stroka,"Адреса платника: %s",adres_plat.ravno());

for(int ii=0; ii < iceb_u_strlen(stroka); ii+=dlinna_str)
 fprintf(ff,"%.*s\n",
  iceb_u_kolbait(dlinna_str,iceb_u_adrsimv(ii,stroka)),
  iceb_u_adrsimv(ii,stroka));

fprintf(ff,"Рахунок:%s MФО:%s\n",nomshet.ravno(),mfo.ravno());
fprintf(ff,"Банк:%-*.*s Довіреність дійсна до %s р.\n\n",
iceb_u_kolbait(50,naim_bank.ravno()),iceb_u_kolbait(50,naim_bank.ravno()),naim_bank.ravno(),
datad.ravno());


fprintf(ff,"\x1B\x50"); /*10-знаков*/
fprintf(ff,"\x1B\x4D"); //12 знаков на дюйм

fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины*/
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff,"          ДОВІРЕНІСТЬ N%s",nomdov->ravno());
fprintf(ff,"\x14"); /*Выключение режима удвоенной ширины*/
fprintf(ff,"\n");
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта
fprintf(ff,"\x1B\x50"); /*10-знаков*/

fprintf(ff,"                      Дата видачі %s р.\n\n",datdov->ravno());

class iceb_u_str vidano(dolg.ravno());
vidano.plus(" ",komu.ravno());

fprintf(ff,"Видано");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
//fprintf(ff," %s %s\n",dolg.ravno(),komu.ravno());
fprintf(ff,"%-*.*s\n",iceb_u_kolbait(70,vidano.ravno()),iceb_u_kolbait(70,vidano.ravno()),vidano.ravno());
if(iceb_u_strlen(vidano.ravno()) > 70)
 fprintf(ff,"%s\n",iceb_u_adrsimv(70,vidano.ravno()));
 
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"\nДокумент що засвідчує особу");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s\n",dokum.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта


fprintf(ff,"серія");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %-*s",iceb_u_kolbait(5,seriq.ravno()),seriq.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff," N");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %-*s",iceb_u_kolbait(20,nomerdok.ravno()),nomerdok.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff," від");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s р.\n",datavid.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"\nВиданий");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s\n",vidan.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"\nНа отримання від");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %.*s\n",iceb_u_kolbait(60,post.ravno()),post.ravno());
if(iceb_u_strlen(post.ravno()) > 60)
 fprintf(ff,"%s\n",iceb_u_adrsimv(60,post.ravno()));
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"цінностей за");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s %s\n",nomnar.ravno(),datanar.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"\x1B\x4D"); //12 знаков на дюйм
fprintf(ff,"\f\
Перелік цінностей, які належить отримати:\n");
fprintf(ff,"\
------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 N |         Найменування цінностей         |Од.вим.|     Кількість (прописом)     |\n");
fprintf(ff,"\
------------------------------------------------------------------------------------\n");
/*
123 1234567890123456789012345678901234567890 1234567 123456789012345678901234567890
*/
/*Читаем записи в документе*/
sprintf(strsql,"select zapis,ei,kol from Uddok1 where datd='%s' and nomd='%s'",
datdov->ravno_sqldata(),nomdov->ravno());
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
char kolih[2048];

int nomzap=0;
int kol_str=0;
while(cur.read_cursor(&row) != 0)
 {
  memset(kolih,'\0',sizeof(kolih));
  strncpy(kolih,iceb_u_nofwords(atof(row[2])),sizeof(kolih)-1);
//  printf("%s-%s\n",__FUNCTION__,kolih);
  kol_str++;
  fprintf(ff,"%3d|%-*.*s|%-*.*s|%-*.*s|\n",
  ++nomzap,
  iceb_u_kolbait(40,row[0]),iceb_u_kolbait(40,row[0]),row[0],
  iceb_u_kolbait(7,row[1]),iceb_u_kolbait(7,row[1]),row[1],
  iceb_u_kolbait(30,kolih),
  iceb_u_kolbait(30,kolih),
  kolih);

  /**********1 дополнительная строка*********************/  
  if(iceb_u_strlen(row[0]) > 40 && iceb_u_strlen(kolih) <= 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-*.*s|%-7.7s|%-30.30s|\n",
    "",
    iceb_u_kolbait(40,iceb_u_adrsimv(40,row[0])),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,row[0])),
    iceb_u_adrsimv(40,row[0]),
    "","");
   }
  if(iceb_u_strlen(row[0]) > 40 && iceb_u_strlen(kolih) > 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-*.*s|%-7.7s|%-*.*s|\n","",
    iceb_u_kolbait(40,iceb_u_adrsimv(40,row[0])),
    iceb_u_kolbait(40,iceb_u_adrsimv(40,row[0])),
    iceb_u_adrsimv(40,row[0]),
    "",
    iceb_u_kolbait(30,iceb_u_adrsimv(30,kolih)),
    iceb_u_kolbait(30,iceb_u_adrsimv(30,kolih)),
    iceb_u_adrsimv(30,kolih));
   }

  if(iceb_u_strlen(row[0]) <= 40 && iceb_u_strlen(kolih) > 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-40.40s|%-7.7s|%-*.*s|\n","","","",
    iceb_u_kolbait(30,iceb_u_adrsimv(30,kolih)),
    iceb_u_kolbait(30,iceb_u_adrsimv(30,kolih)),
    iceb_u_adrsimv(30,kolih));
   }

  /**********2 дополнительная строка*********************/  
  if(iceb_u_strlen(row[0]) > 80 && iceb_u_strlen(kolih) <= 60)
   {
    kol_str++;
    fprintf(ff,"%3s|%-*.*s|%-7.7s|%-30.30s|\n",
    "",
    iceb_u_kolbait(40,iceb_u_adrsimv(80,row[0])),
    iceb_u_kolbait(40,iceb_u_adrsimv(80,row[0])),
    iceb_u_adrsimv(80,row[0]),
    "","");
   }
  if(iceb_u_strlen(row[0]) > 40 && iceb_u_strlen(kolih) > 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-*.*s|%-7.7s|%-*.*s|\n",
    "",
    iceb_u_kolbait(40,iceb_u_adrsimv(80,row[0])),
    iceb_u_kolbait(40,iceb_u_adrsimv(80,row[0])),
    iceb_u_adrsimv(80,row[0]),
    "",
    iceb_u_kolbait(30,iceb_u_adrsimv(60,kolih)),
    iceb_u_kolbait(30,iceb_u_adrsimv(60,kolih)),
    iceb_u_adrsimv(60,kolih));
   }

  if(iceb_u_strlen(row[0]) <= 40 && iceb_u_strlen(kolih) > 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-40.40s|%-7.7s|%-*.*s|\n",
    "",
    "",
    "",
    iceb_u_kolbait(30,iceb_u_adrsimv(60,kolih)),
    iceb_u_kolbait(30,iceb_u_adrsimv(60,kolih)),
    iceb_u_adrsimv(60,kolih));
   }

  kolstr++;
  fprintf(ff,"\
------------------------------------------------------------------------------------\n");
 }
for(; kol_str < 10; kol_str+=2)
 {
  fprintf(ff,"%3s|%-40.40s|%-7.7s|%-30.30s|\n","","","","");
  fprintf(ff,"\
------------------------------------------------------------------------------------\n");
 }
fprintf(ff,"\n\
          Підпис_________________________________________________засвідчую\n\
                 (зразок підпису особи, що одержала довіреність)\n");
fprintf(ff,"\n\
          Керівник підприємства\n\n");
fprintf(ff,"\
          Головний бухгалтер\n\n");

fprintf(ff,"\
          Місце печатки\n");


fclose(ff);


imafo->plus(imaf);
naimf->plus(gettext("Распечатка доверенности"));
//iceb_ustpeh(imaf,1,wpredok);


return(0);
}
/********************************************/
/*чтение содержимого правого уголка*/
/*************************************/
int xudrasdok_r(class iceb_u_spisok *prug,GtkWidget *wpredok)
{
class iceb_u_str stroka("");
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select str from Alx where fil='xudrasdok.alx' order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  prug->plus(row[0]);
 }

return(0);

}
