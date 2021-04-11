/* $Id: zaravpr_oldw.c,v 1.13 2014/03/31 06:05:33 sasa Exp $ */
/*30.03.2016	23.09.1998	Белых А.И.	zaravpr_oldw.c
Выполнение проводок для зароботной платы
*/
#include <math.h>
#include        "buhg_g.h"

void zaravprf(int,short,short,class SQLCURSOR *cur_alx,short*,class iceb_u_spisok*,class iceb_u_double*,FILE*,GtkWidget*);

extern short   *kodbl;  /*Код начисления больничного*/
extern class iceb_u_str kod_dop_nah_bol; /*коды начилений дополнительно входящие в расчёт удержания ЕСВ с больничного*/
extern short    *knnf; //Коды начислений недежными формами 
extern char	*shetb; /*Бюджетные счета*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern double   okrg; /*Округление*/
extern char	*mprov; /*Массив проводок*/
extern double	*mdk;   /*Массив дебетов и кредитов к масиву проводок*/
extern short	kodpenf; /*Код пенсионного фонда*/
extern short	kodbezf;  /*Код фонда занятости*/
extern short	kodsoc;   /*Код фонда социального страхования*/
extern short    *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/
extern class iceb_u_str shrpz; /*Счет расчётов по зарплате*/
extern class iceb_u_str	shrpzbt; /*Счет расчётов по зарплате бюджет*/
extern class iceb_u_spisok dop_zar_sheta; //Дополнительные зарплатные счёта
extern class iceb_u_spisok dop_zar_sheta_spn; /*Списки начислений для каждого дополнительного зарплатного счёта*/

extern int kekv_at;  //Код экономической классификации для атестованых
extern int kekv_nat; //Код экономической классификации для не атестованых
extern int kodzv_nat;//Код звания неатестованный
extern int kekv_nfo; //Код экономической классификации для начислений на фонд оплаты
extern class iceb_u_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/


extern SQL_baza bd;

void zaravpr_oldw(int tbn,short mp,short gp,FILE *ff_prot,GtkWidget *wpredok)
{
short	avkvs=1; //0-Не вставлять автоматом код контрагента 1-вставлять
class iceb_u_str nn("");
short		kp;
short		d,m,g;
short		dd,md,gd;
class iceb_u_str kom("");
double		deb,kre;
class iceb_u_str kor21("");
class iceb_u_str kontshzar("");
int		i,i1;
time_t		vrem;
double		sym;
short		prn;
class iceb_u_str sh("");
class iceb_u_str shet("");
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
class iceb_u_str kontr1("");
short		kodnu; /*Код начисления/удержания*/
class iceb_u_str koresp("");
class iceb_u_str bros("");
int		metpr; /*1-приход 2 расход*/
short		knah;
class iceb_u_str kodgni("");
int		polekor; //Номер поля с кореспондирующими счетами
class iceb_u_str koment("");
struct OPSHET	shetv;
int		kolpr=2;
class iceb_u_str shetzar("");
SQLCURSOR curr;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kekv=0;
class iceb_u_spisok sp_prov;
class iceb_u_double sum_prov_dk;

if(ff_prot != NULL)
  fprintf(ff_prot,"%s-%d.%d tbn=%d\n",__FUNCTION__,mp,gp,tbn);

if(shrpz.ravno()[0] == '\0')
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Не введён зарплатный счёт в файле настройки! Проводки не выполняються!\n",__FUNCTION__);
  return;
 }



if(iceb_pvglkni(mp,gp,wpredok) != 0)
 return;

gd=gp;
md=mp;

//sprintf(nn,"%d-%d",mp,tbn);
nn.new_plus(mp);
nn.plus("-");
nn.plus(tbn);

iceb_u_dpm(&dd,&md,&gd,5);

  
//Проводки удаляем сразу так, как можно обнулить все начисления и удержания
if(iceb_udprgr(ICEB_MP_ZARP,dd,md,gd,nn.ravno(),0,0,wpredok) != 0)
 return;

sprintf(strsql,"select datz,suma,shet,prn,knah,kom from Zarp where \
datz >='%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and suma != 0.",
gp,mp,gp,mp,tbn);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
/*
printw("kolstr=%d\n",kolstr);
OSTANOV();
*/
if(kolstr == 0)
  return;

if(kodzv_nat != 0 && (kekv_at != 0 || kekv_nat != 0)) //Узнаём звание и определяем kekv
 {
  sprintf(strsql,"select zvan from Zarn where tabn=%d and god=%d and mes=%d",tbn,gp,mp);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
   {
    if(atoi(row1[0]) == kodzv_nat)
     kekv=kekv_nat;
    else
     kekv=kekv_at;
    
   }   
  else
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не нашли звание !!!\n");
   }
 }

/*Открываем файл настроек*/
sprintf(strsql,"select str from Alx where fil='zarnpr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Не найдены настройки zarnpr.alx\n",__FUNCTION__);
  return;
 }

