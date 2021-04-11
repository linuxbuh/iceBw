/*$Id: rasttn1w.c,v 1.1 2014/01/31 12:12:59 sasa Exp $*/
/*03.10.2013	28.09.2013	Белых А.И.	rasttn1w.c
Распечатка товарно-транспортной накладной
*/
#include "buhg_g.h"
#include "rasttnw.h"
 
int rasttn1w_m(class rasttn_rek *rek_poi,GtkWidget *wpredok);

extern double okrcn;  /*Округление цены*/
extern SQL_baza bd;

int rasttn1w(short dd,short md,short gd,const char *nomdok,int sklad,class spis_oth *oth,GtkWidget *wpredok)
{
class rasttn_rek rek;
int kolstr=0;
SQL_str row,row_alx;
class SQLCURSOR cur,cur_alx;
char strsql[1024];
int metka_vs=0;

rek.dd=md;
rek.md=md;
rek.gd=gd;
rek.nomdok.new_plus(nomdok);
rek.sklad=sklad;

metka_vs=iceb_menu_danet(gettext("Вторую страницу печатать?"),2,wpredok);


/*Читаем шапку документа*/
sprintf(strsql,"select tip,kontr,pn from Dokummat where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gd,md,dd,sklad,nomdok);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  class iceb_u_str repl;

  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не найден документ !")),
  repl.plus(strsql);

  sprintf(strsql,"%s=%d.%d.%d %s=%s %s=%d",
  gettext("Дата"),
  dd,md,gd,
  gettext("Номер документа"),
  nomdok,
  gettext("Код склада"),
  sklad);

  repl.ps_plus(strsql);
  iceb_menu_soob(repl.ravno(),wpredok);
  return(1);
 }
int tipz=atoi(row[0]);
class iceb_u_str kod_kontr(row[1]);
double pnds=atof(row[2]);

class iceb_u_str naim_kontr00("");
class iceb_u_str naim_kontr("");
class iceb_u_str adres00("");
class iceb_u_str adres("");

int lnds=0;
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=11",gd,sklad,nomdok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 lnds=atoi(row[0]);

class iceb_u_str nomer_dover("");
class iceb_u_str data_dover("");
class iceb_u_str herez_kogo("");
short ddov=0,mdov=0,gdov=0;
class iceb_u_str naim_mes_dov("");

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=1",gd,sklad,nomdok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  iceb_u_polen(row[0],&nomer_dover,1,'#');
  iceb_u_polen(row[0],&data_dover,2,'#');
  iceb_u_rsdat(&ddov,&mdov,&gdov,data_dover.ravno(),1);
  if(mdov != 0)
    iceb_mesc(mdov,1,&naim_mes_dov);
 }

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=2",gd,sklad,nomdok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  herez_kogo.new_plus(row[0]);
 }

if(tipz == 1)
 sprintf(strsql,"select naikon,adres from Kontragent where kodkon='%s'",kod_kontr.ravno_filtr());
else
 sprintf(strsql,"select naikon,adres from Kontragent where kodkon='%s'","00");

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  naim_kontr00.new_plus(row[0]);
  adres00.new_plus(row[1]);
 }

if(tipz == 2)
 sprintf(strsql,"select naikon,adres from Kontragent where kodkon='%s'",kod_kontr.ravno_filtr());
else
 sprintf(strsql,"select naikon,adres from Kontragent where kodkon='%s'","00");

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  naim_kontr.new_plus(row[0]);
  adres.new_plus(row[1]);
 }

rek.avtopredp.new_plus(naim_kontr00.ravno());
rek.punkt_zagr.new_plus(adres00.ravno());
rek.punkt_razg.new_plus(adres.ravno());
rek.zakazchik.new_plus(naim_kontr.ravno());

if(rasttn1w_m(&rek,wpredok) != 0)
 return(1);
//if(rasttn_vvod(dd,md,gd,nomdok,sklad,&rek) != 0 )
// return(1);

/*Определяем сумму по документу*/
sprintf(strsql,"select kolih,cena from Dokummat1 where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' order by kodm asc",gd,md,dd,sklad,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

double suma=0.;
double suma_itogo=0.;
double kolih=0.;
double cena=0.;

while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[0]);
  cena=atof(row[1]);
