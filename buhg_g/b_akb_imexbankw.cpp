/*$Id: b_akb_imexbankw.c,v 1.3 2013/09/26 09:46:46 sasa Exp $*/
/*15.03.2012	15.03.2012	Белых А.И.	b_akb_imexbankw.c



АКБ «ИМЭКСБАНК»
Формат файлов Экспорта и Импорта
Файлы на импорт в Клиент-Банк могут быть только типа «TXT».
Формат «TXT» файла на импорт:
----------------------------------------------------------------------------------
№ | Название поля                |Кол-во символов| Пояснение 
----------------------------------------------------------------------------------
1  Номер документа               1-10       10   Порядковый номер документа
2  Название отправителя          11-70      60   Полное название отправителя документа
3  ОКПО отправителя              71-80      10   ОКПО отправителя
4  МФО банка отправителя         81-90      10   МФО банка отправителя
5  Название банка отправителя    91-150     60   Полное название банка отправителя
6  Р/с отправителя               151-170    20   Расчётный счёт отправителя
7  Название получателя           171-230    60   Полное название получателя
8  ОКПО получателя               231-240    10   ОКПО получателя
9  МФО банка получателя          241-250    10   МФО банка получателя
10 Название банка получателя     251-310    60   Полное название банка получателя
11 Р/с получателя                311-330    20   Расчётный счёт получателя
12 Сумма док-та в копейках       331-350    20   -//-
13 Основание документа           351-511    160  Основание документа – 160 символов
*/
#include        <errno.h>
#include <math.h>
#include "buhg_g.h"
#include        "dok4w.h"

extern struct REC rec;
extern SQL_baza bd;

int b_akb_imexbankw(const char *tabl,GtkWidget *wpredok)
{

char		imaf1[64];
FILE		*ff1;
class iceb_u_str koment("");
char		strsql[1024];
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

sprintf(imaf1,"plat.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return(1);
 }

int suma_int=0;
double suma_double=0.;
double bb=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
/************
  polen(stt,strsql,sizeof(strsql),1,'|');
  rsdat(&d,&m,&g,strsql,1);
  memset(nomerdok,'\0',sizeof(nomerdok));
  polen(stt,nomerdok,sizeof(nomerdok),2,'|');
**************/

  if(readpdokw(tabl,g,row[1],wpredok) != 0)
     continue;


  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);


  //Округляем, чтобы не пропадали копейки
  rec.sumd=iceb_u_okrug(rec.sumd,0.01);

  bb=rec.sumd*100.;
 
  modf(bb,&suma_double);
  suma_int=(int)suma_double;

  fprintf(ff1,"\
%-*.*s\
%-*.*s\
%-*.*s\
%-*.*s\
%-*.*s\
%-20s\
%-*.*s\
%-*.*s\
%-*.*s\
%-*.*s\
%-20s\
%20d\
%-*.*s\n",
  iceb_u_kolbait(10,rec.nomdk.ravno()),
  iceb_u_kolbait(10,rec.nomdk.ravno()),
  rec.nomdk.ravno(),
  iceb_u_kolbait(60,rec.naior.ravno()),
  iceb_u_kolbait(60,rec.naior.ravno()),
  rec.naior.ravno(),
  iceb_u_kolbait(10,rec.kod.ravno()),
  iceb_u_kolbait(10,rec.kod.ravno()),
  rec.kod.ravno(),
  iceb_u_kolbait(10,rec.mfo.ravno()),
  iceb_u_kolbait(10,rec.mfo.ravno()),
  rec.mfo.ravno(),
  iceb_u_kolbait(60,rec.naiban.ravno()),
  iceb_u_kolbait(60,rec.naiban.ravno()),
  rec.naiban.ravno(),
  rec.nsh.ravno(),
  iceb_u_kolbait(60,rec.naior1.ravno()),
  iceb_u_kolbait(60,rec.naior1.ravno()),
  rec.naior1.ravno(),
  iceb_u_kolbait(10,rec.kod1.ravno()),
  iceb_u_kolbait(10,rec.kod1.ravno()),
  rec.kod1.ravno(),
  iceb_u_kolbait(10,rec.mfo1.ravno()),
  iceb_u_kolbait(10,rec.mfo1.ravno()),
  rec.mfo1.ravno(),
  iceb_u_kolbait(60,rec.naiban1.ravno()),
  iceb_u_kolbait(60,rec.naiban1.ravno()),
  rec.naiban1.ravno(),
  rec.nsh1.ravno(),
  suma_int,
  iceb_u_kolbait(160,koment.ravno()),
  iceb_u_kolbait(160,koment.ravno()),
  koment.ravno());
                   
  
 }

fclose(ff1);
iceb_perecod(2,imaf1,wpredok);
return(0);
}
    