class iceb_u_str kontr_baz(ICEB_ZAR_PKTN);
//sprintf(kontr_baz,"%s%d",ICEB_ZAR_PKTN,tbn);
kontr_baz.plus(tbn);
kontshzar.new_plus(kontr_baz.ravno());
if(iceb_provsh(&kontshzar,shrpz.ravno(),&shetv,avkvs,0,wpredok) != 0)
   return;
   
kontshzar.new_plus(kontr_baz.ravno());
if(shrpzbt.ravno()[0] != '\0')
 if(iceb_provsh(&kontshzar,shrpzbt.ravno(),&shetv,avkvs,0,wpredok) != 0)
   return;


kp=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  //Проверяем нужно ли делать проводки для этого начисления/удержания
  prn=atoi(row[3]);  

  if(row[2][0] == '\0')
   {
    if(prn == 1)
     sprintf(strsql,"%s %d %s %s!\n",gettext("Табельный номер"),tbn,gettext("Не введён счёт для начисления"),row[4]);
    if(prn == 2)
     sprintf(strsql,"%s %d %s %s!\n",gettext("Табельный номер"),tbn,gettext("Не введён счёт для удержания"),row[4]);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }

  if(prn == 2) //Удержания
    sprintf(strsql,"select prov from Uder where kod=%s",row[4]);
  if(prn == 1) //Начисления
    sprintf(strsql,"select prov from Nash where kod=%s",row[4]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    if(row1[0][0] == '1')
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"Для кода %s проводки выключены!\n",row[4]);
      continue;
     }
  sym=atof(row[1]);
  if(fabs(sym) < 0.01)
    continue;

  sh.new_plus(row[2]);


//  printw("\nsh=%s\n",sh);
  
  shetzar.new_plus("");
  if(prn == 1)
   {
    if(iceb_u_proverka(shetb,sh.ravno(),0,1) == 0)
     {
      if(shrpzbt.ravno()[0] != '\0')
       shetzar.new_plus(shrpzbt.ravno());   
      else
       shetzar.new_plus(shrpz.ravno());   
     }
    else
      shetzar.new_plus(shrpz.ravno());   
   }
  else
   {
    if(iceb_u_proverka(shetbu,sh.ravno(),0,1) == 0)
     {
      if(shrpzbt.ravno()[0] != '\0')
       shetzar.new_plus(shrpzbt.ravno());   
      else
       shetzar.new_plus(shrpz.ravno());   
     }
    else
     shetzar.new_plus(shrpz.ravno());   
   }
    
  knah=atoi(row[4]);
  iceb_u_rsdat(&d,&m,&g,row[0],2);  

  deb=sym;
  if(prn == 2)
    deb*=(-1);
  kre=0.;
  kom.new_plus(row[5]);

  kontr1.new_plus("");
  kor21.new_plus("");


  
  class iceb_u_str  shet_zar_tek(""); //Счёт зарплаты, который в конечном итоге будет использоваться в проводках
  class iceb_u_str kontr_shet_zar_tek(""); //Код контрагента для счёта зарплаты
    
  //Обязательно тут это копирование должно быть так как настроек в файле может и не быть
  shet_zar_tek.new_plus(shetzar.ravno());
  kontr_shet_zar_tek.new_plus(kontshzar.ravno());

  //Проверяем может это начисление для которого нужно делать проводки по дополнительному зарплатному счёту
  if(prn == 1)
   for(int ii=0; ii < dop_zar_sheta_spn.kolih(); ii++)
   if(iceb_u_proverka(dop_zar_sheta_spn.ravno(ii),knah,0,1) == 0)
    {
     shet_zar_tek.new_plus(dop_zar_sheta.ravno(ii));
     kontr_shet_zar_tek.new_plus(kontr_baz.ravno());
     if(iceb_provsh(&kontr_shet_zar_tek,shet_zar_tek.ravno(),&shetv,avkvs,0,wpredok) != 0)
       continue;

    }  
  /*Читаем файл настройки*/
  cur_alx.poz_cursor(0);
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    
//    printw("%s",row_alx[0]);
    if(row_alx[0][0] == '#' || row_alx[0][0] == 'f')
        continue;
    
    if(prn == 1 && row_alx[0][0] == '-')
     continue;
    if(prn == 2 && row_alx[0][0] == '+')
     continue;
     
    if(iceb_u_polen(row_alx[0],&bros,2,'|') != 0)
       continue;
    kodnu=bros.ravno_atoi();
      if(kodnu == 0 || kodnu != knah)
       continue;
    int nast_tn=0;
    if(iceb_u_polen(bros.ravno(),&nast_tn,2,':') == 0)
       {
        if(nast_tn != 0 && nast_tn != tbn)
         continue;
       }  
    if(iceb_u_polen(bros.ravno(),strsql,sizeof(strsql),3,':') == 0) //Введён счёт вместо зарплатного счёта (661)
     {
      if(strsql[0] != '\0')
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"Введён счёт вместо зарплатного счёта=%s\n",strsql);
        shet_zar_tek.new_plus(strsql);
        
        if(iceb_provsh(&kontr_shet_zar_tek,shet_zar_tek.ravno(),&shetv,avkvs,0,wpredok) != 0)
         continue;
        shet_zar_tek.new_plus(shet_zar_tek.ravno());
       }
     }
    polekor=4;
    // Смотрим настройку на комментарий
    if(iceb_u_polen(bros.ravno(),strsql,sizeof(strsql),2,'/') == 0)
     {
      if(iceb_u_polen(strsql,&koment,1,':') != 0) 
       koment.new_plus(strsql);
      if(koment.ravno()[0] != '\0')
       {
        if(iceb_u_strstrm(row[5],koment.ravno()) != 0)
          polekor++;
       }
     }
      iceb_u_pole(row_alx[0],&bros,1,'|');
      i1=strlen(bros.ravno());
      metpr=0;
      for(i=0; i < i1; i++)
       {
        if(bros.ravno()[i] == '+')
         {
          metpr=1;
          break;
         }
        if(bros.ravno()[i] == '-')
         {
          metpr=2;
          break;
         }
       }

      if(metpr == 0 || metpr != prn)
       continue;


      /*Берем счет*/
      iceb_u_pole(row_alx[0],&bros,3,'|');

      if(iceb_u_pole(bros.ravno(),&shet,1,':') != 0)
       shet.new_plus(bros.ravno());
      else
       iceb_u_polen(bros.ravno(),&kontr1,2,':');

      if(shet.ravno()[0] == '\0' || iceb_u_strstrm(shet.ravno(),"*") == 1) /*Если счёт не введён то подходит для всех счетов этого начисления/удержания*/
       shet.new_plus(sh.ravno());
      
      if(iceb_u_SRAV(shet.ravno(),sh.ravno(),1) != 0)
        continue;

      if(kontr1.ravno()[0] != '\0')
        kontr_shet_zar_tek.new_plus(kontr1.ravno());
      else       
        kontr_shet_zar_tek.new_plus(kontr_baz.ravno());

      if(ff_prot != NULL)
       fprintf(ff_prot,"2проверяем контрагент %s счёт %s\n",kontshzar.ravno(),shet.ravno());
      if(iceb_provsh(&kontr_shet_zar_tek,shet.ravno(),&shetv,avkvs,0,wpredok) != 0)
         continue;

      /*Берем кореспонденцию*/
      iceb_u_polen(row_alx[0],&koresp,polekor,'|');
      if(koresp.ravno()[0] == '\0')
       continue;

