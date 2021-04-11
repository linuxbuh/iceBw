/*$Id: iceb_u_zsimv.c,v 1.2 2013/08/13 06:10:18 sasa Exp $*/
/*08.09.2011	11.11.2009	Белых А.И.	iceb_u_zsimv.c
Запись в строку другой строки с заданной позиции заменяя существующие символы на новые
*/
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "iceb_util.h"

void iceb_u_zsimv(char *stroka,
int kolbait, /*Размер строки в байтах*/
int poz, /*Позиция символа с которой нужно наложить строку нумерация позиций начинается с ноля*/
const char *simvol)
{
//if(iceb_u_kolbait(poz+1,stroka) > kolbait-1)
//  return;

int kolsimv=iceb_u_strlen(stroka);
int kol_simvol=iceb_u_strlen(simvol);
gchar *strout=NULL;

gchar *stroka1=g_strdup_printf("%.*s",iceb_u_kolbait(poz,stroka),stroka);

//printw("%s-START-stroka=%s*stroka1=%s|simvol=%s poz=%d kol_simvol=%d kolsimv=%d\n",__FUNCTION__,stroka,stroka1,simvol,poz,kol_simvol,kolsimv);

if(poz+kol_simvol < kolsimv)
 {
  strout=g_strdup_printf("%s%s%s",stroka1,simvol,iceb_u_adrsimv(poz+kol_simvol,stroka)); /*вставляем вместо символа поэтому poz+1 - пропускаем символ вместо которого вставляем*/
 }
else
 {
  strout=g_strdup_printf("%s%s",stroka1,simvol);
 }
if(kolbait-1 < (int)strlen(strout))
 {
  int kol_simv=iceb_u_kolsimv(kolbait,strout);
//  printw("%s kolbait=%d  strlen(strout)=%d strout=%s kol_simv=%d\n",__FUNCTION__,kolbait,strlen(strout),strout,kol_simv);
  sprintf(stroka,"%.*s",iceb_u_kolbait(kol_simv,strout),strout); /*Записываем только то что влазит*/
 }
else
  strcpy(stroka,strout);

//printw("%s-END-stroka=%s*stroka1=%s|simvol=%s\n",__FUNCTION__,stroka,stroka1,simvol);

g_free(strout);  
g_free(stroka1);

}

