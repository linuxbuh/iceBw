/*$Id: avtprozarw.c,v 1.10 2014/03/31 06:05:33 sasa Exp $*/
/*30.03.2016	10.03.2011	Белых А.И.	avtprozar.c
Подпрограмма австоматического выполнения проводок для произвольной системы настроек
*/
#include "math.h"
#include "buhg_g.h"

void avtprozar_vpnu(int tbn,short mp,short gp,short denz,double suma,const char *koment,const char *kod_kontr_baz,int kekv,const char *shet_v_kar,const char *shet_nu,const char *str_nast,class iceb_u_spisok *sp_prov,class iceb_u_double *sum_prov_dk,FILE *ff_prot,GtkWidget *wpredok);
void avtprozar_soc(int tbn,short mp,short gp,int kekv,const char *kod_kontr_baz,const char *shet_v_kar,class iceb_u_spisok *sp_prov,class iceb_u_double *sum_prov_dk,FILE *ff_prot,GtkWidget *wpredok);

extern int kekv_at;  //Код экономической классификации для атестованых
extern int kekv_nat; //Код экономической классификации для не атестованых
extern int kodzv_nat;//Код звания неатестованный
extern int kekv_nfo; //Код экономической классификации для начислений на фонд оплаты
extern short *kodbl;  /*Коды начисления больничного*/
extern class iceb_u_str kod_dop_nah_bol; /*коды начилений дополнительно входящие в расчёт удержания ЕСВ с больничного*/
extern char *shetb; /*Бюджетные счета*/
extern char *shetbu; /*Бюджетные счета удержаний*/
extern SQL_baza bd;
extern class iceb_u_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/

void avtprozar(int tbn,short mp,short gp,FILE *ff_prot,GtkWidget *wpredok)
{
short	avkvs=1; //0-Не вставлять автоматом код контрагента 1-вставлять
class iceb_u_str pnom1("");
class iceb_u_str shet_zar_poum("");
class iceb_u_str shet_zar_poum_bud("");
class iceb_u_str shet_nu("");
class iceb_u_str knah("");
class iceb_u_str tabnom("");
class iceb_u_str koment_v_alx("");
class iceb_u_spisok sp_prov;
class iceb_u_double sum_prov_dk;
class iceb_u_str kontr_shet_zar_poum("");
class iceb_u_str kontr_shet_kor("");
class iceb_u_str kod_kontr_baz("");
class iceb_u_str shet_v_kar("");

int kekv=0;
int kolpr=2;
char strsql[1024];
short dd=0,md=0,gd=0; /*дата докусента - последний день месяца*/
short dpr=0,mpr=0,gpr=0; /*дата проводки - дана начисления или удержания*/
SQL_str row,row1,row_alx;
class SQLCURSOR cur,cur1,cur_alx;
int kolstr=0;
char nomdok[64];
int prn=0;
char imaf_alx[64];
struct OPSHET	shetv;

if(ff_prot != NULL)
 fprintf(ff_prot,"%s-%d %02d.%04d\n",__FUNCTION__,tbn,mp,gp);
 
if(iceb_pvglkni(mp,gp,wpredok) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Главная книга заблокирована!\n",__FUNCTION__);
  return;
 }

sprintf(nomdok,"%d-%d",mp,tbn);

sprintf(strsql,"%s%d",ICEB_ZAR_PKTN,tbn);
kod_kontr_baz.new_plus(strsql);

/*Определяем счёт в карточке*/
sprintf(strsql,"select shet from Zarn where tabn=%d and god=%d and mes=%d",tbn,gp,mp);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 shet_v_kar.new_plus(row[0]);
else
 {
  sprintf(strsql,"select shet from Kartb where tabn=%d",tbn);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   shet_v_kar.new_plus(row[0]);
 }

if(ff_prot != NULL)
  fprintf(ff_prot,"%s-Счёт в карточке=%s\n",__FUNCTION__,shet_v_kar.ravno());

gd=gp;
md=mp;
dd=1;

iceb_u_dpm(&dd,&md,&gd,5);
//Проводки удаляем сразу так, как можно обнулить все начисления и удержания
if(iceb_udprgr(ICEB_MP_ZARP,dd,md,gd,nomdok,0,0,wpredok) != 0)
 return;

/*Если не введены счета для выполнения проводок по умолчанию, то проводки делать не нужно*/
if(iceb_poldan("Счёт для выполнения проводок по умолчанию",&shet_zar_poum,"avtprozar1.alx",wpredok) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Не найдена настройка \"Счёт для выполнения проводок по умолчанию\" в файле zarprozar1.alx !\n",__FUNCTION__);
  return;
 }
