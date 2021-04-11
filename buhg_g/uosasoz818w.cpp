/*$Id:$*/
/*17.05.2017	17.05.2017	Белых А.И.	uosasoz818w.cpp
Акт списания основных средств приказ министерства финансов Украини N818 от 13.09.2016 г.
*/
#include "buhg_g.h"

extern SQL_baza bd;

int uosasoz818w(const char *data_dok,const char *nomdok,GtkWidget *wpredok)
{
short dd=0,md=0,gd=0;
int metka_u=0; /*0-налоговый 1-бухгалтерский учёт*/
char strsql[512];
char imaf[64];
char imaf2[64];
char imaf3[64];

int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
class iceb_vrvr rv;

rv.ns.plus(4);
rv.begin.plus(0);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(iceb_get_pnk("00",1,wpredok));

rv.ns.plus(7);
rv.begin.plus(33);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(iceb_get_edrpou("00",wpredok));

rv.ns.plus(21);
rv.begin.plus(106);
rv.end.plus(127);
rv.hag.plus(1);
rv.str.plus(nomdok);

sprintf(strsql,"%02d.%02d.%d",dd,md,gd);
rv.ns.plus(21);
rv.begin.plus(129);
rv.end.plus(150);
rv.hag.plus(1);
rv.str.plus(strsql);

if((metka_u=uos_menu_vu(wpredok)) < 0)
 return(1);

iceb_u_rsdat(&dd,&md,&gd,data_dok,1);

sprintf(strsql,"select innom,bs,bsby,iz,izby from Uosdok1 where datd='%d.%02d.%02d' and nomd='%s'",gd,md,dd,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }

sprintf(imaf,"asoz1_%d.lst",getpid());
class iceb_fopen fil;
if(fil.start(imaf,"w",wpredok) != 0)
 return(1);

iceb_vrvr("uosasoz818_1.alx",&rv,fil.ff,wpredok); /*вставка реквизитов в шапку документа*/

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),wpredok);
 }

double bal_st=0.;
double iznos=0.;
class iceb_u_str naim_os("");
class iceb_u_str pasport("");
class iceb_u_str data_vv("");
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naim,pasp,datvv from Uosin where innom=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   {
    naim_os.new_plus(row1[0]);
    pasport.new_plus(row1[1]);
    data_vv.new_plus(iceb_u_datzap(row1[2]));
   }
  else
   {
    naim_os.new_plus("");
    pasport.new_plus("");
    data_vv.new_plus("");
   }

  if(metka_u == 0)
   {
    bal_st=atof(row[1]);
    iznos=atof(row[3]);
   }
  else
   {
    bal_st=atof(row[2]);
    iznos=atof(row[4]);
   }   

  fprintf(fil.ff,"|%-*.*s|%-*.*s|%-22s|%-25s|%22.2f|%10.2f|%12s|\n",
  iceb_u_kolbait(39,naim_os.ravno()),
  iceb_u_kolbait(39,naim_os.ravno()),
  naim_os.ravno(),
  iceb_u_kolbait(14,pasport.ravno()),
  iceb_u_kolbait(14,pasport.ravno()),
  pasport.ravno(),
  row[0],
  data_vv.ravno(),
  bal_st,
  iznos,    
  "");    

 }

iceb_insfil("uosasoz818_1_end.alx",fil.ff,wpredok);

fil.end();

sprintf(imaf2,"asoz2_%d.lst",getpid());
if(fil.start(imaf2,"w",wpredok) != 0)
 return(1);

iceb_insfil("uosasoz818_2.alx",fil.ff,wpredok);

fil.end();

sprintf(imaf3,"asoz3_%d.lst",getpid());
if(fil.start(imaf3,"w",wpredok) != 0)
 return(1);

iceb_insfil("uosasoz818_3.alx",fil.ff,wpredok);

fil.end();

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Первая страница акта списания основных средств"));

oth.spis_imaf.plus(imaf2);
oth.spis_naim.plus(gettext("Вторая страница акта списания основных средств"));

oth.spis_imaf.plus(imaf3);
oth.spis_naim.plus(gettext("Третья страница акта списания основных средств"));

for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
 iceb_ustpeh(oth.spis_imaf.ravno(nom),3,wpredok);

iceb_rabfil(&oth,wpredok);


return(0);

}
