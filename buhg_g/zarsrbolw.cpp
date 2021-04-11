/*$Id: zarsrbol.c,v 1.55 2014/01/31 12:11:34 sasa Exp $*/
/*21.01.2016	20.11.2001	Белых А.И.	zarsrbol.c
Расчёт средней заработной платы для больничного
*/
#include "buhg_g.h"
#include "zar_sprav.h"

class zarsrbol_sn /*свод по начислениям*/
 {
  public:
   class iceb_u_int kod; /*коды начислений*/
   class iceb_u_spisok mes_god;    /*список месяц-год*/
   class iceb_u_spisok kod_mes_god; /*списко код|месяц-год*/
   class iceb_u_double suma;   /*cуммы начислений*/
 };

class zarsrbol_sov /*свод по отработанному времени*/
 {
  public:
   class iceb_u_int kod; /*коды начислений*/
   class iceb_u_spisok mes_god;    /*список месяц-год*/
   class iceb_u_spisok kod_mes_god; /*списко код|месяц-год*/
   class iceb_u_int kolrd;   /*количество отработанных рабочих дней*/
   class iceb_u_int kolkd;   /*количество отработанных календарных дней*/
   class iceb_u_double kolhs;   /*количество отработанных часов*/

 };

void zarsrbol_rn(class iceb_u_str *kodotp,class iceb_u_str *kodgos,class iceb_u_str *kodtabotrv,int *metkaknvr,class iceb_u_str*,class iceb_u_str*,GtkWidget *wpredok);
void zarsrbol_rs(short mn,short gn,int kolmes,double *sum_ed_viplat,float *kolrd_mes,int *kolddpev,double *nash,double *gosob,int *kolrd,double *kolrh,float *kolrh_mes,int *metka_bol,int *kolotp,FILE *ff,FILE *ffprot,GtkWidget *wpredok);
void zarsrbol_rsn(short mn,short gn,short mk,short gk,int tabnom,class zarsrbol_sn *svod_nah,FILE *ff,GtkWidget *wpredok);
void zarsrbol_rsov(short mn,short gn,short mk,short gk,int tabnom,class zarsrbol_sov *svod_otvr,FILE *ff,GtkWidget *wpredok);

extern char	*shetb; /*Бюджетные счета начислений*/
extern short    *kodbl;  /*Код начисления больничного*/
extern SQL_baza bd;

void zarsrbolw(class zar_sprav_rek *data,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *bar,
GtkWidget *wpredok)
{
char		strsql[1024];
SQL_str		row,row1;
int		kolstr=0;
int		kolmes=0; //Количество месяцев в периоде
double		suma=0.;
short		kkk;
char		imaf[64],imafprot[64];
class iceb_u_str kodotp(""); //Коды отпускных
class iceb_u_str kodgos(""); //Коды гос.обязанности
class iceb_u_str kodtabotrv(""); //Коды табеля отработанного времени
class iceb_u_str kod_ed_viplat("");
short		knah=0;
short		dr,mr,gr;
short		metkab=0; //0-не бюджет 1 бюджет


short mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,data->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->datak.ravno());

kolmes=iceb_u_period(1,mn,gn,1,mk,gk,1);
if(kolmes == 0)
 {
  iceb_menu_soob(gettext("Количество месяцев в периоде равно нолю !"),wpredok);
  return;

 }

sprintf(strsql,"%s\n",gettext("Расчёт средней зарплаты для начисления больничного"));
iceb_printw(strsql,buffer,view);

sprintf(strsql,"%s %d.%d %s %d.%d %s %s\n",
gettext("Период с"),mn,gn,
gettext("до"),mk,gk,data->tabnom.ravno(),data->fio.ravno());

iceb_printw(strsql,buffer,view);

sprintf(strsql,"%s:%d\n",gettext("Количество месяцев"),kolmes);

iceb_printw(strsql,buffer,view);


class zarsrbol_sn svod_nah; /*свод начислений*/
class zarsrbol_sov svod_otvr; /*свод по отработанному времени*/

double nash[kolmes]; //Всего начислено
double gosob[kolmes]; //Оплата гособязанности
int    kolrd[kolmes]; //Количество отработанных дней
double kolrh[kolmes]; //Количество отработанных часов
int    kolotp[kolmes]; //Количество отпускных
int kolddpev[kolmes];  /*Количество дней для перерасчёта единоразовых выплат*/

double sum_ed_viplat[kolmes]; //Суммы единоразовых выплат
double sum_ed_viplat_b[kolmes]; //Суммы единоразовых выплат - бюджет
double sum_ed_viplat_h[kolmes]; //Суммы единоразовых выплат - хозрасчёт

