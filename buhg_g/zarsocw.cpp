/* $Id: zarsocw.c,v 1.27 2014/02/28 05:21:01 sasa Exp $ */
/*05.05.2020	03.04.2000	Белых А.И.	zarsocw.c
Вычисление необходимых соц отчислений на фонд зарплаты и запись их в таблицу
*/
#include        "buhg_g.h"

void zarsocw_esv(short mr,short gr,int tabn,GtkWidget*);

extern double   okrg; /*Округление*/
extern char	*shetb; /*Бюджетные счета*/
extern short    *kodmp;   /*Коды материальной помощи*/
extern char     *p_shet_inv; //счет для начисления пенсионных отчислений на фонд зарплаты для инвалидов (если для них нежен другой счет)
extern float     p_tarif_inv; //тариф (процент) отчисления в пенсионный фонд инвалидами
extern short    *kodbl;  /*Код начисления больничного*/
extern class iceb_u_str kod_dop_nah_bol; /*коды начилений дополнительно входящие в расчёт удержания ЕСВ с больничного*/
extern SQL_baza bd;

extern int kodf_esv;
extern int kodf_esv_bol;
extern int kodf_esv_bol_inv;
extern int kodf_esv_inv;
extern int kodf_esv_dog;
extern int kodf_esv_vs;

extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_bol_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/
extern int kod_esv_vs;
extern class iceb_u_str kod_zv_gr;

void zarsocw(short mr,short gr,int tabn,GtkWidget *wpredok)
{
zar_pr_startw(3,tabn,mr,gr,wpredok);

if(iceb_u_sravmydat(1,mr,gr,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) >= 0)
 zarsocw_esv(mr,gr,tabn,wpredok);
else
 {
  char strsql[512];
  sprintf(strsql,"%s-Начисления на фонд оплаты до даты %d.%d.%d не выполняются!\n",__FUNCTION__,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV);
  zar_pr_insw(strsql,wpredok);
 }
zar_pr_endw(wpredok);

}
/************************************************/
void zarsocw_esv(short mr,short gr,int tabn,GtkWidget *wpredok)
{
char strsql[512];
SQLCURSOR cur;
int kolstr=0;
SQL_str row;
int knah=0;
double suma=0.;
float dproc_sr=0.;
float dproc_bl=0.;
float dproc_in=0.;
float dproc_dg=0.;
float dproc_vs=0.;

float proc_vr=0.; /*процент взятый в расчёт*/
int kodf_esv_vr=0;
double suma_bol=0.;
double suma_bolb=0.;
double suma_vr=0.;
double suma_vrb=0.;

class iceb_u_str knvr_vr("");
class iceb_u_str shet_vr("");
class iceb_u_str shetbu_vr(""); /*взятый в расчёт*/

class iceb_u_str knvr("");
class iceb_u_str shet("");
class iceb_u_str shetbu("");

class iceb_u_str knvr_bol("");
class iceb_u_str shet_bol("");
class iceb_u_str shetbu_bol("");

class iceb_u_str knvr_inv("");
class iceb_u_str shet_inv("");
class iceb_u_str shetbu_inv("");

class iceb_u_str knvr_dog("");
class iceb_u_str shet_dog("");
class iceb_u_str shetbu_dog("");

class iceb_u_str knvr_vs("");
class iceb_u_str shet_vs("");
class iceb_u_str shetbu_vs("");


//Проверяем есть ли удержание
/*****************************************
sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv);
if(iceb_sql_readkey(strsql,wpredok) <=  0)
 {  
  sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
  datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_bol);
  if(iceb_sql_readkey(strsql,wpredok) <=  0)
   {
    sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
    datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_inv);
    if(iceb_sql_readkey(strsql,wpredok) <=  0)
    {
     sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
     datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_dog);
     if(iceb_sql_readkey(strsql,wpredok) <=  0)
     {
      sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
      datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_vs);
      if(iceb_sql_readkey(strsql,wpredok) <=  0)
       {
        sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
        datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_bol_inv);
        if(iceb_sql_readkey(strsql,wpredok) <=  0)
         {
          sprintf(strsql,"У работника нет кода удержания социального взноса в списке его удержаний\n");
          zar_pr_insw(strsql,wpredok);
          return;
         }
       }
     }
    }     
  }
 }
******************************/
if(kodf_esv == 0)
 {
  sprintf(strsql,"Код фонда единого социального взноса равен нолю!\n");
  zar_pr_insw(strsql,wpredok);
  return; 
 }
kodf_esv_vr=kodf_esv;

if(kodf_esv_bol == 0)
 {
  sprintf(strsql,"Код фонда единого социального взноса для больничных равен нолю!\n");
  zar_pr_insw(strsql,wpredok);
  return; 
 }



/*Читаем коды не входящие в расчёт и счёта*/
sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"Не найдены код фонда %d\n",kodf_esv);
  zar_pr_insw(strsql,wpredok);
  return;
 }

