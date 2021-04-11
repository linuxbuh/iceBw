/* $Id: zarsprprw.c,v 1.24 2013/09/26 09:47:01 sasa Exp $ */
/*25.07.2019    31.08.1994      Белых А.И.      zarsprprw.c
получение справки всех начислений и удержаний
за любой период
*/
#include <errno.h>
#include        "buhg_g.h"
#include "zar_sprav.h"

void             rekvizkon(SQL_str,FILE*);
void zarsprprw_sap(short mn,short gn,short mk,short gk,const char *harac,const char *fam,const char *inn,const char *dolg,const char *podr,short sovm,FILE *ff1);
int zarsprpr_sub_start(short god,const char *kedrpou,const char *fio,const char *inn,const char *dolg,const char *adres,FILE *ff_ras,GtkWidget *wpredok);
int zarsprpr_sub_end(short mn,short gn,short mk,short gk,double suma,FILE *ff_ras,GtkWidget *wpredok);
void zarsprpr_ssu(short mn,short gn,short mk,short gk,class iceb_u_double *mes_nash,class iceb_u_double *mes_obud,FILE *ff);

extern short    ddd,mmm,ggg;
extern class iceb_u_str kodpn_all;
extern short    *kodmp;   /*Коды материальной помощи*/
extern double   snmpz; /*Сумма не облагаемой материальной помощи заданная*/
extern char     *shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short	*obud; /*Обязательные удержания*/
extern class iceb_u_str kods_esv_all;
extern short kodvn; /*код военного сбора*/

extern SQL_baza bd;

void zarsprprw(class zar_sprav_rek *rek,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *bar,
GtkWidget *wpredok)
{
short    kodpen=0;  /*Код пенсионных отчислений*/
struct  tm      *bf;
time_t		tmr;
class iceb_u_str bros("");
class iceb_u_str str("");
short           koltb; /*Количество видов табелей*/
FILE            *ff,*ff1,*ff2;
char            imaf[64],imaf1[64],imaf2[64],imafvn[64],imafshet[64];
short           m,g;
double          inah,iuder;
double		inahb; //Итого начислено без кодов не входящих в расчёт подоходного налога
double          ino,inob,iuo,bb;
short           kon;     /*Количество статей начисления*/
short           kou;     /*Количество статей удержания*/
class iceb_u_str nai("");
int             i,i1;
double		peno;  /*Пенсионное отчисление*/
double		ipeno;  /*Итого Пенсионное отчисление*/
double          podoh; /*Подоходный налог за месяц*/
double          ipodoh; /*Подоходный налог за год*/
class iceb_u_str fam("");
double          sld;
double          skm,iskm;
double          alim,ialim;
double		fbezr,ifbezr;
short           idn,ikdn;
double          ihas;
short           fo;
short           sovm; /*0-нет 1- Совместитель*/
double		sal;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
int		kodt;
float		dnei,has,kdnei;
short		prn;
int		knah;
double		sym;
short		dz,mz,gz;
class iceb_u_str inn("");
short		ots=5;
class iceb_u_str dolg("");
short		kodalim;
double		saldb;
class iceb_u_str harac("");
class iceb_u_str shet("");
class iceb_u_str kodnvrspr("");
double          soc_strax=0.;
double          isoc_strax=0.;
class iceb_u_str kedrpou("");
class iceb_u_str naim_mes("");
short mn=0,gn=0;
short mk=0,gk=0;
iceb_u_rsdat1(&mn,&gn,rek->datan.ravno());
iceb_u_rsdat1(&mk,&gk,rek->datak.ravno());
if(mk == 0)
 {
  mk=mn;
  gk=gn;
 }

class SQLCURSOR curr;
sprintf(strsql,"select fio,sovm,inn,dolg,harac,podr from Kartb where tabn=%d",rek->tabnom.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),rek->tabnom.ravno_atoi());
  iceb_menu_soob(strsql,wpredok);
  return;
 }

iceb_poldan("Код пенсионного отчисления",strsql,"zarnast.alx",wpredok);
kodpen=atof(strsql);

class iceb_u_str podr(row[5]);
fam.new_plus(row[0]);
dolg.new_plus(row[3]);
harac.new_plus(row[4]);

sovm=0;
if(row[1][0] == '1')
     sovm=1;
inn.new_plus(row[2]);

fo=iceb_menu_danet(gettext("Развёрнутый отчёт ?"),2,wpredok);


iceb_poldan("Код удержания алиментов",&kodalim,"zaralim.alx",wpredok);
//kodalim=atoi(bros);

iceb_poldan("Коды начислений не входящие в справку о доходах",&kodnvrspr,"zarnast.alx",wpredok);

sprintf(strsql,"%d %s.\n",rek->tabnom.ravno_atoi(),fam.ravno());
iceb_printw(strsql,buffer,view,"naim_shrift");

/*Определяем количество начислений и удержаний*/
sprintf(strsql,"select kod from Nash");
class SQLCURSOR cur;
if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kon == 0)
 {
  iceb_menu_soob(gettext("Не введены начисления !"),wpredok);
  return;
 }

class iceb_u_int MKN; //Массив кодов начислений
i=0;
while(cur.read_cursor(&row) != 0)
  MKN.plus(atoi(row[0]),-1);

sprintf(strsql,"select kod from Uder");

