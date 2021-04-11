/*$Id: zvb_corp2w.c,v 1.1 2014/08/31 06:19:23 sasa Exp $*/
/*19.08.2014	19.08.2014	Белых А.И.	zvb_corp2w.c
Формирование файлов для зачисления на картсчета зарплаты
*/
#include    "buhg_g.h"
#include "l_nahud.h"


extern SQL_baza bd;

void zvb_corp2w(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);
char strsql[1024];
char imafr[64];
char imaf_dbf[64];
char imaf_dbf_tmp[64];

FILE  *ff_dbf_tmp;
FILE  *ffr;
int   kolstr;


SQLCURSOR cur;
SQLCURSOR cur1;
SQL_str row,row1;
double suma=0.;



if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }
class iceb_u_str fio("");

if(zvb_corp2_startw(imafr,imaf_dbf,imaf_dbf_tmp,&ff_dbf_tmp,&ffr,wpredok) != 0)
 return;



class iceb_u_str kartshet("");
double sumai=0.;
class iceb_u_str inn("");
float kolstr1=0.;
cur.poz_cursor(0);
int nomer_zap=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
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

  zvb_corp2_strw(++nomer_zap,fio.ravno(),kartshet.ravno(),suma,inn.ravno(),row[1],ff_dbf_tmp,ffr);


    
 }

zvb_corp2_endw(imaf_dbf,imaf_dbf_tmp,imafr,sumai,nomer_zap,ffr,ff_dbf_tmp,wpredok);

class spis_oth oth;
oth.spis_imaf.plus(imafr);
oth.spis_naim.plus(gettext("Распечатка документов для перечисления на карт-счета"));

oth.spis_imaf.plus(imaf_dbf);
oth.spis_naim.plus(gettext("Документ в электронном формате"));

gdite.close();

iceb_rabfil(&oth,wpredok);

}