float kolrd_mes[kolmes];  //Количество рабочих дней в месяце по календарю
float kolrh_mes[kolmes]; //Количесво рабочих часов в месяце

double bnash[kolmes]; //Всего начислено (бюджет)
double bgosob[kolmes]; //Оплата гособязанности  (бюджет)

double nbnash[kolmes]; //Всего начислено  (небюджет)
double nbgosob[kolmes]; //Оплата гособязанности  (небюджет)

int metka_bol[kolmes]; //Метка больничного общая
int metka_bol_hoz[kolmes]; //Метка больничного для хозрасчёта
int metka_bol_bd[kolmes]; //Метка больничного для бюджета

memset(kolrd_mes,'\0',sizeof(kolrd_mes));
memset(kolrh_mes,'\0',sizeof(kolrh_mes));
memset(metka_bol,'\0',sizeof(metka_bol));
memset(metka_bol_hoz,'\0',sizeof(metka_bol_hoz));
memset(metka_bol_bd,'\0',sizeof(metka_bol_bd));

memset(sum_ed_viplat,'\0',sizeof(sum_ed_viplat));
memset(sum_ed_viplat_b,'\0',sizeof(sum_ed_viplat_b));
memset(sum_ed_viplat_h,'\0',sizeof(sum_ed_viplat_h));

memset(nash,'\0',sizeof(nash));
memset(&gosob,'\0',sizeof(gosob));
memset(&kolrd,'\0',sizeof(kolrd));
memset(&kolddpev,'\0',sizeof(kolddpev));
memset(&kolrh,'\0',sizeof(kolrh));
memset(&kolotp,'\0',sizeof(kolotp));

memset(&bnash,'\0',sizeof(bnash));
memset(&bgosob,'\0',sizeof(bgosob));

memset(&nbnash,'\0',sizeof(nbnash));
memset(&nbgosob,'\0',sizeof(nbgosob));


int metkaknvr=0;
class iceb_u_str kodtdpev("");
//Читаем настройки
zarsrbol_rn(&kodotp,&kodgos,&kodtabotrv,&metkaknvr,&kod_ed_viplat,&kodtdpev,wpredok);


//Читаем календарь за период и заполняем массивы количества рабочих часов и дней
short d=1,m=mn,g=gn;
for(int shmes=0; shmes < kolmes ; shmes++)
 {
  redkalw(m,g,&kolrd_mes[shmes],&kolrh_mes[shmes],wpredok);
  iceb_u_dpm(&d,&m,&g,3);
 }
class iceb_fopen fils;
sprintf(imaf,"sz%s_%d.lst",data->tabnom.ravno(),getpid());

if(fils.start(imaf,"w",wpredok) != 0)
 return;

class iceb_fopen filprot;
 
sprintf(imafprot,"szp%s_%d.lst",data->tabnom.ravno(),getpid());
if(filprot.start(imafprot,"w",wpredok) != 0)
 return;
 
iceb_zagolov(gettext("Расчёт средней зарплаты для начисления больничного"),1,mn,gn,31,mk,gk,fils.ff,wpredok);
fprintf(fils.ff,"%s %s\n",data->tabnom.ravno(),data->fio.ravno());
fprintf(fils.ff,"%s:%d\n",gettext("Количество месяцев"),kolmes);

if(kodgos.ravno()[0] != '\0')
 {
  fprintf(fils.ff,"%s:%s\n",gettext("Коды не вошедшие в расчёт"),kodgos.ravno());
  int klst=0;
  if(metkaknvr == 1)
   iceb_printcod(fils.ff,"Nash","kod","naik",0,kodgos.ravno(),&klst);
 }
if(kod_ed_viplat.ravno()[0] != '\0')
 {
  fprintf(fils.ff,"%s:%s\n","Коды единоразовых выплат",kod_ed_viplat.ravno());
 
 }
if(kodtdpev.getdlinna() > 1)
  fprintf(fils.ff,"%s:%s\n",gettext("Коды табеля для перерасчёта одноразовых выплат"),kodtdpev.ravno());

iceb_zagolov(gettext("Расчёт средней зарплаты для начисления больничного"),1,mn,gn,31,mk,gk,filprot.ff,wpredok);
fprintf(filprot.ff,"%s %s\n",data->tabnom.ravno(),data->fio.ravno());
fprintf(filprot.ff,"%s:%d\n",gettext("Количество месяцев"),kolmes);

