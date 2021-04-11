/* $Id: b_nadra.c,v 1.10 2013/08/13 06:09:31 sasa Exp $ */
/*18.03.2012	10.05.2001	Белых А.И.	b_nadra.c
Получение файла с платежными поручениями для банка "НАДРА

        Импорт платежных документов выполняется из файла
PLATDOC\EXTERNAL.FLS, структура которого описана ниже.

        Структура заголовка PLATDOC\EXTERNAL.FLS

позиция длина   содержание

1       124     Не используется
125     6       Количество документов в файле
131     166     Не используется
297     2       Символы перевода строки ( 0D 0A )
299

        Структура записи PLATDOC\EXTERNAL.FLS

позиция длина   содержание

1       9       МФО     клиента
10      14      Счет    клиента
24      9       МФО     корреспондента
33      14      Счет    корреспондента
47      1       дебет/кредит                    (='2')
48      16      Сумма в копейках
64      2       Тип     документа               (='1')
66      10      Номер   документа
76      3       Код валюты                      (='980')
79      12      Не используется
91      38      Наименование корреспондента
129     38      Наименование клиента
167     160     Назначение платежа
327     65      Не используется
392     14      ОКПО клиента
406     14      ОКПО корреспондента
420     173     Не используется
593     2       Символы перевода строки ( 0D 0A )
595

Пример записи

                                                                                                                            000002                                                                                                                                                                      
   321983 2530300000519   322885   260020632032           14800 1      2726980            КУ iм. Тараса Шевченка                ООО "КЛО"                             Передплата за трос буксiр,лампу.Згiдно рахунку 08 вiд 26.10.99р.                код-1151,ПДВ-24,67                                                                                                                                     02070944      21563629                                                                                                                                                                             
   321983 2530300000519   322885   260020632032           14800 1      2726980            КУ iм. Тараса Шевченка                ООО "КЛО"                             Передплата за трос буксiр,лампу.Згiдно рахунку 08 вiд 26.10.99р.                код-1151,ПДВ-24,67                                                                                                                                     02070944      21563629                                                                                                                                                                             
*/

#include        <errno.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern class REC rec;
extern SQL_baza bd;

int b_nadra(const char *tabl,GtkWidget *wpredok)
{
char		imaf1[32];
FILE		*ff1;
int		koldok=0;
class iceb_u_str koment("");
char		strsql[512];
short		d,m,g;
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

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
koldok=kolstr;

sprintf(imaf1,"plat.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return(1);
 }

fprintf(ff1,"%124s%06d%166s\n"," ",koldok," ");

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  if(readpdokw(tabl,g,row[1],wpredok) != 0)
     continue;

  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);
  
  fprintf(ff1,"%9.9s%14.14s%9.9s%14.14s%1.1s%16.f%2.2s%-10.10s%3.3s\
%12.12s%*.*s%*.*s%-*.*s%65.65s%14.14s%14.14s%173.173s\n",
  rec.mfo.ravno(),
  rec.nsh.ravno(),
  rec.mfo1.ravno(),
  rec.nsh1.ravno(),
  "2",
  rec.sumd*100,
  "1",
  row[1],
  "980",
  " ",
  iceb_u_kolbait(38,rec.naior.ravno()),iceb_u_kolbait(38,rec.naior.ravno()),rec.naior.ravno(),
  iceb_u_kolbait(38,rec.naior1.ravno()),iceb_u_kolbait(38,rec.naior1.ravno()),rec.naior1.ravno(),
  iceb_u_kolbait(160,koment.ravno()),iceb_u_kolbait(160,koment.ravno()),koment.ravno(),
  " ",
  rec.kod.ravno(),
  rec.kod1.ravno(),
  " ");
 }
fclose(ff1);
iceb_perecod(2,imaf1,wpredok);
return(0);
}
