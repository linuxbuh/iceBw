/*$Id: zvb_ibank2uaw.c,v 1.10 2013/08/13 06:10:04 sasa Exp $*/
/*24.10.2019	18.09.2008	Белых А.И.	zvb_ibank2uaw.c
Выгрузна в файл для экспорта в систему iBank2 UA
*/
#include <math.h>
#include    "buhg_g.h"
#include "l_nahud.h"

extern SQL_baza bd;

void zvb_ibank2uaw(short prn,const char *zapros,class l_nahud_rek *poisk,const char *kod00,GtkWidget *wpredok)
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

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }
//class iceb_u_str vidnahis(""); 

if(zvb_ibank2ua_startw(imafr,imaf,kod00,&ff,&ffr,wpredok) != 0)
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
    sprintf(strsql,"%s\n%s:%s %s",gettext("Не ввели счёт!"),gettext("Табельный номер"),row[1],fio.ravno());
    iceb_menu_soob(strsql,wpredok);
    continue;
   }


  sumai+=suma;

  zvb_ibank2ua_strw(&nomer_zap,row[1],fio.ravno(),kartshet.ravno(),inn.ravno(),suma,ff,ffr);

 }
 
zvb_ibank2ua_endw(imafr,sumai,ffr,ff,wpredok);

class iceb_u_spisok imafo;
class iceb_u_spisok naimf;

imafo.plus(imafr);
naimf.plus(gettext("Ведомость перечисления на карт-счета"));

imafo.plus(imaf);
naimf.plus(gettext("Документ в электронном формате"));

iceb_rabfil(&imafo,&naimf,wpredok);

}
