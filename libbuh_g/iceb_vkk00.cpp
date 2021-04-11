/*$Id: iceb_vkk00.c,v 1.11 2014/02/28 05:21:03 sasa Exp $*/
/*16.11.2016	17.02.2011	Белых А.И.	iceb_vkk00.c
Ввод наименования контрагента с кодом 00
*/
#include "iceb_libbuh.h"


void iceb_vkk00(GtkWidget *wpredok)
{
char strsql[2048];
class iceb_u_str naim_kontr("");

if(iceb_menu_vvod1(gettext("Введите наименование вашей организации"),&naim_kontr,40,"",wpredok) != 0)
 {  
 }
else
 {
  sprintf(strsql,"insert into Kontragent (kodkon,naikon,ktoz,vrem) values ('00','%s',%d,%ld)",naim_kontr.ravno(),getuid(),time(NULL));
  iceb_sql_zapis(strsql,0,1,wpredok);
 }    


/*вводим нормативно-справочную информация*/

/*подсистема главная кника*/
sprintf(strsql,"insert into Glksval values ('%s','%s','',%d,%ld)",gettext("дол"),gettext("Доллар"),iceb_getuid(wpredok),time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Glksval values ('%s','%s','',%d,%ld)",gettext("евр"),gettext("Евро"),iceb_getuid(wpredok),time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Glksval values ('%s','%s','',%d,%ld)",gettext("руб"),gettext("Рубли"),iceb_getuid(wpredok),time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);

/*Подсистема платёжные документы*/
sprintf(strsql,"insert into Opltp (kod,naik,ktoz,vrem) values('%s','%s',%d,%ld)",
gettext("оп"),
gettext("Оплата поставщику"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Opltt (kod,naik,ktoz,vrem) values('%s','%s',%d,%ld)",
gettext("пт"),
gettext("Платёжное требование"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

/*подсистема Учёт кассовых ордеров*/
sprintf(strsql,"insert into Kasop1 (kod,naik,metkapr,ktoz,vrem) values('%s','%s',%d,%d,%ld)",
gettext("ппл"),
gettext("Получено от подотчётного лица"),
1,
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Kasop1 (kod,naik,metkapr,ktoz,vrem) values('%s','%s',%d,%d,%ld)",
gettext("прс"),
gettext("Получено с расчётного счёта"),
1,
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Kasop2 (kod,naik,metkapr,ktoz,vrem) values('%s','%s',%d,%d,%ld)",
gettext("хр"),
gettext("Хозяйственные расходы"),
1,
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Kasop2 (kod,naik,metkapr,ktoz,vrem) values('%s','%s',%d,%d,%ld)",
gettext("ком"),
gettext("Командировочные"),
1,
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Kasop2 (kod,naik,metkapr,ktoz,vrem) values('%s','%s',%d,%d,%ld)",
gettext("зп"),
gettext("Заработная плата"),
1,
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

/*подсистема Материальный учёт*/

sprintf(strsql,"insert into Foroplat (kod,naik,ktoz,vrem) values('%s','%s',%d,%ld)",
gettext("осс"),
gettext("Оплата с расчётного счёта"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Foroplat (kod,naik,ktoz,vrem) values('%s','%s',%d,%ld)",
gettext("он"),
gettext("Оплата наличными"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Prihod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("вп"),
gettext("Внутреннее перемещение"),
iceb_getuid(wpredok),
time(NULL),
1,1);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Prihod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("ОСТ"),
gettext("Стартовый остаток"),
iceb_getuid(wpredok),
time(NULL),
0,1);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Prihod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("поп"),
gettext("Приход от поставщика"),
iceb_getuid(wpredok),
time(NULL),
0,0);

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Prihod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("пип"),
gettext("Приход из производства"),
iceb_getuid(wpredok),
time(NULL),
0,0);

iceb_sql_zapis(strsql,1,1,wpredok);



sprintf(strsql,"insert into Prihod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("иуд"),
gettext("Изменение учётных данных"),
iceb_getuid(wpredok),
time(NULL),
2,1);

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Rashod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("вп"),
gettext("Внутреннее перемещение"),
iceb_getuid(wpredok),
time(NULL),
1,0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Rashod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("рп"),
gettext("Реализация потребителю"),
iceb_getuid(wpredok),
time(NULL),
0,0);

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Rashod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("рвп"),
gettext("Расход в производство"),
iceb_getuid(wpredok),
time(NULL),
0,0);

iceb_sql_zapis(strsql,1,1,wpredok);



sprintf(strsql,"insert into Rashod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("иуд"),
gettext("Изменение учётных данных"),
iceb_getuid(wpredok),
time(NULL),
2,0);

iceb_sql_zapis(strsql,1,1,wpredok);
sprintf(strsql,"insert into Rashod (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("сп"),
gettext("Списание"),
iceb_getuid(wpredok),
time(NULL),
2,0);

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'2009')",
gettext("шт"),
gettext("Штук"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0301')",
gettext("кг"),
gettext("Килограм"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);



sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0306')",
gettext("т"),
gettext("Тонн"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);



sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0138')",
gettext("л"),
gettext("Литры"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);



sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem) values('%s','%s',%d,%ld)",
gettext("ус"),
gettext("Услуга"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);



sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0102')",
gettext("км"),
gettext("Километр"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0420')",
gettext("квт"),
gettext("Киловатт"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'2454')",
gettext("грн"),
gettext("Гривня"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0415')",
gettext("квт*ч"),
gettext("Киловатт час"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0134')",
gettext("м3"),
gettext("Метр кубический"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'0171')",
gettext("мec"),
gettext("Месяц"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'2355')",gettext("ткм"),gettext("Тонно-киллометры"),iceb_getuid(wpredok),time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);
sprintf(strsql,"insert into Edizmer (kod,naik,ktoz,vrem,kei) values('%s','%s',%d,%ld,'2908')",gettext("мч"),gettext("Моточасы"),iceb_getuid(wpredok),time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Nalog values (0,'1990-1-1',20.,'',%d,%ld)",getuid(),time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);


/*подсистема Учёт основных средств*/

sprintf(strsql,"insert into Uospri (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("вп"),
gettext("Внутреннее перемещение"),
iceb_getuid(wpredok),
time(NULL),
1,1);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uospri (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("поп"),
gettext("Приход от поставщика"),
iceb_getuid(wpredok),
time(NULL),
0,0);

iceb_sql_zapis(strsql,1,1,wpredok);



sprintf(strsql,"insert into Uospri (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("иуд"),
gettext("Изменение учётных данных"),
iceb_getuid(wpredok),
time(NULL),
2,1);

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Uosras (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("вп"),
gettext("Внутреннее перемещение"),
iceb_getuid(wpredok),
time(NULL),
1,0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uosras (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("рп"),
gettext("Реализация потребителю"),
iceb_getuid(wpredok),
time(NULL),
0,0);

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Uosras (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("сп"),
gettext("Списание"),
iceb_getuid(wpredok),
time(NULL),
0,0);

iceb_sql_zapis(strsql,1,1,wpredok);



sprintf(strsql,"insert into Uosras (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("иуд"),
gettext("Изменение учётных данных"),
iceb_getuid(wpredok),
time(NULL),
2,0);

iceb_sql_zapis(strsql,1,1,wpredok);

/*подсистема Учёт услуг*/
sprintf(strsql,"insert into Usloper1 (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("пу"),
gettext("Полученные услуги"),
iceb_getuid(wpredok),
time(NULL),
0,0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Usloper1 (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("ту"),
gettext("Транспортные услуги"),
iceb_getuid(wpredok),
time(NULL),
0,0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Usloper2 (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("ву"),
gettext("Выполненные услуги"),
iceb_getuid(wpredok),
time(NULL),
0,0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Usloper2 (kod,naik,ktoz,vrem,vido,prov) values('%s','%s',%d,%ld,%d,%d)",
gettext("ту"),
gettext("Транспортные услуги"),
iceb_getuid(wpredok),
time(NULL),
0,0);

iceb_sql_zapis(strsql,1,1,wpredok);

/*подсистема Заработная плата*/

sprintf(strsql,"insert into Nash (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
1,
gettext("Должнострой оклад"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Nash (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
2,
gettext("Больничный"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Nash (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
3,
gettext("Отпускные"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Nash (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
4,
gettext("Почасовая оплата"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Nash (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
5,
gettext("Аванс"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
1,
gettext("Налог на доходы физических лиц"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код подоходного налога|1\n' where fil='zarnast.alx' and str like 'Код подоходного налога|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
2,
gettext("Получено в кассе"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код получения зарплаты в кассе|2\n' where fil='zarnast.alx' and str like 'Код получения зарплаты в кассе|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);

/************************
sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
3,
gettext("Единый социальный взнос"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код удержания единого социального взноса|3\n' where fil='zarnast.alx' and str like 'Код удержания единого социального взноса|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
4,
gettext("Единый социальный взнос с больничных"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"update Alx set str='Код удержания единого социального взноса для больничных|4\n' where fil='zarnast.alx' and str like 'Код удержания единого социального взноса для больничных|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
5,
gettext("Единый социальный взнос для договоров подряда"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"update Alx set str='Код удержания единого социального взноса для договоров|5\n' where fil='zarnast.alx' and str like 'Код удержания единого социального взноса для договоров|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
6,
gettext("Единый социальный взнос с инвалидов"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код удержания единого социального взноса для инвалидов|6\n' where fil='zarnast.alx' and str like 'Код удержания единого социального взноса для инвалидов|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
7,
gettext("Единый социальный взнос с военных"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"update Alx set str='Код удержания единого социального взноса для военных|7\n' where fil='zarnast.alx' and str like 'Код удержания единого социального взноса для военных|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
8,
gettext("Единый социальный взнос с больничных для инвалидов"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"update Alx set str='Код удержания единого социального взноса с больничных для инвалидов|8\n' where fil='zarnast.alx' and str like 'Код удержания единого социального взноса с больничных для инвалидов|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);

*****************************/

sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
3,
gettext("Перечислено на карт-счёт"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код перечисления зарплаты на счет|3\n' where fil='zarnast.alx' and str like 'Код перечисления зарплаты на счет|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
4,
gettext("Профсоюзный взнос"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Профсоюзный фонд-код отчисления|4\n' where fil='zarnast.alx' and str like 'Профсоюзный фонд-код отчисления|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Uder (kod,naik,ktoz,vrem,prov) values(%d,'%s',%d,%ld,%d)",
5,
gettext("Военный сбор"),
iceb_getuid(wpredok),
time(NULL),
0);

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код военного сбора|5\n' where fil='zarnast.alx' and str like 'Код военного сбора|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);



sprintf(strsql,"update Alx set str='Обязательные удержания|1,4,5\n' where fil='zarnast.alx' and str like 'Обязательные удержания|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);







sprintf(strsql,"insert into Kateg (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
1,
gettext("Рабочий"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Kateg (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
2,
gettext("Служащий"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Zvan (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
1,
gettext("Не аттестован"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код звания для неатестованых|1\n' where fil='zarnast.alx' and str like 'Код звания для неатестованых|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Коды званий гражданских|1\n' where fil='zarnast.alx' and str like 'Коды званий гражданских|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Zarsoc (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
1,
gettext("Единый социальный взнос"),
iceb_getuid(wpredok),
time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values(%d,'2016-01-01',22.,0.,%d,%ld)",
1,
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код фонда единого социального взноса|1\n' where fil='zarnast.alx' and str like 'Код фонда единого социального взноса|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);




sprintf(strsql,"insert into Zarsoc (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
2,
gettext("Единый социальный взнос для инвалидов"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values(%d,'2016-01-01',8.41,0.,%d,%ld)",
2,
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код фонда единого социального взноса для инвалидов|2\n' where fil='zarnast.alx' and str like 'Код фонда единого социального взноса для инвалидов|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);



sprintf(strsql,"insert into Zarsoc (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
3,
gettext("Единый социальный взнос с больничных"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values(%d,'2016-01-01',22.,0.,%d,%ld)",
3,
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код фонда единого социального взноса для больничных|3\n' where fil='zarnast.alx' and str like 'Код фонда единого социального взноса для больничных|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Zarsoc (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
4,
gettext("Единый социальный взнос для договоров подряда"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values(%d,'2016-01-01',22.,0.,%d,%ld)",
4,
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код фонда единого социального взноса для договоров|4\n' where fil='zarnast.alx' and str like 'Код фонда единого социального взноса для договоров|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Zarsoc (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
5,
gettext("Единый социальный взнос для военных"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values(%d,'2016-01-01',22.,0.,%d,%ld)",
5,
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код фонда единого социального взноса для военных|5\n' where fil='zarnast.alx' and str like 'Код фонда единого социального взноса для военных|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);


sprintf(strsql,"insert into Zarsoc (kod,naik,ktoz,vrem) values(%d,'%s',%d,%ld)",
6,
gettext("Единый социальный взнос с больничных для инвалидов"),
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values(%d,'2016-01-01',8.41,0.,%d,%ld)",
6,
iceb_getuid(wpredok),
time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Код фонда единого социального взноса с больничных для инвалидов|6\n' where fil='zarnast.alx' and str like 'Код фонда единого социального взноса с больничных для инвалидов|%%'");
iceb_sql_zapis(strsql,1,1,wpredok);

/*Учёт путевых листов*/

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("лпг"),gettext("Летняя норма по городу"),iceb_getuid(wpredok),time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("лзг"),gettext("Летняя норма за городом"),iceb_getuid(wpredok),time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("зпг"),gettext("Зимняя норма по городу"),iceb_getuid(wpredok),time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("ззг"),gettext("Зимняя норма за городом"),iceb_getuid(wpredok),time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("пегр"),gettext("Перевозка груза"),iceb_getuid(wpredok),time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("рд"),gettext("Работа двигателя"),iceb_getuid(wpredok),time(NULL));
iceb_sql_zapis(strsql,1,1,wpredok);


/*Установка стартового года*/
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='nastrb.alx' and str like 'Стартовый год|%%'",gt);
iceb_sql_zapis(strsql,1,1,wpredok);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='zarnast.alx' and str like 'Стартовый год|%%'",gt);
iceb_sql_zapis(strsql,1,1,wpredok);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='matnast.alx' and str like 'Стартовый год|%%'",gt);
iceb_sql_zapis(strsql,1,1,wpredok);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='uslnast.alx' and str like 'Стартовый год|%%'",gt);
iceb_sql_zapis(strsql,1,1,wpredok);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='uosnast.alx' and str like 'Стартовый год|%%'",gt);
iceb_sql_zapis(strsql,1,1,wpredok);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='nastdok.alx' and str like 'Стартовый год|%%'",gt);
iceb_sql_zapis(strsql,1,1,wpredok);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='nastud.alx' and str like 'Стартовый год|%%'",gt);
iceb_sql_zapis(strsql,1,1,wpredok);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='ukrnast.alx' and str like 'Стартовый год|%%'",gt);
iceb_sql_zapis(strsql,1,1,wpredok);
sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='uplnast.alx' and str like 'Стартовый год|%%'",gt);
iceb_sql_zapis(strsql,1,1,wpredok);

sprintf(strsql,"update Alx set str='Дата начала просмотра реестра|01.%d.%d\n' where fil='rnn_nast.alx' and str like 'Дата начала просмотра реестра|%%'",mt,gt);
iceb_sql_zapis(strsql,1,1,wpredok);

/*Код оператора для действий выполненных программой без участия оператора*/
sprintf(strsql,"insert into icebuser (login,fio,un,kom,logz,vrem) values('%s','%s',%d,'%s','%s',%ld)",
ICEB_LOGIN_OP_PROG,
gettext("Программа"),
ICEB_KOD_OP_PROG,
gettext("Для действий выполненных без участия оператора"),
iceb_u_getlogin(),time(NULL));

iceb_sql_zapis(strsql,1,1,wpredok);

}
