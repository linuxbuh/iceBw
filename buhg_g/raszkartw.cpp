/*$Id: raszkartw.c,v 1.13 2013/09/26 09:46:54 sasa Exp $*/
/*12.07.2015	25.07.2002	Белых А.И.	raszkartw.c
Распечатать карточку на работника
*/
#include        <errno.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void raszkartw(int tabn,GtkWidget *wpredok)
{
int kolstr=0;
SQL_str		row,row1;
SQLCURSOR cur,cur1;
char		strsql[1024];
FILE		*ff;
char		imaf[64];
short		dn,mn,gn,dk,mk,gk;
class iceb_u_str fio("");
class iceb_u_str dolg("");
class iceb_u_str kateg("");
class iceb_u_str naimkateg("");
class iceb_u_str shetu("");
class iceb_u_str naimshet("");
class iceb_u_str inn("");
class iceb_u_str adres("");
class iceb_u_str pasp_nom; /*номер паспорта*/
class iceb_u_str pasp_dv; /*Дата выдачи паспорта*/
class iceb_u_str pasp_kv; /*Кем выдан паспорт*/
class iceb_u_str kodss(""); /*код спецстажа*/
class iceb_u_str telef("");
class iceb_u_str zvan("");
class iceb_u_str naimzvan("");
class iceb_u_str podr("");
class iceb_u_str naimpodr("");
short		sovm=0;
int kodgni=0;
class iceb_u_str naim_gni("");
class iceb_u_str har_rab("");
class iceb_u_str kod_lg("");
class iceb_u_str kart_shet("");
class iceb_u_str tip_kk("");
short dr=0,mr=0,gr=0;
short dppz=0,mppz=0,gppz=0;
short pol=0; 

sprintf(strsql,"select * from Kartb where tabn=%d",tabn);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabn);
  iceb_menu_soob(strsql,wpredok);
  return;
 }
fio.new_plus(row[1]);
dolg.new_plus(row[2]);
zvan.new_plus(row[3]);
podr.new_plus(row[4]);
kateg.new_plus(row[5]);
iceb_u_rsdat(&dn,&mn,&gn,row[6],2);
iceb_u_rsdat(&dk,&mk,&gk,row[7],2);
shetu.new_plus(row[8]);
sovm=atoi(row[9]);
inn.new_plus(row[10]);
adres.new_plus(row[11]);

pasp_nom.plus(row[12]);
pasp_dv.plus(iceb_u_datzap(row[19]));
pasp_kv.plus(row[13]);


telef.new_plus(row[14]);
har_rab.new_plus(row[16]);
kod_lg.new_plus(row[17]);
kart_shet.new_plus(row[18]);
tip_kk.new_plus(row[20]);
iceb_u_rsdat(&dr,&mr,&gr,row[21],2);
iceb_u_rsdat(&dppz,&mppz,&gppz,row[23],2);
pol=atoi(row[22]);

kodss.new_plus(row[25]);

