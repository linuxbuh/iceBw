/*$Id:$*/
/*18.10.2019	16.10.2010	Белых А.И.	b_stil.cpp

Комплекс «Стиль» - Клиент Банк


           3.6. Импорт финансовых документов клиента из текстового файла
Импорт инициируется из списка финансовых документов клиента. Главное меню-
Документы/Платежные поручения (Платежные требования) - Сервис/Импорт
платежей.
Наименование шаблона импорта - «Iмпорт з текстового файлу+IBAN»
Формат файла - Текстовый файл в кодировке WIN(cp1251)
Каждый финансовый документ в файле импорта состоит из 11 строк. Первые десять
символов каждой строки содержат наименование поля, заканчивающегося символом ':'.
Начиная с одиннадцатого символа, размещается значение поля. Наименования и длины полей
приведены в следующей таблице:

Семантика поля                      Имя поля    Длина поля 

Номер финансового документа**       Номер:      10
Дата финансового документа**        Дата:       10(дд/мм/гггг)
Код валюты финансового документа*** Валюта:     3
Счет клиента***                     Свой Счет:  29
МФО счета клиента***                Свое МФО:   6
Наименование контрагента*           Клиент:     38
Код ЕДРПОУ (ОКПО) контрагента*      ОКПО:       10
Счет контрагента*                   Счет:       29   
МФО счета контрагента*              МФО:        6
Сумма финансового документа*        Сумма:      12(...$$$$.CC)
Назначение платежа*                 Назнач.:    160



Поля обязательные к заполнению отмечены символом *.
Поля, значение которых может генерироваться комплексом, в любых режимах импорта
отмечены символом **.
Поля, значение которых может генерироваться комплексом, только в режиме импорта по
конкретному счету отмечены символом ***.
Строки документа следуют строго в указанном порядке. Каждая строка завершается
символами CR/LF. Строки одного документа могут отделяться от строк другого документа
произвольным количеством строк.
*/


#include        <errno.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern class REC rec;
extern SQL_baza bd;

int b_stilw(const char *tabl,GtkWidget *wpredok)
{
char		imaf1[64];
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
class iceb_fopen ff1;
if(ff1.start(imaf1,"w",wpredok) != 0)
 return(1);

/*Так как в системе предусморено только 38 символов для наименования организации то нужно брать короткое наименование*/

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(readpdokw(tabl,g,row[1],wpredok) != 0)
     continue;
  
  class iceb_u_str naior(rec.naior.ravno());
  class iceb_u_str naior1(rec.naior1.ravno());
 
  if(iceb_u_strlen(rec.naior1.ravno()) > 38)
   {
    if(rec.kodor1.getdlinna() > 1) /*При выборе контрагента из файла код контрагента может быть не определён*/
     {
      naior1.new_plus(iceb_get_pnk(rec.kodor1.ravno(),0,wpredok));

      sprintf(strsql,"%s %s %s\n%s %s",
      gettext("Наименование контрагента"),
      rec.naior1.ravno(),
      gettext("больше 38 символов"),
      gettext("заменили на короткое"),
      naior1.ravno());

      iceb_menu_soob(strsql,wpredok);


     }
   }

  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);

  fprintf(ff1.ff,"\
%-*s:%-*.*s\r\n\
%-*s:%02d.%02d.%d\r\n\
%-*s:%s\r\n\
%-*s:%s\r\n\
%-*s:%s\r\n\
%-*s:%-*.*s\r\n\
%-*s:%s\r\n\
%-*s:%s\r\n\
%-*s:%s\r\n\
%-*s:%-12.2f\r\n\
%-*s:%.*s\r\n\
----------\r\n",


iceb_u_kolbait(9,"Номер:"),
"Номер",
iceb_u_kolbait(9,row[1]),
iceb_u_kolbait(9,row[1]),
row[1],

iceb_u_kolbait(9,"Дата"),
"Дата",
d,m,g,

iceb_u_kolbait(9,"Валюта"),
"Валюта",
"980",

iceb_u_kolbait(9,"Свой Счет"),
"Свой Счет",
rec.nsh.ravno(),

iceb_u_kolbait(9,"Свое МФО"),
"Свое МФО",
rec.mfo.ravno(),

iceb_u_kolbait(9,"Клиент"),
"Клиент",
iceb_u_kolbait(38,rec.naior1.ravno()),
iceb_u_kolbait(38,rec.naior1.ravno()),
rec.naior1.ravno(),

iceb_u_kolbait(9,"ОКПО"),
"ОКПО",
rec.kod1.ravno(),

iceb_u_kolbait(9,"Счет"),
"Счет",
rec.nsh1.ravno(),

iceb_u_kolbait(9,"МФО"),
"МФО",
rec.mfo1.ravno(),

iceb_u_kolbait(9,"Сумма"),
"Сумма",
rec.sumd,

iceb_u_kolbait(9,"Назнач."),
"Назнач.",
iceb_u_kolbait(160,koment.ravno()),
koment.ravno());


  
 }

ff1.end();
iceb_perecod(2,imaf1,wpredok);
return(0);
}
