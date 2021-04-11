/* $Id: rasnasw.c,v 1.13 2013/11/05 10:51:25 sasa Exp $ */
/*12.07.2015    09.07.1996      Белых А.И.      rasnasw.c
Распечатка истории настройки по заданному инвентарному номеру
и подразделению
*/
#include        <errno.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void rasnasw(int in,GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row;
char		strsql[512];
FILE		*ff;
char		imaf[64];
class iceb_u_str naim("");
SQLCURSOR cur;

//Читаем наименование инвентарного номера
sprintf(strsql,"select naim from Uosin where innom=%d",in);
if(sql_readkey(&bd,strsql,&row,&cur) > 0)
  naim.new_plus(row[0]); 

sprintf(strsql,"select * from Uosinp where innom=%d \
order by god,mes asc",in);
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
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),wpredok);
  return;
 }

sprintf(imaf,"uosnas%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

iceb_zagolov(gettext("Список изменений по инвентарному номеру"),ff,wpredok);

fprintf(ff,"%s: %d %s\n",
gettext("Инвентарный номер"),in,naim.ravno());

fprintf(ff,"\
------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
  Дата | Счёт  |Шифр  |Шифр анал.|  Группа |Поправочный коэффици.|Состояние| Номерной |Кто зап.\n\
       | учёта |затрат|  учёта   |Нал.|Бух.|Налоговый |Бухгалтер.|объекта  |   знак   |\n"));
/*
1234567 1234567 123456 1234567890 1234 1234 1234567590 1234567890 123456789 1234567890
*/

fprintf(ff,"\
------------------------------------------------------------------------------------------------\n");
class iceb_u_str sostob("");

while(cur.read_cursor(&row) != 0)
 {
  if(row[8][0] == '0')
   sostob.new_plus(gettext("Бух+Нал+"));
  if(row[8][0] == '1')
   sostob.new_plus(gettext("Бух-Нал-"));
  if(row[8][0] == '2')
   sostob.new_plus(gettext("Бух+Нал-"));
  if(row[8][0] == '3')
   sostob.new_plus(gettext("Бух-Нал+"));
   
  fprintf(ff,"%2s.%s %-*s %-*s %-*s %-*s %-*s %10.10g %10.10g %-*s %-*s %s %s\n",
  row[1],
  row[2],
  iceb_u_kolbait(7,row[3]),row[3],
  iceb_u_kolbait(6,row[4]),row[4],
  iceb_u_kolbait(10,row[5]),row[5],
  iceb_u_kolbait(4,row[6]),row[6],
  iceb_u_kolbait(4,row[12]),row[12],
  atof(row[7]),
  atof(row[13]),
  iceb_u_kolbait(9,sostob.ravno()),sostob.ravno(),
  iceb_u_kolbait(10,row[9]),row[9],
  iceb_kszap(row[10],wpredok),
  iceb_u_vremzap(row[11]));

 }
iceb_podpis(ff,wpredok);
fclose(ff);


class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf);
naimf.plus(gettext("Список изменений по инвентарному номеру"));

iceb_ustpeh(imaf,3,wpredok);

iceb_rabfil(&imafil,&naimf,wpredok);

}