if(shet_zar_poum.getdlinna() <= 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Не введён \"Счёт для выполнения проводок по умолчанию\" в файле zarprozar1.alx !\n",__FUNCTION__);
  return;
 }

if(iceb_poldan("Счёт для выполнения проводок по умолчанию",&shet_zar_poum,"avtprozar2.alx",wpredok) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Не найдена настройка \"Счёт для выполнения проводок по умолчанию\" в файле zarprozar2.alx !\n",__FUNCTION__);
  return;
 }
if(shet_zar_poum.getdlinna() <= 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Не введён \"Счёт для выполнения проводок по умолчанию\" в файле zarprozar2.alx !\n",__FUNCTION__);
  return;
 }



if(ff_prot != NULL)
 fprintf(ff_prot,"kekv_nat=%d kekv_at=%d kekv_nfo=%d kodzv_nat=%d\n",kekv_nat,kekv_at,kekv_nfo,kodzv_nat);        

if(kodzv_nat != 0 && (kekv_at != 0 || kekv_nat != 0)) //Узнаём звание и определяем kekv
 {
  sprintf(strsql,"select zvan from Zarn where tabn=%d and god=%d and mes=%d",tbn,gp,mp);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    if(atoi(row[0]) == kodzv_nat)
     kekv=kekv_nat;
    else
     kekv=kekv_at;

   }   
  else
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s-Не нашли звание !!!\n",__FUNCTION__);
   }
 }

if(ff_prot != NULL)
    fprintf(ff_prot,"%s-kekv=%d\n",__FUNCTION__,kekv);        




sprintf(strsql,"select datz,suma,shet,prn,knah,kom from Zarp where \
datz >='%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and suma != 0. order by prn asc",gp,mp,gp,mp,tbn);
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
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено ни одного начисления или удержания с ненулевой суммой!\n");
  return;
 }
