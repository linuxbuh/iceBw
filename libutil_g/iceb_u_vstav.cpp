/*$Id: iceb_u_vstav.c,v 1.3 2013/10/04 07:27:23 sasa Exp $*/
/*15.09.2011	15.09.2011	Белых А.И.	iceb_u_vstav.c
Вставка строки в заданном диапазоне
*/
#include "iceb_util.h"




/*************************************/

void iceb_u_vstav(char *stroka, /*строка в которую вставляем*/
const char *strin,                     /*строка которую вставляем*/
int poz1,                        /*позиция символа в строке откуда начинаем вставлять нумерация начинается с 0*/
int poz2,                        /*позиция символа в строке по которую вставляем нумерация начинается с 0*/
int hag,                         /*шаг вставки*/
int kolbait_stroka)              /*размер stroka в байтах*/
{
char strtmp[32];
int kol_simv_strin=iceb_u_strlen(strin);
int nom_strin=0;
for(int nom=poz1; nom <= poz2 && nom_strin < kol_simv_strin; nom+=hag)
 {
  sprintf(strtmp,"%.*s",iceb_u_kolbait(1,iceb_u_adrsimv(nom_strin,strin)),iceb_u_adrsimv(nom_strin,strin));
  nom_strin++;
  iceb_u_zvstrp(stroka,nom,strtmp,kolbait_stroka);  
 }

}
/*************************************/
void iceb_u_vstav(char *stroka, /*строка в которую вставляем*/
int znah,                     /*строка которую вставляем*/
int poz1,                        /*позиция символа в строке откуда начинаем вставлять нумерация начинается с 0*/
int poz2,                        /*позиция символа в строке по которую вставляем нумерация начинается с 0*/
int hag,                         /*шаг вставки*/
int kolbait_stroka)              /*размер stroka в байтах*/
{
char strtmp[64];
sprintf(strtmp,"%d",znah);

iceb_u_vstav(stroka,strtmp,poz1,poz2,hag,kolbait_stroka);

}
/**********************************/
void iceb_u_vstav(class iceb_u_str *stroka, /*строка в которую вставляем*/
int znah,                     /*строка которую вставляем*/
int poz1,                        /*позиция символа в строке откуда начинаем вставлять нумерация начинается с 0*/
int poz2,                        /*позиция символа в строке по которую вставляем нумерация начинается с 0*/
int hag)                         /*шаг вставки*/
{

char *bros=NULL;

unsigned kolbait=strlen(stroka->ravno())+1;
if(kolbait < poz2*sizeof(wchar_t)+1)
 kolbait=poz2*sizeof(wchar_t)+1;

bros=new char[kolbait];

strcpy(bros,stroka->ravno()); 

iceb_u_vstav(bros,znah,poz1,poz2,hag,kolbait);
stroka->new_plus(bros);
delete [] bros;
}
/**********************************/
void iceb_u_vstav(class iceb_u_str *stroka, /*строка в которую вставляем*/
const char *znah,                     /*строка которую вставляем*/
int poz1,                        /*позиция символа в строке откуда начинаем вставлять нумерация начинается с 0*/
int poz2,                        /*позиция символа в строке по которую вставляем нумерация начинается с 0*/
int hag)                         /*шаг вставки*/
{

char *bros=NULL;

unsigned kolbait=strlen(stroka->ravno())+1+strlen(znah); /*так как символы могут быть многобайтные то строка может увеличить свой размер*/
if(kolbait < poz2*sizeof(wchar_t)+1)
 kolbait=poz2*sizeof(wchar_t)+1;

bros=new char[kolbait];

strcpy(bros,stroka->ravno()); 

iceb_u_vstav(bros,znah,poz1,poz2,hag,kolbait);
stroka->new_plus(bros);
delete [] bros;
}