shet.new_plus(row[0]);
knvr.new_plus(row[1]);
shetbu.new_plus(row[2]);

if(shet.getdlinna() <= 1)
 {
  sprintf(strsql,"Не введён счёт для фонда %d!\n",kodf_esv);
  zar_pr_insw(strsql,wpredok);
  return;
 }

sprintf(strsql,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
kodf_esv,knvr.ravno(),shetbu.ravno());
zar_pr_insw(strsql,wpredok);

int kod_bol=kodf_esv_bol;
int metka_inv=zarprtnw(mr,gr,tabn,2,wpredok);

if(metka_inv == 1)
 {
  if(kodf_esv_bol_inv == 0)
   {
    sprintf(strsql,"Код фонда единого социального взноса c больничных для инвалидов равен нолю!\n");
    zar_pr_insw(strsql,wpredok);
    return; 
   }

  kod_bol=kodf_esv_bol_inv; 

 }
 
/*Читаем коды не входящие в расчёт и счёта*/
sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kod_bol);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"Не найдены код фонда %d\n",kod_bol);
  zar_pr_insw(strsql,wpredok);
  return;
 }

shet_bol.new_plus(row[0]);
knvr_bol.new_plus(row[1]);
shetbu_bol.new_plus(row[2]);
if(shet_bol.getdlinna() <= 1)
 {
  sprintf(strsql,"Не введён счёт для фонда %d!\n",kod_bol);
  zar_pr_insw(strsql,wpredok);
  return;
 }



sprintf(strsql,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
kod_bol,knvr_bol.ravno(),shetbu_bol.ravno());
zar_pr_insw(strsql,wpredok);

/*Читаем коды не входящие в расчёт и счёта*/
sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv_inv);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"Не найдены код фонда %d\n",kodf_esv_inv);
  zar_pr_insw(strsql,wpredok);
  return;
 }

shet_inv.new_plus(row[0]);
knvr_inv.new_plus(row[1]);
shetbu_inv.new_plus(row[2]);

if(shet_inv.getdlinna() <= 1)
 {
  sprintf(strsql,"Не введён счёт для фонда %d!\n",kodf_esv_inv);
  zar_pr_insw(strsql,wpredok);
  return;
 }

sprintf(strsql,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
kodf_esv_inv,knvr_inv.ravno(),shetbu_inv.ravno());
zar_pr_insw(strsql,wpredok);

/*Читаем коды не входящие в расчёт и счёта*/
sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv_dog);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"Не найдены код фонда %d\n",kodf_esv_dog);
  zar_pr_insw(strsql,wpredok);
  return;
 }

shet_dog.new_plus(row[0]);
knvr_dog.new_plus(row[1]);
shetbu_dog.new_plus(row[2]);

if(shet_dog.getdlinna() <= 1)
 {
  sprintf(strsql,"Не введён счёт для фонда %d!\n",kodf_esv_dog);
  zar_pr_insw(strsql,wpredok);
  return;
 }

sprintf(strsql,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
kodf_esv_dog,knvr_dog.ravno(),shetbu_dog.ravno());
zar_pr_insw(strsql,wpredok);

