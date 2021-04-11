/* $Id: uosactw.c,v 1.16 2013/08/13 06:09:54 sasa Exp $ */
/*13.07.2015    17.02.1997      Белых А.И.      uosactw.c
Распечатка акта приема передачи основных средств
*/
#include        <errno.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void uosactw(const char *data_dok, //Дата документа
const char *ndk, //номер документа
GtkWidget *wpredok)
{
class iceb_u_str str("");
FILE            *ff1;
char            imaf1[112];
int            in;
int             pdr;
double		pbs;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
float		kof;
short		dv,mv,gv;
short		god;
class iceb_u_str pasp("");
class iceb_u_str naim("");
class iceb_u_str model("");
short		tipz;
class iceb_u_str osnov("");
class iceb_u_str kpos("");
char		datpr[64];
class iceb_u_str kprr("");
class iceb_u_str zavnom("");
int		kkk=0;
class iceb_u_str godchar("");
class iceb_u_str mesqc("");
class iceb_u_str kodol("");
double izby=0.;
short d,m,g;
SQLCURSOR cur;
SQLCURSOR curr;
iceb_u_rsdat(&d,&m,&g,data_dok,1);

class iceb_u_str kodedrpou("");

/*Узнаём код */
sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row1,&cur,wpredok) == 1)
 kodedrpou.new_plus(row1[0]);

godchar.new_plus(g);
/*Читаем шапку документа*/
sprintf(strsql,"select tipz,kontr,osnov,kodop,kodol from Uosdok where datd='%d.%02d.%02d' and \
nomd='%s'",g,m,d,ndk);

if(iceb_sql_readkey(strsql,&row1,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s %s %d.%d.%d%s",
  gettext("Не найден документ"),ndk,d,m,g,
  gettext("г."));
  iceb_menu_soob(strsql,wpredok);
  return;
 }
else
 {
  tipz=atoi(row1[0]);
  kpos.new_plus(row1[1]);
  osnov.new_plus(row1[2]);
  kprr.new_plus(row1[3]);
  kodol.new_plus(row1[4]);
 }  


if(tipz == 1)
  sprintf(imaf1,"acp%d.lst",getpid());
if(tipz == 2)
  sprintf(imaf1,"acr%d.lst",getpid());

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
    naimf.plus(gettext("Распечатка акта приёмки"));
    iceb_rabfil(&imafil,&naimf,wpredok);
    return;
   }
 }

sprintf(datpr,"%d.%d.%d",d,m,g);


sprintf(strsql,"select innom,podr,bsby from Uosdok1 where datd='%d.%02d.%02d' and \
nomd='%s'",g,m,d,ndk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),wpredok);
  return;
 }

SQL_str row_alx;
class SQLCURSOR cur_alx;
const char *imaf_alx={"uosact.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return;
 }



if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }

