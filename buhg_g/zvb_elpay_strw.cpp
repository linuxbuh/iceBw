/*$Id:$*/
/*18.10.2014	18.09.2008	Белых А.И.	zvb_ibank2ua_str.c
Строка записи перечисленой на карт-счёт зарплаты для системы Інтернет ELPay 



 Імпорт зарплатної відомості виконується із зовнішнього файлу. 
Файл зарплатної відомості повинен бути формату CSV або MS Excel 2003. 
Сума вказується в копійках. 
1) Формат файлу CSV: текстовий, роздільник - крапка з комою, формат в таблиці 2. 
------------------------------------------------------------------------
№ поля        Поле          Формат      Приклад       Обов'язковість 
------------------------------------------------------------------------
1 ІПН                       NUMBER(10)    3041319127    Так
2 Номер карткового рахунку  NUMBER(14)    262521332     Так
3 Сума                      NUMBER (20)   123,45        Так
4 Валюта                    NUMBER (3)    980           Так
5 ЄДРПОУ                    VARCHAR (10)  33349855      Так
6 ПІБ                       VARCHAR (250) Балаганов А.С. Немає

Файл - довільне, розширення - csv, CHARACTER SET - WIN1251. 
Приклад вмісту файлу: 
3041319127;262521332;12345;980;33349855;Балаганов А.
6576586768;675446356;98720;980;33349855;Петров І.Н.
9879987989;908098678;86930;980;33349855;
8763457659;654768764;87666;980;33349855;Ніколаєнко С.З.


*/
#include "buhg_g.h"
#include <math.h>

void zvb_elpay_strw(int *nomer_zap,
const char *tabnom,
const char *fio,
const char *shet_b,
const char *inn,
double suma,
const char *edrpou,
FILE *ff_csv,
FILE *ff_lst,
FILE *ff_dbf)
{
double sumav=suma;
*nomer_zap+=1;

modf(suma*100,&sumav);

/******************
fprintf(ff_lst,"%5d %-*.*s %10s %-13s %10.2f\n",
*nomer_zap,
iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,
inn,shet_b,suma);
*************************/
zvb_str_lstw(*nomer_zap,fio,shet_b,suma,inn,tabnom,ff_lst);


int ii=sumav;

fprintf(ff_csv,"\
%s;%s;%d;980;%s;%s\n",
inn,
shet_b,
ii,
edrpou,
fio);

fprintf(ff_dbf," %-*.*s%-*.*s%12.2f980%-*.*s%-*.*s",
iceb_u_kolbait(10,inn),
iceb_u_kolbait(10,inn),
inn,
iceb_u_kolbait(29,shet_b),
iceb_u_kolbait(29,shet_b),
shet_b,
suma,
iceb_u_kolbait(10,edrpou),
iceb_u_kolbait(10,edrpou),
edrpou,
iceb_u_kolbait(250,fio),
iceb_u_kolbait(250,fio),
fio);

}
