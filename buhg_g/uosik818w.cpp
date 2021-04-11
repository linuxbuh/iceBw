/*$Id:$*/
/*16.05.2017	16.05.2017	Белых А.И.	uosik818w.cpp
Распечатка инвентаной карточки
Приказ Министерства финансов Украины N818 от 13.09.2016 г.
*/
#include "buhg_g.h"
extern SQL_baza bd;

int uosik818w(int inv_nom,GtkWidget *wpredok)
{
int metka_u=0; /*0-налоговый 1-бухгалтерский учёт*/
char strsql[512];
char imaf[64];
char imaf2[64];

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

sprintf(strsql,"%d",inv_nom);

rv.ns.plus(10);
rv.begin.plus(48);
rv.end.plus(61);
rv.hag.plus(1);
rv.str.plus(strsql);

class iceb_u_str shet("");
class iceb_u_str god_vipuska("");
class iceb_u_str pasport("");
class iceb_u_str zav_nomer("");
class iceb_u_str mat_otv("");
class iceb_u_str naim_os("");

sprintf(strsql,"select * from Uosin where innom=%d",inv_nom);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  god_vipuska.new_plus(row[1]);
  naim_os.new_plus(row[2]);
  pasport.new_plus(row[4]);
  zav_nomer.new_plus(row[6]);  
 } 

/*узнаём счёт учёта*/
sprintf(strsql,"select shetu from Uosinp where innom=%d  order by god desc,mes desc limit 1",inv_nom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  shet.new_plus(row[0]);
 }

/*узнаём материально ответственного*/
int kod_podr=0;
int kod_mo=0;
short dt=0,mt=0,gt=0;
class iceb_u_str fio_mo("");

iceb_u_poltekdat(&dt,&mt,&gt);
poiinw(inv_nom,dt,mt,gt,&kod_podr,&kod_mo,wpredok);

sprintf(strsql,"select naik from Uosol where kod=%d",kod_mo);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 fio_mo.new_plus(row[0]);

/*узнаём балансовую стоимость*/

class bsizw_data bal_st;
bsizw(inv_nom,kod_podr,dt,mt,gt,&bal_st,NULL,wpredok);

if((metka_u=uos_menu_vu(wpredok)) < 0)
 return(1);

double start_bal_st=0;

if(metka_u == 0)
 start_bal_st=bal_st.sbs;
else
 start_bal_st=bal_st.sbsby;



rv.ns.plus(12);
rv.begin.plus(1);
rv.end.plus(130);
rv.hag.plus(1);
rv.str.plus(naim_os.ravno());

sprintf(strsql,"%d",inv_nom);
rv.ns.plus(19);
rv.begin.plus(1);
rv.end.plus(20);
rv.hag.plus(1);
rv.str.plus(strsql);

rv.ns.plus(19);
rv.begin.plus(24);
rv.end.plus(43);
rv.hag.plus(1);
rv.str.plus(shet.ravno());

rv.ns.plus(19);
rv.begin.plus(45);
rv.end.plus(55);
rv.hag.plus(1);
rv.str.plus(god_vipuska.ravno());

rv.ns.plus(19);
rv.begin.plus(57);
rv.end.plus(71);
rv.hag.plus(1);
rv.str.plus(pasport.ravno());

rv.ns.plus(19);
rv.begin.plus(73);
rv.end.plus(82);
rv.hag.plus(1);
rv.str.plus(zav_nomer.ravno());

rv.ns.plus(19);
rv.begin.plus(102);
rv.end.plus(131);
rv.hag.plus(1);
rv.str.plus(fio_mo.ravno());

sprintf(strsql,"%.2f",start_bal_st);
rv.ns.plus(25);
rv.begin.plus(47);
rv.end.plus(631);
rv.hag.plus(1);
rv.str.plus(strsql);

sprintf(strsql,"select * from Uosdok1 where innom=%d order by datd asc,tipz desc",inv_nom);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),wpredok);
  return(1);
 }

sprintf(imaf,"ik1_%d.lst",getpid());
class iceb_fopen fil;
if(fil.start(imaf,"w",wpredok) != 0)
 return(1);

iceb_vrvr("uosik_begin.alx",&rv,fil.ff,wpredok); /*вставка реквизитов в шапку документа*/

int tipz=0;
double balst=0.;
double iznos=0.;
class iceb_u_str naim_oper("");
int nomer_str=0;
while(cur.read_cursor(&row) != 0)
 {
  /*если операция внутренняя то показывать только приход*/
  tipz=atoi(row[1]);
  if(tipz == 1)
    sprintf(strsql,"select vido,naik from Uospri where kod='%s'",row[11]);
  if(tipz == 2)
    sprintf(strsql,"select vido,naik from Uosras where kod='%s'",row[11]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   {
    if(atoi(row1[0]) == 1 && tipz == 2)
     continue;
    naim_oper.new_plus(row1[1]);
   }
  else
   naim_oper.new_plus("");
//  datdok.new_plus(iceb_u_datzap(row[0]));

  if(metka_u == 0)
   {
    balst=atof(row[8]);
    iznos=atof(row[9]);
   }
  else
   {
    balst=atof(row[14]);
    iznos=atof(row[15]);
   }
  fprintf(fil.ff,"%5d|%-21s|%-*.*s|%21.2f|%12s|%13.2f|%12.2f|%14s|\n",
  ++nomer_str,
  iceb_u_datzap(row[0]),
  iceb_u_kolbait(26,naim_oper.ravno()),  
  iceb_u_kolbait(26,naim_oper.ravno()),  
  naim_oper.ravno(),
  balst,
  "",
  balst,
  iznos,
  "");
  
 }

fprintf(fil.ff,"* Заповнюеться у разі безоплатного отримання об'єкта основних засобів.\n");
fprintf(fil.ff,"** Заповнюеться у разі безоплатного отримання об'єкта основних засобів від суб'єктів державного сектору.\n");


fil.end();

/*распечатываем обратную сторону карточки*/

sprintf(imaf2,"ik2_%d.lst",getpid());
if(fil.start(imaf2,"w",wpredok) != 0)
 return(1);


sprintf(strsql,"select zapis from Uosin1 where innom=%d and md=%d order by nomz asc",inv_nom,2);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
if(kolstr == 0)
 {
  iceb_insfil("uosik_end.alx",fil.ff,wpredok);
 }
else
 {
  while(cur.read_cursor(&row) != 0)
   fprintf(fil.ff,"%s",row[0]);
 }

fil.end();


class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Первая страница инвентарной карточки"));
oth.spis_imaf.plus(imaf2);
oth.spis_naim.plus(gettext("Вторая страница инвентарной карточки"));

for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
  iceb_ustpeh(oth.spis_imaf.ravno(nom),3,wpredok);

iceb_rabfil(&oth,wpredok);


return(0);
}
