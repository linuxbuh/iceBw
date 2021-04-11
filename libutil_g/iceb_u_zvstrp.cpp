/*$Id: iceb_u_zvstrp.c,v 1.2 2013/08/13 06:10:18 sasa Exp $*/
/*16.04.2017	05.09.2011	Белых А.И.	iceb_u_zvstrp.c
Замена символов в строке с заданоой позиции символа на другую цепочку символов
Если до позиции вставки или в зоне вставки встречается симовл перевода строки то от преносится
в конец новой строки.
Нумерация позиций начинается с 0
*/
#include <glib.h>
#include "iceb_util.h"

void iceb_u_zvstrp(char *stroka, /*Строка в которую нужно вставить символы*/
int poz_zam, /*Позиция с которой нужно вставлять символы Нумерация начинается с 0*/
const char *vstr, /*строка которую нужно вставить с заданной позиции*/
int dlinna_str) /*длинна массива stroka в байтах*/
{
int metka_ps=0;
int kolih_simv=iceb_u_strlen(stroka);
int metka_nal_ps=0;
int kolih_bait=strlen(stroka);

class iceb_u_str stroka_out(stroka);

if(kolih_simv > 0)
 if(stroka[kolih_bait-1] == '\n')
  {
   kolih_simv-=1;
   metka_nal_ps=1;
  }
//printw("%s-kolih_simv=%d koih_bait=%d poz_zam=%d dlinna_str=%d metka_ps=%d %d\n",__FUNCTION__,kolih_simv,kolih_bait,poz_zam,dlinna_str,metka_ps,iceb_u_strlen(vstr));
/*Если количество символов в строке меньще чем позиция вставки то забиваем пробелами до позиции вставки и переносим перевод строки*/
if(kolih_simv < poz_zam+1 && kolih_bait < dlinna_str-1)
 {
//  printw("11111111111111\n");
  int nomb=kolih_bait-1;

  if(nomb < dlinna_str)
   {
    for(int noms=kolih_simv; noms <= poz_zam+1 ;noms++)
     {  

      if(stroka[nomb] == '\n')
       {
        metka_ps=1;
       }
      stroka[nomb++]=' ';
     }
    stroka[nomb]='\0';
   }  
 }

if(kolih_simv >= poz_zam+1) /*Если количество символов больше чем позиция вставки то проверяем есть ли перевод строки в зоне вставки*/
 {
  int kolih_simv_vstr=iceb_u_strlen(vstr);
//  printw("poz_zam=%d kolih_simv=%d kolih_simv_vstr=%d\n",poz_zam,kolih_simv,kolih_simv_vstr);
 
  for(int nom=poz_zam; nom < kolih_simv+metka_nal_ps && nom < poz_zam+kolih_simv_vstr; nom++)
   {
    if(iceb_u_adrsimv(nom,stroka)[0] == '\n')
     {
      metka_ps=1;
      break;
     }     
   }
 }
//printw("\n%s-start-stroka=%s* vstr=%s metka_ps=%d\n",__FUNCTION__,stroka,vstr,metka_ps);

iceb_u_zsimv(stroka,dlinna_str,poz_zam,vstr);
//printw("%s-end-stroka=%s* metka_ps=%d vstr=%s\n",__FUNCTION__,stroka,metka_ps,vstr);

if(metka_ps > 0)
 {
  if((int)strlen(stroka) > dlinna_str-2) /*Не помещяется перевод строки - один байт на перевод строки и один байт на '\0'*/
   {
//  printw("%s-%d %s\n",__FUNCTION__,strlen(stroka),stroka);
    /*последний символ удалить и вставить перевод строки*/     
    int kolih_simv=iceb_u_strlen(stroka)-1;
    gchar *strtmp=g_strdup_printf("%.*s",iceb_u_kolbait(kolih_simv,stroka),stroka);
    strcpy(stroka,strtmp);
    g_free(strtmp);    
   }
   strcat(stroka,"\n");
//  printw("%s=%s",__FUNCTION__,stroka);
 }
}