if((kou=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kou == 0)
 {
  iceb_menu_soob(gettext("Не введены удержания !"),wpredok);
  return;
 }

class iceb_u_int MKU; //Массив кодов удержаний

i=0;
while(cur.read_cursor(&row) != 0)
  MKU.plus(atoi(row[0]),-1);

sprintf(strsql,"select kod from Tabel");

if((koltb=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(koltb == 0)
 {
  iceb_menu_soob(gettext("Не введены виды табеля !"),wpredok);
  return;
 }

class iceb_u_int MKVT; //Массив кодов видов табеля

i=0;
while(cur.read_cursor(&row) != 0)
  MKVT.plus(atoi(row[0]),-1);

int kol_mes=iceb_u_period(1,mn ,gn,1,mk,gk,1);

class iceb_u_double mes_obud; /*все обязательные удержания по месяцам*/
mes_obud.make_class(kol_mes*obud[0]);

class iceb_u_double mes_nah; /*массив всех начислений по месяцам*/
mes_nah.make_class(kol_mes);

class iceb_u_double MSKN; //Массив содержимого кодов начислений
class iceb_u_double MSKU; //Массив содержимого кодов удержаний
MSKN.make_class(kon);
MSKU.make_class(kou);

class iceb_u_double DNI; //Массив количества рабочих дней
class iceb_u_double KDNI; //Массив количества календарных дней
class iceb_u_double HASI; //Массив количества отработанных часов
DNI.make_class(koltb);
KDNI.make_class(koltb);
HASI.make_class(koltb);

double suma_obud=0.; //Сумма обязательных удержаний за месяц
double suma_obudi=0.; //Сумма обязательных удержаний за период
double suma_vs=0.; /*Сумма военного сбора*/
double suma_vsi=0.; /*Сумма военного сбора итого*/

char imaf_sub[64];

FILE *ff_sub;
sprintf(imaf_sub,"sub%d.lst",getpid());
if((ff_sub = fopen(imaf_sub,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_sub,"",errno,wpredok);
  return;
 }

sprintf(imaf,"sp1%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

sprintf(imaf1,"sp2%d.lst",getpid());
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }

sprintf(imaf2,"sp3%d.lst",getpid());
if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,wpredok);
  return;
 }
class iceb_fopen fil_vn;
sprintf(imafvn,"spvn%d.lst",getpid());
if(fil_vn.start(imafvn,"w",wpredok) != 0)
 return;


char imaf_podoh[64];
FILE *ff_podoh;
sprintf(imaf_podoh,"spodoh%d.lst",getpid());
if((ff_podoh = fopen(imaf_podoh,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_podoh,"",errno,wpredok);
  return;
 }

class iceb_u_str adres("");

/*Читаем реквизиты организации*/
sprintf(strsql,"select * from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_menu_soob(gettext("Не найден код контрагента 00 !"),wpredok);
 }
else
 {
  adres.new_plus(row[3]);
  rekvizkon(row,ff1);
  rekvizkon(row,ff2);
  rekvizkon(row,fil_vn.ff);
  rekvizkon(row,ff_podoh);
  kedrpou.new_plus(row[5]);
 }

zarsprpr_sub_start(gn,kedrpou.ravno(),fam.ravno(),inn.ravno(),dolg.ravno(),adres.ravno(),ff_sub,wpredok);

zarsprprw_sap(mn,gn,mk,gk,harac.ravno(),fam.ravno(),inn.ravno(),dolg.ravno(),podr.ravno(),sovm,ff1);

fprintf(ff1,"\
-------------------------------------------\n");
fprintf(ff1,gettext("\
Месяц  | Начислено |  Единый  |Подоходный |\n\
       |           |соц. взнос|   налог   |\n"));
fprintf(ff1,"\
-------------------------------------------\n");

zarsprprw_sap(mn,gn,mk,gk,harac.ravno(),fam.ravno(),inn.ravno(),dolg.ravno(),podr.ravno(),sovm,fil_vn.ff);


zarsprprw_sap(mn,gn,mk,gk,harac.ravno(),fam.ravno(),inn.ravno(),dolg.ravno(),podr.ravno(),sovm,ff_podoh);
fprintf(ff_podoh,"\
-----------------------------------------\n");
fprintf(ff_podoh,gettext("\
Месяц  |   Начислено   |Подоходный налог|\n"));
fprintf(ff_podoh,"\
-----------------------------------------\n");

zarsprprw_sap(mn,gn,mk,gk,harac.ravno(),fam.ravno(),inn.ravno(),dolg.ravno(),podr.ravno(),sovm,ff2);

fprintf(ff2,"\
%*s---------------------------------------------\n\
%*s%s\n\
%*s---------------------------------------------\n",
ots," ",
ots," ",gettext("|  Дата |   Начислено   |Удержано| Получено |"),
ots," ");


if(fo == 1)
  fprintf(ff,"%d %s\n\
%s %d.%d%s %s %d.%d%s\n",rek->tabnom.ravno_atoi(),fam.ravno(),
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("до"),
mk,gk,
gettext("г."));

sprintf(strsql,"\n%d %s\n",rek->tabnom.ravno_atoi(),fam.ravno());
iceb_printw(strsql,buffer,view,"naim_shrift");
int kolmes=iceb_u_period(1,mn,gn,1,mk,gk,1);

//Создаем класс
class iceb_u_spisok NASH;
class iceb_u_double NASHD;
class iceb_u_spisok UDER;
class iceb_u_double UDERD;
double suma_esv=0.;
double isuma_esv=0.;
short kodsocstr=0;
short kodbzr=0;
ipodoh=ipeno=ihas=iskm=ialim=ifbezr=sld=ino=inob=iuo=0.;
idn=ikdn=0;
m=mn;
kol_mes=0;
int nom_kod=0;
float kolstr1=0.;
for(g=gn; g <=gk ; g++)
for(;  ; m++)
 {
  if(g == gk && m > mk)
   {
    break;
   }
  if(m == 13)
   {
    m=1;
    break;
   }
  
  kol_mes++;
  iceb_pbar(bar,kolmes,++kolstr1);    

  soc_strax=peno=podoh=0.;
  if(fo == 1)
   fprintf(ff,"\
==========================================================\n\
%d.%d%s\n",m,g,gettext("г."));

  /*Ищем табель за месяц*/
  sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and \
  mes=%d and tabn=%d",g,m,rek->tabnom.ravno_atoi());
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }
  suma_obud=0.;
  if(kolstr != 0)
  while(cur.read_cursor(&row) != 0)
   {
    kodt=atoi(row[0]);
    dnei=atof(row[1]);
    has=atof(row[2]);
    kdnei=atof(row[3]);
    
    /*Читаем наименование табеля*/
    sprintf(strsql,"select naik from Tabel where kod=%d", kodt);
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      nai.new_plus(row1[0]);
    else
      nai.new_plus("");
    
    if(fo == 1)
     fprintf(ff,"%d %-*s %s%2.f %s%3.2f %s%2.f\n",
     kodt,iceb_u_kolbait(40,nai.ravno()),nai.ravno(),
     gettext("д"),dnei,
     gettext("ч"),has,
     gettext("кд"),kdnei);

    if((i=MKVT.find(kodt)) != -1)
     {
      if((i=MKVT.find(kodt)) == -1)
       {
        sprintf(strsql,"%s %d %s ТН %d %s %d.%d%s\n",
        gettext("Не найден код табеля"),kodt,
        gettext("для"),rek->tabnom.ravno_atoi(),
        gettext("за"),m,g,
        gettext("г."));

        iceb_printw(strsql,buffer,view,"naim_shrift");

        if(fo == 1)
         fprintf(ff,"%s %d %s ТН %d %s %d.%d%s\n",
         gettext("Не найден код табеля"),kodt,
         gettext("для"),rek->tabnom.ravno_atoi(),
         gettext("за"),m,g,
         gettext("г."));
       }
      else
       {
        DNI.plus((int)dnei,i);
        KDNI.plus((int)kdnei,i);
        HASI.plus(has,i);
       }
     }
   }

  if(fo == 1)
    fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

  sprintf(strsql,"%d.%d%s\n",m,g,gettext("г."));

  iceb_printw(strsql,buffer,view,"naim_shrift");

  sal=zarsaldw(1,m,g,rek->tabnom.ravno_atoi(),&saldb,wpredok);

  sprintf(strsql,"select prn,knah,suma,datz,mesn,godn,kom,shet from \
Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d \
and suma <> 0. order by prn,knah asc",
  g,m,g,m,rek->tabnom.ravno_atoi());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }


  if(m == 1)
   {
    sld=sal;
   }

  fbezr=alim=skm=inah=iuder=inahb=0.;
  suma_vs=0.;

  if(kolstr != 0)
  while(cur.read_cursor(&row) != 0)
   {
    prn=atoi(row[0]);
    knah=atoi(row[1]);
    sym=atof(row[2]);
    iceb_u_rsdat(&dz,&mz,&gz,row[3],2);
    if(prn == 1)
     {
      mes_nah.plus(sym,kol_mes-1);
      sprintf(strsql,"%d,%s",knah,row[7]);
      if((i1=NASH.find(strsql)) == -1)
        NASH.plus(strsql);
      NASHD.plus(sym,i1);

      if((i=MKN.find(knah)) != -1)
        MSKN.plus(sym,i);

      inah+=sym;
      if(iceb_u_proverka(kodnvrspr.ravno(),row[1],0,1) != 0)
        inahb+=sym;

      if(provkodw(kodmp,knah) >= 0)
       {
	skm+=sym;
	iskm+=sym;
       }

     }

    if(prn == 2)
     {
      sprintf(strsql,"%d,%s",knah,row[7]);
      if((i1=UDER.find(strsql)) == -1)
       {
        UDER.plus(strsql);
        UDERD.plus(sym,-1);
       }      
      else
       {
        UDERD.plus(sym,i1);
       }
      if((i=MKU.find(knah)) != -1)
        MSKU.plus(sym,i);

      iuder+=sym;
//      if(knah == kodpn)
      if(iceb_u_proverka(kodpn_all.ravno(),knah,0,1) == 0)
	podoh+=sym;
      if(knah == kodpen)
	peno+=sym;

      if(knah == kodsocstr)
       soc_strax+=sym;
       
      if(knah == kodalim)
       {
	alim+=sym;
	ialim+=sym;
       }

      if(knah == kodbzr)
       {
	fbezr+=sym;
	ifbezr+=sym;
       }

      if((nom_kod=provkodw(obud,knah)) >= 0)
       {
        suma_obud+=sym;
        suma_obudi+=sym;
        mes_obud.plus(sym,((kol_mes-1)*obud[0])+nom_kod);
       }
      if(iceb_u_proverka(kods_esv_all.ravno(),knah,0,1) == 0)
       {
        suma_esv=sym;
        isuma_esv+=sym;
       }
      if(kodvn == knah)
       {
        suma_vs+=sym;
        suma_vsi+=sym;
        
       }
     }

    if(fo == 1)
     {
      /*Читаем наименование начисления*/
      if(prn == 1)
        sprintf(strsql,"select naik from Nash where kod=%d",knah);
      if(prn == 2)
        sprintf(strsql,"select naik from Uder where kod=%d",knah);
      if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
        nai.new_plus(row1[0]);
      else
        nai.new_plus("");

      fprintf(ff,"%3d/%02d/%2s.%s %-*s : %10.2f %-*s %s\n",
      knah,dz,row[4],row[5],iceb_u_kolbait(40,nai.ravno()),nai.ravno(),sym,
      iceb_u_kolbait(6,row[7]),row[7],row[6]);

     }

   }


  sprintf(strsql,"select tabn from Zarn where god=%d and mes=%d and \
 tabn=%d",g,m,rek->tabnom.ravno_atoi());
     
  if(iceb_u_strstrm(harac.ravno(),gettext("Студент")) == 0)
   {
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     {
      fprintf(ff1,"%02d.%d %11.2f %10.2f %11.2f\n",m,g,inahb,suma_esv*(-1), podoh*(-1));
      fprintf(ff_podoh,"%02d.%d %15.2f %15.2f\n",m,g,inahb,podoh*(-1));
      fprintf(ff2,"%*s|%02d.%d|%15.2f|%8.2f|%10.2f|\n",ots," ",m,g,inahb,suma_obud,inahb+suma_obud);
     }
   }
  else
   {
    fprintf(ff1,"%02d.%d %11.2f %10.2f %11.2f\n",m,g,inahb,suma_esv*(-1),podoh*(-1));
    fprintf(ff_podoh,"%02d.%d %15.2f %15.2f\n",m,g,inahb,podoh*(-1));
    fprintf(ff2,"%*s|%02d.%d|%15.2f|%8.2f|%10.2f|\n",ots," ",m,g,inahb,suma_obud,inahb+suma_obud);
   }
  iceb_mesc(m,0,&naim_mes);

  fprintf(ff_sub,"|%-*.*s|%10.2f|%17.2f|%10.2f|%17.2f|%10.2f|\n",
  iceb_u_kolbait(8,naim_mes.ravno()),iceb_u_kolbait(8,naim_mes.ravno()),naim_mes.ravno(),
  inahb,
  podoh,
  0.,
  0.,
  alim*(-1));
/*****************
  fprintf(ff_sub,"\
-------------------------------------------------------------------------------\n");  
***************/
  
  if(fo == 1)
    fprintf(ff,"\
     %-*s %15.2f\n\
     %-*s %15.2f\n\
     %-*s %15.2f\n\
     %-*s %15.2f\n",
     iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),sal,
     iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),inah,
     iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),iuder,
     iceb_u_kolbait(20,gettext("Cальдо конечное")),gettext("Cальдо конечное"),inah+iuder+sal);

  ino+=inah;
  inob+=inahb;
  iuo+=iuder;
  ipodoh+=podoh;
  ipeno+=peno;
  isoc_strax+=soc_strax;
 }

