/* $Id: avtprpldw.c,v 1.19 2013/05/17 14:58:18 sasa Exp $ */
/*10.07.2015    28.05.1998      Белых А.И.      avtprpldw.c
Автоматическое проведение проводок для платежних документов
*/
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern struct REC rec;
extern SQL_baza bd;

void avtprpldw(const char *tabl,
const char *kop,  //Код операции
const char *npp,  //Номер документа
const char *kor,  //Код организации
const char *shet,
FILE *ff_prot,
GtkWidget *wpredok)
{
char		bros[512];
double          bb,deb,kre;
int             i,i1;
class iceb_u_str kodop("");
double		sump; /*Сумма подтвержденная общая*/
double		sumpp; /*Сумма последнего подтверждения*/
double		sumpr; /*Сумма по проводкам*/
time_t		vrem;
class iceb_u_str tipd("");
char		strsql[1024];
class iceb_u_str kontr("");
class iceb_u_str kontr1("");
class iceb_u_str kor1(""),kor2("");
short		metpr;
class iceb_u_str shet1("");
class iceb_u_str shet2("");
class iceb_u_str shet3("");
class iceb_u_str koresp("");
int		pod;
short		dp,mp,gp;
struct OPSHET	shetv;
int		kolpr=2;
int kekv=0;
class iceb_u_spisok sp_prov;
class iceb_u_double sum_prov_dk;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

/********
printw("avtprpld-tabl-%s kop=%s npp-%s kor-%s shet-%s\n",
tabl,kop,npp,kor,shet);
OSTANOV();
**********/

int metka_vkontr=2; /*метка режима выполнения проводки для контрагента если его нет в списке счёта 0-запрос 2-автоматическая вставка*/
if(iceb_poldan("Автоматическая запись нового контрагента в список счёта",strsql,"nastdok.alx",wpredok) == 0)
 {
   if(iceb_u_SRAV("Вкл",strsql,1) == 0)
    metka_vkontr=2;
   else
    metka_vkontr=0;
 }
if(ff_prot != NULL)
 {
  if(metka_vkontr == 0)
   fprintf(ff_prot,"Ручная вставка кода контрагента в список счёта с развёрнутым сальдо если его там нет\n");
  if(metka_vkontr == 2)
   fprintf(ff_prot,"Автоматическая вставка кода контрагента в список счёта с развёрнутым сальдо если его там нет\n");
 }


if(shet[0] == '\0')
  return;

/*Узнаем сумму подтвержденных записей*/
sump=sumpzpdw(tabl,&dp,&mp,&gp,&sumpp,wpredok);

if(iceb_u_SRAV(tabl,"Pltp",0) == 0)
 tipd.new_plus(ICEB_MP_PPOR);
if(iceb_u_SRAV(tabl,"Pltt",0) == 0)
 tipd.new_plus(ICEB_MP_PTRE);

/*Суммируем выполненные проводки*/

sumpr=0.;

sumpr=sumprpdw(rec.dd,rec.md,rec.gd,npp,0,tipd.ravno(),shet,wpredok);
/*
printw("sumpr=%f sump=%f\n",sumpr,sump);
OSTANOV();
*/
/*Нужные проводки сделаны*/

if(sumpr == sump)
 {
  iceb_menu_soob(gettext("Все проводки сделаны !"),wpredok);
  return;
 }


kontr.new_plus(kor);

if(iceb_provsh(&kontr,shet,&shetv,metka_vkontr,0,wpredok) != 0)
   return;
//tp1=shetv.saldo;
kor1.new_plus(kontr.ravno());
kor2.new_plus(kontr.ravno());

class iceb_u_str imaf_nast;
/*Открываем файл настроек*/
if(iceb_u_SRAV(tabl,"Pltp",0) == 0)
 imaf_nast.plus("avtprodok.alx");
if(iceb_u_SRAV(tabl,"Pltt",0) == 0)
 imaf_nast.plus("avtprodokt.alx");

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_nast.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки %s\n",imaf_nast.ravno());
  return;
 }

shet3.new_plus(shet);