//    printw("\n%s",row_alx[0]);
      

      i1=iceb_u_pole2(koresp.ravno(),',');
      for(i=0; i <= i1; i++)
       {

        if(i1 > 0)
         iceb_u_pole(koresp.ravno(),&bros,i+1,',');
        else
         bros.new_plus(koresp.ravno());      

        if(i1 > 0 && i == 0)
         i1--;
         
        /*Проверяем есть ли код контрагента*/
        if(iceb_u_pole(bros.ravno(),&shet,1,':') != 0)
         shet.new_plus(bros.ravno());
        else
         iceb_u_polen(bros.ravno(),&kor21,2,':');

        if(shet.ravno()[0] == '\0')
         continue;
        if(kor21.getdlinna() <= 1)
          kor21.new_plus(kontr_baz.ravno());
        if(iceb_provsh(&kor21,shet.ravno(),&shetv,avkvs,1,wpredok) != 0)
           continue;
/****************
         1я проводка 
           делаем одиночную проводку и переходим на просмотр следующего 
           начисления/удержания
         Nя проводка
           делаем одиночную проводку и следующий в цепочке счёт должен 
           кореспонди
********************/
        kolpr=2;
        if(shetv.stat == 1)
          kolpr=1;
/**********
        printw("shet=%s shet_zar_tek=%s kor21=%s kontr_shet_zar_tek=%s deb=%f kre=%f\n",
        shet,shet_zar_tek,kor21,kontr_shet_zar_tek,deb,kre);
        OSTANOV();        
***************/
        if(prn == 1)
         {
          if(kolpr == 2)
            iceb_zapmpr(g,m,d,shet.ravno(),shet_zar_tek.ravno(),kor21.ravno(),kontr_shet_zar_tek.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
          if(kolpr == 1)
            iceb_zapmpr(g,m,d,shet.ravno(),"",kor21.ravno(),kontr_shet_zar_tek.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
          
         }
        if(prn == 2)
         {
          if(kolpr == 2)
           iceb_zapmpr(g,m,d,shet_zar_tek.ravno(),shet.ravno(),kontr_shet_zar_tek.ravno(),kor21.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
          if(kolpr == 1)
           iceb_zapmpr(g,m,d,"",shet.ravno(),kontr_shet_zar_tek.ravno(),kor21.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
        if(kolpr == 2)
         {
          shet_zar_tek.new_plus(shet.ravno());
          kontr_shet_zar_tek.new_plus(kor21.ravno());
         }
        kor21.new_plus("");
  
       }
// Настройка может быть только одна
      break;
     }
  
  if(kontr1.ravno()[0] != '\0')
    kor21.new_plus(kontr1.ravno());
#if 0
##############################################################3333
  if(iceb_u_SRAV(sh.ravno(),shpn,0) == 0)
   {
    /*Читаем код города налоговой инспекции*/
    sprintf(strsql,"select kodg from Kartb where tabn=%d",tbn);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tbn);
      iceb_menu_soob(strsql,wpredok);
      continue;      
     }    

    /*Читаем код контрагента*/
    kodgni.new_plus("");
    if(atoi(row1[0]) != 0)
     {
      kodgni.new_plus(row1[0]);
      
      sprintf(strsql,"select kontr from Gnali where kod=%s",kodgni.ravno());
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        class iceb_u_str repl;
               
        sprintf(strsql,"%s %s !",
        gettext("Не найден код города налоговой инспекции"),kodgni.ravno());
        repl.plus(strsql);
        
        sprintf(strsql,"%s:%d !",
        gettext("Табельный номер"),tbn);
        repl.ps_plus(strsql);
        
        iceb_menu_soob(&repl,wpredok);
        continue;      
       }
      else
       kor21.new_plus(row1[0]);
     }
   }
################################################################
#endif
  if(kor21.getdlinna() <= 1)
    kor21.new_plus(kontr_baz.ravno());
  
  
  if(iceb_provsh(&kor21,sh.ravno(),&shetv,avkvs,1,wpredok) != 0)
     continue;
  kolpr=2;
  if(shetv.stat == 1)
    kolpr=1;

  if(prn == 1)
   {
/********
    printw("+*sh=%s shet_zar_tek=%s kor21=%s kontr_shet_zar_tek=%s deb=%f kre=%f\n",
    sh,shet_zar_tek,kontr_shet_zar_tek,kor21,deb,kre);
    OSTANOV();
*************/
    if(kolpr == 2)
      iceb_zapmpr(g,m,d,sh.ravno(),shet_zar_tek.ravno(),kor21.ravno(),kontr_shet_zar_tek.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
    if(kolpr == 1)
      iceb_zapmpr(g,m,d,sh.ravno(),"",kor21.ravno(),kontr_shet_zar_tek.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
   }
  if(prn == 2)
   {
/************
    printw("-*shet=%s shet_zar_tek=%s kor1=%s kor2=%s deb=%f kre=%f\n",
    sh,shet_zar_tek,kontr_shet_zar_tek,kor21,deb,kre);
    OSTANOV();
*************/
    if(kolpr == 2)
      iceb_zapmpr(g,m,d,shet_zar_tek.ravno(),sh.ravno(),kontr_shet_zar_tek.ravno(),kor21.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
    if(kolpr == 1)
      iceb_zapmpr(g,m,d,"",sh.ravno(),kontr_shet_zar_tek.ravno(),kor21.ravno(),deb,kre,kom.ravno(),kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
   }
 }



/*Выполнение проводок по соцстаху*/
if(iceb_u_sravmydat(1,mp,gp,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Проводки на фонды до 1.1.2011 не выполняются !!!\n");
 }
else
 zaravprf(tbn,mp,gp,&cur_alx,&kp,&sp_prov,&sum_prov_dk,ff_prot,wpredok);


kor21.new_plus(ICEB_MP_ZARP);
time(&vrem);

sprintf(strsql,"LOCK TABLES Prov WRITE,Blok READ,icebuser READ,Plansh READ");
class iceb_lock_tables lock(strsql);

/*Запись проводок из памяти в базу*/
iceb_zapmpr1(nn.ravno(),"",0,vrem,kor21.ravno(),dd,md,gd,0,&sp_prov,&sum_prov_dk,ff_prot,wpredok);
}

/**************************************/
/*Расчёт проводок по социальным фондам начиная с 1.1.2011*/
/**************************************/

void zaravprf(int tbn,short mp,short gp,class SQLCURSOR *cur_alx,
short *kolprov,
class iceb_u_spisok *sp_prov,
class iceb_u_double *sum_prov_dk,
FILE *ff_prot,
GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row,row2;
char		strsql[1024];
class iceb_u_str kor11("");
class iceb_u_str kor21("");;
class iceb_u_str kontr1("");
class iceb_u_str kontsocsh("");
class iceb_u_str kontsocsh1("");
class iceb_u_str kontsocsh2("");
short		kodz;
double		sum,sumi,sumas,sumap;
class iceb_u_str socshet(""); /*Соц. счёт если он один*/
class iceb_u_str socshet1(""); /*Соц. счёт не бюджетный*/
class iceb_u_str socshet2(""); /*Соц. счёт бюджетный*/
class iceb_u_str nahshet("");
short		d,m,g;
class iceb_u_str koment(""),koment1("");
double		deb,kre;
class iceb_u_str shet1(""),shet("");
class iceb_u_str bros("");
class iceb_u_str koresp("");
int		i,i1;
class iceb_u_str kodnv("");
int		polekor; //Номер поля с кореспондирующими счетами
struct OPSHET   shetv;
int		kolpr=2;
double suma_for_esv=0.; /*Сумма начислений с которой берётся единый социальный взнос*/
double suma_for_esv_bol=0.; /*Cумма начислений больничного для единого социального взноса*/
SQLCURSOR curr;
class iceb_u_str shet_zamena("");
int kekv=kekv_nfo;
SQL_str row_alx;
short	avkvs=1; //0-Не вставлять автоматом код контрагента 1-вставлять
int metka_bol=0;

if(ff_prot != NULL)
 fprintf(ff_prot,"\nВыполнение проводок на фонд зарплаты.\nkekv=%d\n",kekv);
 
class iceb_u_double SUMA;
class iceb_u_spisok NAHIS;
if(zaravpt_osw(tbn,mp,gp,&NAHIS,&SUMA,wpredok) != 0)
 return;
int kodf_esv_bol=0;
if(iceb_poldan("Код фонда единого социального взноса для больничных",&kodf_esv_bol,"zarnast.alx",wpredok) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден Код фонда единого социального взноса для больничных!\n");
 }
int kodf_esv_bol_inv=0;
if(iceb_poldan("Код фонда единого социального взноса с больничных для инвалидов",&kodf_esv_bol_inv,"zarnast.alx",wpredok) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден Код фонда единого социального взноса с больничных для инвалидов!\n");
 }

/*Смотрим записи социальных отчислений*/
sprintf(strsql,"select kodz,shet,sumas,sumap,sumapb from Zarsocz where \
datz='%04d-%d-01' and tabn=%d",gp,mp,tbn);
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
/*
printw("kolstr=%d\n",kolstr);
OSTANOV();
*/
if(kolstr <= 0)
  return;

int kolzap=NAHIS.kolih();
/*В соответцтвии с законом Украины минимальная сумма с которой может начислятся ЕСВ это минимальная зарплата
Если сумма начисления меньше чем та с которой начислено ЕСВ то в расчёт нужно брать её
Определяем сумму болничных и сумму всех остальных начислений
*/
for(int nom=0; nom < kolzap; nom++)
 {
  iceb_u_polen(NAHIS.ravno(nom),&kodz,3,'|');  
  if(iceb_u_proverka(knvr_esv_r.ravno(),kodz,0,1) == 0)
   continue;

  sum=SUMA.ravno(nom);

  if(provkodw(kodbl,kodz) >= 0)
   {   
    suma_for_esv_bol+=sum;
//    if(ff_prot != NULL)
//     fprintf(ff_prot,"%s-1 больничный %d %.2f\n",__FUNCTION__,kodz,sum);
    continue;
   }
  if(iceb_u_proverka(kod_dop_nah_bol.ravno(),kodz,0,1) == 0)
   {
    suma_for_esv_bol+=sum;
//    if(ff_prot != NULL)
//     fprintf(ff_prot,"%s-2 больничный %d %.2f-%s\n",__FUNCTION__,kodz,sum,kod_dop_nah_bol.ravno());
    continue;
   }   
  suma_for_esv+=sum;
 }
if(ff_prot != NULL)
 fprintf(ff_prot,"%s-Сумма больничных %.2f Сумма остальных начислений %.2f\n",__FUNCTION__,suma_for_esv_bol,suma_for_esv);

while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"\nЗапись фонда-%s %s %s %s %s\n\
-----------------------------------------------------------\n",
    row[0],row[1],row[2],row[3],row[4]);

  kodz=atoi(row[0]);
  metka_bol=0;
  if(kodz == kodf_esv_bol || kodz == kodf_esv_bol_inv) /*фонд для больничного нужно смотреть только больничные*/
   metka_bol=1;

  socshet2.new_plus("");
  socshet.new_plus(row[1]);
  if(iceb_u_polen(socshet.ravno(),&socshet1,1,',') == 0)
     iceb_u_polen(socshet.ravno(),&socshet2,2,',');

  sumas=atof(row[2]);
  sumap=atof(row[3]);


  /*Берем коды не входящие в расчёт*/
  sprintf(strsql,"select kodn from Zarsoc where kod=%d\n",kodz);
  if(sql_readkey(&bd,strsql,&row2,&curr) != 1)
   {
    sprintf(strsql,"%s-%s %d !",__FUNCTION__,gettext("Не найден код соц. страхования"),kodz);
    iceb_menu_soob(strsql,wpredok);
    kodnv.new_plus("");
   }  
  else
   kodnv.new_plus(row2[0]);
   
  sprintf(strsql,"%s%d",ICEB_ZAR_PKTN,tbn);
  kontsocsh.new_plus(strsql);
  if(socshet1.ravno()[0] == '\0' && socshet2.ravno()[0] == '\0')
   if(iceb_provsh(&kontsocsh,socshet.ravno(),&shetv,avkvs,0,wpredok) != 0)
     return;

  if(socshet1.ravno()[0] != '\0')
   {
    sprintf(strsql,"%s%d",ICEB_ZAR_PKTN,tbn);
    kontsocsh1.new_plus(strsql);
    if(iceb_provsh(&kontsocsh1,socshet1.ravno(),&shetv,avkvs,0,wpredok) != 0)
     return;
   }
  if(socshet2.ravno()[0] != '\0')
   {
    sprintf(strsql,"%s%d",ICEB_ZAR_PKTN,tbn);
    kontsocsh2.new_plus(strsql);
    if(iceb_provsh(&kontsocsh2,socshet2.ravno(),&shetv,avkvs,0,wpredok) != 0)
     return;
   }
  /*Повторно*/
  kor11.new_plus("");
  kor21.new_plus("");
  sumi=0.;

  for(int nomz=0; nomz < kolzap; nomz++)
   {

    if(fabs(sumi) > fabs(sumas))
      break;

     if(sumas < 0. && sumi < sumas)
      break;

    class iceb_u_str kodnah("");
    char dataz[16];
    iceb_u_polen(NAHIS.ravno(nomz),&kodnah,3,'|');

    if(metka_bol == 0) /*если фонд не больничный то больничные пропускаем*/
     {
      if(provkodw(kodbl,kodnah.ravno_atoi()) >= 0 || iceb_u_proverka(kod_dop_nah_bol.ravno(),kodnah.ravno(),0,1) == 0)
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"Код начисления %s Фонд не больничный - пропускаем\n",kodnah.ravno());
        continue;
       }
      if(suma_for_esv < sumas)
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"%s-Сумма всех начислений меньше чем сумма с которой начислялся ЕСВ %.2f < %.2f\nБерём в расчёт меньшую сумму\n",__FUNCTION__,suma_for_esv,sumas);
        sumas=suma_for_esv; 
       }

     }

    if(metka_bol == 1) /*если фонд больничный то не больничные пропускаем*/
     {
      if(provkodw(kodbl,kodnah.ravno_atoi()) < 0 && iceb_u_proverka(kod_dop_nah_bol.ravno(),kodnah.ravno(),0,1) != 0)
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"Код начисления %s Фонд больничный - пропускаем %s\n",kodnah.ravno(),kod_dop_nah_bol.ravno());
        continue;
       }
      if(suma_for_esv_bol < sumas)
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"%s-Сумма всех начислений меньше чем сумма с которой начислялся ЕСВ %.2f < %.2f\nБерём в расчёт меньшую сумму\n",__FUNCTION__,suma_for_esv_bol,sumas);
        sumas=suma_for_esv_bol; 
       }
     }
    sum=SUMA.ravno(nomz);

    if(ff_prot != NULL)
      fprintf(ff_prot,"%s = %.2f\n",NAHIS.ravno(nomz),sum);

    iceb_u_polen(NAHIS.ravno(nomz),dataz,sizeof(dataz),1,'|');

    /*Проверяем на коды не входящие в расчёт */
    if(kodnv.ravno()[0] != '\0')
     if(iceb_u_pole1(kodnv.ravno(),kodnah.ravno(),',',0,&i1) == 0 ||
      iceb_u_SRAV(kodnv.ravno(),kodnah.ravno(),0) == 0)
       continue;

    iceb_u_rsdat(&d,&m,&g,dataz,2);


    iceb_u_polen(NAHIS.ravno(nomz),&nahshet,2,'|');
    iceb_u_polen(NAHIS.ravno(nomz),&koment,4,'|');
    kre=0.;

    if(fabs(sumi+sum) <= fabs(sumas))
     deb=sum;
    else
     {
      deb=sumas-sumi;
     }
    sumi+=sum;
    