int metka_vp=0; /*Метка что нашли начало подходящей настройки*/
double suma=0.;
while(cur.read_cursor(&row) != 0)
 {
   
  if(ff_prot != NULL) 
   fprintf(ff_prot,"\n%s-%s %s %s %s %s %s\n",__FUNCTION__,row[0],row[1],row[2],row[3],row[4],row[5]);
  //Проверяем нужно ли делать проводки для этого начисления/удержания

  prn=atoi(row[3]);  

  if(row[2][0] == '\0')
   {
    if(prn == 1)
     sprintf(strsql,"%s-%s %d %s %s!\n",__FUNCTION__,gettext("Табельный номер"),tbn,gettext("Не введён счёт для начисления"),row[4]);
    if(prn == 2)
     sprintf(strsql,"%s-%s %d %s %s!\n",__FUNCTION__,gettext("Табельный номер"),tbn,gettext("Не введён счёт для удержания"),row[4]);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }


  if(prn == 2) //Удержания
    sprintf(strsql,"select prov from Uder where kod=%s",row[4]);
  if(prn == 1) //Начисления
    sprintf(strsql,"select prov from Nash where kod=%s",row[4]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    if(row1[0][0] == '1')
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"%s-Для кода %s проводки выключены!\n",__FUNCTION__,row[4]);
      continue;
     }


  iceb_u_rsdat(&dpr,&mpr,&gpr,row[0],2); /*дата записи проводки*/
  
  /*Открываем файл настроек*/
  sprintf(imaf_alx,"avtprozar%d.alx",prn);
  sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
  if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }

  if(kolstr == 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s-Не найдены настройки %s\n",__FUNCTION__,imaf_alx);
    return;
   }

  suma=atof(row[1]);

  if(prn == 2)
   suma*=-1;

  metka_vp=0;

  while(cur_alx.read_cursor(&row_alx) != 0)
   {

    if(row_alx[0][0] == '#')
     continue;

    if(iceb_u_polen(row_alx[0],&pnom1,1,'|') != 0)
     continue;

    if(iceb_u_SRAV("Счёт для выполнения проводок по умолчанию",pnom1.ravno(),0) == 0)
     {
      iceb_u_polen(row_alx[0],&shet_zar_poum,2,'|');
      if(ff_prot != NULL)
       fprintf(ff_prot,"%s-Счёт для выполнения проводок по умолчанию=%s\n",__FUNCTION__,shet_zar_poum.ravno());

      kontr_shet_zar_poum.new_plus(kod_kontr_baz.ravno());
      if(iceb_provsh(&kontr_shet_zar_poum,shet_zar_poum.ravno(),&shetv,avkvs,0,wpredok) != 0)
        return;
      continue;
     }    

    if(iceb_u_SRAV("Счёт для выполнения бюджетных проводок по умолчанию",pnom1.ravno(),0) == 0)
     {
      iceb_u_polen(row_alx[0],&shet_zar_poum_bud,2,'|');
      if(ff_prot != NULL)
       fprintf(ff_prot,"%s-Счёт для выполнения бюджетных проводок по умолчанию=%s\n",__FUNCTION__,shet_zar_poum.ravno());

      if(shet_zar_poum_bud.getdlinna() > 1)
       {
        class iceb_u_str kontr_for_proverki(kod_kontr_baz.ravno());
        if(iceb_provsh(&kontr_for_proverki,shet_zar_poum.ravno(),&shetv,avkvs,0,wpredok) != 0)
          return;
       }
      continue;
     }    

    if(metka_vp == 0)
     {
      if(iceb_u_SRAV("START",pnom1.ravno(),0) != 0)
       continue;

      if(ff_prot != NULL)
       fprintf(ff_prot,"%s",row_alx[0]);

      if(iceb_u_polen(row_alx[0],&knah,2,'|') != 0)
        continue;

      if(iceb_u_proverka(knah.ravno(),row[4],0,0)  != 0)
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"%s-Не подходит код начисления\n",__FUNCTION__);
        continue;
       }

      if(iceb_u_polen(row_alx[0],&shet_nu,3,'|') != 0)
        continue;

      if(iceb_u_proverka(shet_nu.ravno(),row[2],0,0) != 0)
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"%s-Не подходит счёт\n",__FUNCTION__);
        continue;
       }
      if(iceb_u_polen(row_alx[0],&tabnom,4,'|') == 0)
       if(iceb_u_proverka(tabnom.ravno(),tbn,0,0) != 0)
        {
         if(ff_prot != NULL)
          fprintf(ff_prot,"%s-Не подходит табельный номер\n",__FUNCTION__);
         continue;
        }
      if(iceb_u_polen(row_alx[0],&koment_v_alx,5,'|') == 0)
       if(iceb_u_proverka(koment_v_alx.ravno(),row[5],4,0) != 0)
        {
         if(ff_prot != NULL)
          fprintf(ff_prot,"%s-Не подходит коментарий\n",__FUNCTION__);
         continue;
        }
      metka_vp=1;
      continue;
     }
    if(iceb_u_SRAV("END",row_alx[0],1) == 0)
      break;
    if(iceb_u_SRAV("START",row_alx[0],1) == 0)
       break;
