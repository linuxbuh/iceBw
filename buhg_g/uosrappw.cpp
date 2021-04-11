/*$Id:$*/
/*18.05.2017	22.11.2016	Белых А.И.	uosrappw.c
Распечатка акта приёма-передачи основных средств
*/

#include "buhg_g.h"
extern SQL_baza bd;

int uosrappw(const char *datadok,const char *nomdok,GtkWidget *wpredok)
{
short dd=0,md=0,gd=0;
int metka_u=0; /*0-налоговый 1-бухгалтерский учёт*/
char strsql[512];
char imaf[64];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
class iceb_u_str naimfl("");
class iceb_u_str naimfl_peredae("");
class iceb_u_str naimfl_poluch("");
class iceb_vrvr rv;

iceb_u_rsdat(&dd,&md,&gd,datadok,1);

rv.ns.plus(1);
rv.begin.plus(0);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(iceb_get_pnk("00",1,wpredok));

rv.ns.plus(6);
rv.begin.plus(31);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(iceb_get_edrpou("00",wpredok));

rv.ns.plus(19);
rv.begin.plus(101);
rv.end.plus(125);
rv.hag.plus(1);
rv.str.plus(nomdok);

sprintf(strsql,"%02d.%02d.%d",dd,md,gd);
rv.ns.plus(19);
rv.begin.plus(127);
rv.end.plus(146);
rv.hag.plus(1);
rv.str.plus(strsql);


/*Читаем шапку документа*/
sprintf(strsql,"select tipz,kodol,kodolv from Uosdok where datd='%d.%02d.%02d' and nomd='%s'",gd,md,dd,nomdok);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s-%s %s %d.%d.%d%s",
  __FUNCTION__,
  gettext("Не найден документ !"),nomdok,dd,md,gd,
  gettext("г."));
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
if((metka_u=uos_menu_vu(wpredok)) < 0)
 return(1);
/************
class VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Налоговый учёт"));
MENU.VVOD_SPISOK_add_MD(gettext("Бухгалтерский учёт"));
while(menu3w(stdscr,&MENU,&metka_u,-1,-1,0) != 0);

if(metka_u == -1)
 return(1);
**************/
int tipz=atoi(row[0]);
class iceb_u_str kodol(row[1]);
class iceb_u_str kodolv(row[2]);

sprintf(strsql,"select naik from Uosol where kod=%d",kodol.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(tipz == 2)
   naimfl_peredae.new_plus(row[0]);
  else
   naimfl_poluch.new_plus(row[0]);
 }
sprintf(strsql,"select naik from Uosol where kod=%d",kodolv.ravno_atoi());

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(tipz == 2)
   naimfl_poluch.new_plus(row[0]);
  else
   naimfl_peredae.new_plus(row[0]);
 }

sprintf(strsql,"select innom,bs,iz,bsby,izby from Uosdok1 where datd='%d.%02d.%02d' and nomd='%s'",gd,md,dd,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }

sprintf(imaf,"app%d.lst",getpid());
class iceb_fopen fil;
if(fil.start(imaf,"w",wpredok) != 0)
 return(1);

iceb_vrvr("uosapp_h.alx",&rv,fil.ff,wpredok); /*вставка реквизитов в шапку документа*/

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),wpredok);
 }

double bal_st=0.;
double iznos=0.;
int god_vipuska=0;
class iceb_u_str nomer_pasp("");
int itogo_kol=0;
double itogo_sum[3];
memset(itogo_sum,'\0',sizeof(itogo_sum));

while(cur.read_cursor(&row) != 0)
 {
  itogo_kol++;
  sprintf(strsql,"select god,pasp from Uosin where innom=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   {
    god_vipuska=atoi(row1[0]);
    nomer_pasp.new_plus(row1[1]);
   }
  else
   {
    god_vipuska=0;
    nomer_pasp.new_plus("");
   }

  if(metka_u == 0)
   {
    bal_st=atof(row[1]);
    iznos=atof(row[2]);
   }
  else
   {
    bal_st=atof(row[3]);
    iznos=atof(row[4]);
   }   

  fprintf(fil.ff,"%-*.*s %-*.*s %-16s %9d %13.2f %12.2f %12.2f %12.2f %-11d %s\n",
  iceb_u_kolbait(22,naimfl_peredae.ravno()),
  iceb_u_kolbait(22,naimfl_peredae.ravno()),
  naimfl_peredae.ravno(),
  iceb_u_kolbait(22,naimfl_poluch.ravno()),
  iceb_u_kolbait(22,naimfl_poluch.ravno()),
  naimfl_poluch.ravno(),
  row[0],
  1,
  bal_st,
  bal_st,
  iznos,
  iznos,
  god_vipuska,
  nomer_pasp.ravno());

  itogo_sum[0]+=bal_st;
  itogo_sum[1]+=bal_st;
  itogo_sum[2]+=iznos;
 }

fprintf(fil.ff,"%*s %9d %13.2f %12.2f %12.2f %12.2f\n",
iceb_u_kolbait(62,"Всього"),
"Всього",
itogo_kol,
itogo_sum[0],
itogo_sum[1],
itogo_sum[2],
itogo_sum[2]);

iceb_vrvr("uosapp_end.alx",NULL,fil.ff,wpredok); /*вставка концовки*/
fprintf(fil.ff,"\f");
iceb_vrvr("uosapp_end2.alx",NULL,fil.ff,wpredok); /*вставка концовки*/

iceb_podpis(fil.ff,wpredok);
fil.end();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка акта приёмки/передачи основных средств"));

iceb_ustpeh(imaf,3,wpredok);

iceb_rabfil(&oth,wpredok);


return(0);

}
