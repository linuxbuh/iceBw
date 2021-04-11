/*$Id: b_ukrinbank.c,v 1.13 2013/09/26 09:46:46 sasa Exp $*/
/*18.03.2012	21.08.2002	Белых А.И.	b_ukrinbank.c

Структура файла экспорта информации Укринбанка

МФО Дебет             N6
Бал. счёт             N14
Счет 2 порядка        S3   три пробела
Код ЕДРПОУ            S10
Найменування          S38
МФО Кредит            N6
Бал. счёт             N14
Счет 2 порядка        S3   три пробела
Код ЕДРПОУ            S10
Найменування          S38
Сумма                 N18.2
ICO платежа           S3     UAH,USD...
Дата проведено банком ггггммдд
Дата пост. в банк     ггггммдд  дата передачи в банк
Код операции          N3    210-внешний платеж 110-внутри банка
Тип документа         N2    1-платежка
N документа           STRING(10)
Дата документа        ггггммдд
Назначение платежа    S160
Дебет/Кредит          S1   (0-дебет 1-кредит) у нас всегда 1

Телефон 52-06-07

*/
#include        <errno.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern class REC rec;
extern SQL_baza bd;


int b_ukrinbank(const char *tabl,GtkWidget *wpredok)
{
char		imaf[64];
FILE		*ff;
class iceb_u_str koment("");
char		strsql[512];
short		d,m,g;
class iceb_u_str kodop("");
struct  tm      *bf;
time_t          tmm;

int kolstr=0;
class SQLCURSOR cur;
SQL_str row;

sprintf(strsql,"select datd,nomd from %s where vidpl='1'",tabl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного документа для передачи в банк!"),wpredok);
  return(1);
 }


/*
printw(gettext("Запись документа N%s в файл для передачи.\n"),rec.nomdk);
refresh();
*/
time(&tmm);
bf=localtime(&tmm);
short dt=bf->tm_mday;
short mt=bf->tm_mon+1;
short gt=bf->tm_year+1900;

sprintf(imaf,"plat.txt");
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  if(readpdokw(tabl,g,row[1],wpredok) != 0)
     continue;

  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);
  if(iceb_u_SRAV(rec.mfo.ravno(),rec.mfo1.ravno(),0) == 0)
    kodop.new_plus("110");
  else
    kodop.new_plus("210");

  fprintf(ff,"%-6s%-14s%-3s%-10s%-*.*s%-6s%-14s%-3s%-10s%-*s%18.2f\
%-3s%04d%02d%02d%04d%02d%02d\
%-3s%2s%-*s%04d%02d%02d%-*.*s%1s\n",
  rec.mfo.ravno(),
  rec.nsh.ravno(),
  " ",
  rec.kodor.ravno(),
  iceb_u_kolbait(38,rec.naior.ravno()),iceb_u_kolbait(38,rec.naior.ravno()),rec.naior.ravno(),
  rec.mfo1.ravno(),
  rec.nsh1.ravno(),
  " ",
  rec.kodor1.ravno(),
  iceb_u_kolbait(38,rec.naior1.ravno()),rec.naior1.ravno(),
  rec.sumd,
  "UAH",
  0,0,0,
  gt,mt,dt,
  kodop.ravno(),
  "1",
  iceb_u_kolbait(10,row[1]),
  row[1],
  g,m,d,
  iceb_u_kolbait(160,koment.ravno()),iceb_u_kolbait(160,koment.ravno()),koment.ravno(),
  "1");
  

 }

fclose(ff);
iceb_perecod(2,imaf,wpredok);
return(0);
}
