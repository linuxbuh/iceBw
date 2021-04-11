/*$Id: iceb_spks.c,v 1.16 2013/09/26 09:47:05 sasa Exp $*/
/*27.02.2016	18.12.2003	Белых А.И.	iceb_spks.c
Расчёт сальдо по контрагенту и конкретному счету с развернутым сальдо
*/
#include  "iceb_libbuh.h"
#include  "iceb_spks.h"

class iceb_u_str iceb_spks_data::datn;
class iceb_u_str iceb_spks_data::datk;
class iceb_u_str iceb_spks_data::shets;

int     iceb_spks_v(class iceb_spks_data *data,iceb_u_str *zag,GtkWidget *window);
int iceb_spks_r(class iceb_spks_data *datark,class spis_oth *oth,GtkWidget *wpredok);

extern SQL_baza	bd;
extern short startgodb;

void  iceb_spks(const char *shet,const char *kodkon,GtkWidget *wpredok)
{
time_t          tmm;
struct  tm      *bf;
class iceb_spks_data data;
class iceb_u_str zag;
char strsql[1024];
SQL_str row;
SQLCURSOR cur;


data.shetsrs.new_plus(shet);

data.shet.new_plus(data.shetsrs.ravno());


if(data.datn.ravno()[0] == '\0')
 {
  time(&tmm);
  bf=localtime(&tmm);
  if(startgodb != 0)  
    sprintf(strsql,"01.01.%d",startgodb);
  else
    sprintf(strsql,"01.01.%d",bf->tm_year+1900);
  data.datn.new_plus(strsql);

  sprintf(strsql,"%02d.%02d.%d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
  data.datk.new_plus(strsql);  
 }

data.kontr.plus(kodkon);

zag.plus(gettext("Счет"));
zag.plus(": ");
zag.plus_ps(shet);

zag.plus_ps(gettext("Расчёт сальдо по контрагенту"));
zag.plus(kodkon);
zag.plus(" ");

sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodkon);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  zag.plus(row[0]);
  data.naikontr.plus(row[0]);
 } 

if(iceb_spks_v(&data,&zag,wpredok) != 0)
  return;


sprintf(strsql,"%s %s%s %s %s%s",
gettext("Период с"),
data.datn.ravno(),
gettext("г."),
gettext("по"),
data.datk.ravno(),
gettext("г."));

zag.ps_plus(strsql);

if(wpredok != NULL)
  gtk_widget_hide(wpredok);

class spis_oth oth;

if(iceb_spks_r(&data,&oth,wpredok) == 0)
  iceb_rabfil(&oth,NULL);

if(wpredok != NULL)
  gtk_widget_show_all(wpredok);

}
