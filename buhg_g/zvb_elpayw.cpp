/*$d:$*/
/*25.10.2019	17.10.2019	Белых А.И.	zvb_elpay.cpp
Система нтернет ELPay 
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

#include        <errno.h>
#include <math.h>
#include    "buhg_g.h"
#include "l_nahud.h"

extern SQL_baza	bd;

void zvb_elpayw(short prn,const char *zapros,struct l_nahud_rek *poisk,const char *kod00,GtkWidget *wpredok)
{
char strsql[512];
char imaf_csv[64];
char imaf_lst[64];
char imaf_dbf[64];
int   kolstr;
FILE *ff_csv=NULL;
FILE *ff_lst=NULL;
FILE *ff_dbf=NULL;
SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR curr;
SQL_str row,row1;
class iceb_u_str edrpou("");






if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }

if(zvb_elpay_startw(imaf_lst,imaf_csv,imaf_dbf,&edrpou,kod00,&ff_csv,&ff_lst,&ff_dbf,wpredok) != 0)
 return;


class iceb_u_str kartshet("");
class iceb_u_str inn("");
class iceb_u_str fio("");
double suma=0.;
double sumai=0.;
int nomer_zap=0;

while(cur.read_cursor(&row) != 0)
 {
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  suma=atof(row[4]);

  if(fabs(suma) < 0.01)
   continue;

  if(prn == 2)
    suma*=(-1);   

    
  sprintf(strsql,"select fio,bankshet,inn from Kartb where tabn=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   {
    fio.new_plus(row1[0]);
    kartshet.new_plus(row1[1]);
    inn.new_plus(row1[2]);
   }
  else
   {
    fio.new_plus("");
    kartshet.new_plus("");
    inn.new_plus("");
   }
  if(kartshet.ravno()[0] == '\0')
   {
    class iceb_u_spisok soob;
    soob.plus(gettext("Не введён счёт !"));
    sprintf(strsql,"%s:%s %s",gettext("Табельный номер"),row[1],fio.ravno());
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
    continue;
   }


  sumai+=suma;

  zvb_elpay_strw(&nomer_zap,row[1],fio.ravno(),kartshet.ravno(),inn.ravno(),suma,edrpou.ravno(),ff_csv,ff_lst,ff_dbf);

 }

zvb_elpay_endw(imaf_lst,imaf_csv,imaf_dbf,sumai,nomer_zap,ff_lst,ff_csv,ff_dbf,wpredok);



class spis_oth oth;
oth.spis_imaf.plus(imaf_lst);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

oth.spis_imaf.plus(imaf_csv);
oth.spis_naim.plus(gettext("Документ в электронном формате csv"));

oth.spis_imaf.plus(imaf_dbf);
oth.spis_naim.plus(gettext("Документ в электронном формате dbf"));


iceb_rabfil(&oth,wpredok);

}

