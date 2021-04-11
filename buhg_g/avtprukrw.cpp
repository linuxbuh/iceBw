/*$Id: avtprukrw.c,v 1.20 2013/09/26 09:46:45 sasa Exp $*/
/*10.07.2015	16.11.2002	Белых А.И.	avtprukr.c
Подпрограмма автоматического выполнения проводок для "Учета командировочных расходов"
*/
#include        "buhg_g.h"

extern SQL_baza bd;

void avtprukrw(short dd,short md,short gd,const char *nomdok,GtkWidget *wpredok)
{
FILE *ff_prot=NULL;
char		strsql[512];
int		kolstr=0;
SQL_str		row;
class iceb_u_str kodop("");
class iceb_u_str kodops("");
class iceb_u_str shetu("");
class iceb_u_str shetkr("");
class iceb_u_str shetnds("");;
class iceb_u_str kodu("");
class iceb_u_str kodkr("");
class iceb_u_str kodnds("");
class iceb_u_str koment("");
double		deb=0.;
double		kre=0.;
short		dp=0,mp=0,gp=0;
class iceb_u_str kodkont("");
class iceb_u_str kodkontr("");
class iceb_u_str kodr("");
struct OPSHET	shetv;
class iceb_u_str nomao(""); //Номер авансового отчёта
SQLCURSOR curr;
int kekv=0;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int metka_vkontr=2; /*метка режима выполнения проводки для контрагента если его нет в списке счёта 0-запрос 2-автоматическая вставка*/


class iceb_u_spisok sp_prov;
class iceb_u_double sum_prov_dk;

sprintf(strsql,"select shet,kolih,cena,snds,kontr,sn,ss,kodr from Ukrdok1 where \
datd='%04d-%d-%d' and nomd='%s'",gd,md,dd,nomdok);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
if(kolstr == 0)
 return;

//Узнаем дату авансового отчёта

sprintf(strsql,"select kont,vkom,nomao,datao from Ukrdok where god=%d and nomd='%s'",gd,nomdok);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
 {
  kodkont.new_plus(row[0]);
  kodop.new_plus(row[1]);
  nomao.new_plus(row[2]);
  iceb_u_rsdat(&dp,&mp,&gp,row[3],2);
 }
else
 return;
 
if(dp == 0)
 {
  memset(strsql,'\0',sizeof(strsql));
  if(iceb_menu_vvod1(gettext("Введите дату авансового отчёта"),strsql,11,"",wpredok) != 0)
   return;

  if(iceb_u_rsdat(&dp,&mp,&gp,strsql,1) != 0)
   {
    iceb_menu_soob(gettext("Неправильно ввели дату авансового отчёта!"),wpredok);
    return;
   }

  sprintf(strsql,"update Ukrdok set datao='%04d-%d-%d' where god=%d and nomd='%s'",
  gp,mp,dp,gd,nomdok);
  
  iceb_sql_zapis(strsql,1,0,wpredok);
  
 }
if(iceb_pvglkni(mp,gp,wpredok) != 0)
 return;

sprintf(strsql,"select str from Alx where fil='avtprukr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки avtprukr.alx\n");
  return;
 }

if(nomao.ravno()[0] != '\0')
 {
//  sprintf(koment,"N%s",nomao.ravno());
  koment.new_plus("N");
  koment.plus(nomao.ravno());
 } 
while(cur.read_cursor(&row) != 0)
 {
  cur_alx.poz_cursor(0);
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    if(row_alx[0][0] == '#')
     continue;

    if(iceb_u_polen(row_alx[0],&kodops,1,'|') != 0)
     continue;
    
    if(iceb_u_SRAV(kodop.ravno(),kodops.ravno(),0) != 0)
     continue;
    if(iceb_u_polen(row_alx[0],&kodr,6,'|') == 0)
     {
      if(kodr.ravno_atoi() != 0)
       if(atoi(row[7]) != kodr.ravno_atoi())
        continue;      
     }
    if(iceb_u_polen(row_alx[0],&shetu,2,'|') != 0)
     continue;

    if(iceb_u_SRAV(row[0],shetu.ravno(),0) != 0)
     continue;

    if(iceb_u_polen(row_alx[0],&shetkr,3,'|') != 0)
     continue;
    
    
    if(row[5][0] != '\0')
     shetnds.new_plus(row[5]);
    else
     if(iceb_u_polen(row_alx[0],&shetnds,4,'|') != 0)
       continue;
   
   iceb_u_polen(row_alx[0],&kekv,5,'|');

    kodu.new_plus(kodkont.ravno());
    if(iceb_provsh(&kodu,shetu.ravno(),&shetv,metka_vkontr,0,wpredok) != 0)
       continue;

    kodkr.new_plus(kodkont.ravno());
    if(iceb_provsh(&kodkr,shetkr.ravno(),&shetv,metka_vkontr,0,wpredok) != 0)
       continue;

    kodnds.new_plus(kodkont.ravno());
    if(iceb_provsh(&kodnds,shetnds.ravno(),&shetv,metka_vkontr,0,wpredok) != 0)
       continue;

    kodkontr.new_plus(row[4]);
    if(kodkontr.getdlinna() > 1)
     if(iceb_provsh(&kodkontr,shetkr.ravno(),&shetv,metka_vkontr,0,wpredok) != 0)
       continue;
    
    //Проводка на сумму без НДС
    kre=0.;
    deb=atof(row[2])+atof(row[6]);
    if(kodkontr.getdlinna() <= 1)            
      iceb_zapmpr(gp,mp,dp,shetu.ravno(),shetkr.ravno(),kodu.ravno(),kodkr.ravno(),deb,kre,koment.ravno(),2,kekv,&sp_prov,&sum_prov_dk,ff_prot);
    else
     {
      iceb_zapmpr(gp,mp,dp,shetkr.ravno(),shetkr.ravno(),kodkr.ravno(),kodkontr.ravno(),deb,kre,koment.ravno(),2,kekv,&sp_prov,&sum_prov_dk,ff_prot);
      iceb_zapmpr(gp,mp,dp,shetu.ravno(),shetkr.ravno(),kodkontr.ravno(),kodkontr.ravno(),deb,kre,koment.ravno(),2,kekv,&sp_prov,&sum_prov_dk,ff_prot);
     }

    //Проводка на сумму НДС
    kre=0.;
    deb=atof(row[3]);
            
    iceb_zapmpr(gp,mp,dp,shetnds.ravno(),shetkr.ravno(),kodnds.ravno(),kodkr.ravno(),deb,kre,koment.ravno(),2,kekv,&sp_prov,&sum_prov_dk,ff_prot);
   }

  
 }

/*Запись проводок из памяти в базу*/

iceb_zapmpr1(nomdok,kodop.ravno(),0,time(NULL),ICEB_MP_UKR,dd,md,gd,0,&sp_prov,&sum_prov_dk,ff_prot,wpredok);


}