kkk=0;
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

   case 3 :
     iceb_u_vstav(&str,kodedrpou.ravno(),29,48,1);
     break;

    case 6 :
     iceb_mesc(m,1,&mesqc);
     sprintf(strsql,"%02d %s %04dр.",d,mesqc.ravno(),g);

     iceb_u_vstav(&str,strsql,123,149,1);
     break;

    case 14 :
     iceb_u_vstav(&str,ndk,130,140,1);
     iceb_u_vstav(&str,datpr,142,152,1);
     iceb_u_vstav(&str,kodol.ravno(),155,169,1);
     iceb_u_vstav(&str,kprr.ravno(),172,178,1);
     break;

    case 28 :
      while(cur.read_cursor(&row) != 0)
       {
        
        in=atol(row[0]);
        pdr=atoi(row[1]);
        pbs=atof(row[2]);
        
        class bsizw_data bal_st;
        bsizw(in,pdr,d,m,g,&bal_st,NULL,wpredok);


//        bsby=bal_st.sbsby+bal_st.bsby;
        izby=bal_st.sizby+bal_st.izby+bal_st.iz1by;

        class poiinpdw_data rekin;
        /*Читаем переменные данные*/
        poiinpdw(in,m,g,&rekin,wpredok);

        /*Читаем поправочный коэффициент*/
        kof=0.;
        sprintf(strsql,"select kof from Uosgrup1 where kod='%s'",rekin.hnaby.ravno());

        if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
         {
          sprintf(strsql,"%s %s !",gettext("Не найден код группы"),rekin.hnaby.ravno());
          iceb_menu_soob(strsql,wpredok);
         }
        else
         kof=atof(row1[0]);

        god=0;
        dv=mv=gv=0;
        
        /*Читаем постоянную часть*/   
        sprintf(strsql,"select god,pasp,datvv,zavnom,naim,model \
from Uosin where innom=%d",in);

        if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
         {
          sprintf(strsql,"%s %d !",gettext("Не найден инвентарный номер"),in);
          iceb_menu_soob(strsql,wpredok);
          pasp.new_plus("");
          zavnom.new_plus("");
          model.new_plus("");
          naim.new_plus("");
         }
        else
         {
          god=atoi(row1[0]);
          pasp.new_plus(row1[1]);
          iceb_u_rsdat(&dv,&mv,&gv,row1[2],2);
          zavnom.new_plus(row1[3]);
          naim.new_plus(row1[4]);
          model.new_plus(row1[5]);
         }

        if(tipz == 1)
         fprintf(ff1," %*.*s %5.5d %*.*s %*.*s %5.5s %9.9s %9.9g %10d\
 %*.*s %*.*s %*.*s %8.8g %8.8g %6.6g %5.5s %7.7s %11.11g %4d %02d.%04d\
 %-*.*s\n",
         iceb_u_kolbait(4,kpos.ravno()),iceb_u_kolbait(4,kpos.ravno()),kpos.ravno(),
         pdr,
         iceb_u_kolbait(5,rekin.shetu.ravno()),iceb_u_kolbait(5,rekin.shetu.ravno()),rekin.shetu.ravno(),
         iceb_u_kolbait(9,rekin.hau.ravno()),iceb_u_kolbait(9,rekin.hau.ravno()),rekin.hau.ravno(),
         " "," ",
         pbs,
         in,
         iceb_u_kolbait(10,zavnom.ravno()),iceb_u_kolbait(10,zavnom.ravno()),zavnom.ravno(),
         iceb_u_kolbait(17,rekin.hzt.ravno()),iceb_u_kolbait(17,rekin.hzt.ravno()),rekin.hzt.ravno(),
         iceb_u_kolbait(6,rekin.hnaby.ravno()),iceb_u_kolbait(6,rekin.hnaby.ravno()),rekin.hnaby.ravno(),
         kof,0.,
         rekin.popkf," "," ",izby,god,mv,gv,
         iceb_u_kolbait(14,pasp.ravno()),iceb_u_kolbait(14,pasp.ravno()),pasp.ravno());

        if(tipz == 2)
         fprintf(ff1," %4.4d %*.*s %5.5s %9.9s %*.*s %*.*s %9.9g %10d\
 %*.*s %*.*s %*.*s %8.8g %8.8g %6.6g %5.5s %7.7s %11.11g %4d %02d.%04d\
 %-*.*s\n",
         pdr,
         iceb_u_kolbait(5,kpos.ravno()),iceb_u_kolbait(5,kpos.ravno()),kpos.ravno(),
         " "," ",
         iceb_u_kolbait(5,rekin.shetu.ravno()),iceb_u_kolbait(5,rekin.shetu.ravno()),rekin.shetu.ravno(),
         iceb_u_kolbait(9,rekin.hau.ravno()),iceb_u_kolbait(9,rekin.hau.ravno()),rekin.hau.ravno(),
         pbs,in,
         iceb_u_kolbait(10,zavnom.ravno()),iceb_u_kolbait(10,zavnom.ravno()),zavnom.ravno(),
         iceb_u_kolbait(17,rekin.hzt.ravno()),iceb_u_kolbait(17,rekin.hzt.ravno()),rekin.hzt.ravno(),
         iceb_u_kolbait(6,rekin.hnaby.ravno()),iceb_u_kolbait(6,rekin.hnaby.ravno()),rekin.hnaby.ravno(),
         kof,0.,
         rekin.popkf," "," ",izby*(-1),god,mv,gv,
         iceb_u_kolbait(14,pasp.ravno()),iceb_u_kolbait(14,pasp.ravno()),pasp.ravno());
       }
     break;

    case 29 :
     iceb_u_vstav(&str,osnov.ravno(),34,73,1);
     break;

    case 33 :
     sprintf(strsql,"%s %s",naim.ravno(),model.ravno());
     iceb_u_vstav(&str,strsql,62,139,1);
     break;

    case 31 :
     iceb_u_vstav(&str,godchar.ravno(),22,27,1);
     break;

    case 41 :
     fprintf(ff1,"\x1b\x6C%c",1); /*Установка левого поля*/
     fprintf(ff1,"\f\n");
     continue;

    case 61 :
     iceb_mesc(m,1,&mesqc);
     sprintf(strsql,"%02d %s %04dр.",d,mesqc.ravno(),g);

     iceb_u_vstav(&str,strsql,69,89,1);

     break;
   }

  fprintf(ff1,"%s",str.ravno());

 }

iceb_podpis(ff1,wpredok);

fclose(ff1);
class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf1);
naimf.plus(gettext("Распечатка акта приёмки"));
iceb_ustpeh(imaf1,0,wpredok);
iceb_rabfil(&imafil,&naimf,wpredok);

return;

}