//    printw("sumas=%f sumap=%f deb=%f\n",sumas,sumap,deb);
    if(ff_prot != NULL)
     fprintf(ff_prot,"Сумма начисления %.2f\n\
Вычисляем сумму взноса которая приходится на эту сумму\n\
%.2f*%.2f/%.2f=%.2f\n",
      deb, 
      deb,sumap,sumas,
      deb*sumap/sumas);
      
    deb=deb*sumap/sumas;
   
    //не округляем округлять будем при выполнении проводок
//    deb=okrug(deb,okrg);         

//    printw("deb=%f\n",deb);
//    OSTANOV();
         
    kontr1.new_plus("");
    kor21.new_plus("");

    shet1.new_plus(socshet.ravno());
    
    kor11.new_plus(kontsocsh.ravno());
    if(socshet1.ravno()[0] != '\0')
     {
      shet1.new_plus(socshet1.ravno());
      kor11.new_plus(kontsocsh1.ravno());
     }   
    if(socshet2.ravno()[0] != '\0' && shetb != NULL)
      if(iceb_u_pole1(shetb,nahshet.ravno(),',',0,&i1) == 0 || iceb_u_SRAV(shetb,nahshet.ravno(),0) == 0)
       {
        shet1.new_plus(socshet2.ravno());
        kor11.new_plus(kontsocsh2.ravno());
       }
    if(fabs(deb) < 0.01)
     continue;

    shet_zamena.new_plus(""); /*Только здесь*/

    /*Читаем файл настройки*/
    cur_alx->poz_cursor(0);
    while(cur_alx->read_cursor(&row_alx) != 0)
     {
      
      if(row_alx[0][0] == '#')
        continue;        
      if(row_alx[0][0] == '+')
        continue;        
      if(row_alx[0][0] == '-')
        continue;        
      if(iceb_u_polen(row_alx[0],&bros,1,'|') != 0)
       continue;
      if(iceb_u_strstrm(bros.ravno(),"f") == 0)
       continue;

      /*Берем код*/
      iceb_u_polen(row_alx[0],&bros,2,'|');
      if(iceb_u_atof(bros.ravno()) != kodz)
        continue;         

      /*Соц счёт который будет использоваться в проводке - в настройке проводок он может быть заменён*/
//      class iceb_u_str soc_shet_prov(shet1); 

      polekor=4;
      // Смотрим настройку на комментарий
      if(iceb_u_polen(bros.ravno(),strsql,sizeof(strsql),2,'/') == 0)
       {
        if(iceb_u_polen(strsql,&koment1,1,':') != 0)
         koment1.new_plus(strsql);
        if(koment1.ravno()[0] != '\0')
         {
          if(iceb_u_strstrm(koment.ravno(),koment1.ravno()) != 0)
            polekor++;
         }
       }     
      /*Смотрим табельный номер*/
      if(iceb_u_polen(bros.ravno(),strsql,sizeof(strsql),2,':') == 0)
       {
        if(iceb_u_proverka(strsql,tbn,0,0) != 0)
          continue;
       }

      /*Смотрим счёт в замену соц. счёта*/
      if(iceb_u_polen(bros.ravno(),strsql,sizeof(strsql),3,':') == 0)
       {
        if(strsql[0] != '\0')
         {
          shet1.new_plus(strsql);
          if(ff_prot != NULL)
           fprintf(ff_prot,"Заменили соц. счёт на %s\n",strsql);
          if(iceb_provsh(&kor11,shet1.ravno(),&shetv,avkvs,0,wpredok) != 0)
              continue;
         }
       }
      /*Берем счет*/
      iceb_u_polen(row_alx[0],&bros,3,'|');
              
      if(iceb_u_polen(bros.ravno(),&shet,1,':') != 0)
       {
        shet.new_plus(bros.ravno());
       }
      else
       {
        iceb_u_polen(bros.ravno(),&kontr1,2,':');
        //Проверяем для какого кода начисления настройка
        class iceb_u_str kod_nah_nast("");
        if(iceb_u_polen(bros.ravno(),&kod_nah_nast,4,':') == 0)
         if(iceb_u_proverka(kod_nah_nast.ravno(),kodnah.ravno(),0,0) != 0)
          continue;
       }        

      if(shet.ravno()[0] == '\0' || iceb_u_strstrm(shet.ravno(),"*") == 1) /*Если счёт не введён то подходит*/
       shet.new_plus(nahshet.ravno());
        
       
      if(iceb_u_SRAV(shet.ravno(),nahshet.ravno(),1) != 0)
       {
        kontr1.new_plus("");
        continue;
       }

      if(ff_prot != NULL)
       {
        fprintf(ff_prot,"%s",row_alx[0]);
        fprintf(ff_prot,"Обнаружили настройкy на счёт-%s код контрагента=%s\n",shet.ravno(),kontr1.ravno());
       }         
     //настройка на замену одного счета другим
     //делаем это только после сравнения счетов
     iceb_u_polen(bros.ravno(),&shet_zamena,3,':');  // только здесь
     if(shet_zamena.ravno()[0] == '*')
      {
       SQLCURSOR cursh;
       SQL_str   rowsh;
       //взять счёт из карточки
       sprintf(strsql,"select shet from Zarn where tabn=%d and god=%d and mes=%d",tbn,gp,mp);
       if(sql_readkey(&bd,strsql,&rowsh,&cursh) != 1)
        {
         class iceb_u_str repl;
         sprintf(strsql,"%s-Не найдена настройка по дате %d.%d для определения счета в карточке !",
         __FUNCTION__,mp,gp);
         repl.plus(strsql);
         
         sprintf(strsql,"Табельный номер %d",tbn);
         repl.ps_plus(strsql);
         iceb_menu_soob(&repl,wpredok);
        }
       else
        shet_zamena.new_plus(rowsh[0]);
      }
       
      /*Берем кореспонденцию*/
      iceb_u_polen(row_alx[0],&koresp,polekor,'|');
      if(koresp.ravno()[0] == '\0')
       continue;
      if(ff_prot != NULL)
        fprintf(ff_prot,"Строка кореспондирующих счетов = %s\n",koresp.ravno());

      i1=iceb_u_pole2(koresp.ravno(),',');
      for(i=0; i <= i1; i++)
       {
         
        shet.new_plus("");
        kor21.new_plus("");

        if(i1 > 0)
         iceb_u_polen(koresp.ravno(),&bros,i+1,',');
        else
         bros.new_plus(koresp.ravno());      

        if(ff_prot != NULL)
          fprintf(ff_prot,"Взяли счёт-%s\n",bros.ravno());

        if(i1 > 0 && i == 0)
         i1--;
         
        /*Проверяем есть ли код контрагента*/
        if(iceb_u_polen(bros.ravno(),&shet,1,':') != 0)
         shet.new_plus(bros.ravno());
        else
         iceb_u_polen(bros.ravno(),&kor21,2,':');

        if(ff_prot != NULL)
          fprintf(ff_prot,"Счёт-%s код контрагента-%s\n",shet.ravno(),kor21.ravno());

        if(shet.ravno()[0] == '\0')
         continue;

        if(kor21.ravno()[0] == '\0')
         { 
          sprintf(strsql,"%s%d",ICEB_ZAR_PKTN,tbn);
          kor21.new_plus(strsql);
         }
        if(iceb_provsh(&kor21,shet.ravno(),&shetv,avkvs,1,wpredok) != 0)
           continue;
        //Если счёт не балансовый то никаких проводок для соц-фондов быть не может
        kolpr=2;
        if(shetv.stat == 1)
           kolpr=1; 

        if(ff_prot != NULL)
          fprintf(ff_prot,"\n-shet=%s shet1=%s kor21=%s kor11=%s deb=%.2f kre=%.2f\nshet_zamena=%s\n",shet.ravno(),shet1.ravno(),kor21.ravno(),kor11.ravno(),deb,kre,shet_zamena.ravno());

        if(kolpr == 2)
          iceb_zapmpr(g,m,d,shet.ravno(),shet1.ravno(),kor21.ravno(),kor11.ravno(),deb,kre,koment.ravno(),kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
        if(kolpr == 1)
          iceb_zapmpr(g,m,d,shet.ravno(),"",kor21.ravno(),kor11.ravno(),deb,kre,koment.ravno(),kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);

        if(kolpr == 2)
         {
          shet1.new_plus(shet.ravno());
          kor11.new_plus(kor21.ravno());
         }
        kor21.new_plus("");
       }
      break; //если дошли сюда значит найдена подходящая настройка и проводки сделаны дальше смотреть не нужно
     }

    if(kontr1.ravno()[0] != '\0')
     {
      kor21.new_plus(kontr1.ravno());
     }

    if(kor21.getdlinna() <= 1)
     {
      sprintf(strsql,"%s%d",ICEB_ZAR_PKTN,tbn);
      kor21.new_plus(strsql);
     }
//    printw("nahshet=%s shet1=%s kor21=%s kor11=%s deb=%f\n",
//    nahshet,shet1,kor21,kor11,deb);

    if(iceb_provsh(&kor21,nahshet.ravno(),&shetv,avkvs,1,wpredok) != 0)
       continue;

    kolpr=2;
    if(shetv.stat == 1)
      kolpr=1;

    /*Если не было цепочки проводок то счёт не заменён*/
    if(shet_zamena.ravno()[0] == '*')
     {
      SQLCURSOR cursh;
      SQL_str   rowsh;
      //взять счёт из карточки
      sprintf(strsql,"select shet from Zarn where tabn=%d and god=%d and mes=%d",tbn,gp,mp);
      if(sql_readkey(&bd,strsql,&rowsh,&cursh) != 1)
       {
        class iceb_u_str repl;
        sprintf(strsql,"%s-Не найдена настройка по дате %d.%d для определения счета в карточке !",
        __FUNCTION__,mp,gp);
        repl.plus(strsql);
        
        sprintf(strsql,"Табельный номер %d",tbn);
        repl.ps_plus(strsql);
        iceb_menu_soob(&repl,wpredok);
       }
      else
       shet_zamena.new_plus(rowsh[0]);
     }
    
    if(shet_zamena.ravno()[0] != '\0')
      nahshet.new_plus(shet_zamena.ravno());

    if(ff_prot != NULL)
      fprintf(ff_prot,"\n*nahshet=%s shet1=%s kor21=%s kor11=%s deb=%f kre=%f\n\
shet_zamena=%s\n",nahshet.ravno(),shet1.ravno(),kor21.ravno(),kor11.ravno(),deb,kre,shet_zamena.ravno());

    if(kolpr == 2)
      iceb_zapmpr(g,m,d,nahshet.ravno(),shet1.ravno(),kor21.ravno(),kor11.ravno(),deb,kre,koment.ravno(),kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
    if(kolpr == 1)
      iceb_zapmpr(g,m,d,nahshet.ravno(),"",kor21.ravno(),kor11.ravno(),deb,kre,koment.ravno(),kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
 
   }
 }
//OSTANOV();
}