SQLCURSOR cur;
SQLCURSOR curr;
int nomvs=0;
float kolstr1=0.;
dr=1; mr=mn; gr=gn;
for(kkk=0; kkk < kolmes; kkk++)
 {
  iceb_pbar(bar,kolmes,++kolstr1);    
  sprintf(strsql,"%02d.%04d",mr,gr);
  if(svod_nah.mes_god.find(strsql) < 0)
   svod_nah.mes_god.plus(strsql);
  if(svod_otvr.mes_god.find(strsql) < 0)
   svod_otvr.mes_god.plus(strsql);

  fprintf(filprot.ff,"\n%d.%d\n",mr,gr);
  sprintf(strsql,"select knah,suma,shet from Zarp where tabn=%s and \
datz >= '%d-%d-01' and datz <= '%d-%d-31' and prn='1' and suma <> 0.",
  data->tabnom.ravno(),gr,mr,gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }
  if(kolstr != 0)
   {
    while(cur.read_cursor(&row) != 0)
     {
      knah=atoi(row[0]);
      suma=atof(row[1]);    
      
      if(svod_nah.kod.find(knah) < 0)
       svod_nah.kod.plus(knah);


      sprintf(strsql,"%d|%02d.%04d",knah,mr,gr);
      if((nomvs=svod_nah.kod_mes_god.find(strsql)) < 0)
        svod_nah.kod_mes_god.plus(strsql);
      svod_nah.suma.plus(suma,nomvs);
      
      sprintf(strsql,"select naik from Nash where kod=%d",knah);
      iceb_sql_readkey(strsql,&row1,&curr,wpredok);
      
      fprintf(filprot.ff,"%2d %-*s %8.2f\n",knah,iceb_u_kolbait(40,row1[0]),row1[0],suma);
             

      metkab=0;
      if(iceb_u_proverka(shetb,row[2],0,1) == 0)
        metkab=1;

      if(iceb_u_proverka(kod_ed_viplat.ravno(),row[0],0,1) == 0) //коды единоразовых выплат
       {
        sum_ed_viplat[kkk]+=suma;
        if(metkab == 1)
         sum_ed_viplat_b[kkk]+=suma;
        else
         sum_ed_viplat_h[kkk]+=suma;
       }

      if(iceb_u_proverka(kodgos.ravno(),row[0],0,1) == 0) //коды не входящие в расчёт
       {
        gosob[kkk]+=suma;
        if(metkab == 1)
         bgosob[kkk]+=suma;
        else
         nbgosob[kkk]+=suma;
       }

      nash[kkk]+=suma;    
      if(metkab == 1)
       {
        bnash[kkk]+=suma;    
       }
      else      
       {
        nbnash[kkk]+=suma;    
       }
     }

    fprintf(filprot.ff,"%*s:%8.2f\n",iceb_u_kolbait(33,gettext("Итого")),gettext("Итого"),nash[kkk]);
   }  
  //Узнаем количество отработанных дней и количество дней в отпуске
  sprintf(strsql,"select kodt,dnei,kdnei,has from Ztab where \
tabn=%s and god=%d and mes=%d",data->tabnom.ravno(),gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   { 
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }

  while(cur.read_cursor(&row) != 0)
   {
    if(svod_otvr.kod.find(row[0]) < 0)
     svod_otvr.kod.plus(row[0]);

    sprintf(strsql,"%s|%02d.%04d",row[0],mr,gr);
    if((nomvs=svod_otvr.kod_mes_god.find(strsql)) < 0)
     svod_otvr.kod_mes_god.plus(strsql);

    svod_otvr.kolrd.plus(row[1],nomvs);    
    svod_otvr.kolkd.plus(row[2],nomvs);    
    svod_otvr.kolhs.plus(atof(row[3]),nomvs);    

    sprintf(strsql,"select naik from Tabel where kod=%s",row[0]);
    sql_readkey(&bd,strsql,&row1,&curr);
    fprintf(filprot.ff,"%-2s %-*s %2s %2s\n",row[0],iceb_u_kolbait(30,row1[0]),row1[0],row[1],row[2]);

    if(iceb_u_proverka(kodtabotrv.ravno(),row[0],0,1) == 0)
     {
//      kolrd[kkk]+=atoi(row[1]);
      kolrd[kkk]+=atoi(row[2]); /*c 4.7.2015*/
      kolrh[kkk]+=atof(row[3]);
     }
//    if(iceb_u_proverka(kodtabotp.ravno(),row[0],0,1) == 0)
//      kolotp[kkk]+=atoi(row[2]);
    if(iceb_u_proverka(kodtdpev.ravno(),row[0],0,1) == 0)
      kolddpev[kkk]+=atoi(row[2]); /*c 04.07.2015*/
//      kolddpev[kkk]+=atoi(row[1]);
   }  
  iceb_u_dpm(&dr,&mr,&gr,3);
 }

/*смотрим есть ли больничные в счёт месяцев расчёта*/
fprintf(filprot.ff,"Определяем больничные в счёт месяцев расчёта\n");
dr=1; mr=mn; gr=gn;
for(kkk=0; kkk < kolmes; kkk++)
 {
  fprintf(filprot.ff,"\n%d.%d\n",mr,gr);
  sprintf(strsql,"select knah,suma,shet,datz from Zarp where tabn=%s and \
godn=%d and mesn = %d and prn='1' and suma <> 0.",data->tabnom.ravno(),gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }
  if(kolstr != 0)
   {
    while(cur.read_cursor(&row) != 0)
     {
   
      if(provkodw(kodbl,row[0]) < 0)
       continue;

      fprintf(filprot.ff,"%s %2s %10s %6s\n",row[3],row[0],row[1],row[2]);      
      metka_bol[kkk]=1;

      metkab=0;
      if(iceb_u_proverka(shetb,row[2],0,1) == 0)
        metkab=1;

      if(metkab == 1)
       {
        metka_bol_bd[kkk]=metka_bol[kkk];
       }
      else      
       {
        metka_bol_hoz[kkk]=metka_bol[kkk];
       }

     }
   }
  iceb_u_dpm(&dr,&mr,&gr,3);
 }
//Распечатываем результат

zarsrbol_rs(mn,gn,kolmes,sum_ed_viplat,kolrd_mes,kolddpev,nash,gosob,kolrd,kolrh,kolrh_mes,metka_bol,kolotp,fils.ff,filprot.ff,wpredok);

if(shetb != NULL)
 {
  sprintf(strsql,"%s\n",gettext("Бюджет"));
  iceb_printw(strsql,buffer,view);
  fprintf(fils.ff,"\n%s\n",gettext("Бюджет"));
  zarsrbol_rs(mn,gn,kolmes,sum_ed_viplat_b,kolrd_mes,kolddpev,bnash,bgosob,kolrd,kolrh,kolrh_mes,metka_bol_bd,kolotp,fils.ff,filprot.ff,wpredok);

  sprintf(strsql,"%s\n",gettext("Хозрасчёт"));
  iceb_printw(strsql,buffer,view);
  fprintf(fils.ff,"\n%s\n",gettext("Хозрасчёт"));
  zarsrbol_rs(mn,gn,kolmes,sum_ed_viplat_h,kolrd_mes,kolddpev,nbnash,nbgosob,kolrd,kolrh,kolrh_mes,metka_bol_hoz,kolotp,fils.ff,filprot.ff,wpredok);
 }

class iceb_fioruk_rk ruk;
class iceb_fioruk_rk gb;
iceb_fioruk(1,&ruk,wpredok);
iceb_fioruk(2,&gb,wpredok);

fprintf(fils.ff,"\n%*s__________________________%s\n",
iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
ruk.famil_inic.ravno());

fprintf(fils.ff,"\n%*s__________________________%s\n",
iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),
gb.famil_inic.ravno());