/*Читаем коды не входящие в расчёт и счёта*/
if(kod_esv_vs != 0)
 {
  sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv_vs);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"Не найдены код фонда %d\n",kodf_esv_vs);
    zar_pr_insw(strsql,wpredok);
    return;
   }

  shet_vs.new_plus(row[0]);
  knvr_vs.new_plus(row[1]);
  shetbu_vs.new_plus(row[2]);

  sprintf(strsql,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
  kodf_esv_vs,knvr_vs.ravno(),shetbu_vs.ravno());
  zar_pr_insw(strsql,wpredok);
 }
 

/*Читаем действующие на дату расчёта проценты*/
sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv,mr,gr);
  zar_pr_insw(strsql,wpredok);
  return;
 }
dproc_sr=atof(row[0]);
/*Читаем действующие на дату расчёта проценты*/
sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kod_bol,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kod_bol,mr,gr);
  zar_pr_insw(strsql,wpredok);
  return;
 }
dproc_bl=atof(row[0]);



  sprintf(strsql,"\
Процент с работника:%.2f\n\
Процент с больничного:%.2f\n",
dproc_sr,
dproc_bl);
 zar_pr_insw(strsql,wpredok);

if(shetb != NULL)
 {
    sprintf(strsql,"\
Бюджетные счета:%s\n",
   shetb);
 zar_pr_insw(strsql,wpredok); 
 }
proc_vr=dproc_sr;
knvr_vr.new_plus(knvr.ravno());
shet_vr.new_plus(shet.ravno());
shetbu_vr.new_plus(shetbu.ravno());




if(metka_inv == 1)
 {
  sprintf(strsql,"Находится в списке инвалидов процент %.2f\n",proc_vr);
  zar_pr_insw(strsql,wpredok);
   if(kodf_esv_inv == 0)
   {
    sprintf(strsql,"Код фонда единого социального взноса для инвалидов равен нолю!\n");
    zar_pr_insw(strsql,wpredok);
    return; 
   }
  /*Читаем действующие на дату расчёта проценты*/
  sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv_inv,gr,mr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv_inv,mr,gr);
    zar_pr_insw(strsql,wpredok);
    return;
   }
  dproc_in=atof(row[0]);

  kodf_esv_vr=kodf_esv_inv;
  proc_vr=dproc_in;
  knvr_vr.new_plus(knvr_inv.ravno());
  shet_vr.new_plus(shet_inv.ravno());
  shetbu_vr.new_plus(shetbu_inv.ravno());
 }

int metka_dog=zarprtnw(mr,gr,tabn,3,wpredok); //Метка работника работающего по договору
if(metka_dog == 1)
 {
  sprintf(strsql,"Находится в списке работающих по договорам подряда\n");
  zar_pr_insw(strsql,wpredok);
  if(kodf_esv_dog == 0)
   {
    sprintf(strsql,"Код фонда единого социального взноса для договоров равен нолю!\n");
    zar_pr_insw(strsql,wpredok);
    return; 
   }
  /*Читаем действующие на дату расчёта проценты*/
  sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv_dog,gr,mr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv_dog,mr,gr);
    zar_pr_insw(strsql,wpredok);
    return;
   }

  dproc_dg=atof(row[0]);

  kodf_esv_vr=kodf_esv_dog;
  proc_vr=dproc_dg;
  knvr_vr.new_plus(knvr_dog.ravno());
  shet_vr.new_plus(shet_dog.ravno());
  shetbu_vr.new_plus(shetbu_dog.ravno());
 }

int metka_sovm=0;
sprintf(strsql,"select zvan,sovm from Zarn where tabn=%d and god=%d and mes=%d",tabn,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  metka_sovm=atoi(row[1]);  
  if(atoi(row[0]) != 0)
   if(iceb_u_proverka(kod_zv_gr.ravno(),row[0],0,0) != 0)
    {

     sprintf(strsql,"Военный %s != %s\n",kod_zv_gr.ravno(),row[0]);
     zar_pr_insw(strsql,wpredok);
     if(kodf_esv_vs == 0)
      {
       sprintf(strsql,"Код фонда единого социального взноса для военных равен нолю!\n");
       zar_pr_insw(strsql,wpredok);
       return; 
      }

      /*Читаем действующие на дату расчёта проценты*/
     sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv_vs,gr,mr);
     if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
      {
       sprintf(strsql,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv_dog,mr,gr);
       zar_pr_insw(strsql,wpredok);
       return;
      }

     dproc_vs=atof(row[0]);
     kodf_esv_vr=kodf_esv_vs;
     proc_vr=dproc_vs;
     knvr_vr.new_plus(knvr_vs.ravno());
     shet_vr.new_plus(shet_vs.ravno());
     shetbu_vr.new_plus(shetbu_vs.ravno());
    }
 }
