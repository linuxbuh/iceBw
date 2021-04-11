/*$Id: zarsprvmnw.c,v 1.16 2014/04/30 06:14:59 sasa Exp $*/
/*25.07.2019	08.04.2002	Белых А.И.	zarsprvmnw.c
Получение справки о доходах по датам в счёт которых были 
сделаны начисления
*/
#include        <errno.h>
#include        "buhg_g.h"
#include "zar_sprav.h"

void             rekvizkon(SQL_str,FILE*);

extern SQL_baza bd;

void zarsprvmnw(class zar_sprav_rek *data,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *bar,
GtkWidget *wpredok)
{
class iceb_u_str dolg("");
class iceb_u_str fam("");
char		strsql[1024];
SQL_str		row;
SQLCURSOR curr;

/****************Расчёт*********************/


int		kolstr=0;
char		imaf[64];
FILE		*ff;
short           sovm=0; /*0-нет 1- Совместитель*/
class iceb_u_str inn("");
short		mes,god;
short           mesz=0,godz=0;

sprintf(strsql,"select fio,sovm,inn,dolg,harac,podr from Kartb where tabn=%d",data->tabnom.ravno_atoi());
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %d !", gettext("Не найден табельный номер"),data->tabnom.ravno_atoi());
  iceb_menu_soob(strsql,wpredok);
  return;
 }

fam.new_plus(row[0]);
dolg.new_plus(row[3]);
inn.new_plus(row[2]);


short mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,data->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->datak.ravno());
int kolmes=iceb_u_period(1,mn,gn,1,mk,gk,1);
if(kolmes == 0)
 {
  iceb_menu_soob(gettext("Количество месяцев в периоде равно нолю !"),wpredok);
  return;
 }


sprintf(strsql,"%s %d.%d %s %d.%d %s %s\n",
gettext("Период с"),mn,gn,
gettext("до"),mk,gk,data->tabnom.ravno(),data->fio.ravno());

iceb_printw(strsql,buffer,view);

sprintf(strsql,"%s:%d\n",gettext("Количество месяцев"),kolmes);

iceb_printw(strsql,buffer,view);

if(gn == gk)
 sprintf(strsql,"select suma,godn,mesn from Zarp where tabn=%d and \
prn='1' and suma <> 0. and (godn = %d and (mesn >= %d and mesn <= %d)) order by godn,mesn asc",
 data->tabnom.ravno_atoi(),gn,mn,mk);
else
 sprintf(strsql,"select suma,godn,mesn from Zarp where tabn=%d and \
prn='1' and suma <> 0. and ((godn < %d and godn > %d) or(godn = %d and mesn >= %d) or (godn = %d and mesn <= %d)) order by godn,mesn asc",
 data->tabnom.ravno_atoi(),gn,gk,gn,mn,gk,mk);



//printw("\n%s\n",strsql);
//OSTANOV();

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не введены начисления !"),wpredok);
  return;
 }

sprintf(imaf,"sprdm%d_%d.lst",data->tabnom.ravno_atoi(),getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

/*Читаем реквизиты организации*/
sprintf(strsql,"select * from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  iceb_menu_soob(gettext("Не найден контрагент с кодом 00 !"),wpredok);
 }
else
 {
  rekvizkon(row,ff);
 }

fprintf(ff,"\n\
	      %s\n\
%s %d.%d%s %s %d.%d%s\n\
%s %s %s ",
gettext("С п р а в к а."),
gettext("о заработной плате за период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."),
gettext("Работник"),
fam.ravno(),
gettext("работает"));

if(sovm == 0)
  fprintf(ff," %s.\n",gettext("постоянно"));
else
  fprintf(ff," %s.\n",gettext("по совместительству"));
fprintf(ff,"%s %s\n",gettext("Регистрационный номер учётной карточки"),inn.ravno());
fprintf(ff,"%s: %s\n",gettext("Должность"),dolg.ravno());

short		ots=10;
fprintf(ff,"\
%*s-------------------------\n\
%*s%s\n\
%*s-------------------------\n",
ots," ",
ots," ","|  Дата |   Начислено   |",
ots," ");


godz=mesz=0;
double suma=0.;
double sumames=0.;
double sumaitog=0.;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(bar,kolstr,++kolstr1);    
  suma=atof(row[0]);
  god=atoi(row[1]);
  mes=atoi(row[2]);
  
  if(mesz != mes)
   {
    if(mesz != 0)
     {
      sprintf(strsql,"%02d.%d %15.2f\n",mesz,godz,sumames);
      iceb_printw(strsql,buffer,view);

      fprintf(ff,"%*s|%02d.%d %15.2f|\n",ots," ",mesz,godz,sumames);
     }
    sumames=0.;
    mesz=mes;
    godz=god;
   }  
  sumames+=suma;
  sumaitog+=suma;
  
 }
sprintf(strsql,"%02d.%d %15.2f\n",mesz,godz,sumames);
iceb_printw(strsql,buffer,view);

sprintf(strsql,"%*s %15.2f\n",iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),sumaitog);
iceb_printw(strsql,buffer,view);

fprintf(ff,"%*s|%02d.%d %15.2f|\n",ots," ",mesz,godz,sumames);
memset(strsql,'\0',sizeof(strsql));
iceb_u_preobr(sumaitog,strsql,0);

fprintf(ff,"%*s-------------------------\n\
%*s %*s %15.2f\n\n\
%s\n%s\n",
ots," ",
ots," ",
iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),
sumaitog,
gettext("Общая сумма доходов:"),strsql);

class iceb_fioruk_rk rek;
iceb_fioruk(1,&rek,wpredok);

fprintf(ff,"\n\n%*s:_______________________%s\n",iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),rek.famil_inic.ravno());
iceb_fioruk(2,&rek,wpredok);
fprintf(ff,"\n%*s:_______________________%s\n",iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),rek.famil_inic.ravno());

iceb_podpis(ff,wpredok);
fclose(ff);

data->oth.spis_imaf.plus(imaf);
data->oth.spis_naim.plus(gettext("Справка о доходах"));
iceb_ustpeh(imaf,3,wpredok);
}