iceb_podpis(fils.ff,wpredok);
iceb_podpis(filprot.ff,wpredok);


fils.end();
filprot.end();


char imaf_sv[64];
sprintf(imaf_sv,"zarsprsn%d.lst",getpid());
if(fils.start(imaf_sv,"w",wpredok) != 0)
 return;

iceb_zagolov(gettext("Свод начислений и отработанного времени"),1,mn,gn,31,mk,gk,fils.ff,wpredok);

zarsrbol_rsn(mn,gn,mk,gk,atoi(data->tabnom.ravno()),&svod_nah,fils.ff,wpredok);
zarsrbol_rsov(mn,gn,mk,gk,atoi(data->tabnom.ravno()),&svod_otvr,fils.ff,wpredok);

iceb_podpis(fils.ff,wpredok);
fils.end();


data->oth.spis_imaf.plus(imaf);
data->oth.spis_naim.plus(gettext("Расчёт средней зарплаты для начисления больничного"));
data->oth.spis_imaf.plus(imaf_sv);
data->oth.spis_naim.plus(gettext("Свод начислений и отработанного времени"));
data->oth.spis_imaf.plus(imafprot);
data->oth.spis_naim.plus(gettext("Протокол расчёта"));

for(int nom=0; nom < data->oth.spis_imaf.kolih(); nom++)
 iceb_ustpeh(data->oth.spis_imaf.ravno(nom),3,wpredok);

