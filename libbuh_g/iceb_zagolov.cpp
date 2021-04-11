/*$Id: iceb_zagolov.c,v 1.4 2013/08/19 06:50:10 sasa Exp $*/
/*19.08.2013	04.12.2011	Белых А.И.	iceb_zagolov.c
Выдача заголовка в отчёт
*/
#include "iceb_libbuh.h"



/******************************/

void iceb_zagolov(const char *naim,short dn,short mn,short gn,
short dk,short mk,short gk,FILE *ff,GtkWidget *wpredok)
{
if(ff == NULL)
 return;

time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

 
fprintf(ff,"%s\n\n",iceb_get_pnk("00",0,wpredok));

fprintf(ff,"%s\n",naim);

if(dn != 0)
  fprintf(ff,"%s %d.%d.%d%s %s %d.%d.%d%s\n",
  gettext("Период с"),
  dn,mn,gn,
  gettext("г."),
  gettext("по"),
  dk,mk,gk,
  gettext("г."));
else
 {
  if(dk != 0)
   fprintf(ff,"%s:%02d.%02d.%d%s\n",gettext("Дата"),
   dk,mk,gk,gettext("г."));
  if(dk == 0 && mk != 0)
   fprintf(ff,"%s:%02d.%d%s\n",gettext("Дата"),
   mk,gk,gettext("г."));
  if(dk == 0 && mk == 0 && gk != 0)
   fprintf(ff,"%s:%d%s\n",gettext("Дата"),
   gk,gettext("г."));
 }  
fprintf(ff,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

}
/******************************/

void iceb_zagolov(const char *naim,const char *dat_start,
const char *dat_end,FILE *ff,GtkWidget *wpredok)
{
if(ff == NULL)
 return;
class iceb_u_str datan("");
class iceb_u_str datak("");
class iceb_u_str vremn("");
class iceb_u_str vremk("");

time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);
if(iceb_u_polen(dat_start,&datan,1,' ') == 0)
 iceb_u_polen(dat_start,&vremn,2,' ');
else
 datan.new_plus(dat_start);

if(iceb_u_polen(dat_end,&datak,1,' ') == 0)
 iceb_u_polen(dat_end,&vremk,2,' ');
else
 datak.new_plus(dat_end);
 
fprintf(ff,"%s\n\n",iceb_get_pnk("00",0,wpredok));

fprintf(ff,"%s\n",naim);

if(dat_start[0] != '\0')
 {
  if(vremn.getdlinna() <= 1)
   fprintf(ff,"%s %s %s %s %s %s %s\n",
   gettext("Период с"),
   datan.ravno(),
   gettext("г."),
   gettext("по"),
   datak.ravno(),
   gettext("г."),
   vremk.ravno());
  else
   fprintf(ff,"%s %s %s %s %s %s %s %s\n",
   gettext("Период с"),
   datan.ravno(),
   gettext("г."),
   vremn.ravno(),
   gettext("по"),
   datak.ravno(),
   gettext("г."),
   vremk.ravno());
 }
else
 {
  if(dat_end[0] != '\0')
   fprintf(ff,"%s:%s %s %s\n",gettext("Дата"),
   dat_end,gettext("г."),vremk.ravno());
 }  
fprintf(ff,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

}
/*********************************/
void iceb_zagolov(const char *naim,FILE *ff,GtkWidget *wpredok)
{
iceb_zagolov(naim,0,0,0,0,0,0,ff,wpredok);


}