//    if(ff_prot != NULL)
//       fprintf(ff_prot,"%s",row_alx[0]);
    avtprozar_vpnu(tbn,mpr,gpr,dpr,suma,row[5],kod_kontr_baz.ravno(),kekv,shet_v_kar.ravno(),row[2],row_alx[0],&sp_prov,&sum_prov_dk,ff_prot,wpredok);
     
   }  

  if(metka_vp == 0) /*выполняем проводку по умолчанию*/
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s-Не найдено настройки - выполняем проводку по умолчанию\n",__FUNCTION__);
    class iceb_u_str shet_for_prov(shet_zar_poum.ravno());
    if(shet_zar_poum.getdlinna() <= 1)
     {
      if(ff_prot != NULL)
       {
        if(prn == 1)
         fprintf(ff_prot,"%s-Не введён \"Счёт для выполнения проводок по умолчанию\" в файл avtprozar1.alx !\n",__FUNCTION__);
        if(prn == 2)
         fprintf(ff_prot,"%s-Не введён \"Счёт для выполнения проводок по умолчанию\" в файл avtprozar2.alx !\n",__FUNCTION__);
       }
      return;
     }    

    if(prn == 1)
     {
      if(iceb_u_proverka(shetb,row[2],0,1) == 0)
        if(shet_zar_poum_bud.getdlinna() > 1)
         shet_for_prov.new_plus(shet_zar_poum_bud.ravno());
     }
    else
     {
      if(iceb_u_proverka(shetbu,row[2],0,1) == 0)
        if(shet_zar_poum_bud.getdlinna() > 1)
         shet_for_prov.new_plus(shet_zar_poum_bud.ravno());
     }

    kontr_shet_kor.new_plus(kod_kontr_baz.ravno());
    if(iceb_provsh(&kontr_shet_kor,row[2],&shetv,avkvs,0,wpredok) != 0)
      return;

    kontr_shet_zar_poum.new_plus(kod_kontr_baz.ravno());
    if(iceb_provsh(&kontr_shet_zar_poum,shet_for_prov.ravno(),&shetv,avkvs,0,wpredok) != 0)
      return;


    kolpr=2;

    if(prn == 1)
      iceb_zapmpr(gpr,mpr,dpr,row[2],shet_for_prov.ravno(),kontr_shet_kor.ravno(),kontr_shet_zar_poum.ravno(),suma,0.,row[5],kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
    else
      iceb_zapmpr(gpr,mpr,dpr,shet_for_prov.ravno(),row[2],kontr_shet_zar_poum.ravno(),kontr_shet_kor.ravno(),suma,0.,row[5],kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
   }   
 }
/*проводки на начиленные отчисления в соц.фонды*/
avtprozar_soc(tbn,mp,gp,kekv,kod_kontr_baz.ravno(),shet_v_kar.ravno(),&sp_prov,&sum_prov_dk,ff_prot,wpredok);

/*Запись проводок из памяти в базу*/
sprintf(strsql,"LOCK TABLES Prov WRITE,Alx READ,Blok READ,icebuser READ,Plansh READ");
class iceb_lock_tables lt(strsql);