return;
}
/************************/
/*Чтение настроек*/
/*************************/
void zarsrbol_rn(class iceb_u_str *kodotp,class iceb_u_str *kodgos,class iceb_u_str *kodtabotrv,int *metkaknvr,class iceb_u_str *kod_ed_viplat,
class iceb_u_str *kodtdpev, /*Коды табеля для перерасчёта единоразовой выплаты*/
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_u_str stroka1("");

sprintf(strsql,"select str from Alx where fil='zarsrbol.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarsrbol.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],&stroka1,1,'|') != 0)
   continue;
/*********************
  if(iceb_u_SRAV(stroka1.ravno(),"Коды начисления отпускных",0) == 0)
   {
    iceb_u_polen(row_alx[0],kodotp,2,'|');
    continue;
   }
*********************/
  if(iceb_u_SRAV(stroka1.ravno(),"Коды не входящие в расчёт средней зарплаты для начисления больничного",0) == 0)
   {
    iceb_u_polen(row_alx[0],kodgos,2,'|');
    continue;
   }
  if(iceb_u_SRAV(stroka1.ravno(),"Коды видов табеля отработанного времени",0) == 0)
   {
    iceb_u_polen(row_alx[0],kodtabotrv,2,'|');
    continue;
   }
  if(iceb_u_SRAV(stroka1.ravno(),"Коды табеля для перерасчёта одноразовых выплат",0) == 0)
   {
    iceb_u_polen(row_alx[0],kodtdpev,2,'|');
    continue;
   }
/**************************
  if(iceb_u_SRAV(stroka1.ravno(),"Коды видов табеля отпускных",0) == 0)
   {
    iceb_u_polen(row_alx[0],kodtabotp,2,'|');
    continue;
   }
********************/
  if(iceb_u_SRAV(stroka1.ravno(),"Коды единоразовых выплат",0) == 0)
   {
    iceb_u_polen(row_alx[0],kod_ed_viplat,2,'|');
    continue;
   }
  if(iceb_u_SRAV(stroka1.ravno(),"Печатать коды невходящие в расчёт",0) == 0)
   {
    iceb_u_polen(row_alx[0],&stroka1,2,'|');
    if(iceb_u_SRAV(stroka1.ravno(),"Вкл",1) == 0)
      *metkaknvr=1; 
    continue;
   }

 }
}
/*****************************************/
/*                          */
/***************************************/
void zarsrbol_rs(short mn,short gn,int kolmes,
double *sum_ed_viplat,
float *kolrd_mes,
int *kolddpev,
double *nash,
double *gosob,
int *kolrd,
double *kolrh,
float *kolrh_mes,
int *metka_bol,
int *kolotp,
FILE *ff,FILE *ffprot,
GtkWidget *wpredok)
{
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Дата  |  Количество дней  |    Начисленно     |Суммы ед|В расчёт|  Берем в |\n\
       | Отраб.   |В месяце| Всего    |Невошло |ино.раз.|с ед.раз|  расчёт  |\n\
       |          |        |          |        |выплат  |выплат  |          |\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------\n");

int iotr=0;
int iotr_mes=0;
double ioth=0;
double ioth_mes=0;
int iotp=0;
double isumn=0.;
double igos=0.;
double ivras=0.;
double vras=0;
double ivras_h=0.;
double vras_h=0;
short dr=1; short mr=mn; short gr=gn;
char metka_rd[8];
char metka_rh[8];
double suma_ed_vras;
double isuma_ed_vras=0;
double isum_ed_viplat=0;
short dnr=1,mnr=mn,gnr=gn;
class iceb_u_str boll("");
class zar_read_tn1h nastr;

for(int kkk=0; kkk < kolmes; kkk++)
 {
  suma_ed_vras=sum_ed_viplat[kkk];

  if(sum_ed_viplat[kkk] != 0.)
   {
    fprintf(ffprot,"\nЕсть сумма единоразовой выплаты %.2f в %d месяце\n\
Количество рабочих дней в месяце:%f\n\
Количество дней для перерасчёта одноразовой выплаты=%d\n",
   sum_ed_viplat[kkk],kkk+1,kolrd_mes[kkk],kolddpev[kkk]);

    if(kolrd_mes[kkk] != kolddpev[kkk])
     {
      float kold=kolddpev[kkk];
      if(kold == 0)
       fprintf(ffprot,"Количество дней для перерасчёта одноразовой выплаты равно нолю!\n");
      if(kolddpev[kkk] > kolrd_mes[kkk])
        kold=kolrd_mes[kkk];
      suma_ed_vras=sum_ed_viplat[kkk]/kolrd_mes[kkk]*kold;
      fprintf(ffprot,"*Пересчитываем единоразовую выплату=%.2f/%f*%f=%.2f\n",
      sum_ed_viplat[kkk],kolrd_mes[kkk],kold,suma_ed_vras);
     }
    }

  vras_h=vras=nash[kkk]-gosob[kkk]-sum_ed_viplat[kkk]+suma_ed_vras;

  if(vras < 0.)
   vras_h=vras=0.;

  //читаем настройки на этот месяц

  zar_read_tn1w(1,mr,gr,&nastr,wpredok);
  iceb_u_dpm(&dnr,&mnr,&gnr,3);
  
  if(vras > nastr.max_sum_for_soc)
    vras_h=vras=nastr.max_sum_for_soc;
  
  memset(metka_rd,'\0',sizeof(metka_rd));
  memset(metka_rh,'\0',sizeof(metka_rh));
  metka_rh[0]=' ';    
  metka_rd[0]=' ';    
  if(kolrd[kkk] < kolrd_mes[kkk])
    metka_rd[0]='*';
     
  if(kolrh[kkk] < kolrh_mes[kkk])
    metka_rh[0]='*';
  
  boll.new_plus("");
  if(metka_bol[kkk] == 1)
   boll.new_plus(gettext("Больничный")); 

  fprintf(ff,"%02d.%d %-3d%s/%4.f%s %-3.f/%4.f %10.2f %8.2f %8.2f %8.2f %10.2f %s\n",
  mr,gr,kolrd[kkk],metka_rd,kolrh[kkk],metka_rh,kolrd_mes[kkk],kolrh_mes[kkk],nash[kkk],gosob[kkk],
  sum_ed_viplat[kkk],suma_ed_vras,
  vras,boll.ravno());
  
  iotr+=kolrd[kkk];
  ioth+=kolrh[kkk];
  iotr_mes+=(int)kolrd_mes[kkk];
  ioth_mes+=kolrh_mes[kkk];
  iotp+=kolotp[kkk];
  isumn+=nash[kkk];
  igos+=gosob[kkk];
  ivras+=vras;
  ivras_h+=vras_h;
  isuma_ed_vras+=suma_ed_vras;
  isum_ed_viplat+=sum_ed_viplat[kkk];
  
  iceb_u_dpm(&dr,&mr,&gr,3);
 }
/**************
if(iotr != 0)
 printw("%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
 ivras,iotr,ivras/iotr);
else
 printw("%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
 ivras,iotr,0.);
*************/
fprintf(ff,"\
-----------------------------------------------------------------------------\n");

fprintf(ff,"%-*.*s %-3d /%4.f  %-3d/%4.f %10.2f %8.2f %8.2f %8.2f %10.2f\n",
iceb_u_kolbait(7,gettext("Итого")),
iceb_u_kolbait(7,gettext("Итого")),
gettext("Итого"),
iotr,ioth,iotr_mes,ioth_mes,isumn,igos,
isum_ed_viplat,isuma_ed_vras,
ivras);

if(iotr != 0)
 fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
 ivras,iotr,ivras/iotr);
