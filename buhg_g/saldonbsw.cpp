/*$Id: saldonbsw.c,v 1.14 2013/09/26 09:46:55 sasa Exp $*/
/*12.07.2015	19.06.2001	Белых А.И.	saldonbsw.c
Обоpотный баланс по не балансовым счетам
*/
#include        "buhg_g.h"

class saldonbs_data
 {
  public:
  class iceb_u_spisok SHET;
  class iceb_u_spisok SHETRS;
  class iceb_u_spisok KONTR;

  class iceb_u_double DEB; //Дебеты счетов до периода
  class iceb_u_double KRE; //Кредиты счетов до периода
  class iceb_u_double DEB1; //Дебеты счетов за период
  class iceb_u_double KRE1; //Кредиты счетов за период
  class iceb_u_double DEBRS; //Дебеты по счет/контрагент до периода
  class iceb_u_double KRERS; //Кредиты по счет/контрагент до периода
  class iceb_u_double DEBRS1; //Дебеты по счет/контрагент периода
  class iceb_u_double KRERS1; //Кредиты по счет/контрагент периода
 };
 
int rozraxnbs(short,short,short,short,short,short,short,class saldonbs_data*,GtkTextBuffer *buffer,GtkWidget *view,GtkWidget *bar,GtkWidget *wpredok);


void rasbnsrs(short i,const char *sh,const char *nash,int vids,int sb,short gn,class saldonbs_data *rekr,double *ideb,double *ikre,double *ideb1,double *ikre1,double *ideb2,double *ikre2,time_t vrem,GtkWidget*);


extern short	startgodb; /*Стартовый год*/
extern SQL_baza bd;

void saldonbsw(short dn,short mn,short gn,short dk,short mk,short gk,
GtkTextBuffer *buffer,
GtkWidget *view,
GtkWidget *bar,
GtkWidget *wpredok)
{
int		kolshet=0; //Количество счетов в массиве
char		strsql[512];
SQL_str		row;
time_t		tmm=0;
int		i=0;
class iceb_u_str nais("");
double		deb=0.,kre=0.,deb1=0.,kre1=0.,deb2=0.,kre2=0.;
double		ideb=0.,ikre=0.,ideb1=0.,ikre1=0.,ideb2=0.,ikre2=0.;
int		sb=1; //Расчитать по субсчетам
SQLCURSOR curr;
time(&tmm);

sprintf(strsql,"Расчёт баланса по не балансовим счетам\n");
iceb_printw(strsql,buffer,view);

sprintf(strsql,"%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."));

iceb_printw(strsql,buffer,view);

class saldonbs_data rekr;

if(rozraxnbs(dn,mn,gn,dk,mk,gk,sb,&rekr,buffer,view,bar,wpredok) != 0)
  return;



kolshet=rekr.SHET.kolih();
for(i=0; i <kolshet ; i++)
 {

  sprintf(strsql,"select saldo,nais,vids from Plansh where ns='%s'",rekr.SHET.ravno(i));
  
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
   {
    sprintf(strsql,gettext("Нет счета %s в плане счетов !"),rekr.SHET.ravno(i));
    iceb_menu_soob(strsql,wpredok);
    continue;
   }
  int vids=atoi(row[2]);
  int saldo=atoi(row[0]);
  nais.new_plus(row[1]);  

  sprintf(strsql,"%-5s %s\n",rekr.SHET.ravno(i),nais.ravno());
  iceb_printw(strsql,buffer,view);

//  strzag(LINES-1,0,kolshet,i);
  iceb_pbar(bar,kolshet,i);    

  if(saldo == 3)
   {
    rasbnsrs(i,rekr.SHET.ravno(i),nais.ravno(),vids,sb,gn,&rekr,&ideb,&ikre,&ideb1,&ikre1,&ideb2,&ikre2,tmm,wpredok);
   }
  else
   {
    
    deb=rekr.DEB.ravno(i);
    kre=rekr.KRE.ravno(i);
    deb1=rekr.DEB1.ravno(i);
    kre1=rekr.KRE1.ravno(i);
    deb2=deb+deb1;
    kre2=kre+kre1;     
    
    if(deb == 0. && kre == 0. && deb1 == 0. && kre1 == 0. &&\
    deb2 == 0. && kre2 == 0.)
      continue;

    if(deb > kre)
     {
      ideb+=deb-kre;
     }
    else
     {
      ikre+=kre-deb;
     }


    ikre1+=kre1;
    ideb1+=deb1;

    if(deb2 > kre2)
     {
      ideb2=deb2-kre2;
      sprintf(strsql,"insert into Saldo \
values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
      "0",0,rekr.SHET.ravno(i),"",deb2-kre2,0.,iceb_getuid(wpredok),tmm);   
     }
    else
     {
      ikre2=kre2-deb2;
      sprintf(strsql,"insert into Saldo \
values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
      "0",0,rekr.SHET.ravno(i),"",0.,kre2-deb2,iceb_getuid(wpredok),tmm);   
     }

     iceb_sql_zapis(strsql,0,0,wpredok);
   }
 }
