/* $Id: avtprkasw.c,v 1.20 2013/09/26 09:46:45 sasa Exp $ */
/*13.03.2020	05.10.2000	Белых А.И.	avtprkasw.c
Автоматическое выполнение пpоводок для кассовых оpдеpов
*/
#include        "buhg_g.h"

void avtprkas_kekv(int tipz,const char *kod_op,const char *shet_kor,int *kekv,GtkWidget *wpredok);

extern SQL_baza bd;

void	avtprkasw(const char kassa[],short tipz,const char nomd[],
short dd,short md,short gd, //Дата документа
const char shetk[],const char shetd[],const char kodop[],
GtkWidget *wpredok)
{
int kekv=0;
char		strsql[512];
int		kolstr;
SQL_str		row;
short		d,m,g;
double		deb=0.,kre=0.;
class iceb_u_str kor1(""),kor2("");
time_t		vrem;
class iceb_u_str kto(ICEB_MP_KASA);
double		suma;
double		sum;
struct OPSHET	shetkk;
struct OPSHET	shetdd;
class SQLCURSOR cur;
/*
printw("\navtprkas-%s %d %s %d.%d.%d %s %s %s\n",
kassa,tipz,nomd,dd,md,gd,shetk,shetd,kodop);
OSTANOV();
*/

if(iceb_prsh1(shetk,&shetkk,wpredok) != 0)
   return;

if(iceb_prsh1(shetd,&shetdd,wpredok) != 0)
   return;

if(shetkk.stat == 1)
 {
  sprintf(strsql,gettext("Счёт %s внебалансовый !"),shetk);
  iceb_menu_soob(strsql,wpredok);
  return;
 }

if(shetdd.stat == 1)
 {
  sprintf(strsql,gettext("Счёт %s внебалансовый !"),shetd);
  iceb_menu_soob(strsql,wpredok);
  return;
 }
class iceb_u_str kod_val("");
/*Узнаём код валюты*/
sprintf(strsql,"select kv from Kas where kod=%d",atoi(kassa));
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  kod_val.new_plus(row[0]);
 }
class iceb_getkue_dat kue(kod_val.ravno(),wpredok);

class iceb_lock_tables kkkk("LOCK TABLES Prov WRITE,Kasord READ,Kasord1 READ,Blok READ,icebuser READ,Glkkv READ,Alx READ,Plansh READ,Kontragent READ");

sprintf(strsql,"select datp,kontr,suma,koment from Kasord1 where \
kassa=%s and nomd='%s' and tp=%d and god=%d and suma != 0. and \
datp != '0000-00-00'",kassa,nomd,tipz,gd);
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

/*Удаляем все проводки с проверкой возможности удаления*/
if(iceb_udprgr(ICEB_MP_KASA,dd,md,gd,nomd,atoi(kassa),tipz,wpredok) != 0)
 return;

avtprkas_kekv(tipz,kodop,shetd,&kekv,wpredok); /*Определяем kekv*/

/*Узнаём основание*/
SQL_str row1;
class SQLCURSOR cur1;

class iceb_u_str osnov("");
sprintf(strsql,"select osnov from Kasord where nomd='%s' and kassa=%d and tp=%d and god=%d",
nomd,atoi(kassa),tipz,gd);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) > 0)
 osnov.new_plus(row1[0]);



class iceb_u_str kom_new("N");
kom_new.plus(nomd);

if(osnov.getdlinna() > 1)
 kom_new.plus(" ",osnov.ravno());


class iceb_u_str koment("");
suma=0.;
time(&vrem);
while(cur.read_cursor(&row) != 0)
 {
  koment.new_plus(kom_new.ravno());
  
  if(iceb_u_rsdat(&d,&m,&g,row[0],2) == 0)
   {
    if(iceb_pvglkni(m,g,wpredok) != 0)
     continue;
    
   }

  koment.plus(" ",iceb_get_pnk(row[1],2,wpredok));

  sum=kue.fromue(row[2],row[0]);

  if(tipz == 1)
   {
    deb=sum;
    kre=0.;
   }
  if(tipz == 2)
   {
    deb=0;
    kre=sum;
   }
   
  kor1.new_plus("");
  kor2.new_plus("");

  if(shetkk.saldo == 3 || shetdd.saldo == 3)
   {
    kor1.new_plus(row[1]);
    kor2.new_plus(row[1]);
   }  
  if(row[3][0] != '\0')
   koment.plus("/",row[3]);
  iceb_zapprov(0,g,m,d,shetk,shetd,kor1.ravno(),kor2.ravno(),kto.ravno(),nomd,kodop,deb,kre,koment.ravno_filtr(),2,atoi(kassa),vrem,gd,md,dd,tipz,kekv,wpredok);
  suma+=sum;
 }

}
/**************************/
/*Определение кода экономических затрат для проводки*/
/*****************************************************/

void avtprkas_kekv(int tipz,const char *kod_op,const char *shet_kor,int *kekv,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
*kekv=0;

class iceb_u_str imaf_nastr;
if(tipz == 1)
 imaf_nastr.new_plus("avtprokasp.alx");
if(tipz == 2)
 imaf_nastr.new_plus("avtprokasr.alx");

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_nastr.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

class iceb_u_str kod_opf("");
class iceb_u_str shet_korf("");
int kekvf=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;  

  if(iceb_u_polen(row_alx[0],&kod_opf,1,'|') != 0)
   continue;

  if(iceb_u_SRAV(kod_opf.ravno(),kod_op,0) != 0)
   continue;

  if(iceb_u_polen(row_alx[0],&shet_korf,2,'|') != 0)
   continue;

  if(iceb_u_SRAV(shet_korf.ravno(),shet_kor,0) != 0)
   continue;

  if(iceb_u_polen(row_alx[0],&kekvf,3,'|') != 0)
   continue;

  *kekv=kekvf;
  break;  

 }

}
