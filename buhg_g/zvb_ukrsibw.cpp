/*$Id: zvb_ukrsibw.c,v 1.11 2013/08/13 06:10:05 sasa Exp $*/
/*13.07.2015	04.04.2004	Белых А.И.	zvb_ukrsibw.c
Выгузка для укрсиббанка
*/
#include    "buhg_g.h"
#include "l_nahud.h"

void		ukrsib_h(char *imaf,long kolz);

extern SQL_baza bd;

void zvb_ukrsibw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);
char strsql[512];
char imafdbf[64];
char imaftmp[64];
char imafr[64];
FILE  *fftmp;
FILE  *ffr;
int   kolstr;
class iceb_u_str rukov("");
class iceb_u_str glavbuh("");
class iceb_u_str bros("");


SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR curr;
SQL_str row,row1;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }


iceb_poldan("Табельный номер руководителя",&bros,"zarnas.alx",wpredok);

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    rukov.new_plus(row1[0]);
 } 

iceb_poldan("Табельный номер бухгалтера",&bros,"zarnas.alx",wpredok);

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros.ravno());
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    glavbuh.new_plus(row1[0]);
 }
class iceb_u_str operatorr("");
ukrsib_startw(imafr,imaftmp,&operatorr,&fftmp,&ffr,wpredok);

class iceb_u_str fio("");

float kolstr1=0.;
int nomer=0;

class iceb_u_str kartshet("");
double suma;
double sumai=0.;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

    
  sprintf(strsql,"select fio,bankshet from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    fio.new_plus(row1[0]);
    kartshet.new_plus(row1[1]);

   }
  else
   {
    kartshet.new_plus("");
    fio.new_plus("");
   }
  if(kartshet.ravno()[0] == '\0')
   {
    class iceb_u_str repl;
    repl.plus(gettext("Не введён карт-счёт !"));

    sprintf(strsql,"%s:%s %s",gettext("Табельный номер"),row[1],fio.ravno());
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    continue;
   }

  suma=atof(row[4]);
  if(prn == 2)
    suma*=(-1);   

  sumai+=suma;

  ukrsib_strw(dt,mt,gt,suma,kartshet.ravno(),fio.ravno(),&nomer,row[1],operatorr.ravno(),ffr,fftmp);

    
 }

ukrsib_endw(imafdbf,imaftmp,sumai,nomer,rukov.ravno(),glavbuh.ravno(),ffr,fftmp,wpredok);

class iceb_u_spisok imaf;
class iceb_u_spisok naimf;
imaf.plus(imafr);
imaf.plus(imafdbf);
naimf.plus(gettext("Ведомость для зачисления на карт-счёта"));
naimf.plus(gettext("Файл для передачи в банк"));

gdite.close();

iceb_rabfil(&imaf,&naimf,wpredok);
}
