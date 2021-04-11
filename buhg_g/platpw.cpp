/*$Id: platpw.c,v 1.27 2014/06/30 06:35:55 sasa Exp $*/
/*03.01.2020	29.09.2001	Белых А.И.	platpw.c
Печать платежного поручения в соответствии с постановлением
правления национального банка Украины от 29 марта 2001г. N135
Зарегистрировано в министерстве юстиции Украины 25 апреля 2001г
за N368/5559
Официальный весник Украины N18 часть 2 за 2001 год страница 848
В разделе 2 пункт 6 оговорена нумерация платежек

Если вернули 0 документ сформирован
             1 нет
*/
#include        "buhg_g.h"
#include        "dok4w.h"

extern short	kolstrdok; //Количество строк между документами
extern class REC rec;
extern short	ots; /*Отступ от левого края*/
extern short	kor; /*Корректор межстрочного растояния*/
extern short    mvs;       /*1-с разбивкой суммы по счетам 0-без*/
extern SQL_baza bd;

int platpw1(char *imaf1,short kp,short nomk,FILE *ff,int viddok,GtkWidget *wpredok);
int platpw127(const char *imaf1,short kp,short nomk,FILE *ff,int viddok,GtkWidget *wpredok);


int platpw(char *imaf1,
short kp, //Количество копий
short nomk, //Номер печатаемой копии
FILE *ff,
int viddok, //0-Платежное поручение 1- платежное требование
GtkWidget *wpredok)
{
if(iceb_u_sravmydat(13,1,2020,rec.dd,rec.md,rec.gd) <= 0)
 return(platpw127(imaf1,kp,nomk,ff,viddok,wpredok));
else
 return(platpw1(imaf1,kp,nomk,ff,viddok,wpredok));

}

/*******************************************************/

