/*$Id: uslpoddokw.c,v 1.1 2013/09/26 09:46:58 sasa Exp $*/
/*23.09.2013	07.04.2009	Белых А.И.	uslpoddokw.c
Подпрограмма подтверждения записей в документе учёта услуг
*/
#include <math.h>
#include "buhg_g.h"

extern SQL_baza bd;

void uslpoddokw(const char *datdok,
const char *datpod,
const char *nomdok,
int podr,
int tipz,
int metka_soob, /*0-выводить сообщения 1-нет*/
GtkWidget *wpredok)
{
short dd=0,md=0,gd=0;
short d1=0,m1=0,g1=0;
iceb_u_rsdat(&dd,&md,&gd,datdok,1);
iceb_u_rsdat(&d1,&m1,&g1,datpod,1);

uslpoddokw(dd,md,gd,d1,m1,g1,nomdok,podr,tipz,metka_soob,wpredok);
}
/*********************************/

void uslpoddokw(short dd,short md,short gd,
short d1,short m1,short g1,
const char *nomdok,
int podr,
int tipz,
int metka_soob, /*0-выводить сообщения 1-нет*/
GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;

sprintf(strsql,"select metka,kodzap,kolih,cena,ei,shetu,nz from \
Usldokum1 where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
gd,md,dd,podr,nomdok,tipz);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(metka_soob == 0)
   {
    sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не найдено ни одной записи !"));
    iceb_menu_soob(strsql,wpredok);
   }
  return;
 }
int nz=0;
int metka=0;
int kodzap=0;
double cena=0.;
double kol1=0.,kol2=0.;
class iceb_u_str naimat("");
while(cur.read_cursor(&row) != 0)
 {
  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kol1=atof(row[2]);
  cena=atof(row[3]);
  nz=atoi(row[6]);  

  /*Узнаем наименование материалла*/
  if(metka == 0)
   sprintf(strsql,"select naimat from Material where kodm=%s",row[1]);
  if(metka == 1)
   sprintf(strsql,"select naius from Uslugi where kodus=%s",row[1]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
    {
     if(metka_soob == 0)
      {    
       sprintf(strsql,"Не найден код %s !",row[1]);
       iceb_menu_soob(strsql,wpredok);
      }     
     naimat.new_plus("");
    }
  else
    naimat.new_plus(row1[0]);
/******************
  if(metka_soob == 0)
   printw("%-4s %-*.*s %10.10g - ",
   row[1],
   iceb_tu_kolbait(20,naimat.ravno()),
   iceb_tu_kolbait(20,naimat.ravno()),
   naimat.ravno(),
   kol1);
   refresh();
********************/

  /*Читаем в карточке количество реально выданное-полученное*/

  kol2=readkoluw(dd,md,gd,podr,nomdok,tipz,metka,kodzap,nz,wpredok);

  //Количество может быть отрицательным
  if(fabs(kol1) > fabs(kol2))
   {
/********************
    if(metka_soob == 0)
     printw(gettext("подтверждена\n"));
*************************/
    zappoduw(tipz,dd,md,gd,d1,m1,g1,nomdok,metka,kodzap,kol1-kol2,cena,row[4],row[5],podr,nz,wpredok);
   }
/*********************
  else
    if(metka_soob == 0)
     printw(gettext("подтверждена раньше\n"));
********************/

 }

}
