/*$Id: zvb_alfabankw.c,v 1.8 2014/08/31 06:19:22 sasa Exp $*/
/*19.10.2009	15.10.2009	Белых А.И.	zvb_alfabankw.c
Формирование файлов для зачисления на картсчета зарплаты
*/
#include    "buhg_g.h"
#include "l_nahud.h"

int zvb_alfabank_startw(char *imafr,char *imaf_dbf, char *imaf_dbf_tmp,FILE **ff,FILE **ffr,GtkWidget *wpredok);
void zvb_alfabank_endw(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp,GtkWidget *wpredok);
void zvb_alfabank_strw(int nomerpp,const char *fio,const char *nomersh,double suma,const char *inn,const char *kedrpou,FILE *ff_dbf_tmp,FILE *ffras);

extern SQL_baza bd;


void zvb_alfabankw(short prn,const char *zapros,class l_nahud_rek *poisk,
GtkWidget *wpredok)
{
char strsql[512];
char imafr[64];
char imaf_dbf[64];
char imaf_dbf_tmp[64];
class iceb_u_str kedrpou("");
FILE  *ff_dbf_tmp;
FILE  *ffr;
int   kolstr;


SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR curr;
SQL_str row,row1;
double suma=0.;


sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 kedrpou.new_plus(row[0]);




if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }
class iceb_u_str fio("");

if(zvb_alfabank_startw(imafr,imaf_dbf,imaf_dbf_tmp,&ff_dbf_tmp,&ffr,wpredok) != 0)
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

  zvb_alfabank_strw(++nomer_zap,fio.ravno(),kartshet.ravno(),suma,inn.ravno(),kedrpou.ravno(),ff_dbf_tmp,ffr);


    
 }

zvb_alfabank_endw(imaf_dbf,imaf_dbf_tmp,sumai,nomer_zap,ffr,ff_dbf_tmp,wpredok);

class iceb_u_spisok imafo;
class iceb_u_spisok naimf;
imafo.plus(imafr);
naimf.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imaf_dbf);
naimf.plus(gettext("Документ в электронном формате"));

iceb_ustpeh(imafo.ravno(0),1,wpredok);

iceb_rabfil(&imafo,&naimf,wpredok);

}
