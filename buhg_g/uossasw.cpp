/* $Id: uossasw.c,v 1.13 2013/08/13 06:09:55 sasa Exp $ */
/*01.01.2018    29.05.1997      Белых А.И.      uossasw.c
Акт списания автотранспортного средства
*/
#include        <errno.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void uossasw(const char *data_dok, //Дата
const char *ndk, //Номер документа
GtkWidget *wpredok)
{
short d,m,g;
class iceb_u_str str("");
FILE            *ff1;
char            imaf1[112];
int            in;
int             pdr;
double		pbs,piz;
long		kolstr;
SQL_str         row,row1,row_alx;
SQLCURSOR curr,cur_alx;
char		strsql[1024];
float		kof;
short		dv,mv,gv;
short		god;
class iceb_u_str naim("");
class iceb_u_str model("");
char		datpr[32];
class iceb_u_str kprr("");
class iceb_u_str zavnom("");

iceb_u_rsdat(&d,&m,&g,data_dok,1);

/*Читаем шапку документа*/
sprintf(strsql,"select tipz,kontr,osnov,kodop from Uosdok where datd='%d.%02d.%02d' and \
nomd='%s'",g,m,d,ndk);

if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
 {
  sprintf(strsql,"%s %s %d.%d.%d%s",
  gettext("Не найден документ"),ndk,d,m,g,
  gettext("г."));
  iceb_menu_soob(strsql,wpredok);
  return;
 }

kprr.new_plus(row1[3]);

sprintf(imaf1,"asa%d.lst",getpid());

errno=0;
if((ff1 = fopen(imaf1,"r")) == NULL)
 if(errno != ENOENT)
   {
    iceb_er_op_fil(imaf1,"",errno,wpredok);
    return;
   }

if(errno != ENOENT) /*Файл сущесвует*/
 {
  fclose(ff1);

  if(iceb_menu_danet(gettext("Распечатка уже есть! Переформировать ?"),2,wpredok) == 2)
   {

    class iceb_u_spisok imafil;
    class iceb_u_spisok naimf;
    imafil.plus(imaf1);
    naimf.plus(gettext("Распечатка акта списания транспортного средства"));
    iceb_rabfil(&imafil,&naimf,wpredok);
    return;
   }
 }


sprintf(strsql,"select innom,podr,bs,iz from Uosdok1 where datd='%d.%02d.%02d' and \
nomd='%s'",g,m,d,ndk);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),wpredok);
  return;
 }

sprintf(strsql,"select str from Alx where fil='uossas.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uossas.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }


if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }

sprintf(datpr,"%d.%d.%d",d,m,g);

int kkk=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  kkk++;
  str.new_plus(row_alx[0]);
  switch(kkk)
   {
    case 1 :
     iceb_u_vstav(&str,iceb_get_pnk("00",0,wpredok),0,39,1);
     break;

    case 15:
     iceb_u_vstav(&str,ndk,149,159,1);
     iceb_u_vstav(&str,datpr,161,170,1);
     iceb_u_vstav(&str,kprr.ravno(),173,180,1);
     break;
    case 29:
     goto vper;
   }
  fprintf(ff1,"%s",str.ravno());
 }
vper:;

class poiinpdw_data rekin;  

while(cur.read_cursor(&row) != 0)
 {
  in=atol(row[0]);
  pdr=atoi(row[1]);
  pbs=atof(row[2]);
  piz=atof(row[3]);

  rekin.clear();
  /*Читаем переменные данные*/
  poiinpdw(in,m,g,&rekin,wpredok);

  /*Читаем поправочный коэффициент*/
  kof=0.;
  sprintf(strsql,"select kof from Uosgrup where kod='%s'",rekin.hna.ravno());

  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код группы"),rekin.hna.ravno());
    iceb_menu_soob(strsql,wpredok);
   }
  else
   kof=atof(row1[0]);

  god=0;
  dv=mv=gv=0;

  /*Читаем постоянную часть*/   
  sprintf(strsql,"select god,pasp,datvv,zavnom,naim,model from Uosin \
where innom=%d",in);

  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден инвентарный номер"),in);
    iceb_menu_soob(strsql,wpredok);
    naim.new_plus("");
    model.new_plus("");
    zavnom.new_plus("");
   }
  else
   {
    god=atoi(row1[0]);
    iceb_u_rsdat(&dv,&mv,&gv,row[2],2);
    zavnom.new_plus(row1[3]);
    naim.new_plus(row1[4]);
    model.new_plus(row1[5]);
   }

/*               1   2   3   4   5    6        7     8
    9     10   11    12    13   14  15  16    17*/

  fprintf(ff1," %7d %5s %9s %*s %*s %10.10g %11.11g %10d\
 %*.*s %*s %6.6g %8.8g %8.8g %*s %*s %4d %02d.%d\n",
  pdr,
  " ",
  " ",
  iceb_u_kolbait(5,rekin.shetu.ravno()),rekin.shetu.ravno(),
  iceb_u_kolbait(9,rekin.hau.ravno()),rekin.hau.ravno(),
  pbs*(-1),piz*(-1),in,
  iceb_u_kolbait(10,zavnom.ravno()),iceb_u_kolbait(10,zavnom.ravno()),zavnom.ravno(),
  iceb_u_kolbait(17,rekin.hzt.ravno()),rekin.hzt.ravno(),
  0.,kof,0.,
  iceb_u_kolbait(7,rekin.hna.ravno()),rekin.hna.ravno(),
  iceb_u_kolbait(7,rekin.hna.ravno()),rekin.hna.ravno(),
  god,mv,gv);

 }
 
kkk=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  kkk++;
  str.new_plus(row_alx[0]);

  if(str.ravno()[0] == '=')
   {
    fprintf(ff1,"\f\n");
    continue;
   }  
  switch(kkk)
   {
    case 3:
     iceb_u_vstav(&str,naim.ravno(),53,72,1);
     iceb_u_vstav(&str,model.ravno(),90,109,1);
     break;    

    case 5:
     iceb_u_vstav(&str,rekin.nomz.ravno(),55,64,1);
     break;

     
   }
  fprintf(ff1,"%s",str.ravno());
 }

iceb_podpis(ff1,wpredok);

fclose(ff1);

class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf1);
naimf.plus(gettext("Распечатка акта списания транспортного средства"));

iceb_ustpeh(imaf1,0,wpredok);

iceb_rabfil(&imafil,&naimf,wpredok);

}
