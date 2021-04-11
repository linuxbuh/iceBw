/*$Id: zvb_kievrusw.c,v 1.4 2013/08/13 06:10:04 sasa Exp $*/
/*13.07.2015	11.10.2011	Белых А.И.	zvb_kievrusw.c
Формирование файлов для зачисления на картсчета зарплаты

Бескревный Сергей Николаевич
начальник отдела ИТ
отделения "Винницкая региональная дирекция" ПАТ "Банк <<Киевская Русь>>"
тел.: (0432) 552813
e-mail: beskrevniy.s.n@kruss.kiev.ua
              vinnica@kruss.kiev.ua


Структура         DKF.dbf
SBK_NUM	S14			№ карт.рах.
SBK_SUM	N12.2			Нараховано
SBK_INN	S10			Ідентифікаційний код
SBK_FIO	S50			Фамілія Ім'я по Батькові

*/
#include    "buhg_g.h"
#include "l_nahud.h"


extern SQL_baza bd;



void zvb_kievrusw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
char strsql[512];
char imafr[64];
char imaf_dbf[64];
char imaf_dbf_tmp[64];

FILE  *ff_dbf_tmp;
FILE  *ffr;
int   kolstr;


SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR curr;
SQL_str row,row1;
double suma=0.;



if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }
class iceb_u_str fio("");

if(zvb_kievrus_startw(imafr,imaf_dbf,imaf_dbf_tmp,&ff_dbf_tmp,&ffr,wpredok) != 0)
 return;


class iceb_u_str kartshet("");
double sumai=0.;
class iceb_u_str inn("");

cur.poz_cursor(0);
int nomer_zap=0;
while(cur.read_cursor(&row) != 0)
 {
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  suma=atof(row[4]);
  if(suma == 0.)
   continue;
  if(prn == 2)
    suma*=(-1);   

    
  sprintf(strsql,"select fio,bankshet,inn from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    fio.new_plus(row1[0]);
    kartshet.new_plus(row1[1]);
    inn.new_plus(row1[2]);
   }
  else
   {
    kartshet.new_plus("");
    inn.new_plus("");
    fio.new_plus("");
   }
  if(kartshet.ravno()[0] == '\0')
   {
    sprintf(strsql,"%s\n%s:%s %s",gettext("Не введён счёт !"),gettext("Табельный номер"),row[1],fio.ravno());
    iceb_menu_soob(strsql,wpredok);
    continue;
   }


  sumai+=suma;

  zvb_kievrus_strw(++nomer_zap,fio.ravno(),kartshet.ravno(),suma,inn.ravno(),row[1],ff_dbf_tmp,ffr);


    
 }

zvb_kievrus_endw(imaf_dbf,imaf_dbf_tmp,sumai,nomer_zap,ffr,ff_dbf_tmp,wpredok);

class spis_oth oth;
oth.spis_imaf.plus(imafr);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

oth.spis_imaf.plus(imaf_dbf);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

iceb_ustpeh(oth.spis_imaf.ravno(0),1,wpredok);

iceb_rabfil(&oth,wpredok);


}
