/* $Id: koreshw.c,v 1.14 2014/02/28 05:20:59 sasa Exp $ */
/*16.04.2017    11.12.1995      Белых А.И.      koreshw.c
Распечатка корешков
*/
#include        <errno.h>
#include        "buhg_g.h"

extern short	*obud; /*Обязательные удержания*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern SQL_baza bd;

void koreshw(int ttt,short mp,short gp,const char *fio,const char *inn,const char *dolgn,int podr,int kateg,GtkWidget *wpredok)
{
FILE            *kaw;
class iceb_u_str st("");
class iceb_u_str str("");
class iceb_u_str np("");
short           kodnah=0;
double          prom;
short           kd;
char            imaf[64];
short           im;
double          inah=0.,iu=0.,ius=0.;
double          inahb=0.,iub=0.,iusb=0.;
short           mkvk; /*0 не выдано кассой 1-выдано кассой*/
double		sal;
short		prn;
int		knah;
double		sym;
short		d,m,g;
short		mesn,godn;
char		strsql[1024];
SQL_str         row,row1;
long		kolstr;
class iceb_u_str naim("");
int		i1;
double		saldb;
SQLCURSOR curr;



sprintf(imaf,"koreh%d_%d.lst",ttt,getpid());

if((kaw = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

inah=iu=ius=0.;
/*Читаем сальдо*/
sal=zarsaldw(1,mp,gp,ttt,&saldb,wpredok);

sprintf(strsql,"select naik from Podr where kod=%d",podr);
if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
 {
  sprintf(strsql,"%s %d\n",gettext("Не найден код подразделения"),podr);
  iceb_menu_soob(strsql,wpredok);
 }
else
  np.new_plus(row1[0]);

peh1w(np.ravno(),fio,ttt,inn,mp,gp,dolgn,kateg,kaw,NULL,NULL,NULL,wpredok); /*rasveds*/

sprintf(strsql,"select prn,knah,suma,datz,mesn,godn,kdn,podr,shet from \
Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and \
tabn=%d and suma <> 0. order by prn,knah asc",gp,mp,gp,mp,ttt);

class SQLCURSOR cur;
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

im=0;
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],
  row[5],row[6]);
  refresh();
*/
  prn=atoi(row[0]);
  knah=atoi(row[1]);
  sym=atof(row[2]);
  iceb_u_rsdat(&d,&m,&g,row[3],2);
  mesn=atoi(row[4]);
  godn=atoi(row[5]);
  kd=atoi(row[6]);
    
  if(kodnah != prn)
   {
    if(prn == 1)
     {
      str.new_plus(gettext(" Н а ч и с л е н и я"));
      strfilw(str.ravno(),str.ravno(),1,kaw);
     }
   if(prn == 2)
    {
     str.new_plus(gettext(" У д е р ж а н и я"));
     strfilw(str.ravno(),str.ravno(),1,kaw);
    }
   kodnah=prn;
  }

 if(prn == 1)
   sprintf(strsql,"select naik from Nash where kod=%d",knah);
 if(prn == 2)
   sprintf(strsql,"select naik from Uder where kod=%d",knah);
    
 naim.new_plus("");
 if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
  {
   if(prn == 1)
    sprintf(strsql,"%s %s",gettext("Не найден код начисления"),row[3]);
   if(prn == 2)
    sprintf(strsql,"%s %s",gettext("Не найден код удержания"),row[3]);
   iceb_menu_soob(strsql,wpredok);
  }
 else
   naim.new_plus(row1[0]);


 st.new_plus("");

 prom=sym;
 mkvk=0;
 if(prn == 2)
  {
   prom*=-1;

   if(obud != NULL)
    {
     for(i1=1; i1<= obud[0]; i1++)
      if(obud[i1] == knah)
       {
        break;
       }

     if(i1 > obud[0])
       if(godn != gp || mesn != mp)
          mkvk=1;

     }

   im++;
  }

 iceb_u_prnb(prom,&st);

 if(kd != 0)
  sprintf(strsql,"%3d %-*.*s %2d %2d.%4d %-2s %12s",
  knah,
  iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
  kd,mesn,godn,row[7],st.ravno());
 else
  sprintf(strsql,"%3d %-*.*s %2s %2d.%4d %-2s %12s",
  knah,
  iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
  " ",mesn,godn,row[7],st.ravno());

 if(prn == 2 && mkvk == 1)
  strfilw(strsql," ",0,kaw);
 else
  strfilw(strsql,strsql,0,kaw);

 if(prn == 1)
  {
   inah+=sym;
   if(iceb_u_proverka(shetb,row[8],0,1) == 0)
     inahb+=sym;
   
  }
 if(prn == 2)
  {
   iu+=sym;
   if(iceb_u_proverka(shetbu,row[8],0,1) == 0)
     iub+=sym;
   if(mkvk == 0)
    {
     ius+=sym;
     if(iceb_u_proverka(shetbu,row[8],0,1) == 0)
       iusb+=sym;

    }
  }

 }

if(im > 0 || sal != 0.)
 {
  itogizw(inah,iu,ius,sal,inahb,iub,iusb,saldb,kaw,NULL,NULL,NULL); /*rasved.c*/
 }

iceb_podpis(kaw,wpredok);
fclose(kaw);

class iceb_u_spisok imafils;
class iceb_u_spisok naimf;

imafils.plus(imaf);
naimf.plus(gettext("Зарплатный корешок"));

iceb_ustpeh(imaf,3,wpredok);

iceb_rabfil(&imafils,&naimf,wpredok);

}
