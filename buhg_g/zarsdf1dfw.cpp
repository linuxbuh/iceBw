/*$Id: zarsdf1dfw.c,v 1.16 2013/09/26 09:47:01 sasa Exp $*/
/*06.08.2015	01.04.2004	Белых А.И.	zarsdf1dfw.c
Чтение списка кодов признаков доходов с кодами начислений которые к ним относятся.
*/
#include "buhg_g.h"
#include "sprkvrt1w.h"

extern SQL_baza bd;
extern short *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/

void zarsdf1dfw(class sprkvrt1_nast *nast,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

class iceb_u_str bros("");
class iceb_u_str kodd("");

sprintf(strsql,"select str from Alx where fil='zarsdf1df.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarsdf1df.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],&kodd,1,'|') != 0)
   continue;
  
  if(iceb_u_SRAV(kodd.ravno(),"Коды не входящие в форму 1ДФ",0) == 0)
   {
    iceb_u_sozmas(&nast->kodnvf8dr,row_alx[0]);
    continue;
   }
  if(iceb_u_SRAV(kodd.ravno(),"Коды удержаний не входящие в форму 1ДФ",0) == 0)
   {
    iceb_u_sozmas(&nast->kodud,row_alx[0]);
    continue;
   }

  if(iceb_u_SRAV("Счета для поиска доходов по коду",kodd.ravno(),1) == 0)
   {
  
    iceb_u_polen(row_alx[0],&bros,2,'|');
    if(bros.ravno()[0] == '\0')
     continue;

    iceb_fplus(1,bros.ravno(),&nast->sheta,&cur_alx);
     
    iceb_u_polen(row_alx[0],&bros,7,' ');
    nast->kodi_pd_prov.plus(bros.ravno_atoi(),-1);

    continue;
   }
  if(iceb_u_SRAV("Коды командировочных расходов для кода дохода",kodd.ravno(),1) == 0)
   {

    iceb_u_pole(row_alx[0],&bros,7,' ');
    if(bros.ravno_atoi() == 0)
     continue;

    nast->kodkr.plus(bros.ravno_atoi(),-1);
  
    iceb_u_pole(row_alx[0],&bros,2,'|');

    iceb_fplus(1,bros.ravno(),&nast->kodd_for_kodkr,&cur_alx);
     

    continue;
   }
  if(iceb_u_SRAV("Физическое лицо",kodd.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&bros,2,'|');
    if(iceb_u_SRAV(bros.ravno(),"Вкл",1) == 0)
     {
      nast->TYP=1;
     }
    continue;
   }

  if(kodd.ravno_atoi() == 0)
   continue;

  if(kodd.ravno_atoi() == 133)
   {
    iceb_u_polen(row_alx[0],&nast->gosstrah,2,'|');
    continue;
   }   

  nast->kodi_pd.plus(atoi(kodd.ravno()),-1);
  iceb_u_polen(row_alx[0],&kodd,2,'|');
  nast->kodi_nah.plus(kodd.ravno());    
  /*по первому коду проверяем берётся подоходный или нет . Остальные коды по идее должны соотвецтвовать первому*/
  if(provkodw(knvr,kodd.ravno_atoi()) >= 0)
   nast->metka_podoh.plus(1);
  else
   nast->metka_podoh.plus(0);
 }

}