sprintf(strsql,"\nВнебалансовые счета:\n\
%s - %15s",gettext("Сальдо начальное"),iceb_u_prnbr(ideb));
iceb_printw(strsql,buffer,view);

sprintf(strsql," %15s\n",iceb_u_prnbr(ikre));
iceb_printw(strsql,buffer,view);

sprintf(strsql,"%s - %15s",gettext("Оборот за период"),iceb_u_prnbr(ideb1));
iceb_printw(strsql,buffer,view);

sprintf(strsql," %15s\n",iceb_u_prnbr(ikre1));
iceb_printw(strsql,buffer,view);

sprintf(strsql,"%s - %15s",gettext("Сальдо конечное "),iceb_u_prnbr(ideb2));
iceb_printw(strsql,buffer,view);

sprintf(strsql," %15s\n",iceb_u_prnbr(ikre2));
iceb_printw(strsql,buffer,view);

}

/******************************/
/* Формирование массивов      */
/******************************/

int     rozraxnbs(short dn,short mn,short gn,short dk,short mk,
short gk,short sb, //0-по счетам 1-по субсчетам
class saldonbs_data *rekr,
GtkTextBuffer *buffer,
GtkWidget *view,
GtkWidget *bar,
GtkWidget *wpredok)
{
short		godn;
char		strsql[512];
int		kolstr;
SQL_str		row;
double		deb,kre;
int		i=0,i1=0,i2=0;
class iceb_u_str shet("");
short		tipsaldo; //Тип сальдо
short		vids;     //Вид счета
short		den,mes,god;
class iceb_u_str kontr("");

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

sprintf(strsql,"%s:%d\n",gettext("Стаpтовый год"),godn);
iceb_printw(strsql,buffer,view);

sprintf(strsql,"%s\n",gettext("Формируем список счетов"));
iceb_printw(strsql,buffer,view);

/*Формируем массив счетов*/

sprintf(strsql,"select ns,vids,saldo from Plansh where stat=1 order by ns asc");

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
if(kolstr == 0)
  return(1);

int pozz=0;
while(cur.read_cursor(&row) != 0)
 {
  pozz++;

  if(atoi(row[1]) == 1 && sb == 0)
    continue;
  shet.new_plus(row[0]);
  if(sb == 1 && atoi(row[1]) == 0 && pozz < kolstr-1)
   {
    cur.read_cursor(&row);
    /*Если нет субсчетов запоминаем счет*/
    if(iceb_u_SRAV(shet.ravno(),row[0],1) != 0)
     {
      cur.poz_cursor(pozz-1);
      cur.read_cursor(&row);
     }
    else
     pozz++;
   }

  rekr->SHET.plus(row[0]);
  if(atoi(row[2]) == 3)
   {
    rekr->SHETRS.plus(row[0]);
   }

 }
sprintf(strsql,"%s:%d\n",gettext("Количество счетов"),rekr->SHET.kolih());
iceb_printw(strsql,buffer,view);

sprintf(strsql,"%s:%d\n",gettext("Количество счетов с развернутым сальдо"),rekr->SHETRS.kolih());
iceb_printw(strsql,buffer,view);

rekr->DEB.make_class(rekr->SHET.kolih());
rekr->KRE.make_class(rekr->SHET.kolih());

rekr->DEB1.make_class(rekr->SHET.kolih());
rekr->KRE1.make_class(rekr->SHET.kolih());

//Определяем количество контрагентов
for(i=0; i < rekr->SHETRS.kolih() ;i ++)
 {
  sprintf(strsql,"select distinct kodkon from Skontr where ns='%s'",rekr->SHETRS.ravno(i));
  if(cur.make_cursor(&bd,strsql) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }
  while(cur.read_cursor(&row) != 0)
    if(rekr->KONTR.find_r(row[0]) < 0)
       rekr->KONTR.plus(row[0]);

 }
sprintf(strsql,"%s:%d\n",gettext("Количество контрагентов"),rekr->KONTR.kolih());
iceb_printw(strsql,buffer,view);
int razmer=rekr->SHETRS.kolih()*rekr->KONTR.kolih();
if(razmer > 0)
 {
  rekr->DEBRS.make_class(razmer);
  rekr->KRERS.make_class(razmer);
  rekr->DEBRS1.make_class(razmer);
  rekr->KRERS1.make_class(razmer);
 }

/***********************************/
/*Узнаем стартовые сальдо по счетам*/
/***********************************/
sprintf(strsql,"%s\n",gettext("Вычисляем стартовые сальдо по счетам"));
iceb_printw(strsql,buffer,view);

for(i=0; i < rekr->SHET.kolih() ; i++)
 {
  sprintf(strsql,"select saldo,vids from Plansh where ns='%s'",rekr->SHET.ravno(i));
  
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,gettext("Нет счета %s в плане счетов !"),rekr->SHET.ravno(i));
    iceb_menu_soob(strsql,wpredok);
    continue;
   }

  tipsaldo=0;
  if(atoi(row[0]) == 3)
    tipsaldo=1;
  vids=atoi(row[1]);
  
  if(vids == 0) /*Счёт*/
   sprintf(strsql,"select ns,deb,kre,kodkon from Saldo where kkk='%d' \
and gs=%d and ns like '%s%%'",tipsaldo,godn,rekr->SHET.ravno(i));

  if(vids == 1) /*Субсчёт*/
   sprintf(strsql,"select ns,deb,kre,kodkon from Saldo where kkk='%d' \
and gs=%d and ns='%s'",tipsaldo,godn,rekr->SHET.ravno(i));

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
     continue;
    }
   if(kolstr == 0)
     continue;
  deb=kre=0.;
  while(cur.read_cursor(&row) != 0)
   {
    deb+=atof(row[1]);
    kre+=atof(row[2]);
   }

  if(tipsaldo == 0)  /*Счёта с не развернутым сальдо*/
   {
    if(deb >= kre)
     {
      deb-=kre;
      kre=0.;
     }
    else
     {
      kre-=deb;
      deb=0.;
     }
   }

  rekr->DEB.plus(deb,i);
  rekr->KRE.plus(kre,i);

 }