int platpw127(const char *imaf1,
short kp, //Количество копий
short nomk, //Номер печатаемой копии
FILE *ff,
int viddok, //0-Платежное поручение 1- платежное требование
GtkWidget *wpredok)
{
class iceb_u_str stroka("");
char		stroka1[512];
class iceb_u_str bank_plat("");
class iceb_u_str bank_poluh("");
char		imaf[112];
short		kolstrok=0;
char		bros[512];
char dt2[128]; //Дата платежа
double		bb=0.;
short dt,mt,gt;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
int kol_ps=0;
iceb_u_poltekdat(&dt,&mt,&gt);

printf("%s\n",__FUNCTION__);

class iceb_u_str mescc("");
iceb_mesc(rec.md,1,&mescc);
sprintf(dt2,"%02d %s %d%s",rec.dd,mescc.ravno(),rec.gd,gettext("г."));

if(iceb_u_sravmydat(3,7,2009,rec.ddi,rec.mdi,rec.gdi) <= 0)
 {
  if(viddok == 0)
    sprintf(imaf,"%s","pp06112019.alx");
  if(viddok == 1)
    sprintf(imaf,"%s","pt06112019.alx");
 }
else
 {
  if(viddok == 0)
    sprintf(imaf,"%s","platp.alx");
  if(viddok == 1)
    sprintf(imaf,"%s","platt.alx");
 }

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

#if 0
#######################
fprintf(ff,"\x12"); //Выключить уплотненный режим
fprintf(ff,"\x1b\x6C%c",ots); /*Установка левого поля*/
fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
fprintf(ff,"\x1B\x78%c",1); //Включить черновой режим печати
##############################
#endif


fprintf(ff,"\x1B\x33%c",20); /*Уменьшаем межстрочный интервал n/216 дюйма*/
fprintf(ff,"\x1b\x45"); //Режим выделенного шрифта
kol_ps=iceb_u_pole2(rec.naz_plat.ravno(),'\n');
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  stroka.new_plus(row_alx[0]);
  kolstrok++;
  switch(kolstrok)
   {

    case 2: //Номер платежного поручения
     fprintf(ff,"\x1B\x33%c",16-kor); /*Уменьшаем межстрочный интервал n/216 дюйма*/
     iceb_u_vstav(&stroka,rec.nomdk.ravno(),30,49,1);
     break;

    case 3: 
      fprintf(ff,"\x1B\x33%c",30-kor); /*Уменьшаем межстрочный интервал n/216 дюйма*/
      break;

    case 4: //Дата документа
     iceb_u_vstav(&stroka,dt2,16,44,1);
     break;

    case 6: //Год
     iceb_u_vstav(&stroka,gt,64,68,1);
     break;

    case 7: //Плательщик
     if(viddok == 0)
       iceb_u_vstav(&stroka,rec.naior.ravno(),8,60,1);
     if(viddok == 1)
       iceb_u_vstav(&stroka,rec.naior1.ravno(),8,60,1);

     break;

    case 8: //Наименование плательщика, которое не поместилось в первой строке
     
     if(viddok == 0)
      if(iceb_u_strlen(rec.naior.ravno()) > 53)
       iceb_u_vstav(&stroka,iceb_u_adrsimv(53,rec.naior.ravno()),0,69,1);
     if(viddok == 1)
      if(iceb_u_strlen(rec.naior1.ravno()) > 53)
       iceb_u_vstav(&stroka,iceb_u_adrsimv(53,rec.naior1.ravno()),0,69,1);

     break;

    case 10: //Код СДЗПОУ
     if(viddok == 0)
       iceb_u_vstav(&stroka,rec.kod.ravno(),11,25,1);
     if(viddok == 1)
       iceb_u_vstav(&stroka,rec.kod1.ravno(),11,25,1);
     break;

    case 12: //Банк плательщика
      if(viddok == 0)
       {
        bank_plat.new_plus(rec.naiban.ravno());
        bank_plat.plus(" в ",rec.gorod.ravno());
       }
      if(viddok == 1)
       {
        bank_plat.new_plus(rec.naiban1.ravno());
        bank_plat.plus(" в ",rec.gorod1.ravno());
       }
      iceb_u_vstav(&stroka,bank_plat.ravno(),14,31,1);
     break;
   
    case 13: //Банк плательщика, код банка, счёт дебета, сумма платежа
     if(iceb_u_strlen(bank_plat.ravno()) > 18)
        iceb_u_vstav(&stroka,iceb_u_adrsimv(18,bank_plat.ravno()),0,31,1);
     if(viddok == 0)
      {
//       iceb_u_vstav(&stroka,rec.mfo.ravno(),37,45,1);
       iceb_u_vstav(&stroka,rec.nsh.ravno(),38,68,1);
      }
     if(viddok == 1)
      {
//       iceb_u_vstav(&stroka,rec.mfo1.ravno(),37,45,1);
       iceb_u_vstav(&stroka,rec.nsh1.ravno(),38,68,1);
      }
     bb=rec.sumd+rec.uslb;

     iceb_u_prnb1(bb,bros);

     iceb_u_vstav(&stroka,bros,72,99,1);

     break;

    case 15: //Получатель 1 строка
     if(viddok == 0)
      iceb_u_vstav(&stroka,rec.naior1.ravno(),10,61,1);
    if(viddok == 1)
      iceb_u_vstav(&stroka,rec.naior.ravno(),10,61,1);
     break;

    case 16: //Получатель 2 строка
     
     if(viddok == 0)
      {
       if(iceb_u_strlen(rec.naior1.ravno()) > 52)
         iceb_u_vstav(&stroka,iceb_u_adrsimv(52,rec.naior1.ravno()),0,61,1);
      }
     if(viddok == 1)
      {
       if(iceb_u_strlen(rec.naior.ravno()) > 52)
         iceb_u_vstav(&stroka,iceb_u_adrsimv(52,rec.naior.ravno()),0,61,1);
      }
     break;

    case 18: //Код
     iceb_u_vstav(&stroka,rec.kod1.ravno(),11,25,1);
     break;


    case 20: /*Банк получателя*/
      if(viddok == 0)
       {
        bank_poluh.new_plus(rec.naiban1.ravno());
        bank_poluh.plus(" в ",rec.gorod1.ravno());
       }
      if(viddok == 1)
       {
        bank_poluh.new_plus(rec.naiban.ravno());
        bank_poluh.plus(" в ",rec.gorod.ravno());
       }
      iceb_u_vstav(&stroka,bank_poluh.ravno(),16,31,1);
      break;

    case 21: //Кредит счёт N
      bros[0]='\0';
      if(rec.uslb != 0. && mvs == 1)
       {
        bb=rec.sumd;
        iceb_u_prnb1(bb,bros);

       }

     if(viddok == 0)
        iceb_u_vstav(&stroka,rec.nsh1.ravno(),38,72,1);
     if(viddok == 1)
        iceb_u_vstav(&stroka,rec.nsh.ravno(),38,72,1);
     iceb_u_vstav(&stroka,bros,72,99,1);
     break;
        
    case 24: //Сумма словами
      bb=rec.sumd+rec.uslb;

      memset(bros,'\0',sizeof(bros));
      iceb_u_preobr(bb,bros,0);
      iceb_u_vstav(&stroka,bros,0,101,1);

      break;

    case 25: //Код нерезидента
      iceb_u_vstav(&stroka,rec.kodnr.ravno(),63,70,1);
      break;
#if 0
####################################33
    case 27: //Назначение платежа
    case 28: //Назначение платежа
    case 29: //Назначение платежа
    case 30: //Назначение платежа
    
      memset(stroka1,'\0',sizeof(stroka1));
      if(FGETSS(stroka1,sizeof(stroka1),ff1.ff) != 0)
       {
        iceb_u_vstav(&stroka,stroka1,0,299,1);
       }
      break;
#######################################
#endif
    case 27: //Назначение платежа
      if(kol_ps == 0)
       {
        //Включение/выключения режима 12 знаков на дюйм
        iceb_u_vstav(&stroka,rec.naz_plat.ravno(),0,299,1 );
       }
      else
       {
        iceb_u_polen(rec.naz_plat.ravno(),stroka1,sizeof(stroka1),1,'\n');
        iceb_u_vstav(&stroka,stroka1,0,299,1 );
       }       
      break;

    case 28: //Назначение платежа
      if(kol_ps >= 1)
       {
        iceb_u_polen(rec.naz_plat.ravno(),stroka1,sizeof(stroka1),2,'\n');
        iceb_u_vstav(&stroka,stroka1,0,299,1 );
       }       
      break;

    case 29: //Назначение платежа
      if(kol_ps >= 2)
       {
        iceb_u_polen(rec.naz_plat.ravno(),stroka1,sizeof(stroka1),3,'\n');
        iceb_u_vstav(&stroka,stroka1,0,299,1 );
       }       
      break;

    case 30: //Назначение платежа
      if(kol_ps >= 3)
       {
        iceb_u_polen(rec.naz_plat.ravno(),stroka1,sizeof(stroka1),4,'\n');
        iceb_u_vstav(&stroka,stroka1,0,299,1 );
       }       
      break;

    default:
     break;
   }
  fprintf(ff,"%s",stroka.ravno());
 }