sprintf(strsql,"\
-------------------------------------------------\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n",
iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),sld,
iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),ino,
iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),iuo,
iceb_u_kolbait(20,gettext("Cальдо конечное")),gettext("Cальдо конечное"),ino+iuo+sld);

iceb_printw(strsql,buffer,view,"naim_shrift");

fprintf(ff_sub,"|%*.*s|%10.2f|%17.2f|%10.2f|%17.2f|%10.2f|\n",
iceb_u_kolbait(8,gettext("Итого")),
iceb_u_kolbait(8,gettext("Итого")),
gettext("Итого"),
ino,
ipodoh,
0.,
0.,
ialim*(-1));

fprintf(ff_sub,"\
-------------------------------------------------------------------------------\n");  

zarsprpr_sub_end(mn,gn,mk,gk,ino+ialim,ff_sub,wpredok);


iceb_u_preobr(inob,&str,0);

fprintf(ff1,"\
-------------------------------------------\n");

fprintf(ff1,"%*s %11.2f %10.2f %11.2f\n\n",
iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),
inob,isuma_esv*(-1),ipodoh*(-1));

fprintf(ff1,"%s:\n%-.*s\n",gettext("Общая сумма доходов"),iceb_u_kolbait(60,str.ravno()),str.ravno());
fprintf(fil_vn.ff,"\n%s:\n%-.*s\n",gettext("Общая сумма доходов"),iceb_u_kolbait(60,str.ravno()),str.ravno());

