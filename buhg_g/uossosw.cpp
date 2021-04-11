/* $Id: uossosw.c,v 1.14 2013/08/13 06:09:55 sasa Exp $ */
/*16.10.2015    29.05.1997      Белых А.И.      uossosw.c
Акт списания основных средств
*/
#include        <errno.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void uossosw(const char *data_dok, //дата документа
const char *ndk,  //Номер документа
GtkWidget *wpredok)
{
short d,m,g;
class iceb_u_str str("");
class iceb_u_str bros("");
FILE            *ff1;
char            imaf1[64];
int            in;
int             pdr;
double		pbs,piz;
long		kolstr;
SQL_str         row,row1,row_alx;
SQLCURSOR curr,cur_alx;
char strsql[2048];
float		kof;
short		dv,mv,gv;
short		god;
class iceb_u_str naim("");
class iceb_u_str model("");
class iceb_u_str osnov("");
char		datpr[32];
class iceb_u_str kprr("");
class iceb_u_str zavnom("");

iceb_u_rsdat(&d,&m,&g,data_dok,1);


sprintf(imaf1,"aso%d.lst",getpid());
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
    naimf.plus(gettext("Распечатка акта списания основых средств"));
    iceb_rabfil(&imafil,&naimf,wpredok);
    
    return;
   }
 }
sprintf(datpr,"%d.%d.%d",d,m,g);

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
osnov.new_plus(row1[2]);
kprr.new_plus(row1[3]);

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

sprintf(strsql,"select str from Alx where fil='uossos.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uossos.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }

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
     iceb_u_vstav(&str,ndk,131,140,1);
     iceb_u_vstav(&str,datpr,143,152,1);
     iceb_u_vstav(&str,kprr.ravno(),155,163,1);
     break;
    case 29:
     goto vper;
   }
  fprintf(ff1,"%s",str.ravno());
 }
vper:;

while(cur.read_cursor(&row) != 0)
 {
  
  in=atol(row[0]);
  pdr=atoi(row[1]);
  pbs=atof(row[2]);
  piz=atof(row[3]);

  class poiinpdw_data rekin;  
  /*Читаем переменные данные*/
  poiinpdw(in,m,g,&rekin,wpredok);

  /*Читаем поправочный коэффициент*/
  kof=0.;
  sprintf(strsql,"select kof from Uosgrup where kod='%s'",rekin.hna.ravno());

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
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

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден инвентарный номер"),in);
    iceb_menu_soob(strsql,wpredok);
    model.new_plus("");
    naim.new_plus("");
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
   pdr," "," ",
   iceb_u_kolbait(5,rekin.shetu.ravno()),rekin.shetu.ravno(),
   iceb_u_kolbait(9,rekin.hau.ravno()),rekin.hau.ravno(),
   pbs*(-1),piz*(-1),in,
   iceb_u_kolbait(10,zavnom.ravno()),iceb_u_kolbait(10,zavnom.ravno()),zavnom.ravno(),
   iceb_u_kolbait(17,rekin.hzt.ravno()),rekin.hzt.ravno(),
   0.,kof,0.,
   iceb_u_kolbait(5,rekin.hna.ravno()),rekin.hna.ravno(),
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
     iceb_u_vstav(&str,osnov.ravno(),12,50,1);
     bros.new_plus(naim.ravno());
     bros.plus(" ",model.ravno());

     iceb_u_vstav(&str,bros.ravno(),81,159,1);
     break;    

    case 36:
     sprintf(strsql,"%s %s",naim.ravno(),model.ravno());
     iceb_u_vstav(&str,strsql,20,99,1);
     break;
     
   }
  fprintf(ff1,"%s",str.ravno());
 }

iceb_podpis(ff1,wpredok);

fclose(ff1);
class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf1);
naimf.plus(gettext("Распечатка акта списания основых средств"));
iceb_ustpeh(imaf1,3,wpredok);
iceb_rabfil(&imafil,&naimf,wpredok);
}
