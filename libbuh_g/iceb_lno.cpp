/*$Id: iceb_lno.c,v 1.1 2014/05/07 10:58:10 sasa Exp $*/
/*22.01.2015	30.04.2014	Белых А.И.	iceb_lno.c
Работа с файлом личных настроек оператора
*/
#include        <errno.h>
#include "iceb_libbuh.h"

int iceb_lno(const char *imaf,GtkWidget *wpredok)
{
char strsql[1024];

class iceb_u_str putnafil(g_get_home_dir());
putnafil.plus(G_DIR_SEPARATOR_S,".iceB");


putnafil.plus(G_DIR_SEPARATOR_S,imaf);
 
class iceb_u_str flag(imaf);
int metka_vr=0;
/*так как это личные настройки то оператор должен иметь возможность с ними работать*/
//metka_vr=iceb_prn(wpredok); /*проверяем возможность корректировки настроек*/

//Установить флаг работы с файлом
class iceb_sql_flag flagrf;

if(metka_vr == 0)
if(flagrf.flag_on(flag.ravno()) != 0)
 {
  iceb_menu_soob(gettext("С файлом уже работает другой оператор !"),wpredok);
  metka_vr=1; /*режим просмотра*/
 }

FILE *ff;

if((ff=fopen(putnafil.ravno(),"r")) == NULL)
 {
  if(errno == ENOENT) /*нет такого файла*/
   {
    if(iceb_alxout(imaf,putnafil.ravno(),wpredok) != 0)
     {
      sprintf(strsql,"%s-%s %s",__FUNCTION__,gettext("Не найден в базе файл"),imaf);
      iceb_menu_soob(strsql,wpredok);
      return(1);
     }
   }
  else
   {
    iceb_er_op_fil(imaf,__FUNCTION__,errno,wpredok);
    return(1);
   }
 }
else
  fclose(ff);


if(metka_vr == 0)
 {
  iceb_vizred(putnafil.ravno(),NULL); 
 }
else
 {
  iceb_prosf(putnafil.ravno(),NULL);
 }





return(0);
}
/*************************************************/
/*меню выбора личных настроек оператора*/
/*************************************************/
void iceb_lnom()
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;
int nomer=0;
titl.plus(gettext("Личные настройки"));

zagolovok.plus(gettext("Личные настройки оператора"));

punkt_m.plus(gettext("Настройки передачи файлов по сети"));//0
punkt_m.plus(gettext("Другие настройки оператора"));//1


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);
  switch (nomer)
   {
    case -1:
      return;

    case 0:
      iceb_lno("spis_komp.alx",NULL);
      break;


    case 1:
      iceb_lno("lno.alx",NULL);
      break;
 
   }
   
 }



}