zarsprpr_ssu(mn,gn,mk,gk,&mes_nah,&mes_obud,fil_vn.ff); /*распечатка таблицы*/

fprintf(ff_podoh,"\
-----------------------------------------\n");
fprintf(ff_podoh,"%*s %15.2f %15.2f\n\n\
%s:\n%-.*s\n",
iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),inob,ipodoh*(-1),
gettext("Общая сумма доходов"),
iceb_u_kolbait(60,str.ravno()),str.ravno());

if(iceb_u_strlen(str.ravno()) > 60)
 {
  fprintf(ff1,"%s\n",iceb_u_adrsimv(60,str.ravno()));
  fprintf(fil_vn.ff,"%s\n",iceb_u_adrsimv(60,str.ravno()));
  fprintf(ff_podoh,"%s\n",iceb_u_adrsimv(60,str.ravno()));
 }

fprintf(ff2,"\
%*s---------------------------------------------\n\
%*s %*s %15.2f %8.2f %10.2f\n\n\
%s:\n%s\n",
ots," ",
ots," ",
iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),
inob,suma_obudi,inob+suma_obudi,
gettext("Общая сумма доходов"),str.ravno());



time(&tmr);
bf=localtime(&tmr);

fprintf(ff1,"\n%s\n",gettext("Подоходный налог и отчисления в социальные фонды\nудержаны и перечислены полностью"));
class iceb_fioruk_rk rukov;
class iceb_fioruk_rk glavbuh;
iceb_fioruk(1,&rukov,wpredok);
iceb_fioruk(2,&glavbuh,wpredok);

fprintf(ff1,"\n\n\
%-*s %20s %s\n\n\
%-*s %20s %s\n\n",
iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),"",rukov.fio.ravno(),
iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),"",glavbuh.fio.ravno());

fprintf(ff1,"\
    %02d.%02d.%d%s\n",
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));


fprintf(fil_vn.ff,"\n\n\
%s\n\n\
%-*s %20s %s\n\n\
%-*s %20s %s\n\n\
    %02d.%02d.%d%s\n",
gettext("Налог на доходы с физических особ, военный сбор и единый социальный\n\
взнос удержаны та перечислены полностью.\n\
В течение года плательщику налога предоставлялась ставка 18%.\n"),
iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),"",rukov.fio.ravno(),
iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),"",glavbuh.fio.ravno(),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));

fprintf(ff_podoh,"\n\n\
%-*s %20s %s\n\n\
%-*s %20s %s\n\n\
    %02d.%02d.%d%s\n",
iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),"",rukov.fio.ravno(),
iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),"",glavbuh.fio.ravno(),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));

fprintf(ff2,"\n\n\
%-*s %20s %s\n\n\
%-*s %20s %s\n\n\
    %02d.%02d.%d%s\n",
iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),"",rukov.fio.ravno(),
iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),"",glavbuh.fio.ravno(),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));


if(fo == 1)
  fprintf(ff,"\
-------------------------------------------------\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n\
%-*s %15.2f\n",
iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),sld,
iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),ino,
iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),iuo,
iceb_u_kolbait(20,gettext("Cальдо конечное")),gettext("Cальдо конечное"),ino+iuo+sld);