iceb_zapmpr1(nomdok,"",0,time(NULL),ICEB_MP_ZARP,dd,md,gd,0,&sp_prov,&sum_prov_dk,ff_prot,wpredok);


}
/****************************************/
/*выполнение проводок*/
/***************************/
void avtprozar_vpnu(int tbn,short mp,short gp,
short denz,
double suma,
const char *koment,
const char *kod_kontr_baz,
int kekv,
const char *shet_v_kar,
const char *shet_nu,
const char *str_nast,
class iceb_u_spisok *sp_prov,
class iceb_u_double *sum_prov_dk,
FILE *ff_prot,
GtkWidget *wpredok)
{
class iceb_u_str shet_deb("");
class iceb_u_str shet_kre("");
class iceb_u_str kodkon_deb("");
class iceb_u_str kodkon_kre("");
class iceb_u_str pnom("");
class iceb_u_str koment_prov(koment);
class iceb_u_str kekv_prov("");
short	avkvs=1; //0-Не вставлять автоматом код контрагента 1-вставлять

if(ff_prot != NULL)
 fprintf(ff_prot,"%s-%d Дата-%02d.%02d.%04d Сумма-%.2f Коментарий-%s строка настройки=%s\n",__FUNCTION__,tbn,denz,mp,gp,suma,koment,str_nast);

if(iceb_u_polen(str_nast,&shet_deb,1,'|') != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Не найден счёт дебета!\n",__FUNCTION__);
  return;
 }
if(iceb_u_polen(str_nast,&shet_kre,2,'|') != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Не найден счёт кредита!\n",__FUNCTION__);
  return;
 }

if(iceb_u_SRAV(shet_deb.ravno(),"SHET_V_KAR",0) == 0)
 shet_deb.new_plus(shet_v_kar);

if(iceb_u_SRAV(shet_deb.ravno(),"SHET_V_NU",0) == 0)
 shet_deb.new_plus(shet_nu);

if(iceb_u_SRAV(shet_kre.ravno(),"SHET_V_KAR",0) == 0)
 shet_kre.new_plus(shet_v_kar);

if(iceb_u_SRAV(shet_kre.ravno(),"SHET_V_NU",0) == 0)
 shet_kre.new_plus(shet_nu);
 
pnom.new_plus(shet_deb.ravno());
if(iceb_u_polen(pnom.ravno(),&shet_deb,1,':') == 0)
  iceb_u_polen(pnom.ravno(),&kodkon_deb,2,':');
else
 shet_deb.new_plus(pnom.ravno());

pnom.new_plus(shet_kre.ravno());
if(iceb_u_polen(pnom.ravno(),&shet_kre,1,':') == 0)
  iceb_u_polen(pnom.ravno(),&kodkon_kre,2,':');
else
 shet_kre.new_plus(pnom.ravno());

iceb_u_polen(str_nast,&kekv_prov,3,'|');
if(kekv_prov.getdlinna() <= 1)
 kekv_prov.new_plus(kekv);

iceb_u_polen(str_nast,&koment_prov,4,'|');
if(koment[0] != '\0')
 koment_prov.plus(" ",koment);
 
struct OPSHET	shetv;

if(kodkon_deb.getdlinna() <= 1)
  kodkon_deb.new_plus(kod_kontr_baz);
 
if(iceb_provsh(&kodkon_deb,shet_deb.ravno(),&shetv,avkvs,0,wpredok) != 0)
  return;

if(kodkon_kre.getdlinna() <= 1)
  kodkon_kre.new_plus(kod_kontr_baz);

if(iceb_provsh(&kodkon_kre,shet_kre.ravno(),&shetv,avkvs,0,wpredok) != 0)
  return;

int kolpr=2;

iceb_zapmpr(gp,mp,denz,shet_deb.ravno(),shet_kre.ravno(),kodkon_deb.ravno(),kodkon_kre.ravno(),suma,0.,koment_prov.ravno(),kolpr,kekv_prov.ravno_atoi(),sp_prov,sum_prov_dk,ff_prot);

}

/******************************************/
/*Выполнение проводок для соц.начислений на фонд зарплаты*/
/*********************************************************/

