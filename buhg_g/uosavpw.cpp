/*$Id:$*/
/*19.12.2019 	15.05.2017	Белых А.И.	uosavpw.cpp
Распечатка акта внутреннего перемещения основных средств
*/
#include "buhg_g.h"

extern SQL_baza bd;

int uosavpw(const char *data_dok,const char *nomdok,GtkWidget *wpredok)
{
short dd=0,md=0,gd=0;
int metka_u=0; /*0-налоговый 1-бухгалтерский учёт*/
char strsql[512];
char imaf[64];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
class iceb_vrvr rv;

iceb_u_rsdat(&dd,&md,&gd,data_dok,1);

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

rv.ns.plus(15);
rv.begin.plus(81);
rv.end.plus(95);
rv.hag.plus(1);
rv.str.plus(nomdok);

sprintf(strsql,"%02d.%02d.%d",dd,md,gd);
rv.ns.plus(15);
rv.begin.plus(98);
rv.end.plus(108);
rv.hag.plus(1);
rv.str.plus(strsql);

if((metka_u=uos_menu_vu(wpredok)) < 0)
 return(1);


sprintf(strsql,"select innom,bs,bsby from Uosdok1 where datd='%d.%02d.%02d' and nomd='%s'",gd,md,dd,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }

sprintf(imaf,"avp%d.lst",getpid());
class iceb_fopen fil;
if(fil.start(imaf,"w",wpredok) != 0)
 return(1);

iceb_vrvr("uosavp_begin.alx",&rv,fil.ff,wpredok); /*вставка реквизитов в шапку документа*/

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),wpredok);
 }

double bal_st=0.;
int itogo_kol=0;
double itogo_sum=0;
class iceb_u_str naim_os("");
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   {
    naim_os.new_plus(row1[0]);
   }
  else
   {
    naim_os.new_plus("");
   }

  if(metka_u == 0)
   {
    bal_st=atof(row[1]);
   }
  else
   {
    bal_st=atof(row[2]);
   }   

  fprintf(fil.ff,"|%-*.*s|%-15s|%-*s|%9s|%13.2f|%12s|%8s|\n",
  iceb_u_kolbait(40,naim_os.ravno()),
  iceb_u_kolbait(40,naim_os.ravno()),
  naim_os.ravno(),
  row[0],
  iceb_u_kolbait(7,"Шт"),
  "Шт",
  "1",
  bal_st,
  "",
  "");    

  itogo_kol++;    
  itogo_sum+=bal_st;
 }

fprintf(fil.ff,"----------------------------------------------------------------------------------------------------------------\n");
fprintf(fil.ff,"%*s|%9d|%13.2f|\n",
iceb_u_kolbait(65,"Всього"),
"Всього",
itogo_kol,
itogo_sum);

rv.free_class();
class iceb_u_str stroka("");

iceb_u_preobr((double)itogo_kol,&stroka,2);

//sprintf(strsql,"%d",itogo_kol);
rv.ns.plus(3);
rv.begin.plus(20);
rv.end.plus(31);
rv.hag.plus(1);
rv.str.plus(stroka.ravno());

stroka.new_plus("");
iceb_u_preobr(itogo_sum,&stroka,0);

//sprintf(strsql,"%.2f",itogo_sum);
rv.ns.plus(3);
rv.begin.plus(45);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(stroka.ravno());


class iceb_fioruk_rk rr;

iceb_fioruk(2,&rr,wpredok);
rv.ns.plus(13);
rv.begin.plus(53);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(rr.famil_inic.ravno());


/*узнаём материально ответственных лиц*/
sprintf(strsql,"select kodol,kodolv from Uosdok where datd='%04d-%02d-%02d' and nomd='%s'",
gd,md,dd,nomdok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 { 
 
  int kodol=atoi(row[0]);
  int kodolv=atoi(row[1]);
  class iceb_u_str kodol_fio("");
  class iceb_u_str kodol_dolg("");
  class iceb_u_str kodolv_fio("");
  class iceb_u_str kodolv_dolg("");

  sprintf(strsql,"select naik,dolg from Uosol where kod=%d",kodol);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    kodol_fio.new_plus(row[0]);
    kodol_dolg.new_plus(row[1]);
   }  

  sprintf(strsql,"select naik,dolg from Uosol where kod=%d",kodolv);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    kodolv_fio.new_plus(row[0]);
    kodolv_dolg.new_plus(row[1]);
   }  

  rv.ns.plus(7);
  rv.begin.plus(8);
  rv.end.plus(33);
  rv.hag.plus(1);
  rv.str.plus(kodol_dolg.ravno());

  rv.ns.plus(7);
  rv.begin.plus(53);
  rv.end.plus(100);
  rv.hag.plus(1);
  rv.str.plus(kodol_fio.ravno());

  rv.ns.plus(10);
  rv.begin.plus(8);
  rv.end.plus(33);
  rv.hag.plus(1);
  rv.str.plus(kodolv_dolg.ravno());

  rv.ns.plus(10);
  rv.begin.plus(53);
  rv.end.plus(100);
  rv.hag.plus(1);
  rv.str.plus(kodolv_fio.ravno());

 }










iceb_vrvr("uosavp_end.alx",&rv,fil.ff,wpredok); /*вставка концовки*/

iceb_podpis(fil.ff,wpredok);
fil.end();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка акта внутреннего перемещения основных средств"));

iceb_ustpeh(imaf,3,wpredok);

iceb_rabfil(&oth,wpredok);


return(0);
}