sprintf(strsql,"Берём в расчёт:\n\
Код фонда:%d\n\
Процент:%.2f\n\
Коды не входящие в расчёт:%s\n\
Счёт хозрасчётный:%s\n\
Счёт бюджетный:%s\n",
kodf_esv_vr,
proc_vr,
knvr_vr.ravno(),
shet_vr.ravno(),
shetbu_vr.ravno());

zar_pr_insw(strsql,wpredok);

class zar_read_tn1h nastr;
zar_read_tn1w(1,mr,gr,&nastr,wpredok); /*читаем до блокировки таблиц*/

/*Блокируем таблицу*/
sprintf(strsql,"LOCK TABLES Zarsocz WRITE,Zarsoc WRITE,Zarp WRITE,Zarpr WRITE,Alx READ,icebuser READ,Kartb READ");
class iceb_lock_tables flag(strsql);
  


/*Удаляем все записи если они есть*/
sprintf(strsql,"delete from Zarsocz where datz='%d-%d-01' and tabn=%d",gr,mr,tabn);

iceb_sql_zapis(strsql,1,0,wpredok);


sprintf(strsql,"select knah,suma,shet,godn,mesn from Zarp where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and tabn=%d and \
prn='1' and suma <> 0. order by knah asc",gr,mr,gr,mr,tabn);


/*
printw("\n%s\n",strsql);
refresh();
*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"Не найдено ни одного начисления!");

  zar_pr_insw(strsql,wpredok);
  return;

 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
  zar_pr_insw(strsql,wpredok);

  knah=atoi(row[0]);

  suma=atof(row[1]);

  if(provkodw(kodbl,knah) >= 0 || iceb_u_proverka(kod_dop_nah_bol.ravno(),knah,0,1) == 0)
   {
    if(iceb_u_proverka(knvr_bol.ravno(),row[0],0,1) == 0)
     continue;
    if(iceb_u_proverka(shetb,row[2],0,1) == 0)
     suma_bolb+=suma;
    else
     suma_bol+=suma;
   }
  else
   {
    if(iceb_u_proverka(knvr_vr.ravno(),row[0],0,1) == 0)
     continue;
    if(iceb_u_proverka(shetb,row[2],0,1) == 0)
      suma_vrb+=suma;
    else
      suma_vr+=suma;
   }
 }




sprintf(strsql,"suma_bol=%.2f suma_vr=%.2f suma_bolb=%.2f suma_vrb=%.2f proc_vr=%.2f\n",suma_bol,suma_vr,suma_bolb,suma_vrb,proc_vr);
zar_pr_insw(strsql,wpredok);

if(iceb_u_sravmydat(1,mr,gr,1,1,2015) >= 0)
 {
  if(metka_sovm == 0 && metka_dog == 0 && metka_inv == 0) /*не работает совместителем или по договору*/
   if(suma_vr+suma_vrb+suma_bol < nastr.minzar)
    {
     zar_pr_insw("Не ивалид не работает по доовору и не совместитель\n",wpredok);
     sprintf(strsql,"Сумма в расчёт %.2f меньше минимальной зарплаты.\n",suma_vr+suma_vrb+suma_bol);
     zar_pr_insw(strsql,wpredok);

     short dnr=0,mnr=0,gnr=0;
     short dkr=0,mkr=0,gkr=0;
     int metka_rs_mz=0;
     /*Если принят или уволен то начисляется с фактической зарплаты*/
     sprintf(strsql,"select datn,datk from Kartb where tabn=%d",tabn);
     if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      {
       iceb_u_rsdat(&dnr,&mnr,&gnr,row[0],2);
       iceb_u_rsdat(&dkr,&mkr,&gkr,row[1],2);
       if(mnr == mr && gnr == gr)
        metka_rs_mz=1;       
       if(mkr == mr && gkr == gr)
        metka_rs_mz=1;       
      }     

    if(metka_rs_mz == 0)
     {  
      sprintf(strsql,"Расчёт производим от минимальной зарплаты %.2f",nastr.minzar);

      zar_pr_insw(strsql,wpredok);

      sprintf(strsql,"Увеличиваем на сумму %.2f-%.2f-%.2f-%.2f=%.2f\n",
      nastr.minzar,suma_vr,suma_vrb,suma_bol,nastr.minzar-(suma_vr+suma_vrb+suma_bol));
      zar_pr_insw(strsql,wpredok);
      
      suma_vr+=nastr.minzar-(suma_vr+suma_vrb+suma_bol);
     }
    else
     {
      sprintf(strsql,"Принят/уволен с работы. Налог считаем с фактической зарплаты. %02d.%02d.%d-%02d.%02d.%d\n",dnr,mnr,gnr,dkr,mkr,gkr);
      zar_pr_insw(strsql,wpredok);
      
     }
    }
 }
 
