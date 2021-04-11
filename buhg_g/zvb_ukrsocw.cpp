/*$Id: zvb_ukrsocw.c,v 1.17 2013/09/26 09:47:01 sasa Exp $*/
/*22.07.2019	11.02.2004	Белых А.И.	zvb_ukrsocw.c
Выгрузка для Укрсоцбанка
*/
#include        <errno.h>
#include "buhg_g.h"
#include "l_nahud.h"

extern SQL_baza bd;

void zvb_ukrsocw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
char		strsql[512];
int		kolstr=0;
SQL_str		row,row1;
SQLCURSOR curr;
class iceb_u_str bros("");
class iceb_u_str rukov("");
class iceb_u_str glavbuh("");
FILE		*ff;
FILE		*fftmp;
char            imaf[64];
char            imaftmp[64];
char            imafzag[64];
time_t          vrem;


sprintf(imafzag,"ukrsoc.txt");
if(iceb_menu_vvod1(gettext("Введите имя файла"),imafzag,sizeof(imafzag),"",wpredok) != 0)
 return;

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);

time(&vrem);
struct tm *bf=localtime(&vrem);


class iceb_u_str imaf_nast("zarnast.alx");

iceb_u_poldan("Табельный номер руководителя",&bros,imaf_nast.ravno());
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    rukov.new_plus(row1[0]);
 } 

iceb_u_poldan("Табельный номер бухгалтера",&bros,imaf_nast.ravno());

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros.ravno());
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    glavbuh.new_plus(row1[0]);
 }


SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return; 
 }
sprintf(imaftmp,"ukrsoc%d.tmp",getpid());
if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return;
 }

sprintf(imaf,"ukrsoc%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

fprintf(ff,"\
   Зведена відомість сум для зарахування на спеціальні карткові рахунки N_____\n\
------------------------------------------------------------------------------\n\
| Дата                             |             %02d.%02d.%04d                  |\n\
| Назва організації                |%-*.*s|\n\
| Назва та N філії АКБ \"Укрсоцбанк\"|Вінницька облана філія N050              |\n\
| Додаткова інформація             |                                         |\n\
------------------------------------------------------------------------------\n",
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
iceb_u_kolbait(41,iceb_get_pnk("00",0,wpredok)),iceb_u_kolbait(41,iceb_get_pnk("00",0,wpredok)),iceb_get_pnk("00",0,wpredok));

fprintf(ff,"\
--------------------------------------------------------------------------------------\n\
 N  |     Прізвище, ім'я по батькові         | Картковий счёт |Сумма до зарахування|\n\
--------------------------------------------------------------------------------------\n"); 

double suma=0;
class iceb_u_str fio("");
class iceb_u_str kshet("");
double itogo=0.;

float kolstr1=0.;
int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  //Читем карточку
  sprintf(strsql,"select fio,bankshet,tipkk from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   {
    fio.new_plus(row1[0]);
    kshet.new_plus(row1[1]);
   }
  else
   {
    kshet.new_plus("");
    fio.new_plus("");
   }
  if(kshet.ravno()[0] == '\0')
   {
    class iceb_u_str repl;
    repl.plus(gettext("Не введён карт-счёт !"));

    sprintf(strsql,"%s:%s %s",gettext("Табельный номер"),row[1],fio.ravno());
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    continue;
   }

  suma=atof(row[4])*(-1);
  if(prn == 2)
   suma*=-1;
  itogo+=suma;
    
  fprintf(ff,"%-4d %-*.*s %-19.19s %10.2f\n",
  ++nomer,iceb_u_kolbait(40,fio.ravno()),iceb_u_kolbait(40,fio.ravno()),fio.ravno(),kshet.ravno(),suma);
  fprintf(fftmp,"@%s,0,%.2f\n",kshet.ravno(),suma);
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------\n"); 

fprintf(ff,"\
                                                          Всього: %10.2f\n",itogo);

fprintf(ff,"\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov.ravno(),glavbuh.ravno());

fprintf(ff,"\n\
Відмітки банку:\n\
Дата приймання Зведеної відомості на паперовому носії та магнітному носії_____________\n\
Посада та призвище, ім'я та по батькові працівника Банку, який прийняв Зведену відомість\n\n\
_____________________________________________________\n\n\
_____________________________________________________\n\n\
підпис_______________________________________________\n");


iceb_podpis(ff,wpredok);

fclose(ff);
fclose(fftmp);






if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return;
 }

if((ff = fopen(imafzag,"w")) == NULL)
 {
  iceb_er_op_fil(imafzag,"",errno,wpredok);
  return;
 }

fprintf(ff,"@29244050010001,0,%.2f\n\
@302010,0,0,0.00\n",itogo);

while(fgets(strsql,sizeof(strsql),fftmp) != NULL)
  fprintf(ff,"%s",strsql);
  
fclose(ff);
fclose(fftmp);
unlink(imaftmp);

class iceb_u_spisok imafr;
class iceb_u_spisok naimf;
imafr.plus(imaf);
imafr.plus(imafzag);
naimf.plus(gettext("Ведомость для зачисления на карт-счёта."));
naimf.plus(gettext("Файл для передачи в банк."));

iceb_ustpeh(imaf,1,wpredok);

gdite.close();

iceb_rabfil(&imafr,&naimf,wpredok);

}



