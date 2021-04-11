/*$Id: zagf1dfw.c,v 1.2 2013/08/13 06:09:59 sasa Exp $*/
/*01.10.2014	30.10.2012	Белых А.И.	zagf1dfw.c
Распечатка заголовка формы 1ДФ
*/
#include "buhg_g.h"

extern SQL_baza bd;

void zagf1dfw(int kolrh, //Количество работников
int kolrs,//Количество совместителей
short kvrt,const char *kod,short TYP,
int *klst,int vidd, //1-вилучення
short dn,short mn,short gn,
short dk,short mk,short gk,
int metka_oth, //0-звітний 1-новий звітний 2-уточнюючий
FILE *ff1,FILE *ff2,
GtkWidget *wpredok)
{
class iceb_u_str stroka("");

iceb_u_zagolov("Розсчёт довідки за квартал (форма N1ДФ)",dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,wpredok),ff2);
fprintf(ff2,"%s:%d\n","Квартал",kvrt);



int kkk=0;
spkvdw(&kkk,ff2);

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zarsf1df_1.alx"};

class iceb_u_str adres("");
sprintf(strsql,"select adres from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row_alx,&cur_alx,wpredok) == 1)
 adres.new_plus(row_alx[0]);

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return;
 }


class iceb_u_str naim_ni("");
class iceb_u_str kodni("");
iceb_poldan("Код налоговой инспекции",&kodni,"zarsdf1df.alx",wpredok);

iceb_poldan("Название налоговой инспекции",&naim_ni,"zarsdf1df.alx",wpredok);

char period[64];
memset(period,'\0',sizeof(period));

if(kvrt == 1)
  sprintf(period,"I квартал %d р.",gn);
if(kvrt == 2)
  sprintf(period,"II квартал %d р.",gn);
if(kvrt == 3)
  sprintf(period,"III квартал %d р.",gn);
if(kvrt == 4)
  sprintf(period,"IV квартал %d р.",gn);

int nomstr=0;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomstr++;
  stroka.new_plus(row_alx[0]);
  switch(nomstr)
   {
    case 7:
     iceb_u_vstav(&stroka,kod,1,27,2 );
     break;

    case 10:
      if(metka_oth == 0) //звитний
       iceb_u_vstav(&stroka,"X",88,94,1 );
     break;

    case 12:
      if(TYP == 0) //юридическое лицо
       iceb_u_vstav(&stroka,"X",2,3,1 );
      if(metka_oth == 1) //новий звитний
       iceb_u_vstav(&stroka,"X",87,95,1 );
     break;

    case 14:
      if(TYP == 1) //физицеское лицо
       iceb_u_vstav(&stroka,"X",2,3,1 );
      if(metka_oth == 2) //уточнюючий
       iceb_u_vstav(&stroka,"X",87,94,1 );
     break;

    case 19:
     iceb_u_vstav(&stroka,iceb_get_pnk("00",0,wpredok),16,114,1 );
     break;

    case 22:
     iceb_u_vstav(&stroka,adres.ravno(),17,114,1 );
     break;    

    case 25:
     iceb_u_vstav(&stroka,naim_ni.ravno(),27,114,1 );
     break;

    case 26:
     iceb_u_vstav(&stroka,kodni.ravno(),1,20,2 );
     break;     
    case 28:
     iceb_u_vstav(&stroka,period,37,104,1 );
     break;
     
    case 31:
     if(kolrh != 0)
      {  
       iceb_u_vstav(&stroka,kolrh,44,53,1 );
      }
     break;   

    case 33:
     if(kolrs != 0)
      { 
       iceb_u_vstav(&stroka,kolrs,44,53,1 );
      }
     break;   
   }


  fprintf(ff1,"%s",stroka.ravno());
  *klst+=1;
 }
 
}