fprintf(ff,"%d %s\n\
%s %d.%d%s %s %d.%d%s\n",rek->tabnom.ravno_atoi(),fam.ravno(),
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."));


sprintf(strsql,"\n%s:\n",gettext("Начисления"));
iceb_printw(strsql,buffer,view,"naim_shrift");

fprintf(ff,"\n%s:\n",gettext("Начисления"));
bb=0.;
for(i=0 ; i<kon;i++)
 {
  knah=MKN.ravno(i);
  sym=MSKN.ravno(i);
  if(sym == 0.)
    continue;
  sprintf(strsql,"select naik from Nash where kod=%d",knah);
     
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    nai.new_plus(row1[0]);
  else
    nai.new_plus("");

  sprintf(strsql,"%3d %-*s %15.2f\n",knah,iceb_u_kolbait(40,nai.ravno()),nai.ravno(),sym);
  iceb_printw(strsql,buffer,view,"naim_shrift");

  fprintf(ff,"%3d %-*s %15.2f\n",knah,iceb_u_kolbait(40,nai.ravno()),nai.ravno(),sym);

  bb+=sym;
 }

if(fo == 1)
 {
  sprintf(strsql,"----------------------------------------------------\n\
%3s %*s %15.2f\n%s:\n"," ",iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),
  bb,gettext("Удержания"));

  iceb_printw(strsql,buffer,view,"naim_shrift");

  fprintf(ff,"----------------------------------------------------\n\
%3s %*s %15.2f\n%s:\n"," ",iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),
  bb,gettext("Удержания"));
  bb=0.;
  for(i=0 ; i<kou;i++)
   {
    sym=MSKU.ravno(i);
    if(sym == 0.)
      continue;
    knah=MKU.ravno(i);
    sprintf(strsql,"select naik from Uder where kod=%d",knah);
       
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      nai.new_plus(row1[0]);
    else
      nai.new_plus("");

    sprintf(strsql,"%3d %-*s %15.2f\n",knah,iceb_u_kolbait(40,nai.ravno()),nai.ravno(),sym);
    iceb_printw(strsql,buffer,view,"naim_shrift");

    fprintf(ff,"%3d %-*s %15.2f\n",knah,iceb_u_kolbait(40,nai.ravno()),nai.ravno(),sym);
    bb+=sym;
   }

  sprintf(strsql,"----------------------------------------------------\n\
%3s %*s %15.2f\n"," ",iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),bb);
  iceb_printw(strsql,buffer,view,"naim_shrift");
 }

/*Распечатываем суммы по табелям*/

sprintf(strsql,"%s: %d\n",gettext("Количество видов табеля"),koltb);
iceb_printw(strsql,buffer,view,"naim_shrift");

fprintf(ff,"----------------------------------------------------\n\
%3s %*s %15.2f\n%s:\n"," ",iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),bb,
gettext("Табель"));

fprintf(ff,"\
-----------------------------------------------------------------\n");
fprintf(ff,gettext("Код|           Наименование табеля          |Дни | Часы   |К.дни|\n"));
fprintf(ff,"\
-----------------------------------------------------------------\n");

for(i=0 ; i<koltb;i++)
 {
  dnei=DNI.ravno(i);
  kdnei=KDNI.ravno(i);
  has=HASI.ravno(i);
  if(dnei == 0 && kdnei == 0. && has == 0)
   continue;  

  knah=MKVT.ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",knah);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    nai.new_plus(row1[0]);
  else
    nai.new_plus("");

  fprintf(ff,"%3d %-*s %4.f %8.8g %4.f\n",knah,iceb_u_kolbait(40,nai.ravno()),nai.ravno(),dnei,has,kdnei);

  idn+=(int)dnei;
  ikdn+=(int)kdnei;
  ihas+=has;
 }
fprintf(ff,"\
-----------------------------------------------------------------\n\
%3s %*s %4d %8.8g %4d\n"," ",iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),idn,ihas,ikdn);




m=mn;
for(g=gn; g <=gk ; g++)
for(;  ; m++)
 {
  if(g == gk && m > mk)
   {
    break;
   }
  if(m == 13)
   {
    m=1;
    break;
   }
  fprintf(ff,"----------------- %d.%d%s -----------------\n",
  m,g,gettext("г."));
  vztbw(rek->tabnom.ravno_atoi(),m,g,&kon,&ihas,&kou,1,ff,view,buffer,wpredok);
 }




iceb_podpis(ff,wpredok);
iceb_podpis(ff1,wpredok);
iceb_podpis(ff2,wpredok);
iceb_podpis(fil_vn.ff,wpredok);
iceb_podpis(ff_podoh,wpredok);
iceb_podpis(ff_sub,wpredok);

fclose(ff);
fclose(ff1);
fclose(ff2);
fil_vn.end();
fclose(ff_podoh);
fclose(ff_sub);

//Распечатка начислений/удержаний по счетам

sprintf(imafshet,"sp5_%d.lst",getpid());
if((ff = fopen(imafshet,"w")) == NULL)
 {
  iceb_er_op_fil(imafshet,"",errno,wpredok);
  return;
 }

fprintf(ff,"%d %s\n\
%s %d.%d%s %s %d.%d%s\n",rek->tabnom.ravno_atoi(),fam.ravno(),
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."));

fprintf(ff,"\n%s:\n",gettext("Начисления"));
i=1;
bb=0.;
for(i=0; i < NASH.kolih() ; i++)
 {
//  printf("%s\n",NASH.ravno(i));
  iceb_u_polen(NASH.ravno(i),&str,1,',');
  iceb_u_polen(NASH.ravno(i),&shet,2,',');
  sprintf(strsql,"select naik from Nash where kod=%s",str.ravno());
     
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    nai.new_plus(row1[0]);
  else
    nai.new_plus("");
  fprintf(ff,"%2s %-*s %*s %8.2f\n",str.ravno(),iceb_u_kolbait(40,nai.ravno()),nai.ravno(),iceb_u_kolbait(5,shet.ravno()),shet.ravno(),NASHD.ravno(i));
  bb+=NASHD.ravno(i);
 }

fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n%s:\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),
bb,gettext("Удержания"));
i=1;
bb=0.;

for(i=0; i < UDER.kolih(); i++)
 {
  iceb_u_polen(UDER.ravno(i),&str,1,',');
  iceb_u_polen(UDER.ravno(i),&shet,2,',');
  sprintf(strsql,"select naik from Uder where kod=%s",str.ravno());
     
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    nai.new_plus(row1[0]);
  else
    nai.new_plus("");

  fprintf(ff,"%2s %-*s %*s %8.2f\n",str.ravno(),iceb_u_kolbait(40,nai.ravno()),nai.ravno(),iceb_u_kolbait(5,shet.ravno()),shet.ravno(),UDERD.ravno(i));
  bb+=UDERD.ravno(i);
 }
fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),bb);

//Распечатка бюджет/не бюджет

