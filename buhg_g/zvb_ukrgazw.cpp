/*$Id: zvb_ukrgazw.c,v 1.7 2013/08/13 06:10:05 sasa Exp $*/
/*13.07.2015	22.10.2009	Белых А.И.	zvb_ukrgazw.c
Формирование файлов для зачисления на картсчета зарплаты
*/
#include    "buhg_g.h"
#include "l_nahud.h"

extern SQL_baza bd;

void zvb_ukrgazw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
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

if(zvb_ukrgaz_startw(imafr,imaf_dbf,imaf_dbf_tmp,&ff_dbf_tmp,&ffr,wpredok) != 0)
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
   }
   
  if(kartshet.ravno()[0] == '\0')
   {
    sprintf(strsql,"%s\n%s:%s %s",gettext("Не ввели счёт!"),gettext("Табельный номер"),row[1],fio.ravno());
    iceb_menu_soob(strsql,wpredok);
    continue;
   }


  sumai+=suma;

  zvb_ukrgaz_strw(++nomer_zap,fio.ravno(),kartshet.ravno(),suma,inn.ravno(),row[1],ff_dbf_tmp,ffr);


    
 }

zvb_ukrgaz_endw(imaf_dbf,imaf_dbf_tmp,sumai,nomer_zap,ffr,ff_dbf_tmp,wpredok);

class iceb_u_spisok imafo;
class iceb_u_spisok naimf;
imafo.plus(imafr);
naimf.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imaf_dbf);
naimf.plus(gettext("Документ в электронном формате"));

iceb_ustpeh(imafo.ravno(0),1,wpredok);

iceb_rabfil(&imafo,&naimf,wpredok);

}
