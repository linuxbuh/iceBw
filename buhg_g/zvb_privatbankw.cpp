/*$Id: zvb_privatbank.c,v 5.1 2014/08/31 06:18:19 sasa Exp $*/
/*22.09.2016	17.10.2014	Белых А.И.	zvb_privatbankw.c
Формирование файлов для зачисления на картсчета зарплаты
*/
#include    "buhg_g.h"
#include "l_nahud.h"

extern SQL_baza bd;


void zvb_privatbankw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
char strsql[512];
char imafr[64];
char imaf_dbf[64];
char imaf_dbf_tmp[64];

FILE  *ff_dbf_tmp;
FILE  *ffr;
int   kolstr=0;
static class iceb_u_str branch("");
static class iceb_u_str zpkod("");
static class iceb_u_str rlkod("");
class iceb_u_str fio("");
class iceb_u_str kartshet("");
double sumai=0.;
class iceb_u_str inn("");
int nomer_zap=0;

SQLCURSOR cur;
SQLCURSOR cur1;
SQL_str row,row1;
double suma=0.;

/*Если применять "Ждите" то не правильно формируется dbf файл*/
/*Если нет меню для ввода дополнительных реквихитов то "Ждите" не влияет на формирование файлов*/
if(zvb_privatbank_startw(imafr,imaf_dbf,imaf_dbf_tmp,&branch,&zpkod,&rlkod,&ff_dbf_tmp,&ffr,wpredok) != 0)
 return;


if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,zapros,wpredok);
  return;
 }



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
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   {
    kartshet.new_plus(row1[1]);
    inn.new_plus(row1[2]);
    fio.new_plus(row1[0]);
   }
  else
   {
    kartshet.new_plus("");
    inn.new_plus("");
    fio.new_plus("");
   }
  if(kartshet.ravno()[0] == '\0')
   {
    sprintf(strsql,"%s\n%s:%s %s",gettext("Не введён счёт!"),gettext("Табельный номер"),row[1],fio.ravno());
    iceb_menu_soob(strsql,wpredok);
    continue;
   }


  sumai+=suma;

//  zvb_privatbank_strw(++nomer_zap,fio.ravno(),kartshet.ravno(),suma,inn.ravno(),row[1],branch.ravno(),zpkod.ravno(),rlkod.ravno(),ff_dbf_tmp,ffr);
  zvb_privatbank_strw(++nomer_zap,fio.ravno(),kartshet.ravno(),suma,inn.ravno(),inn.ravno(),branch.ravno(),zpkod.ravno(),rlkod.ravno(),ff_dbf_tmp,ffr);


    
 }

zvb_privatbank_endw(imaf_dbf,imaf_dbf_tmp,sumai,nomer_zap,ffr,ff_dbf_tmp,wpredok);

class spis_oth oth;
oth.spis_imaf.plus(imafr);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

oth.spis_imaf.plus(imaf_dbf);
oth.spis_naim.plus(gettext("Документ в электронном формате"));


iceb_ustpeh(oth.spis_imaf.ravno(0),1,wpredok);

iceb_rabfil(&oth,wpredok);

}
