/*$Id: uosoz6w.c,v 1.3 2013/08/13 06:09:55 sasa Exp $*/
/*13.07.2015	05.01.2012	Белых А.И.	uosoz6.c
Распечатка инвентарной карточки учёта основных средств (типовая форма NoОЗ-6)
*/
#include <errno.h>
#include "buhg_g.h"

extern SQL_baza bd;

int uosoz6(int innom,GtkWidget *wpredok)
{
char strsql[1024];
class iceb_u_str stroka("");
int kolstr=0;
SQL_str row,rowalx;
class SQLCURSOR cur,curalx;
int viduh=0;
double bal_st=0.;
double iznos=0.;
class iceb_u_str edrpou("");
class iceb_u_str zavnom("");
class iceb_u_str pasp("");
class iceb_u_str naimos("");
int godv=0;
FILE *ff;

if(uosvibuw(&viduh,wpredok) != 0)
 return(1);

sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 edrpou.new_plus(row[0]);

sprintf(strsql,"select god,naim,pasp,zavnom from Uosin where innom=%d",innom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  godv=atoi(row[0]);
  naimos.new_plus(row[1]);
  pasp.new_plus(row[2]);
  zavnom.new_plus(row[3]);  
 } 

sprintf(strsql,"select str from Alx where fil='uosoz6start.alx' order by ns asc");
if((kolstr=curalx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найден бланк первой стороны карточки!"),wpredok);
  return(1);
 }

sprintf(strsql,"select * from Uosdok1 where innom=%d order by datd asc,tipz desc",innom);
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
char imaf[64];
sprintf(imaf,"uos%d.lst",innom);

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }


/*читаем первую запись чтобы узнать начальную балансовую стоимость*/
cur.read_cursor(&row);
if(viduh == 0)
 bal_st=atof(row[8]);
else
 bal_st=atof(row[14]);

int nomstr=0;  
while(curalx.read_cursor(&rowalx) != 0)
 {
  nomstr++;
  stroka.new_plus(rowalx[0]);
      
  switch(nomstr)
   {

    case 1:
     iceb_u_vstav(&stroka,iceb_get_pnk("00",0,wpredok),0,100,1);
     break;

    case 3:
     iceb_u_vstav(&stroka,edrpou.ravno(),28,40,1);
     break;

   case 12: /*дата, номер документа, бал стоимость первого докумета*/
     iceb_u_vstav(&stroka,iceb_u_datzap(row[0]),158,168,1);
     iceb_u_vstav(&stroka,row[4],171,179,1);
     iceb_u_vstav(&stroka,bal_st,180,198,1);
     break;

   case 14: /*наименование ос*/
     iceb_u_vstav(&stroka,naimos.ravno(),0,212,1);
     break;     
   };
  fprintf(ff,"%s",stroka.ravno());
 }

cur.poz_cursor(0);
class poiinpdw_data rekin;  
short d,m,g;
double popkf=0.;
class iceb_u_str datdok("");
double proc=0.;
int tipz=0;
while(cur.read_cursor(&row) != 0)
 {
  /*если операция внутренняя то показывать только приход*/
  tipz=atoi(row[1]);
  if(tipz == 1)
    sprintf(strsql,"select vido from Uospri where kod='%s'",row[11]);
  if(tipz == 2)
    sprintf(strsql,"select vido from Uosras where kod='%s'",row[11]);
  if(iceb_sql_readkey(strsql,&rowalx,&curalx,wpredok) == 1)
   if(atoi(rowalx[0]) == 1 && tipz == 2)
    continue;

  iceb_u_rsdat(&d,&m,&g,row[0],2);  
  datdok.new_plus(iceb_u_datzap(row[0]));
  poiinpdw(innom,m,g,&rekin,wpredok);
  proc=0.;
  if(viduh == 0)
    sprintf(strsql,"select kof from Uosgrup where kod='%s'",rekin.hna.ravno());
  else    
    sprintf(strsql,"select kof from Uosgrup1 where kod='%s'",rekin.hna.ravno());
  if(iceb_sql_readkey(strsql,&rowalx,&curalx,wpredok) == 1)
    proc=atof(rowalx[0]);   
  if(viduh == 0)
   {
    bal_st=atof(row[8]);
    popkf=rekin.popkf;
    iznos=atof(row[9]);
   }
  else
   {
    bal_st=atof(row[14]);
    iznos=atof(row[15]);
    popkf=rekin.popkfby;
   }
/*             1   2   3   4       5    6       7    8     9    10   11
 12  13   14  15   19   20   21    22     23  24  25   26*/

fprintf(ff," %-5d %-5s %-6s %10.10g %-11s %-4.4s %7.7g %4.4g %7s %6.6g %-4.4s\
 %-5s %-10s %-7s %-7s %10d %-10s %-10s %10.10g %d %-10s %-5s %7.7g\n",
atoi(row[5]),rekin.shetu.ravno(),rekin.hau.ravno(),bal_st,rekin.hzt.ravno(),rekin.hna.ravno(),0.,proc," ",
popkf," ",
rekin.hna.ravno(),datdok.ravno(),row[4]," ",innom,zavnom.ravno(),pasp.ravno(),iznos,godv," "," ",0.);
 
 }

fprintf(ff,"\f"); /*печать оботротной стороны карточки*/

sprintf(strsql,"select zapis from Uosin1 where innom=%d and md=1 order by nomz asc",innom);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_insfil("uosoz6end.alx",ff,wpredok);
 }
else
 {
  while(cur.read_cursor(&row) != 0)
    fprintf(ff,"%s",row[0]);
 }
iceb_podpis(ff,wpredok);
fclose(ff);

iceb_ustpeh(imaf,2,wpredok);


class iceb_u_spisok imafil;
class iceb_u_spisok naimf;

imafil.plus(imaf);
naimf.plus(gettext("Инвентарная карточка (типовая форма NoОЗ-6)"));

iceb_rabfil(&imafil,&naimf,wpredok);

return(0);
}