//  Для вложенного налога
  if(lnds == 0)
    suma=cena+cena*pnds/100.;
  else
    suma=cena;

  suma=iceb_u_okrug(suma,okrcn);
  suma=suma*kolih;
  suma_itogo+=iceb_u_okrug(suma,okrcn);

  

 }
class iceb_u_str suma_prop("");
iceb_u_preobr(suma_itogo,&suma_prop,0);

sprintf(strsql,"select str from Alx where fil='ttnl1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"ttnl1.alx");
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
char imaf[64];
sprintf(imaf,"rttn%d.lst",getpid());

class iceb_fopen rs;
if(rs.start(imaf,"w",wpredok) != 0)
 return(1);

fprintf(rs.ff,"\x1B\x33%c",30); /*Уменьшение межстрочного интервала*/

int nomer_str=0;
class iceb_u_str stroka("");
class iceb_u_str naim_mes;
iceb_mesc(md,1,&naim_mes);
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
    continue;

  nomer_str++;

  stroka.new_plus(row_alx[0]);
  
  switch(nomer_str)
   {
    case 6: /*Номер накладной*/
     iceb_u_vstav(&stroka,nomdok,103,150,1);
     break;

    case 8: /*Дата автомобиль*/
     iceb_u_vstav(&stroka,dd,2,6,1);
     iceb_u_vstav(&stroka,naim_mes.ravno(),7,20,1);
     iceb_u_vstav(&stroka,gd,17,30,1);

     sprintf(strsql,"%s %s",rek.marka_avt.ravno(),rek.gos_nomer.ravno());
     iceb_u_vstav(&stroka,strsql,36,135,1);
     iceb_u_vstav(&stroka,rek.nom_put_list.ravno(),160,173,1);
     break;
    
    case 10: /*автопредприятие фамилия водителя*/
     iceb_u_vstav(&stroka,rek.avtopredp.ravno(),17,68,1);
     iceb_u_vstav(&stroka,rek.fio_vod.ravno(),74,110,1);
     break;

    case 12: /*Замовник*/
     iceb_u_vstav(&stroka,rek.zakazchik.ravno(),19,143,1);
     break;

    case 14: /*вантажовідправник*/
     iceb_u_vstav(&stroka,naim_kontr00.ravno(),18,143,1);
     break;

    case 16: /*вантажоодержувач*/
     iceb_u_vstav(&stroka,naim_kontr00.ravno(),17,143,1);
     break;

    case 18: /*пункт навантаження розвантаження*/
     iceb_u_vstav(&stroka,rek.punkt_zagr.ravno(),19,76,1);
     iceb_u_vstav(&stroka,rek.punkt_razg.ravno(),96,148,1);
     break;

    case 20: /*гос. номер прицепа*/
     iceb_u_vstav(&stroka,rek.gos_nomer_pric.ravno(),110,152,1);
     break;

    case 31: /*запись в документу*/
     sprintf(strsql,"%s N%s %s %02d.%02d.%04d %s",
     gettext("Накладная"),nomdok,gettext("от"),dd,md,gd,gettext("г."));
     
     iceb_u_vstav(&stroka,strsql,20,160,1);
     break;

    case 40: /*сумма прописью*/
     iceb_u_vstav(&stroka,suma_prop.ravno(),25,107,1);
     break;

    case 42: /*номер доверенности*/
     iceb_u_vstav(&stroka,nomer_dover.ravno(),142,173,1);
     break;

    case 44: /*дата доверенности*/
     if(ddov != 0)
      {
       iceb_u_vstav(&stroka,ddov,132,147,1);
       sprintf(strsql,"%s %d",naim_mes_dov.ravno(),gdov);
       iceb_u_vstav(&stroka,strsql,137,170,1);
      }
     break;

    case 46: /*кому выдана доверенность*/
     iceb_u_vstav(&stroka,herez_kogo.ravno(),134,173,1);
     break;

   };

  fprintf(rs.ff,"%s",stroka.ravno());
 }

if(metka_vs == 1)
 {
  fprintf(rs.ff,"\f");
  iceb_insfil("ttnl2.alx",rs.ff,wpredok);
 }

iceb_podpis(rs.ff,wpredok);

rs.end();

iceb_ustpeh(imaf,2,wpredok);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Товарно-транспортная накладная")); 

return(0);

}
