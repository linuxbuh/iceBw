/*$Id:$*/
/*06.08.2015	06.08.2015	Белых А.И.	zarkvrt1_kr.c
Поиск нужных данных в подсистему "Учёт командировочных расходов"
*/

#include "buhg_g.h"
#include "sprkvrt1w.h"

extern SQL_baza bd;

void zarkvrt1_kr(short mn,short gn,short mk,short gk,const char *imatmptab,class sprkvrt1_nast *nast,GtkWidget *wpredok)
{
char strsql[1024];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
printf("%s-%s\n",__FUNCTION__,gettext("Ищем в подсистеме \"Учёт командировочных расходов\""));

if(nast->kodkr.kolih() == 0)
 {
  printf("%s\n",gettext("Не введены настройки для поиска"));
  return;
 }

sprintf(strsql,"select datd,nomd,kodr,kolih,cena,ediz,snds,kdrnn from Ukrdok1 where datd >= '%04d-%02d-%02d' and \
datd <= '%04d-%02d-%02d' and kdrnn <> ''",gn,mn,1,gk,mk,31);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
if(kolstr == 0)
 {
  printf("%s\n",gettext("Не найдено ни одной записи !"));
  return;
 }
double suma=0.;
//int kolstr1=0;
int koddoh=0;
class iceb_u_str egrpou("");
class iceb_u_str fio("");
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  koddoh=0;
  for(int nom=0; nom < nast->kodkr.kolih(); nom++)
   {
    if(iceb_u_proverka(nast->kodd_for_kodkr.ravno(nom),row[2],0,1) == 0)
     {
      koddoh=nast->kodkr.ravno(nom);
      break;
     }
   }

  if(koddoh == 0)
   continue;  
  
  sprintf(strsql,"select naikon,kod,regnom from Kontragent where kodkon='%s'",row[7]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   {
    if(row1[2][0] == '\0')
     continue;    
   }
  fio.new_plus(row1[0]);
  egrpou.new_plus(row1[1]);  

  suma=atof(row[4])+atof(row[6]);

  printf("%s %-*s %3s %5.5g %10.2f %-*s %s %s\n",
  iceb_u_datzap(row[0]),
  iceb_u_kolbait(10,row[1]),
  row[1],
  row[2],
  atof(row[3]),
  suma,
  iceb_u_kolbait(6,row[5]),
  row[5],
  row[7],
  iceb_get_pnk(row[7],0,wpredok));  

  zarkvrt_zapvtablw(imatmptab,egrpou.ravno(),koddoh,"",
  suma,
  suma,
  0.,
  0.,
  "0000-00-00","0000-00-00",fio.ravno_filtr(),row[7],NULL,wpredok);
 }


}