if(nomk == 0 || nomk == 2)
if(kp > 1)
  for(int i=0; i< kolstrdok ; i++)
     fprintf(ff,"\n");


return(0);
}



/*******************************************************/

int platpw1(char *imaf1,
short kp, //Количество копий
short nomk, //Номер печатаемой копии
FILE *ff,
int viddok, //0-Платежное поручение 1- платежное требование
GtkWidget *wpredok)
{
class iceb_u_str bank_plat("");
class iceb_u_str bank_poluh("");
class iceb_u_str stroka("");
char		stroka1[1024];
char		stroka2[2048];
char		imaf[512];
short		kolstrok=0;
char		bros[512];
char dt2[64]; //Дата платежа
double		bb=0.;
short dt,mt,gt;
int kol_ps=0;
//printf("%s\n",__FUNCTION__);

iceb_u_poltekdat(&dt,&mt,&gt);

class iceb_u_str mescc("");
iceb_mesc(rec.md,1,&mescc);
sprintf(dt2,"%02d %s %d%s",rec.dd,mescc.ravno(),rec.gd,gettext("г."));



if(iceb_u_sravmydat(3,7,2009,rec.ddi,rec.mdi,rec.gdi) <= 0)
 {
  if(viddok == 0)
    sprintf(imaf,"pp03072009.alx");
  if(viddok == 1)
    sprintf(imaf,"pt03072009.alx");
 }
else
 {
  if(viddok == 0)
    sprintf(imaf,"platp.alx");
  if(viddok == 1)
    sprintf(imaf,"platt.alx");
 }

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[1024];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

#if 0
###############################
fprintf(ff,"\x12"); //Выключить уплотненный режим
fprintf(ff,"\x1b\x6C%c",ots); /*Установка левого поля*/
fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
fprintf(ff,"\x1B\x78%c",1); //Включить черновой режим печати
##################################
#endif

if(iceb_u_sravmydat(3,7,2009,rec.ddi,rec.mdi,rec.gdi) > 0)
 if(kp > 1)
  fprintf(ff,"%70s N%d\n",gettext("Экземпляр"),nomk+1);

//fprintf(ff,"\x1B\x55\x31"); /*Включение однонаправленного режима*/
fprintf(ff,"\x1B\x33%c",18); /*Уменьшаем межстрочный интервал n/216 дюйма*/
//fprintf(ff,"\x1B\x41%c",7-kor); /*Уменьшаем межстрочный интервал n/72 дюйма*/
fprintf(ff,"\x1b\x45"); //Режим выделенного шрифта
kol_ps=iceb_u_pole2(rec.naz_plat.ravno(),'\n');
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  kolstrok++;
  stroka.new_plus(row_alx[0]);
  switch(kolstrok)
   {

    case 2: //Номер платежного поручения
     fprintf(ff,"\x1B\x33%c",14-kor); /*Уменьшаем межстрочный интервал n/216 дюйма*/
//     fprintf(ff,"\x1B\x41%c",5-kor); /*Уменьшаем межстрочный интервал n/72 дюйма*/
     iceb_u_vstav(&stroka,rec.nomdk.ravno(),30,49,1 );
     break;

    case 3: 
      fprintf(ff,"\x1B\x33%c",28-kor); /*Уменьшаем межстрочный интервал n/216 дюйма*/
//      fprintf(ff,"\x1B\x41%c",10-kor); /*Уменьшаем межстрочный интервал n/72 дюйма*/
      break;

    case 4: //Дата документа
     iceb_u_vstav(&stroka,dt2,16,44,1 );
     break;

    case 6: //Год
     iceb_u_vstav(&stroka,gt,64,68,1 );
     break;

    case 7: //Плательщик
     if(viddok == 0)
       iceb_u_vstav(&stroka,rec.naior.ravno(),8,60,1 );
     if(viddok == 1)
       iceb_u_vstav(&stroka,rec.naior1.ravno(),8,60,1 );

     break;

    case 8: //Наименование плательщика, которое не поместилось в первой строке
     
     if(viddok == 0)
      if(iceb_u_strlen(rec.naior.ravno()) > 53)
       iceb_u_vstav(&stroka,iceb_u_adrsimv(53,rec.naior.ravno()),1,69,1 );
//       iceb_u_vstav(&stroka,&rec.naior.ravno()[43],1,69,1 );
     if(viddok == 1)
      if(iceb_u_strlen(rec.naior1.ravno()) > 53)
       iceb_u_vstav(&stroka,iceb_u_adrsimv(53,rec.naior1.ravno()),1,69,1 );
//       iceb_u_vstav(&stroka,&rec.naior1.ravno()[43],1,64,1 );

/***************** дата валютирования теперь не печатаестся
     if(rec.dv != 0)
      {
       sprintf(bros,"%02d",rec.dv);
       iceb_u_vstav(&stroka,bros,53,55,1 );

       sprintf(bros,"%02d",rec.mv);
       iceb_u_vstav(&stroka,bros,59,61,1 );

       sprintf(bros,"%d",rec.gv);
       iceb_u_vstav(&stroka,bros,64,68,1 );
      }
     else
      {
       sprintf(bros,"%d",gt);
       iceb_u_vstav(&stroka,bros,64,68,1 );
      }    
********************/
     break;

    case 10: //Код СДЗПОУ
     if(viddok == 0)
       iceb_u_vstav(&stroka,rec.kod.ravno(),11,25,1 );
     if(viddok == 1)
       iceb_u_vstav(&stroka,rec.kod1.ravno(),11,25,1 );
     break;

    case 12: //Банк плательщика
      if(viddok == 0)
       {
//        sprintf(bank_plat,"%s в %s",rec.naiban.ravno(),rec.gorod.ravno());
        bank_plat.new_plus(rec.naiban.ravno());
        bank_plat.plus(" в ",rec.gorod.ravno());
       }
      if(viddok == 1)
       {
//        sprintf(bank_plat,"%s в %s",rec.naiban1.ravno(),rec.gorod1.ravno());
        bank_plat.new_plus(rec.naiban1.ravno());
        bank_plat.plus(" в ",rec.gorod1.ravno());
       }
      iceb_u_vstav(&stroka,bank_plat.ravno(),14,31,1 );
     break;

    case 13: //Банк плательщика, код банка, счёт дебета, сумма платежа
     if(iceb_u_strlen(bank_plat.ravno()) > 18)
        iceb_u_vstav(&stroka,iceb_u_adrsimv(18,bank_plat.ravno()),0,31,1 );
//        iceb_u_vstav(&stroka,&bank_plat[18],0,31,1 );
     if(viddok == 0)
      {
       iceb_u_vstav(&stroka,rec.mfo.ravno(),37,45,1 );
       iceb_u_vstav(&stroka,rec.nsh.ravno(),57,100,1 );
      }
     if(viddok == 1)
      {
       iceb_u_vstav(&stroka,rec.mfo1.ravno(),37,45,1 );
       iceb_u_vstav(&stroka,rec.nsh1.ravno(),57,100,1 );
      }
     bb=rec.sumd+rec.uslb;

     iceb_u_prnb1(bb,bros);

     iceb_u_vstav(&stroka,bros,91,99,1);

     break;

    case 15: //Получатель
     if(viddok == 0)
      iceb_u_vstav(&stroka,rec.naior1.ravno(),10,61,1 );
     if(viddok == 1)
      iceb_u_vstav(&stroka,rec.naior.ravno(),10,61,1 );
     break;

    case 16: //Получатель 2 строка
     
     if(viddok == 0)
      {
       if(iceb_u_strlen(rec.naior1.ravno()) > 52)
         iceb_u_vstav(&stroka,iceb_u_adrsimv(52,rec.naior1.ravno()),0,61,1 );
      }
     if(viddok == 1)
      {
       if(iceb_u_strlen(rec.naior.ravno()) > 52)
         iceb_u_vstav(&stroka,iceb_u_adrsimv(52,rec.naior.ravno()),0,61,1 );
      }
     break;

    case 18: //Код, номер счета за услуги, сумма за услуги
     iceb_u_vstav(&stroka,rec.kod1.ravno(),11,25,1 );
     break;

    case 19: //Кредит счёт N
      bros[0]='\0';
      if(rec.uslb != 0. && mvs == 1)
       {
        bb=rec.sumd;
        iceb_u_prnb1(bb,bros);

       }
     if(viddok == 0)
        iceb_u_vstav(&stroka,rec.nsh1.ravno(),53,100,1 );
     if(viddok == 1)
        iceb_u_vstav(&stroka,rec.nsh.ravno(),53,100,1 );
      iceb_u_vstav(&stroka,bros,87,99,1 );
      break;

    case 20: /*Банк получателя*/
      if(viddok == 0)
       {
//        sprintf(bank_poluh,"%s в м.%s",rec.naiban1.ravno(),rec.gorod1.ravno());
        bank_poluh.new_plus(rec.naiban1.ravno());
        bank_poluh.plus(" в ",rec.gorod1.ravno());
       }
      if(viddok == 1)
       {
//        sprintf(bank_poluh,"%s в м.%s",rec.naiban.ravno(),rec.gorod.ravno());
        bank_poluh.new_plus(rec.naiban.ravno());
        bank_poluh.plus(" в ",rec.gorod.ravno());

       }
      iceb_u_vstav(&stroka,bank_poluh.ravno(),16,31,1 );
      break;

    case 21: //Банк получателя, МФО, счет
      if(iceb_u_strlen(bank_poluh.ravno()) > 16)
        iceb_u_vstav(&stroka,iceb_u_adrsimv(16,bank_poluh.ravno()),0,31,1 );
//        iceb_u_vstav(&stroka,&bank_poluh[16],0,31,1 );
      if(viddok == 0)
       {
        iceb_u_vstav(&stroka,rec.mfo1.ravno(),37,45,1 );
       }
      if(viddok == 1)
       {
        iceb_u_vstav(&stroka,rec.mfo.ravno(),37,45,1 );
       }
      iceb_u_vstav(&stroka,rec.shbzu.ravno(),57,100,1 );
      bros[0]='\0';
      if(rec.uslb != 0. && mvs == 1)
       {
         iceb_u_prnb1(rec.uslb,bros);

       }

      iceb_u_vstav(&stroka,bros,91,99,1 );
       
     break;

    case 24: //Сумма словами
      bb=rec.sumd+rec.uslb;

      memset(bros,'\0',sizeof(bros));
      iceb_u_preobr(bb,bros,0);
      iceb_u_vstav(&stroka,bros,0,101,1 );

      break;

    case 25: //Код нерезидента
      iceb_u_vstav(&stroka,rec.kodnr.ravno(),63,70,1 );
      break;

    case 27: //Назначение платежа
      if(kol_ps == 0)
       {
        //Включение/выключения режима 12 знаков на дюйм
        sprintf(stroka2,"\x1B\x4D%10s%s\x1B\x50","",rec.naz_plat.ravno());
        iceb_u_vstav(&stroka,stroka2,0,299,1 );
       }
      else
       {
        iceb_u_polen(rec.naz_plat.ravno(),stroka1,sizeof(stroka1),1,'\n');
        //Включение/выключения режима 12 знаков на дюйм
        sprintf(stroka2,"\x1B\x4D%10s%s\x1B\x50","",stroka1);
        iceb_u_vstav(&stroka,stroka2,0,299,1 );
       }       
      break;

    case 28: //Назначение платежа
      if(kol_ps >= 1)
       {
        iceb_u_polen(rec.naz_plat.ravno(),stroka1,sizeof(stroka1),2,'\n');
        //Включение/выключения режима 12 знаков на дюйм
        sprintf(stroka2,"\x1B\x4D%10s%s\x1B\x50","",stroka1);
        iceb_u_vstav(&stroka,stroka2,0,299,1 );
       }       
      break;

    case 29: //Назначение платежа
      if(kol_ps >= 2)
       {
        iceb_u_polen(rec.naz_plat.ravno(),stroka1,sizeof(stroka1),3,'\n');
        //Включение/выключения режима 12 знаков на дюйм
        sprintf(stroka2,"\x1B\x4D%10s%s\x1B\x50","",stroka1);
        iceb_u_vstav(&stroka,stroka2,0,299,1 );
       }       
      break;

    case 30: //Назначение платежа
      if(kol_ps >= 3)
       {
        iceb_u_polen(rec.naz_plat.ravno(),stroka1,sizeof(stroka1),4,'\n');
        //Включение/выключения режима 12 знаков на дюйм
//        sprintf(stroka2,"\x1B\x4D%s\x1B\x50",stroka1);
//        iceb_u_vstav(&stroka,stroka2,0,299,1 );
        iceb_u_vstav(&stroka,stroka1,0,299,1 );
       }       
      break;

 
    default:
     break;
   }

  fprintf(ff,"%s",stroka.ravno());

 }

if(nomk == 0 || nomk == 2)
if(kp > 1)
  for(int i=0; i< kolstrdok ; i++)
     fprintf(ff,"\n");

return(0);
}