time(&vrem);
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
    continue;
    
  if(iceb_u_polen(row_alx[0],&kodop,2,'|') != 0)
   continue;
  
  if(iceb_u_SRAV(kodop.ravno(),kop,0) != 0)
   continue;
  
  memset(bros,'\0',sizeof(bros));    
  iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|');
  i1=strlen(bros);
  metpr=0;
  if(iceb_u_strstrm(bros,"У") == 1)
      metpr=1;
  if(iceb_u_strstrm(bros,"С") == 1)
      metpr=2;

  if(metpr == 0)
   continue;

  deb=kre=0.;
  if(metpr == 1)
   {
    deb=iceb_u_okrug(rec.uslb,0.01);
    kre=0.;   
   }

  if(metpr == 2)
   {
    bb=sumpp-rec.uslb;
    deb=iceb_u_okrug(bb,0.01);
    kre=0.;
   }

  if(fabs(deb) < 0.01)
    continue;
       
  /*Берем счет*/
  memset(bros,'\0',sizeof(bros));
  iceb_u_polen(row_alx[0],bros,sizeof(bros),3,'|');

  if(iceb_u_polen(bros,&shet1,1,':') != 0)
   shet1.new_plus(bros);
  else
   iceb_u_polen(bros,&kontr1,2,':');
  
  /*Берем кореспонденцию*/
  iceb_u_polen(row_alx[0],&koresp,4,'|');

  i1=iceb_u_pole2(koresp.ravno(),',');
  
  for(i=0; i <= i1; i++)
   {
    shet2.new_plus("");
    kontr.new_plus("");

    if(i1 > 0)
     iceb_u_pole(koresp.ravno(),bros,i+1,',');
    else
     strcpy(bros,koresp.ravno());      

    if(i1 > 0 && i == 0)
     i1--;
     
    /*Проверяем есть ли код контрагента*/
    if(iceb_u_pole(bros,&shet2,1,':') != 0)
     shet2.new_plus(bros);
    else
     iceb_u_pole(bros,&kontr,2,':');

    if(shet2.ravno()[0] == '\0')
     continue;

    if(iceb_provsh(&kontr,shet2.ravno(),&shetv,metka_vkontr,0,wpredok) != 0)
       continue;
//    tp2=shetv.saldo;

    if(kontr.getdlinna() > 1)
      kor1.new_plus(kontr.ravno());
    if(kor2.ravno()[0] == '\0' && kontr.ravno()[0] != '\0')
      kor2.new_plus(kontr.ravno());
    
    iceb_zapmpr(gp,mp,dp,shet2.ravno(),shet3.ravno(),kor1.ravno(),kor2.ravno(),deb,kre,"",kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);

    shet3.new_plus(shet2.ravno());
    kor1.new_plus(kontr.ravno());
    kor2.new_plus(kontr.ravno());
   }


  if(kontr1.getdlinna() <= 1)
     kontr1.new_plus(kor);


   if(iceb_provsh(&kontr1,shet1.ravno(),&shetv,metka_vkontr,0,wpredok) != 0)
      continue;

 

//   tp1=shetv.saldo;

   if(kontr1.getdlinna() > 1)
     kor1.new_plus(kontr1.ravno());
   if(kor2.ravno()[0] == '\0' && kontr1.ravno()[0] != '\0')
     kor2.new_plus(kontr1.ravno());

//   printw("shet3=%s shet1=%s kor1=%s kor2=%s kontr1=%s deb=%.2f kre=%.2f\n",
//   shet3,shet1,kor1,kor2,kontr1,deb,kre);
//   OSTANOV();       

   iceb_zapmpr(gp,mp,dp,shet1.ravno(),shet3.ravno(),kor1.ravno(),kor2.ravno(),deb,kre,"",kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);

 }
/*Запись проводок из памяти в базу*/
pod=0;
time(&vrem);
int tipz=0;
if(iceb_u_SRAV(tabl,"Pltp",0) == 0)
 tipz=2;
if(iceb_u_SRAV(tabl,"Pltt",0) == 0)
 tipz=1;
iceb_zapmpr1(npp,kop,pod,vrem,tipd.ravno(),rec.dd,rec.md,rec.gd,tipz,&sp_prov,&sum_prov_dk,ff_prot,wpredok);

}