/******************************************/
/*Просматриваем записи и заполняем массивы*/
/******************************************/

sprintf(strsql,"%s\n",gettext("Просматриваем проводки, заполняем массивы"));
iceb_printw(strsql,buffer,view);

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov \
where val=-1 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d'",godn,1,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(2);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено проводок !"),wpredok);
  return(2);
 }

int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5]);
//  strzag(LINES-2,0,kolstr,++kolstr1);
  iceb_pbar(bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&den,&mes,&god,row[0],2);
  shet.new_plus(row[1]);
  kontr.new_plus(row[3]);
  deb=atof(row[4]);   
  kre=atof(row[5]);
  if((i=rekr->SHET.find_r(shet.ravno())) < 0)
   {
    class iceb_u_str repl;
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    repl.plus(strsql);

    sprintf(strsql,"%s %s",gettext("Не найден счёт в массиве счетов !"),shet.ravno());
    repl.ps_plus(strsql);

    iceb_menu_soob(&repl,wpredok);

    continue;
   }

  if((i1=rekr->SHETRS.find_r(shet.ravno())) >= 0)
    i2=rekr->KONTR.find_r(kontr.ravno());
  if(iceb_u_sravmydat(den,mes,god,dn,mn,gn) < 0)
   {
    rekr->DEB.plus(deb,i);
    rekr->KRE.plus(kre,i);
    if(i1 >= 0 && i2 >= 0)
     {
      rekr->DEBRS.plus(deb,i1*rekr->SHETRS.kolih()+i2);
      rekr->KRERS.plus(kre,i1*rekr->SHETRS.kolih()+i2);
     }
   }
  else
   {

    rekr->DEB1.plus(deb,i);
    rekr->KRE1.plus(kre,i);
    if(i1 >= 0 && i2 >= 0)
     {
      rekr->DEBRS1.plus(deb,i1*rekr->SHETRS.kolih()+i2);
      rekr->KRERS1.plus(kre,i1*rekr->SHETRS.kolih()+i2);
     }
   }
 }