else
 fprintf(ff,"%s: %.2f/%d=%.2f\n",gettext("Средняя зарплата"),
 ivras,iotr,0.);

if(ioth != 0)
 fprintf(ff,"%s: %.2f/%.1f=%.2f\n",gettext("Средняя зарплата"),
 ivras_h,ioth,ivras/ioth);
else
 fprintf(ff,"%s: %.2f/%.1f=%.2f\n",gettext("Средняя зарплата"),
 ivras_h,ioth,0.);
}
/**************************************/
/*Распечатка свода начислений*/
/***************************************/
void zarsrbol_rsn(short mn,short gn,
short mk,short gk,
int tabnom,
class zarsrbol_sn *svod_nah,FILE *ff,
GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str fio("");


fprintf(ff,"%s\n",gettext("Свод начислений"));

sprintf(strsql,"select fio from Kartb where tabn=%d",tabnom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 fio.new_plus(row[0]);
fprintf(ff,"%s:%d %s\n",gettext("Работник"),tabnom,fio.ravno());

class iceb_u_str liniq("--------------------------");
for(int nom=0; nom <= svod_nah->mes_god.kolih(); nom++)
 liniq.plus("-----------");

fprintf(ff,"%s\n",liniq.ravno());
 
fprintf(ff,"\
%-*.*s|%-*.*s|",
iceb_u_kolbait(4,gettext("Код")),
iceb_u_kolbait(4,gettext("Код")),
gettext("Код"),
iceb_u_kolbait(20,gettext("Наименование")),
iceb_u_kolbait(20,gettext("Наименование")),
gettext("Наименование"));

for(int nom=0; nom < svod_nah->mes_god.kolih(); nom++)
 fprintf(ff,"%-10s|",svod_nah->mes_god.ravno(nom));

fprintf(ff,"%-*.*s|\n",
iceb_u_kolbait(10,gettext("Итого")),
iceb_u_kolbait(10,gettext("Итого")),
gettext("Итого"));



fprintf(ff,"%s\n",liniq.ravno());
int nom_vs=0;

for(int nom_kod=0; nom_kod < svod_nah->kod.kolih(); nom_kod++)
 {
  sprintf(strsql,"select naik from Nash where kod=%d",svod_nah->kod.ravno(nom_kod));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   fio.new_plus(row[0]);
  else
   fio.new_plus("");

  fprintf(ff,"%4d|%-*.*s|",
  svod_nah->kod.ravno(nom_kod),
  iceb_u_kolbait(20,fio.ravno()),
  iceb_u_kolbait(20,fio.ravno()),
  fio.ravno());
  double itogo_str=0;
  for(int nom_dat=0; nom_dat < svod_nah->mes_god.kolih(); nom_dat++)
   {
    sprintf(strsql,"%d|%s",svod_nah->kod.ravno(nom_kod),svod_nah->mes_god.ravno(nom_dat));
    if((nom_vs=svod_nah->kod_mes_god.find(strsql)) >= 0)
     {
      fprintf(ff,"%10.2f|",svod_nah->suma.ravno(nom_vs));
      itogo_str+=svod_nah->suma.ravno(nom_vs);
     }
    else
     fprintf(ff,"%10s|","");
   }
  fprintf(ff,"%10.2f|\n",itogo_str);
 }

fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*.*s|",
iceb_u_kolbait(25,gettext("Итого")),
iceb_u_kolbait(25,gettext("Итого")),
gettext("Итого"));

for(int nom_dat=0; nom_dat < svod_nah->mes_god.kolih(); nom_dat++)
 {
  double itogo_str=0;
  for(int nom_kod=0; nom_kod < svod_nah->kod.kolih(); nom_kod++)
   {
    sprintf(strsql,"%d|%s",svod_nah->kod.ravno(nom_kod),svod_nah->mes_god.ravno(nom_dat));
    if((nom_vs=svod_nah->kod_mes_god.find(strsql)) >= 0)
     {
      itogo_str+=svod_nah->suma.ravno(nom_vs);
     }
   }  
  fprintf(ff,"%10.2f|",itogo_str);
  
 }

fprintf(ff,"%10.2f|\n",svod_nah->suma.suma());


}

