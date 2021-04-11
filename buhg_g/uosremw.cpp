/* $Id: uosremw.c,v 1.13 2013/08/13 06:09:55 sasa Exp $ */
/*15.10.2015    30.05.1997      Белых А.И.      uosremw.c
Распечатка акта ремонта/модернизации основных средств
*/
#include        <errno.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void uosremw(const char *data_dok, //Дата документа
const char *ndk, //Номер документа
GtkWidget *wpredok)
{
short d,m,g;
class iceb_u_str str("");
FILE            *ff1;
char            imaf1[64];
int            in;
int             pdr;
double          bs=0.;
double		pbs;
long		kolstr,kolstr1;
SQL_str         row,row1,row_alx;
SQLCURSOR curr,cur_alx;
char		strsql[2048];
short		dv,mv,gv;
class iceb_u_str naim("");
class iceb_u_str model("");
short		tipz;
class iceb_u_str kpos("");
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
tipz=atoi(row1[0]);
kpos.new_plus(row1[1]);
kprr.new_plus(row1[3]);

if(tipz == 1)
  sprintf(imaf1,"arp%d.lst",getpid());
if(tipz == 2)
  sprintf(imaf1,"arr%d.lst",getpid());

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
    naimf.plus(gettext("Распечатка акта ремонта/дооборудования"));
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

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }

sprintf(strsql,"select str from Alx where fil='uosrem.alx' order by ns asc");
if((kolstr1=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return;
 }

if(kolstr1 == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uosrem.alx");
  iceb_menu_soob(strsql,wpredok);
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

    case 21 :
     iceb_u_vstav(&str,ndk,47,56,1);
     sprintf(datpr,"%d.%d.%d",d,m,g);
     iceb_u_vstav(&str,datpr,59,68,1);
     iceb_u_vstav(&str,kprr.ravno(),88,97,1);
     break;

    case 35 :
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
  
  class bsizw_data bal_st;
  
  bsizw(in,pdr,d,m,g,&bal_st,NULL,wpredok);
  bs=bal_st.sbs+bal_st.bs;

  /*Читаем переменные данные*/
  class poiinpdw_data rekin;
  poiinpdw(in,m,g,&rekin,wpredok);

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
    iceb_u_rsdat(&dv,&mv,&gv,row[2],2);
    zavnom.new_plus(row1[3]);
    naim.new_plus(row1[4]);
    model.new_plus(row1[5]);
   }

  if(tipz == 1)
   fprintf(ff1," %*s %*.*s %*.*s %5.5s %9.9s %9.9g %10d %*.*s\n",
   iceb_u_kolbait(30,kpos.ravno()),kpos.ravno(),
   iceb_u_kolbait(5,rekin.shetu.ravno()),iceb_u_kolbait(5,rekin.shetu.ravno()),rekin.shetu.ravno(),
   iceb_u_kolbait(9,rekin.hau.ravno()),iceb_u_kolbait(9,rekin.hau.ravno()),rekin.hau.ravno(),
   " "," ",bs,in,
   iceb_u_kolbait(10,zavnom.ravno()),iceb_u_kolbait(10,zavnom.ravno()),zavnom.ravno());

  if(tipz == 2)
   fprintf(ff1," %-30d %5.5s %9.9s %*.*s %*.*s %9.9g %10d %*.*s\n",
   pdr," "," ",
   iceb_u_kolbait(5,rekin.shetu.ravno()),iceb_u_kolbait(5,rekin.shetu.ravno()),rekin.shetu.ravno(),
   iceb_u_kolbait(9,rekin.hau.ravno()),iceb_u_kolbait(9,rekin.hau.ravno()),rekin.hau.ravno(),
   pbs*(-1),in,
   iceb_u_kolbait(10,zavnom.ravno()),iceb_u_kolbait(10,zavnom.ravno()),zavnom.ravno());

 }

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
naimf.plus(gettext("Распечатка акта ремонта/дооборудования"));

iceb_ustpeh(imaf1,3,wpredok);

iceb_rabfil(&imafil,&naimf,wpredok);
}