return(0);
}

/****************************************/
/*Счёт с развернутым сальдо              */
/*****************************************/
void rasbnsrs(short i, //Номер счета в массиве счетов
const char *sh,
const char *nash,  //Наименование счета
int vids,  //0-счет 1-субсчет
int sb, //0-по счетам 1-по субсчетам
short gn, //Год начала
class saldonbs_data *rekr,
double *ideb,double *ikre,
double *ideb1,double *ikre1,double *ideb2,double *ikre2,
time_t vrem,
GtkWidget *wpredok)
{
short           i1,i2;
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
double		db=0.,kr=0.,db1=0.,kr1=0.;
double		deb=0.,kre=0.,deb1=0.,kre1=0.,deb2=0.,kre2=0.;
short		godn;
short	vplsh=1; /*0-двух уровневый план счетов 1-многоуровневый*/

/*
printw("Обрабатываем счёт с развернутым сальдо.\n");
printw("Счёт %s\n",sh);
refresh();
*/

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

if(iceb_poldan("Многопорядковый план счетов",strsql,"nastb.alx",wpredok) == 0)
 if(iceb_u_SRAV("Вкл",strsql,1) != 0)
  vplsh=0;
  
/*printw("%d %s %s rs\n",i,sh,nash);*/
if(vplsh == 1)
 sprintf(strsql,"select kodkon,ns from Skontr where ns='%s'",sh);
if(vplsh == 0 && sb == 0)
 sprintf(strsql,"select kodkon,ns from Skontr where ns like '%s%%'",sh);
if(vplsh == 0 && sb == 1)
 sprintf(strsql,"select kodkon,ns from Skontr where ns='%s'",sh);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
int kolshetrs=rekr->SHETRS.kolih();
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {

  /*Определяем стартовое сальдо по организации*/

   sprintf(strsql,"select deb,kre,ns from Saldo where kkk='1' and gs=%d and \
ns like '%s' and kodkon='%s'",godn,row[1],row[0]);

   db=kr=db1=kr1=0.;
   
   SQLCURSOR cur1;
   if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
     continue;
    }
   while(cur1.read_cursor(&row1) != 0)
    {
     db+=atof(row1[0]);
     kr+=atof(row1[1]);
    }
   i2=rekr->KONTR.find_r(row[0]);
   i1=rekr->SHETRS.find_r(row[1]);
//   printw("i1=%d i2=%d kolshetrs=%d\n",i1,i2,kolshetrs);
   
   if(i1 >= 0 && i2 >= 0)
    {

     db+=rekr->DEBRS.ravno(i1*kolshetrs+i2);
     kr+=rekr->KRERS.ravno(i1*kolshetrs+i2);
     db1+=rekr->DEBRS1.ravno(i1*kolshetrs+i2);
     kr1+=rekr->KRERS1.ravno(i1*kolshetrs+i2);
    }

//   printw("db=%.2f kr=%.2f db1=%.2f kr1=%.2f\n",
//   db,kr,db1,kr1);
//   OSTANOV();
   if(db > kr)
    {
     deb+=db-kr;
    }
   else
    {
     kre+=kr-db;
    }

   deb1+=db1;
   kre1+=kr1;


   if(db+db1 > kr+kr1)
    {
     if((db+db1)-(kr+kr1) != 0.)
      {
       deb2+=(db+db1)-(kr+kr1);
       sprintf(strsql,"insert into Saldo \
values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
       "1",0,sh,row[0],(db+db1)-(kr+kr1),0.,iceb_getuid(wpredok),vrem);   

       iceb_sql_zapis(strsql,0,0,wpredok);
      }
    }
   else
    {
     if((kr+kr1)-(db+db1) != 0.)
      {
       kre2+=(kr+kr1)-(db+db1);
       sprintf(strsql,"insert into Saldo \
values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
       "1",0,sh,row[0],0.,(kr+kr1)-(db+db1),iceb_getuid(wpredok),vrem);   
       iceb_sql_zapis(strsql,0,0,wpredok);
      }
    }
 }

*ideb+=deb;
*ikre+=kre;
*ideb1+=deb1;
*ikre1+=kre1;
*ideb2+=deb2;
*ikre2+=kre2;

}
