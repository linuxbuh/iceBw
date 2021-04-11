/*$Id: zvb_hreshatikw.c,v 1.10 2013/08/13 06:10:04 sasa Exp $*/
/*16.10.2009	04.04.2004	Белых А.И.	zvb_hreshatikw.c
Выгузка для банка Хрещатик
телефон 59-20-30
        59-20-31
        59-20-39
        59-20-40
Тюренков Алексей Валерьевич моб. 8068 8386276 <Tyurenkov@xbank.com.ua>
Начальник Кашпрук Валерий Валерьевич <Kashpruk@xbank.com.ua>

*/
#include    "buhg_g.h"
#include "l_nahud.h"
#include "zvb_hreshatik.h"

extern SQL_baza bd;



void zvb_hreshatikw(short prn,const char *zapros,class l_nahud_rek *poisk,
GtkWidget *wpredok)
{
char strsql[512];
char imaf[64];
char imafr[64];
FILE  *ff;
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
double sum=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;
//  if(poidirnu(row,poisk,fio) != 0)
//     continue;

  sum=atof(row[4]);
  if(prn == 2)
    sum*=(-1);   
  suma+=sum;
 }

short d_pp=0,m_pp=0,g_pp=0;


if(zvb_hreshatik_startw(imafr,imaf,suma,&d_pp,&m_pp,&g_pp,&ff,&ffr,wpredok) != 0)
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
    fio.new_plus("");
    kartshet.new_plus("");
    inn.new_plus("");
   }
  if(kartshet.ravno()[0] == '\0')
   {
    sprintf(strsql,"%s\n%s:%s %s",gettext("Не ввели счёт!"),gettext("Табельный номер"),row[1],fio.ravno());
    iceb_menu_soob(strsql,wpredok);
    continue;
   }


  sumai+=suma;


  zvb_hreshatik_strw(d_pp,m_pp,g_pp,&nomer_zap,fio.ravno(),kartshet.ravno(),inn.ravno(),suma,ff,ffr);

    
 }

zvb_hreshatik_endw(sumai,nomer_zap,ffr,ff,wpredok);

class iceb_u_spisok imafo;
class iceb_u_spisok naimf;
imafo.plus(imafr);
naimf.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imaf);
naimf.plus(gettext("Документ в электронном формате"));

iceb_ustpeh(imafo.ravno(0),1,wpredok);

iceb_rabfil(&imafo,&naimf,wpredok);

}


