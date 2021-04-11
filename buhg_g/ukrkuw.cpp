/*$Id: ukrkuw.c,v 1.15 2013/08/13 06:09:54 sasa Exp $*/
/*30.03.2016	15.09.2003	Белых А.И.	ukrku.c
Распечатка командировочного удостоверения
*/
#include <errno.h>
#include "buhg_g.h"

extern SQL_baza bd;

void ukrkuw(short gd,const char *nomd,GtkWidget *wpredok)
{
FILE    *ffku;
char    imaf[64];
char    strsql[512];
SQLCURSOR cur;
SQLCURSOR curr;
class iceb_u_str kont("");
class iceb_u_str pnaz("");
class iceb_u_str organ("");
class iceb_u_str celk("");
short   dn,mn,gn;
short   dk,mk,gk;
class iceb_u_str nompr("");
short   dp,mp,gp;
SQL_str row;
class iceb_u_str stroka("");
int     nomer=0;
class iceb_u_str fio("");
class iceb_u_str dolgn("");


sprintf(strsql,"select kont,pnaz,organ,celk,datn,datk,nompr,datp \
from Ukrdok where god=%d and nomd='%s'",gd,nomd);

/*printf("ukrku-%s\n",strsql);*/

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  iceb_menu_soob(gettext("Не нашли шапку документа!"),wpredok);
  return;
 }
kont.new_plus(row[0]);

pnaz.new_plus(row[1]);

organ.new_plus(row[2]);

celk.new_plus(row[3]);

iceb_u_rsdat(&dn,&mn,&gn,row[4],2);

iceb_u_rsdat(&dk,&mk,&gk,row[5],2);

nompr.new_plus(row[6]);

iceb_u_rsdat(&dp,&mp,&gp,row[7],2);

sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kont.ravno());
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
  fio.new_plus(row[0]);

short kolsimv=iceb_u_strlen(ICEB_ZAR_PKTN);
//short dlinnakont=strlen(kont.ravno());

class iceb_u_str tabnom("");
class iceb_u_str nomp("");

/****************
int i2=0;
for(int i=dlinnaprist; i < dlinnakont;i++)
  tabnom[i2++]=kont.ravno()[i];
****************/
tabnom.new_plus(iceb_u_adrsimv(kolsimv,kont.ravno()));

//Читаем должность
sprintf(strsql,"select fio,dolg,nomp from Kartb where tabn=%d",tabnom.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
 {
  fio.new_plus(row[0]);
  dolgn.new_plus(row[1]);
  nomp.new_plus(row[2]);
 }
int koldn=iceb_u_period(dn,mn,gn,dk,mk,gk,0);


SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

sprintf(strsql,"select str from Alx where fil='ukrku.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"ukrku.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }


sprintf(imaf,"ku%d.lst",getpid());

if((ffku = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer++;
  stroka.new_plus(row_alx[0]);
  switch(nomer)
   {
    case 9:
      iceb_u_vstav(&stroka,nomd,53,65,1);
      break;

    case 11:
      iceb_u_vstav(&stroka,fio.ravno(),30,67,1);
      break;

    case 12:
      iceb_u_vstav(&stroka,dolgn.ravno(),23,67,1);
      break;

    case 13:
      iceb_u_vstav(&stroka,pnaz.ravno(),40,67,1);
      break;

    case 14:
      iceb_u_vstav(&stroka,organ.ravno(),23,67,1);
      break;

    case 15:
      iceb_u_vstav(&stroka,celk.ravno(),23,67,1);
      break;

    case 17:
      sprintf(strsql,"%d",koldn);
      iceb_u_vstav(&stroka,strsql,19,20,1);
      memset(strsql,' ',sizeof(strsql));
      if(dn != 0)
       sprintf(strsql,"%02d.%02d.%04d",dn,mn,gn);

      iceb_u_vstav(&stroka,strsql,29,38,1);
/************
      По просьбе клиента дату конца не печататем
      memset(strsql,' ',sizeof(strsql));
      if(dk != 0)
        sprintf(strsql,"%02d.%02d.%04d",dk,mk,gk);
      iceb_u_vstav(&stroka,strsql,45,54,1);
***************/
      break;

    case 18:
      iceb_u_vstav(&stroka,nompr.ravno(),17,25,1);

      sprintf(strsql,"%02d.%02d.%04d",dp,mp,gp);
      iceb_u_vstav(&stroka,strsql,31,40,1);

      break;

    case 19:
      iceb_u_vstav(&stroka,nomp.ravno(),33,62,1);
      break;
     
   }

  fprintf(ffku,"%s",stroka.ravno());

 }

iceb_podpis(ffku,wpredok);

fclose(ffku);

class iceb_u_spisok imafil;
class iceb_u_spisok naimfil;

imafil.plus(imaf);
naimfil.plus(gettext("Командировочное удостоверение"));
iceb_ustpeh(imaf,1,wpredok);
iceb_rabfil(&imafil,&naimfil,wpredok);


}