if(shetb != NULL)
 {
  fprintf(ff,"\n%s:\n",gettext("Начисления"));
  fprintf(ff,"%s.\n",gettext("Бюджет"));
  i=1;
  bb=0.;
  for(i=0; i < NASH.kolih(); i++)
   {
    iceb_u_polen(NASH.ravno(i),&str,1,',');
    iceb_u_polen(NASH.ravno(i),&shet,2,',');

    if(iceb_u_proverka(shetb,shet.ravno(),0,1) != 0)
     continue;

    sprintf(strsql,"select naik from Nash where kod=%s",str.ravno());
       
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      nai.new_plus(row1[0]);
    else
      nai.new_plus("");

    fprintf(ff,"%2s %-*s %*s %8.2f\n",str.ravno(),iceb_u_kolbait(40,nai.ravno()),nai.ravno(),iceb_u_kolbait(5,shet.ravno()),shet.ravno(),NASHD.ravno(i));
    bb+=NASHD.ravno(i);
   }


  fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n\n%s.\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),
  bb,
  gettext("Хозрасчёт"));


  i=1;
  bb=0.;
  for(i=0; i < NASH.kolih(); i++)
   {
    iceb_u_polen(NASH.ravno(i),&str,1,',');
    iceb_u_polen(NASH.ravno(i),&shet,2,',');

    if(iceb_u_proverka(shetb,shet.ravno(),0,1) == 0)
     continue;
//    if(pole1(shetb,shet,',',0,&i1) == 0 || iceb_u_SRAV(shetb,shet,0) == 0)
//       continue;


    sprintf(strsql,"select naik from Nash where kod=%s",str.ravno());
       
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      nai.new_plus(row1[0]);
    else
      nai.new_plus("");

    fprintf(ff,"%2s %-*s %*s %8.2f\n",str.ravno(),iceb_u_kolbait(40,nai.ravno()),nai.ravno(),iceb_u_kolbait(5,shet.ravno()),shet.ravno(),NASHD.ravno(i));
    bb+=NASHD.ravno(i);
   }


  fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),bb);

 }

if(shetbu != NULL)
 {
  fprintf(ff,"\n%s:\n",gettext("Удержания"));
  fprintf(ff,"%s.\n",gettext("Бюджет"));
  i=1;
  bb=0.;
  for(i=0; i < UDER.kolih(); i++)
   {
  
    iceb_u_polen(UDER.ravno(i),&str,1,',');
    iceb_u_polen(UDER.ravno(i),&shet,2,',');
  
    if(iceb_u_proverka(shetbu,shet.ravno(),0,1) != 0)
     continue;
    sprintf(strsql,"select naik from Uder where kod=%s",str.ravno());
       
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      nai.new_plus(row1[0]);
    else
      nai.new_plus("");
    fprintf(ff,"%2s %-*s %*s %8.2f\n",str.ravno(),iceb_u_kolbait(40,nai.ravno()),nai.ravno(),iceb_u_kolbait(5,shet.ravno()),shet.ravno(),UDERD.ravno(i));
    bb+=UDERD.ravno(i);
   }


  fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n\n%s.\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),
  bb,
  gettext("Хозрасчёт"));


  i=1;
  bb=0.;
  for(i=0; i < UDER.kolih(); i++)
   {
    iceb_u_polen(UDER.ravno(i),&str,1,',');
    iceb_u_polen(UDER.ravno(i),&shet,2,',');

    if(iceb_u_proverka(shetbu,shet.ravno(),0,1) == 0)
     continue;


    sprintf(strsql,"select naik from Uder where kod=%s",str.ravno());
       
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
      nai.new_plus(row1[0]);
    else
      nai.new_plus("");

    fprintf(ff,"%2s %-*s %*s %8.2f\n",str.ravno(),iceb_u_kolbait(40,nai.ravno()),nai.ravno(),iceb_u_kolbait(5,shet.ravno()),shet.ravno(),UDERD.ravno(i));
    bb+=UDERD.ravno(i);
   }


  fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n"," ",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"),bb);

 }

iceb_podpis(ff,wpredok);

fclose(ff);

rek->oth.spis_imaf.plus(imaf);
rek->oth.spis_naim.plus(gettext("Развёрнутый отчёт о начислениях/удержаниях"));

rek->oth.spis_imaf.plus(imaf1);
rek->oth.spis_naim.plus(gettext("Справка с удержаниями в соц.фонды"));

rek->oth.spis_imaf.plus(imaf2);
rek->oth.spis_naim.plus(gettext("Справка с общей суммой удержаний"));

rek->oth.spis_imaf.plus(imafvn);
rek->oth.spis_naim.plus(gettext("Справка со всеми удержаниями"));

rek->oth.spis_imaf.plus(imafshet);
rek->oth.spis_naim.plus(gettext("Свёрнутый отчёт о доходах и удержаниях"));

rek->oth.spis_imaf.plus(imaf_podoh);
rek->oth.spis_naim.plus(gettext("Справка с подоходным налогом"));

rek->oth.spis_imaf.plus(imaf_sub);
rek->oth.spis_naim.plus(gettext("Справка для оформления социальной помощи (жилищной субсидии)"));

for(int nomer=0; nomer < rek->oth.spis_imaf.kolih(); nomer++)
 iceb_ustpeh(rek->oth.spis_imaf.ravno(nomer),3,wpredok);

}
/***********************/
/*Реквизиты организации*/
/***********************/
void  rekvizkon(SQL_str row,FILE *ff)
{
int otstup=40;
int hag=35;
fprintf(ff,"%*s %s\n",otstup,"",row[1]);
int hag1=hag-iceb_u_strlen(gettext("Адрес"))-1;

//fprintf(ff,"%*s %s %-*.*s\n",otstup,"",gettext("Адрес"),hag1,hag1,row[3]);
fprintf(ff,"%*s %s %-*.*s\n",
otstup,"",gettext("Адрес"),
iceb_u_kolbait(hag1,row[3]),
iceb_u_kolbait(hag1,row[3]),
row[3]);

for(int ii=hag1; ii < iceb_u_strlen(row[3]); ii+=hag)
 fprintf(ff,"%*s %-*.*s\n",
 otstup,"",
 iceb_u_kolbait(hag,iceb_u_adrsimv(ii,row[3])),
 iceb_u_kolbait(hag,iceb_u_adrsimv(ii,row[3])),
 iceb_u_adrsimv(ii,row[3]));
// fprintf(ff,"%*s %-*.*s\n",otstup,"",hag,hag,&row[3][ii]);

fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Код ЕГРПОУ"),row[5]);
fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Р/С"),row[7]);
fprintf(ff,"%*s %s %s\n",otstup,"",gettext("МФО"),row[6]);

char bros[1024];

sprintf(bros,"%s %s %s %s",gettext("в"),row[2],gettext("гор."),row[4]);
for(int ii=0; ii < iceb_u_strlen(bros); ii+=hag)
  fprintf(ff,"%*s %-*.*s\n",
  otstup," ",
  iceb_u_kolbait(hag,iceb_u_adrsimv(ii,bros)),
  iceb_u_kolbait(hag,iceb_u_adrsimv(ii,bros)),
  iceb_u_adrsimv(ii,bros));
//  fprintf(ff,"%*s %-*.*s\n",otstup," ",hag,hag,&bros[ii]);
  
fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Налоговый номер"),row[8]);
fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Номер свидетельства плательщика НДС"),row[9]);
fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Телефон"),row[10]);

}