/**************************************/
/*Распечатка свода отработанного времени*/
/***************************************/
void zarsrbol_rsov(short mn,short gn,
short mk,short gk,
int tabnom,
class zarsrbol_sov *svod_otvr,FILE *ff,
GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str fio("");


fprintf(ff,"\n%s\n",gettext("Свод отработанного времени"));

sprintf(strsql,"select fio from Kartb where tabn=%d",tabnom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 fio.new_plus(row[0]);
fprintf(ff,"%s:%d %s\n",gettext("Работник"),tabnom,fio.ravno());

class iceb_u_str liniq("--------");
for(int nom=0; nom <= svod_otvr->kod.kolih(); nom++)
 liniq.plus("---------------");

fprintf(ff,"%s\n",liniq.ravno());
 
fprintf(ff,"\
%-*.*s|",
iceb_u_kolbait(7,gettext("Дата")),
iceb_u_kolbait(7,gettext("Дата")),
gettext("Дата"));
int max_dl_str=0;

for(int nom=0; nom < svod_otvr->kod.kolih(); nom++)
 {
  sprintf(strsql,"select naik from Tabel where kod=%d",svod_otvr->kod.ravno(nom));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   fio.new_plus(row[0]);
  else
   fio.new_plus("");

  sprintf(strsql,"%d %s",svod_otvr->kod.ravno(nom),fio.ravno());

  fprintf(ff,"%-*.*s|",
  iceb_u_kolbait(14,strsql),
  iceb_u_kolbait(14,strsql),
  strsql);

  if(max_dl_str < iceb_u_strlen(strsql))
   max_dl_str=iceb_u_strlen(strsql);
 }

fprintf(ff,"%-*.*s|\n",
iceb_u_kolbait(14,gettext("Итого")),
iceb_u_kolbait(14,gettext("Итого")),
gettext("Итого"));

/*Печать продолжения наименования*/
int kol_str=max_dl_str/14;
//printw("\n%s-%d %d\n",__FUNCTION__,kol_str,max_dl_str);
//OSTANOV();

for(int nom_str=0; nom_str < kol_str;nom_str++)
 {
  fprintf(ff,"%7s|","");
  for(int nom=0; nom < svod_otvr->kod.kolih(); nom++)
   {
    sprintf(strsql,"select naik from Tabel where kod=%d",svod_otvr->kod.ravno(nom));
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     fio.new_plus(row[0]);
    else
     fio.new_plus("");

    sprintf(strsql,"%d %s",svod_otvr->kod.ravno(nom),fio.ravno());

    fprintf(ff,"%-*.*s|",
    iceb_u_kolbait(14,iceb_u_adrsimv((nom_str+1)*14,strsql)),
    iceb_u_kolbait(14,iceb_u_adrsimv((nom_str+1)*14,strsql)),
    iceb_u_adrsimv((nom_str+1)*14,strsql));
   }  
  fprintf(ff,"%14s|\n","");
 }

/*Строка подчёркивания после наименования*/
fprintf(ff,"%7s|","");

for(int nom=0; nom <= svod_otvr->kod.kolih(); nom++)
 {
  fprintf(ff,"--------------|");
 }
fprintf(ff,"\n");

fprintf(ff,"%7s|","");
for(int nom=0; nom <= svod_otvr->kod.kolih(); nom++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|%-*.*s|",
  iceb_u_kolbait(4,gettext("Рабочие")),
  iceb_u_kolbait(4,gettext("Рабочие")),
  gettext("Рабочие"),
  iceb_u_kolbait(4,gettext("Календарные")),
  iceb_u_kolbait(4,gettext("Календарные")),
  gettext("Календарные"),
  iceb_u_kolbait(4,gettext("Часы")),
  iceb_u_kolbait(4,gettext("Часы")),
  gettext("Часы"));
 }
fprintf(ff,"\n");

fprintf(ff,"%s\n",liniq.ravno());
int nom_vs=0;

for(int nom_mes=0; nom_mes < svod_otvr->mes_god.kolih(); nom_mes++)
 {

  fprintf(ff,"%s|",svod_otvr->mes_god.ravno(nom_mes));

  double itogo_str[3];
  memset(itogo_str,'\0',sizeof(itogo_str));
  for(int nom_kod=0; nom_kod < svod_otvr->kod.kolih(); nom_kod++)
   {
    sprintf(strsql,"%d|%s",svod_otvr->kod.ravno(nom_kod),svod_otvr->mes_god.ravno(nom_mes));

    if((nom_vs=svod_otvr->kod_mes_god.find(strsql)) >= 0)
     {
      fprintf(ff,"%4d|%4d|%4.4g|",svod_otvr->kolrd.ravno(nom_vs),svod_otvr->kolkd.ravno(nom_vs),svod_otvr->kolhs.ravno(nom_vs));
      itogo_str[0]+=svod_otvr->kolrd.ravno(nom_vs);
      itogo_str[1]+=svod_otvr->kolkd.ravno(nom_vs);
      itogo_str[2]+=svod_otvr->kolhs.ravno(nom_vs);
     }
    else
     fprintf(ff,"%4s|%4s|%4s|","","","");
   }
  fprintf(ff,"%4.f|%4.f|%4.4g|\n",itogo_str[0],itogo_str[1],itogo_str[2]);
 }

fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*.*s|",
iceb_u_kolbait(7,gettext("Итого")),
iceb_u_kolbait(7,gettext("Итого")),
gettext("Итого"));

for(int nom_kod=0; nom_kod < svod_otvr->kod.kolih(); nom_kod++)
 {
  double itogo_str[3];
  memset(itogo_str,'\0',sizeof(itogo_str));
  for(int nom_mes=0; nom_mes < svod_otvr->mes_god.kolih(); nom_mes++)
   {
    sprintf(strsql,"%d|%s",svod_otvr->kod.ravno(nom_kod),svod_otvr->mes_god.ravno(nom_mes));
    if((nom_vs=svod_otvr->kod_mes_god.find(strsql)) >= 0)
     {
      itogo_str[0]+=svod_otvr->kolrd.ravno(nom_vs);
      itogo_str[1]+=svod_otvr->kolkd.ravno(nom_vs);
      itogo_str[2]+=svod_otvr->kolhs.ravno(nom_vs);
     }
   }  
  fprintf(ff,"%4.f|%4.f|%4.4g|",itogo_str[0],itogo_str[1],itogo_str[2]);
  
 }

fprintf(ff,"%4.d|%4.d|%4.4g|\n",svod_otvr->kolrd.suma(),svod_otvr->kolkd.suma(),svod_otvr->kolhs.suma());
//fprintf(ff,"%10.2f|\n",svod_otvr->suma.suma());


}