void avtprozar_soc(int tbn,short mp,short gp,
int kekv,
const char *kod_kontr_baz,
const char *shet_v_kar,
class iceb_u_spisok *sp_prov,
class iceb_u_double *sum_prov_dk,
FILE *ff_prot,
GtkWidget *wpredok)
{
char strsql[1024];
int kolstr=0;
class SQLCURSOR cur,cur1,cur_alx;
SQL_str row,row2,row_alx;
class iceb_u_str pnom1("");
const char *imaf_alx={"avtprozar3.alx"};
class iceb_u_str kodnv("");
class iceb_u_str kontr_for_soc(kod_kontr_baz);
class iceb_u_str kontr_for_nah(kod_kontr_baz);
class iceb_u_str koment_v_alx("");
double suma_for_esv=0.; /*Сумма начислений с которой берётся единый социальный взнос*/
double suma_for_esv_bol=0.; /*Cумма начислений больничного для единого социального взноса*/
int kodz=0;
int metka_bol=0;
short d,m,g;
double sum=0.;
short	avkvs=1; //0-Не вставлять автоматом код контрагента 1-вставлять
if(ff_prot != NULL)
 fprintf(ff_prot,"\n%s-Выполнение проводок на фонд зарплаты.  kekv=%d\n",__FUNCTION__,kekv);
 
class iceb_u_double SUMA;
class iceb_u_spisok NAHIS;

if(zaravpt_osw(tbn,mp,gp,&NAHIS,&SUMA,wpredok) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Не нашли начислений!\n",__FUNCTION__);
  return;
 }
int kodf_esv_bol=0;

if(iceb_poldan("Код фонда единого социального взноса для больничных",&kodf_esv_bol,"zarnast.alx",wpredok) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Не найден Код фонда единого социального взноса для больничных!\n",__FUNCTION__);
 }
int kodf_esv_bol_inv=0;

if(iceb_poldan("Код фонда единого социального взноса с больничных для инвалидов",&kodf_esv_bol_inv,"zarnast.alx",wpredok) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Не найден Код фонда единого социального взноса с больничных для инвалидов!\n",__FUNCTION__);
 }

/*Смотрим записи социальных отчислений*/
sprintf(strsql,"select kodz,shet,sumas,sumap,sumapb from Zarsocz where datz='%04d-%d-01' and tabn=%d",gp,mp,tbn);
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
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Не найдено ни одной записи в таблице Zarsocz!\n",__FUNCTION__);
  return;
 }
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

  struct OPSHET	shetv;

  double sumas=atof(row[2]);
  double sumap=atof(row[3]);

  /*Берем коды не входящие в расчёт*/
  kodnv.new_plus("");
  sprintf(strsql,"select kodn from Zarsoc where kod=%d\n",kodz);
  if(sql_readkey(&bd,strsql,&row2,&cur1) != 1)
   {
    sprintf(strsql,"%s-%s %d !",__FUNCTION__,gettext("Не найден код соц. страхования"),kodz);
    iceb_menu_soob(strsql,wpredok);
   }  
  else
   kodnv.new_plus(row2[0]);

  /*Открываем файл настроек*/
  sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
  if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }

  if(kolstr == 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s-Не найдены настройки %s\n",__FUNCTION__,imaf_alx);
    return;
   }
  double sumi=0.;
  for(int nomz=0; nomz < kolzap; nomz++)
   {

    if(fabs(sumi) > fabs(sumas))
      break;

     if(sumas < 0. && sumi < sumas)
      break;

    class iceb_u_str kodnah("");
    char dataz[11];

//    printw("%s = %.2f\n",NAHIS.ravno(nomz),sum);

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
    iceb_u_polen(NAHIS.ravno(nomz),dataz,sizeof(dataz),1,'|');

    /*Проверяем на коды не входящие в расчёт */
    if(iceb_u_proverka(kodnv.ravno(),kodnah.ravno(),0,1) == 0)
     continue;

    iceb_u_rsdat(&d,&m,&g,dataz,2);


    class iceb_u_str shet_v_nah("");
    iceb_u_polen(NAHIS.ravno(nomz),&shet_v_nah,2,'|');
    kontr_for_nah.new_plus(kod_kontr_baz);
 
    if(iceb_provsh(&kontr_for_nah,shet_v_nah.ravno(),&shetv,avkvs,0,wpredok) != 0)
      continue;
    
    class iceb_u_str koment_v_nah("");
    
    iceb_u_polen(NAHIS.ravno(nomz),&koment_v_nah,4,'|');
    double deb=0.;
    
    if(fabs(sumi+sum) <= fabs(sumas))
     deb=sum;
    else
     {
      deb=sumas-sumi;
     }
    sumi+=sum;
    
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s-Сумма начисления %.2f\n\
Вычисляем сумму взноса которая приходится на эту сумму\n\
%.2f*%.2f/%.2f=%.2f\n",
      __FUNCTION__,
      deb, 
      deb,sumap,sumas,
      deb*sumap/sumas);
      
    deb=deb*sumap/sumas;

    int metka_vp=0;
    class iceb_u_str kod_fonda("");
    class iceb_u_str shet_v_nast("");
    class iceb_u_str tabnom_v_nast("");
    metka_vp=0;
    cur_alx.poz_cursor(0); /*обязательно*/
    while(cur_alx.read_cursor(&row_alx) != 0)
     {

      if(row_alx[0][0] == '#')
       continue;

      if(iceb_u_polen(row_alx[0],&pnom1,1,'|') != 0)
       continue;

      if(metka_vp == 0)
       {
        if(iceb_u_SRAV("START",pnom1.ravno(),0) != 0)
         continue;

        if(ff_prot != NULL)
         fprintf(ff_prot,"%s",row_alx[0]);
         
        if(iceb_u_polen(row_alx[0],&kod_fonda,2,'|') != 0)
         continue;

        if(iceb_u_proverka(kod_fonda.ravno(),kodz,0,0) != 0)
         {
          if(ff_prot != NULL)
           fprintf(ff_prot,"%s-Не подходит код фонда %s != %d\n",__FUNCTION__,kod_fonda.ravno(),kodz);
          continue;
         }

        if(iceb_u_polen(row_alx[0],&shet_v_nast,3,'|') != 0)
         continue;

        if(iceb_u_proverka(shet_v_nast.ravno(),shet_v_nah.ravno(),0,0) != 0)
         {
          if(ff_prot != NULL)
           fprintf(ff_prot,"%s-Не подходит счёт %s != %s\n",__FUNCTION__,shet_v_nast.ravno(),shet_v_nah.ravno());
          continue;
         }

        if(iceb_u_polen(row_alx[0],&tabnom_v_nast,4,'|') == 0)
         if(iceb_u_proverka(tabnom_v_nast.ravno(),tbn,0,0) != 0)
          {
           if(ff_prot != NULL)
            fprintf(ff_prot,"%s-Не подходит табельный номер %s != %d\n",__FUNCTION__,tabnom_v_nast.ravno(),tbn);
           continue;
          }

        if(iceb_u_polen(row_alx[0],&koment_v_alx,5,'|') == 0)
         if(iceb_u_proverka(koment_v_alx.ravno(),koment_v_nah.ravno(),4,0) != 0)
          {
           if(ff_prot != NULL)
            fprintf(ff_prot,"%s-Не подходит коментарий\n",__FUNCTION__);
           continue;
          }
        metka_vp=1;
        continue;
       }

      if(iceb_u_SRAV("END",row_alx[0],1) == 0)
         break;
      if(iceb_u_SRAV("START",row_alx[0],1) == 0)
         break;
      avtprozar_vpnu(tbn,m,g,d,deb,koment_v_nah.ravno(),kod_kontr_baz,kekv,shet_v_kar,row[1],row_alx[0],sp_prov,sum_prov_dk,ff_prot,wpredok);
     }

    if(metka_vp == 0) /*выполняем проводку по умолчанию*/
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"%s-Не найдено настройки - выполняем проводку по умолчанию\n",__FUNCTION__);
      int kolpr=2;
      kontr_for_soc.new_plus(kod_kontr_baz);
      class iceb_u_str shet_v_fonde("");
      class iceb_u_str shet_v_fonde_bud("");
     
      if(iceb_u_polen(row[1],&shet_v_fonde,1,',') != 0)
       shet_v_fonde.new_plus(row[1]);
      else
       iceb_u_polen(row[1],&shet_v_fonde_bud,2,',');

      if(shet_v_fonde_bud.getdlinna() > 1)
        if(iceb_u_proverka(shetb,shet_v_nah.ravno(),0,1) == 0)
          shet_v_fonde.new_plus(shet_v_fonde_bud.ravno());            

      if(iceb_provsh(&kontr_for_soc,shet_v_fonde.ravno(),&shetv,avkvs,0,wpredok) != 0)
        continue;

      iceb_zapmpr(g,m,d,shet_v_nah.ravno(),shet_v_fonde.ravno(),kontr_for_nah.ravno(),kontr_for_soc.ravno(),deb,0.,koment_v_nah.ravno(),kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
     }   

   }


 }
}