sprintf(strsql,"select naik from Zvan where kod=%s",zvan.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  naimzvan.new_plus(row[0]);

sprintf(strsql,"select naik from Podr where kod=%s",podr.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naimpodr.new_plus(row[0]);

sprintf(strsql,"select naik from Kateg where kod=%s",kateg.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  naimkateg.new_plus(row[0]);

sprintf(strsql,"select nais from Plansh where ns='%s'",shetu.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  naimshet.new_plus(row[0]);

sprintf(strsql,"select naik from Gnali where kod=%d",kodgni);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_gni.new_plus(row[0]);

sprintf(imaf,"kart%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
   iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }


iceb_zagolov(gettext("Карточка работника"),0,0,0,0,0,0,ff,wpredok);

fprintf(ff,"\n");

if(sovm == 1)
 fprintf(ff,"%s\n",gettext("Совместитель")); 

if(pol == 0)
 fprintf(ff,"%s:%s\n",gettext("Пол"),gettext("Мужчина"));
if(pol == 1)
 fprintf(ff,"%s:%s\n",gettext("Пол"),gettext("Женщина"));

fprintf(ff,"\
%-*s %d\n\
%-*s %s\n\
%-*s %s\n\
%-*s %s\n\n\
%-*s %s\n\
%-*s %s\n\
%-*s %s\n\n\
%-*s %s\n\
%-*s %s\n\
%-*s %s %s\n\
%-*s %s %s\n\
%-*s %s %s\n\
%-*s %s\n\
%-*s %s %s\n",
iceb_u_kolbait(35,gettext("Табельный номер")),gettext("Табельный номер"),tabn,
iceb_u_kolbait(35,gettext("Индивидуальный налоговый номер")),gettext("Индивидуальный налоговый номер"),inn.ravno(),
iceb_u_kolbait(35,gettext("Фамилия")),gettext("Фамилия"),fio.ravno(),
iceb_u_kolbait(35,gettext("Адрес")),gettext("Адрес"),adres.ravno(),
iceb_u_kolbait(35,gettext("Паспорт")),gettext("Паспорт"),pasp_nom.ravno(),
iceb_u_kolbait(35,gettext("Дата выдачи паспорта")),gettext("Дата выдачи паспорта"),pasp_dv.ravno(),
iceb_u_kolbait(35,gettext("Кем выдан паспорт")),gettext("Кем выдан паспорт"),pasp_kv.ravno(),
iceb_u_kolbait(35,gettext("Телефон")),gettext("Телефон"),telef.ravno(),
iceb_u_kolbait(35,gettext("Должность")),gettext("Должность"),dolg.ravno(),
iceb_u_kolbait(35,gettext("Звание")),gettext("Звание"),zvan.ravno(),naimzvan.ravno(),
iceb_u_kolbait(35,gettext("Подразделение")),gettext("Подразделение"),podr.ravno(),naimpodr.ravno(),
iceb_u_kolbait(35,gettext("Счёт учета")),gettext("Счёт учета"),shetu.ravno(),naimshet.ravno(),
iceb_u_kolbait(35,gettext("Код спецстажа")),gettext("Код спецстажа"),kodss.ravno(),
iceb_u_kolbait(35,gettext("Категория")),gettext("Категория"),kateg.ravno(),naimkateg.ravno());

fprintf(ff,"%-*s %d %s\n",iceb_u_kolbait(35,gettext("Город налоговой инспекции")),gettext("Город налоговой инспекции"),kodgni,naim_gni.ravno());
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(35,gettext("Характер работы")),gettext("Характер работы"),har_rab.ravno());
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(35,gettext("Код льготы")),gettext("Код льготы"),kod_lg.ravno());
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(35,gettext("Номер карт-счёта")),gettext("Номер карт-счёта"),kart_shet.ravno());
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(35,gettext("Тип кредитной карты")),gettext("Тип кредитной карты"),tip_kk.ravno());
fprintf(ff,"%-*s %02d.%02d.%d\n",iceb_u_kolbait(35,gettext("Дата рождения")),gettext("Дата рождения"),dr,mr,gr);
fprintf(ff,"%-*s %02d.%02d.%d\n",iceb_u_kolbait(35,gettext("Дата повышения зарплаты")),gettext("Дата повышения зарплаты"),dppz,mppz,gppz);
 
fprintf(ff,"\
%-*s %02d.%02d.%d%s\n",
iceb_u_kolbait(35,gettext("Дата приема")),gettext("Дата приема"),dn,mn,gn,
gettext("г."));

if(dk != 0)
  fprintf(ff,"\
%-*s %02d.%02d.%d%s\n",
iceb_u_kolbait(35,gettext("Дата увольнения")),gettext("Дата увольнения"),dk,mk,gk,
gettext("г."));

sprintf(strsql,"select zapis from Kartb1 where tabn=%d order by nomz asc",tabn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
 {
  fprintf(ff,"\n%s\n",gettext("Дополнительная информация"));
  fprintf(ff,"---------------------------------------------------------\n");

  while(cur.read_cursor(&row) != 0)
   fprintf(ff,"%s\n",row[0]);
 }

/*Смотрим список картсчетов работника*/
sprintf(strsql,"select kb,nks from Zarkh where tn=%d",tabn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
 {
  fprintf(ff,"\n%s\n",gettext("Список картсчетов работника"));
  fprintf(ff,"---------------------------------------------------------\n");
  class iceb_u_str naim_bank;
  while(cur.read_cursor(&row) != 0)
  {
    /*узнаём наименование банка*/
    naim_bank.new_plus("");
    sprintf(strsql,"select naik from Zarsb where kod=%s",row[0]);    
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) > 0)
     naim_bank.new_plus(row1[0]);
    fprintf(ff,"%2s %-*.*s %s\n",
    row[0],
    iceb_u_kolbait(20,naim_bank.ravno()),
    iceb_u_kolbait(20,naim_bank.ravno()),
    naim_bank.ravno(),
    row[1]);
  }
 }


iceb_podpis(ff,wpredok);


fclose(ff);

class iceb_u_spisok imafs;
class iceb_u_spisok naimf;

imafs.plus(imaf);
naimf.plus(gettext("Карточка работника"));
iceb_ustpeh(imaf,3,wpredok);
iceb_rabfil(&imafs,&naimf,wpredok);

}