/***********************************/
/*Шапка*/
/************************************/
void zarsprprw_sap(short mn,short gn,
short mk,short gk,
const char *harac,
const char *fam,
const char *inn,
const char *dolg,
const char *podr,
short sovm,
FILE *ff1)
{
char strsql[512];
if(iceb_u_strstrm(harac,gettext("Студент")) == 0)
 {

  fprintf(ff1,"\n\
	      %s\n\
%s %d.%d%s %s %d.%d%s\n\
%s %s %s",
  gettext("С п р а в к а"),
  gettext("о заработной плате за период с"),
  mn,gn,
  gettext("г."),
  gettext("по"),
  mk,gk,
  gettext("г."),
  gettext("Работник"),
  fam,
  gettext("работает"));

  if(sovm == 0)
   {
    if(harac[0] == '\0')
      fprintf(ff1," %s.\n",gettext("постоянно"));
    else
      fprintf(ff1," %s.\n",harac);
   }
  else
   fprintf(ff1," %s.\n",gettext("по совместительству"));

  fprintf(ff1,"%s %s\n",gettext("Регистрационный номер учётной карточки"),inn);
  fprintf(ff1,"%s: %s\n",gettext("Должность"),dolg);
 }
else
 {
  SQL_str row;
  class SQLCURSOR cur;
  class iceb_u_str naipodr("");
  
  sprintf(strsql,"select naik from Podr where kod=%d",atoi(podr));
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naipodr.new_plus(row[0]);
  else
    naipodr.new_plus("");    
  fprintf(ff1,"\n\
	      %s\n\
%s %d.%d%s %s %d.%d%s\n",
gettext("С п р а в к а"),
gettext("о начислении стипендии за период с"),
mn,gn,
gettext("г."),
gettext("до"),
mk,gk,
gettext("г."));
  fprintf(ff1,"%s. %s %s\n",
  gettext("Учится на дневном отделении"),
  gettext("Группа"),naipodr.ravno());
  
  fprintf(ff1,"%s %s\n",gettext("Студент"),fam);

  fprintf(ff1,"%s %s\n",gettext("Регистрационный номер учётной карточки"),inn);

 }



}
/*******************************************/
/*Заголовок справки для получения субсидии*/
/********************************************/
int zarsprpr_sub_start(short god,const char *kedrpou,const char *fio,const char *inn,const char *dolg,
const char *adres,
FILE *ff_ras,
GtkWidget *wpredok)
{
class iceb_u_str stroka("");
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
char stroka1[1024];
const char *imaf_alx={"zarsns_start.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

class iceb_u_str naim_kontr(iceb_get_pnk("00",0,wpredok));

int nomer_str=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  stroka.new_plus(row_alx[0]);
  switch(nomer_str)
   {
    case 10:
     sprintf(stroka1,"\x1B\x45%.*s\x1B\x46",iceb_u_kolbait(52-4,naim_kontr.ravno()),naim_kontr.ravno()); /*включение/выключение режима выделенной печати*/
     iceb_u_vstav(&stroka,stroka1,20,79,1);
     break;

    case 12: /*адрес*/
     sprintf(stroka1,"\x1B\x45%.*s\x1B\x46",iceb_u_kolbait(55-4,adres),adres); /*включение/выключение режима выделенной печати*/
     iceb_u_vstav(&stroka,stroka1,17,79,1);
     break;

    case 13:
     sprintf(stroka1,"\x1B\x45%s\x1B\x46",kedrpou); /*включение/выключение режима выделенной печати*/
     iceb_u_vstav(&stroka,stroka1,31,79,1);
     break;

    case 17:
     sprintf(stroka1,"\x1B\x45%.*s\x1B\x46",iceb_u_kolbait(61-4,fio),fio); /*включение/выключение режима выделенной печати*/
     iceb_u_vstav(&stroka,stroka1,11,79,1);
     break;

    case 18: /*наименование*/
     sprintf(stroka1,"\x1B\x45%.*s\x1B\x46",iceb_u_kolbait(79-51,naim_kontr.ravno()),naim_kontr.ravno()); /*включение/выключение режима выделенной печати*/
     iceb_u_vstav(&stroka,stroka1,51,79,1);
     break;

    case 19: /*продолжение наименования*/
     if(iceb_u_strlen(naim_kontr.ravno()) >= 79-51)
      {
       sprintf(stroka1,"\x1B\x45%s\x1B\x46",iceb_u_adrsimv(79-51-1,naim_kontr.ravno())); /*включение/выключение режима выделенной печати*/
       iceb_u_vstav(&stroka,stroka1,0,79,1);
      }
     break;
    case 21:
     sprintf(stroka1,"\x1B\x45%.*s\x1B\x46",iceb_u_kolbait(79-14-4,dolg),dolg); /*включение/выключение режима выделенной печати*/
     iceb_u_vstav(&stroka,stroka1,14,79,1);
     break;
    case 23:
     sprintf(stroka1,"\x1B\x45%s\x1B\x46",inn); /*включение/выключение режима выделенной печати*/
     iceb_u_vstav(&stroka,stroka1,23,71,1);
     break;
    case 27:
     iceb_u_vstav(&stroka,god,1,4,1);
     break;
   }
  fprintf(ff_ras,"%s",stroka.ravno());
 }

return(0);
}
/*************************/
/*концовка справки на субсидию*/
/******************************/
int zarsprpr_sub_end(short mn,short gn,short mk,short gk,double suma,FILE *ff_ras,GtkWidget *wpredok)
{
class iceb_fioruk_rk rukov;
class iceb_fioruk_rk glavbuh;

char strsql[512];
class iceb_u_str stroka("");
short dk=0;
iceb_u_dpm(&dk,&mk,&gk,5);

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
const char *imaf_alx={"zarsns_end.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

iceb_fioruk(1,&rukov,wpredok);
iceb_fioruk(2,&glavbuh,wpredok);

int nomer_str=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  stroka.new_plus(row_alx[0]);
  switch(nomer_str)
   {
    case 1:
     sprintf(strsql,"01.%02d.%04d %s %s %02d.%02d.%04d %s",
     mn,gn,
     gettext("г."),
     gettext("по"),
     dk,mk,gk,
     gettext("г."));
     
     iceb_u_vstav(&stroka,strsql,33,62,1);
     break;

    case 2:
     memset(strsql,'\0',sizeof(strsql));
     iceb_u_preobr(suma,strsql,0);
     iceb_u_vstav(&stroka,strsql,25,79,1);
     
     break;

    case 7:
     iceb_u_vstav(&stroka,rukov.dolg.ravno(),0,33,1);
     sprintf(strsql,"%s %s",rukov.famil.ravno(),rukov.inic);
     iceb_u_vstav(&stroka,strsql,54,79,1);
     break;

    case 15:
     sprintf(strsql,"%s %s",glavbuh.famil.ravno(),glavbuh.inic);
     iceb_u_vstav(&stroka,strsql,54,79,1);
     break;
   }
  fprintf(ff_ras,"%s",stroka.ravno());
 }
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
fprintf(ff_ras,"%02d.%02d.%d %s N______\n",dt,mt,gt,gettext("г."));

return(0);

}
/************************************/
/*распечатка справки со всеми удержаниями*/
/*******************************************/
void zarsprpr_ssu(short mn,short gn,
short mk,short gk,
class iceb_u_double *mes_nash,
class iceb_u_double *mes_obud,
FILE *ff)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
const int razmer_kol=11;
class iceb_u_str naim_uder("");
class iceb_u_str liniq("--------------------");
int kol_mes=iceb_u_period(1,mn,gn,1,mk,gk,1);

