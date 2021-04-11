/*$Id: zvb_nadraw.c,v 1.14 2013/08/13 06:10:05 sasa Exp $*/
/*13.07.2015	26.09.2003	Белых А.И.	zvb_nadraw.c
Получение распечатки для передачи данных по зарплате в банк Надра
*/
#include        <errno.h>
#include    "buhg_g.h"
#include "l_nahud.h"

extern SQL_baza bd;

void zvb_nadraw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);
char		strsql[512];
int		kolstr=0;
SQL_str		row,row1;
SQLCURSOR curr;
class iceb_u_str bros("");
class iceb_u_str rukov("");
class iceb_u_str glavbuh("");
FILE		*ff;
char            imaf[32];


iceb_poldan("Табельный номер руководителя",&bros,"zarnast.alx",wpredok);
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    rukov.new_plus(row1[0]);
 } 

iceb_poldan("Табельный номер бухгалтера",&bros,"zarnast.alx",wpredok);

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

sprintf(imaf,"nadra%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
fprintf(ff,"\
                                      до Договору про надання послуг\n\
                                      N___________\"____\"_____________р.\n\n\
      Підприємство %s\n\n\
                     Зведена відомість N_____від \"_____\"____________р.\n\n",iceb_get_pnk("00",0,wpredok));
                     

fprintf(ff,"\
----------------------------------------------------------------------------\n\
 N  |N картрахунку|  Тип картки  |          ПІБ                 |   Сумма  |\n\
----------------------------------------------------------------------------\n"); 
double suma=0;
class iceb_u_str fio("");
class iceb_u_str kshet("");
double itogo=0.;
class iceb_u_str tipkk("");
int nomer=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  //Читем карточку
  sprintf(strsql,"select fio,bankshet,tipkk from Kartb where tabn=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
   {
    fio.new_plus(row1[0]);
    kshet.new_plus(row1[1]);
    tipkk.new_plus(row1[2]);
   }
  else
   {
    fio.new_plus("");
    kshet.new_plus("");
   }
  suma=atof(row[4]);
  if(prn == 2)
   suma*=-1;
   
  itogo+=suma;
    
  fprintf(ff,"%-4d %-13s %-*s %-*.*s %10.2f\n",
  ++nomer,kshet.ravno(),iceb_u_kolbait(14,tipkk.ravno()),tipkk.ravno(),iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno(),suma);

 }
fprintf(ff,"\
----------------------------------------------------------------------------\n");

fprintf(ff,"\
Загальна сума до виплати, грн.                                   %10.2f\n",itogo);

fprintf(ff,"\
Сумма коміссії Банку, грн.\n\
Загальна сума для перерахування в банк, грн.\n\
Дата виплати заробітної плати,\n\n");

fprintf(ff,"\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov.ravno(),glavbuh.ravno());

fclose(ff);

class iceb_u_spisok imafr;
class iceb_u_spisok naimf;
imafr.plus(imaf);
naimf.plus(gettext("Ведомость для зачисления на карт-счёта"));

iceb_ustpeh(imaf,1,wpredok);

gdite.close();

iceb_rabfil(&imafr,&naimf,wpredok);

}

