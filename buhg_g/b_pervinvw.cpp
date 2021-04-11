/*$Id: b_pervinvw.c,v 1.10 2013/08/13 06:09:31 sasa Exp $*/
/*18.03.2012	25.03.2005	Белых А.И.	b_pervinvw.c
Подготовка файла платёжек для Первого инвесиционного банка
Если вернули 0- всё впорядке

Формат записи:

1  - Номер строки в файле (Начиная с 1)
2  - Статус докумета (всегда 0)
3  - Дебет МФО
4  - Дебет счёт
5  - Дебет ОКПО
6  - Кредит МФО
7  - Кредит счёт
8  - Кредит ОКПО
9  - Дата документа (дд.мм.гггг)
10 - Номер документа
11 - Название плательщика
12 - Название получателя
13 - Сумма документа
14 - Дата валютирования
15 - Дата акцепта
16 - Назначение платежа
17 - Валюта (по умолчанию 980, если не заполнено)
18 - Информационный (значения:F,T. По умолчанию F, если не заполнено
19 - Код назначения платежа НБУ (по умолчанию 0, если не заполнено)

*/

#include        <errno.h>
#include "buhg_g.h"
#include        "dok4w.h"

extern class REC rec;
extern SQL_baza bd;

int b_pervinvw(const char *tabl,GtkWidget *wpredok)
{

char		imaf1[32];
FILE		*ff1;
class iceb_u_str koment("");
char		strsql[512];
short		d,m,g;
short dv=rec.dv ,mv=rec.mv,gv=rec.gv;
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

//printw("b_brokbiznes\n");
//refresh();
if(dv == 0)
 {
  iceb_u_poltekdat(&dv,&mv,&gv);
 }


sprintf(imaf1,"plat.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return(1);
 }

int nomstr=0;

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  if(readpdokw(tabl,g,row[1],wpredok) != 0)
     continue;


  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);
  nomstr++;  
  fprintf(ff1,"%d|0|%s|%s|%s|%s|%s|%s|%02d.%02d.%d|%s|%s|%s|%.2f|%02d.%02d.%04d||%s|980|\n",
  nomstr,
  rec.mfo.ravno(),
  rec.nsh.ravno(),
  rec.kod.ravno(),
  rec.mfo1.ravno(),
  rec.nsh1.ravno(),
  rec.kod1.ravno(),
  d,m,g,
  row[1],
  rec.naior.ravno(),
  rec.naior1.ravno(),
  rec.sumd,
  dv,mv,gv,
  koment.ravno());


 }

fclose(ff1);
iceb_perecod(2,imaf1,wpredok);
return(0);

}
