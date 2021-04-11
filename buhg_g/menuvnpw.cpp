/*$Id: menuvnpw.c,v 1.9 2013/10/04 07:27:16 sasa Exp $*/
/*02.10.2013	20.05.2009	Белых А.И.	menuvnpw.c
Выбор файла настройки проводок
*/

#include "buhg_g.h"


void menuvnpw(int metka,/*1-материальный учёт 2-учёт услуг 3-учёт основных средств*/
int tipz, /*0 всё 1-приходные 2-расходные*/
GtkWidget *wpredok)
{
int kom=0,kom1=0;
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;
char strsql[512];
titl.plus(gettext("Выбор"));
zagolovok.plus(iceb_get_pnk("00",0,wpredok),40);
zagolovok.ps_plus(gettext("Выберите нужное"));

int metka_nsnp=0; /*метка наличия файла старой настройки проводок*/

if(metka == 1)
 {
  sprintf(strsql,"select ns from Alx where fil='avtpro.alx' limit 1");
  if(iceb_sql_readkey(strsql,wpredok) <= 0)
   metka_nsnp=1;
 }

if(metka == 2)
 {
  sprintf(strsql,"select ns from Alx where fil='avtprouslp.alx' limit 1");
  if(iceb_sql_readkey(strsql,wpredok) <= 0)
   metka_nsnp=1;
 }

if(metka != 3 && metka_nsnp == 0)
 {

  punkt_m.plus(gettext("Настройка проводок в таблице"));//0
  punkt_m.plus(gettext("Настройка списков проводок"));//1


  if((kom=iceb_menu_mv(&titl,&zagolovok,&punkt_m,kom,NULL)) < 0)
   return;
 }
else
 kom=1;

if(tipz == 1)
 {
  if(metka == 1)
   {
    if(kom == 0 && metka_nsnp == 0)
      iceb_f_redfil("avtpro.alx",0,wpredok);
    else
      iceb_f_redfil("avtpromu1.alx",0,wpredok);
    return;
   }    
  if(metka == 2)
   {
    if(kom == 0 && metka_nsnp == 0)
      iceb_f_redfil("avtprouslp.alx",0,wpredok);
    else
      iceb_f_redfil("avtprousl1.alx",0,wpredok);
   }    
  if(metka == 3)
   {
    iceb_f_redfil("avtprouos1.alx",0,wpredok);
   }    
  return;
 }

if(tipz == 2)
 {
  if(metka == 1)
   {
    if(kom == 0 && metka_nsnp == 0)
      iceb_f_redfil("avtpro.alx",0,wpredok);
    else
      iceb_f_redfil("avtpromu2.alx",0,wpredok);
    return;
   }    
  if(metka == 2)
   {
    if(kom == 0 && metka_nsnp == 0)
      iceb_f_redfil("avtprouslr.alx",0,wpredok);
    else
      iceb_f_redfil("avtprousl2.alx",0,wpredok);
   }    
  if(metka == 3)
   {
    iceb_f_redfil("avtprouos2.alx",0,wpredok);
   }    
  return;
 }

switch (kom)
 {
  case 2:
  case -1:
    return;

  case 0:
    if(metka == 1)
     {
      iceb_f_redfil("avtpro.alx",0,wpredok);
      return;
     }    

    punkt_m.free_class();
    punkt_m.plus(gettext("Настройка выполнения проводок для приходных документов"));//0
    punkt_m.plus(gettext("Настройка выполнения проводок для расходных документов"));//1


    kom1=0;
    if((kom1=iceb_menu_mv(&titl,&zagolovok,&punkt_m,kom1,NULL)) < 0)
     return;

    printf("kom1=%d\n",kom1);
    switch(kom1)
     {

      case 0:
        iceb_f_redfil("avtprouslp.alx",0,wpredok);
        break;

      case 1:
        iceb_f_redfil("avtprouslr.alx",0,wpredok);
        break;
     }
    

    break;

  case 1:

      punkt_m.free_class();
      punkt_m.plus(gettext("Настройка выполнения проводок для приходных документов"));//0
      punkt_m.plus(gettext("Настройка выполнения проводок для расходных документов"));//1


      kom1=0;
      if((kom1=iceb_menu_mv(&titl,&zagolovok,&punkt_m,kom1,NULL)) < 0)
       return;

      switch(kom1)
       {

        case 0:
          if(metka == 1)
            iceb_f_redfil("avtpromu1.alx",0,wpredok);
          if(metka == 2)
            iceb_f_redfil("avtprousl1.alx",0,wpredok);
          if(metka == 3)
            iceb_f_redfil("avtprouos1.alx",0,wpredok);
          break;
        case 1:
          if(metka == 1)
            iceb_f_redfil("avtpromu2.alx",0,wpredok);
          if(metka == 2)
            iceb_f_redfil("avtprousl2.alx",0,wpredok);
          if(metka == 3)
            iceb_f_redfil("avtprouos2.alx",0,wpredok);
          break;
       }
      
    break;
 }


}
