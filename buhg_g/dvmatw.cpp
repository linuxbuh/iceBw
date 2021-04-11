/* $Id: dvmatw.c,v 1.14 2013/05/17 14:58:19 sasa Exp $ */
/*04.09.2015	13.3.1999	Белых А.И.	dvmatw.c
Движение по документам по материалу
*/
#include        <errno.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "l_mater_dv.h"

extern SQL_baza bd;

void            dvmatw(class mater_dv_data *data,int kodm,GtkWidget *wpredok)
{
FILE		*ff1;
char		imaf[64];
double		kolih,cena,itog=0.,itog2=0.;
short		d,m,g;
class iceb_u_str nn("");
short		tipz;
class iceb_u_str kop("");
class iceb_u_str kor("");
int		skll,nk;
class iceb_u_str naim("");
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[512];
double		kol2=0.;
SQLCURSOR cur;
SQLCURSOR cur1;
 
/***************  
if(data->sklad.getdlinna() > 1)
 printf(gettext("Склад %s\n"),data->sklad.ravno());
else
 printf(gettext("По всем складам\n"));
**************/  
/*Читаем наименование материалла*/
sprintf(strsql,"select naimat from Material where kodm=%d",kodm);
if(sql_readkey(&bd,strsql,&row1,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodm);
  repl.plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return;
 }

naim.new_plus(row1[0]);

printf(gettext("Наименование %s\n"),naim.ravno());

if(data->sklad.getdlinna() > 1)
 sprintf(strsql,"select datd,nomd,sklad,nomkar,kolih,cena,tipz from \
Dokummat1 where datd >= '%s' and datd <= '%s' and \
kodm=%d and sklad=%d order by datd asc",
 data->datan.ravno_sqldata(),data->datak.ravno_sqldata(),kodm,data->sklad.ravno_atoi());
else
 sprintf(strsql,"select datd,nomd,sklad,nomkar,kolih,cena,tipz \
from Dokummat1 where datd >= '%s' and datd <= '%s' \
and kodm=%d order by datd asc",data->datan.ravno_sqldata(),data->datak.ravno_sqldata(),kodm);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),wpredok);
  return;
 }


sprintf(imaf,"dvi%d.lst",getpid());
if((ff1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
  return;
 }

fprintf(ff1,"%s\n%s %s => %s %s %d\n\
%s: %s\n",
iceb_get_pnk("00",0,wpredok),
gettext("Дата"),
data->datan.ravno(),
data->datak.ravno(),
gettext("Код материалла"),
kodm,
gettext("Наименование материалла"),
naim.ravno());

if(data->sklad.getdlinna() > 1)
 fprintf(ff1,"%s %s\n",gettext("Склад"),data->sklad.ravno());
else
 fprintf(ff1,"%s\n",gettext("По всем складам"));

fprintf(ff1,"\
------------------------------------------------------------------------------\n");

fprintf(ff1,gettext(" Дата     |Номер док.|Ном.к.|Количес.|Подтвер.| Цена   |Скл.|Опер.|Контрагент|\n"));

fprintf(ff1,"\
------------------------------------------------------------------------------\n");
short dn,mn,gn;

iceb_u_rsdat(&dn,&mn,&gn,data->datan.ravno(),1);

itog=0.;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
    
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  /*Пpопускаем стаpтовые остатки если это остатки не года начала поиска*/
  if(iceb_u_SRAV(row[1],"000",0) == 0 && g != gn)
   continue;
  nn.new_plus(row[1]);
  skll=atoi(row[2]);
  nk=atoi(row[3]);
  kolih=atof(row[4]);
  cena=atof(row[5]);
  tipz=atoi(row[6]);
  
  
  /*Читаем шапку накладной*/
  sprintf(strsql,"select kontr,kodop from Dokummat \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
  g,m,d,skll,nn.ravno());

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf(gettext("Не найден документ %s %d.%d.%d %s\n"),
    row[1],d,m,g,row[2]);
    continue;    
   }
  kor.new_plus(row1[0]);
  kop.new_plus(row1[1]);

  if(tipz == 2)
    kolih*=(-1);
  itog+=kolih;
  
  kol2=0.;
  if(nk != 0)
    kol2=readkolkw(skll,nk,d,m,g,nn.ravno(),wpredok);
  if(tipz == 2)
    kol2*=(-1);
    
  itog2+=kol2;
/************  
  printf("%02d.%02d.%d %-*s %-6d %8.8g %8.8g %8.8g %-4d %-5s %s\n",
  d,m,g,
  iceb_u_kolbait(10,nn.ravno()),nn.ravno(),
  nk,kolih,kol2,cena,skll,kop.ravno(),kor.ravno());
*************/    
  fprintf(ff1,"%02d.%02d.%d %-*s %-6d %8.8g %8.8g %8.8g %-4d %-5s %-10s\n",
  d,m,g,
  iceb_u_kolbait(10,nn.ravno()),nn.ravno(),
  nk,kolih,kol2,cena,skll,kop.ravno(),kor.ravno());

 }
if(fabs(itog) < 0.00000001)
 itog=0.;
if(fabs(itog2) < 0.00000001)
 itog2=0.;
 
fprintf(ff1,"\
------------------------------------------------------------------------------\n");

fprintf(ff1,"%*s %8.8g %8.8g\n",iceb_u_kolbait(28,gettext("Остаток")),gettext("Остаток"),itog,itog2);

//printf("%*s %8.8g %8.8g\n",iceb_u_kolbait(28,gettext("Остаток")),gettext("Остаток"),itog,itog2);

iceb_podpis(ff1,wpredok);
fclose(ff1);

iceb_ustpeh(imaf,3,wpredok);

class iceb_u_spisok imafs;
class iceb_u_spisok naimoth;

imafs.plus(imaf);
naimoth.plus(gettext("Движение материалла по документам"));

iceb_rabfil(&imafs,&naimoth,wpredok);

}