for(int nom=0; nom < obud[0]; nom++)
 {
  if(mes_obud->prov_kol(kol_mes,obud[0],nom) == 0)
   continue;
  liniq.plus("------------");
 }

liniq.plus("------------");/*получено*/


fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"%-*.*s|%-*.*s|",
iceb_u_kolbait(7,gettext("Дата")),
iceb_u_kolbait(7,gettext("Дата")),
gettext("Дата"),
iceb_u_kolbait(razmer_kol,gettext("Начислено")),
iceb_u_kolbait(razmer_kol,gettext("Начислено")),
gettext("Начислено"));

float max_dlinna_naim=0;

/*первая строка шапки*/
for(int nom=0; nom < obud[0]; nom++)
 {
  if(mes_obud->prov_kol(kol_mes,obud[0],nom) == 0)
   continue;
  sprintf(strsql,"select naik from Uder where kod=%d",obud[nom+1]);
  if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
   naim_uder.new_plus(row[0]);
  else
   naim_uder.new_plus("");
     
  fprintf(ff,"%-*.*s|",
  iceb_u_kolbait(razmer_kol,naim_uder.ravno()),
  iceb_u_kolbait(razmer_kol,naim_uder.ravno()),
  naim_uder.ravno());

  if(iceb_u_strlen(naim_uder.ravno()) > max_dlinna_naim)
   max_dlinna_naim=iceb_u_strlen(naim_uder.ravno());
  
 }

fprintf(ff,"%-*.*s|\n",
iceb_u_kolbait(razmer_kol,gettext("Получено")),
iceb_u_kolbait(razmer_kol,gettext("Получено")),
gettext("Получено"));

float kol_strok=max_dlinna_naim/razmer_kol;

/*последующие строки шапки*/
int adres_str=razmer_kol;
for(float ks=1.; ks < kol_strok;ks+=1.)
 {
  fprintf(ff,"       |           |");
  for(int nom=0; nom < obud[0]; nom++)
   {
    if(mes_obud->prov_kol(kol_mes,obud[0],nom) == 0)
     continue;
    sprintf(strsql,"select naik from Uder where kod=%d",obud[nom+1]);
    if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
     naim_uder.new_plus(row[0]);
    else
     naim_uder.new_plus("");
       
    fprintf(ff,"%-*.*s|",
    iceb_u_kolbait(razmer_kol,iceb_u_adrsimv(adres_str,naim_uder.ravno())),
    iceb_u_kolbait(razmer_kol,iceb_u_adrsimv(adres_str,naim_uder.ravno())),
    iceb_u_adrsimv(adres_str,naim_uder.ravno()));

   }
  fprintf(ff,"           |\n");
  adres_str+=razmer_kol;  
 }


fprintf(ff,"%s\n",liniq.ravno());

short d=1,m=mn,g=gn;

double itogo_str=0.;
for(int nom=0; nom < kol_mes; nom++)
 {
  itogo_str=0.;
  fprintf(ff,"%02d.%04d|%*.2f|",m,g,razmer_kol,mes_nash->ravno(nom));
  itogo_str+=mes_nash->ravno(nom);
  for(int nom1=0; nom1 < obud[0];nom1++)
   {
    if(mes_obud->prov_kol(kol_mes,obud[0],nom1) == 0)
     continue;
    fprintf(ff,"%*.2f|",razmer_kol,mes_obud->ravno(nom*obud[0]+nom1));
    itogo_str+=mes_obud->ravno(nom*obud[0]+nom1);
   }     
  
  fprintf(ff,"%*.2f|\n",razmer_kol,itogo_str);
  iceb_u_dpm(&d,&m,&g,3);
 }
fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"%*.*s|%*.2f|",
iceb_u_kolbait(7,gettext("Итого")),
iceb_u_kolbait(7,gettext("Итого")),
gettext("Итого"),
razmer_kol,
mes_nash->suma());

/*распечатываем итоговую строку*/
for(int nom=0; nom < obud[0]; nom++)
 {
  if(mes_obud->prov_kol(kol_mes,obud[0],nom) == 0)
     continue;
  fprintf(ff,"%*.2f|",razmer_kol,mes_obud->suma_kol(kol_mes,obud[0],nom));
 }
fprintf(ff,"%*.2f|\n",razmer_kol,mes_nash->suma()+mes_obud->suma());
}