if(suma_bol != 0. || suma_bolb != 0.)
 {
  double sumao=suma_bol*dproc_bl/100.;
  double suma_b=suma_bolb*dproc_bl/100.;
  sumao=iceb_u_okrug(sumao,okrg);
  suma_b=iceb_u_okrug(suma_b,okrg);

  class iceb_u_str shet_zap(shet_bol.ravno());

  if(shetbu_bol.getdlinna() > 1)
   shet_zap.plus(",",shetbu_bol.ravno());
     
  sprintf(strsql,"insert into Zarsocz (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb) \
values ('%d-%d-01',%d,%d,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
  gr,mr,tabn,kod_bol,shet_zap.ravno(),suma_bol+suma_bolb,sumao+suma_b,dproc_bl,iceb_getuid(wpredok),time(NULL),suma_b,suma_bolb);
  iceb_sql_zapis(strsql,1,0,wpredok);
 }


if(suma_vr != 0. || suma_vrb != 0.)
 {
  if(suma_vr >  nastr.max_sum_for_soc) //Максимальная сумма с которой начисляются соц.отчисления
   {
    sprintf(strsql,"Cумма %.2f > %.2f Берём в расчёт %.2f\n",suma_vr, nastr.max_sum_for_soc, nastr.max_sum_for_soc);
    zar_pr_insw(strsql,wpredok);

    suma_vr= nastr.max_sum_for_soc;

   }
  if(suma_vrb >  nastr.max_sum_for_soc) //Максимальная сумма с которой начисляются соц.отчисления
   {
    sprintf(strsql,"Cумма %.2f > %.2f Берём в расчёт %.2f\n",suma_vrb, nastr.max_sum_for_soc, nastr.max_sum_for_soc);
    zar_pr_insw(strsql,wpredok);

    suma_vrb= nastr.max_sum_for_soc;

   }


  double sumao=suma_vr*proc_vr/100.;
  double suma_b=suma_vrb*proc_vr/100.;
  sumao=iceb_u_okrug(sumao,okrg);
  suma_b=iceb_u_okrug(suma_b,okrg);
  class iceb_u_str shet_zap(shet_vr.ravno());

  if(shetbu_vr.getdlinna() > 1)
   shet_zap.plus(",",shetbu_vr.ravno());
  
  sprintf(strsql,"insert into Zarsocz (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb) \
values ('%d-%d-01',%d,%d,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
  gr,mr,tabn,kodf_esv_vr,shet_zap.ravno(),suma_vr+suma_vrb,sumao+suma_b,proc_vr,iceb_getuid(wpredok),time(NULL),suma_b,suma_vrb);
  iceb_sql_zapis(strsql,1,0,wpredok);
 }
}

