/*$Id: iceb_u_proverka.c,v 1.12 2013/08/13 06:10:17 sasa Exp $*/
/*06.10.2012	06.05.2001	Белых А.И.	iceb_u_proverka.c
Проверка реквизита
Если вернули 0 подходит
             1 нет
Способ сравнения может быть задан в строке поиска
''  - подходит если поле пустое
'*' - подходит если поле не пустое
'=' - полное сравнение
'%' - подходит если находится в любом месте поля
*/
#include	<stdio.h>
#include	<string.h>
#include	"iceb_util.h"


int iceb_u_proverka(const char *etalon, 
int rekvizit,
int metka,
int variant) /*0-если еталона нет подходит 1-если эталона нет неподходит*/
{

char bros[64];
sprintf(bros,"%d",rekvizit);

return(iceb_u_proverka(etalon,bros,metka,variant));

}

/*******************************************/


int iceb_u_proverka(const char *etalon, 
const char *rekvizit,
int metka, /*0-полное сравнение 1-совпаденние начала 
             2 полное с преобразованием  3 cовпадение начала с преобразованием
             4-наличие цепочки символов в любом месте строки с преобразованием
             Должно совпадать с подпрограммой pole1*/
int variant) /*0-если еталона нет подходит 1-если эталона нет неподходит*/
{
#if 0
#######################################333
int i1=0;

if(variant == 0)
 {
  if(etalon == NULL)
    return(0);
  if(etalon[0] == '\0')
    return(0);
  if(rekvizit == NULL)
    return(0);
 }
if(variant == 1)
 {
  if(etalon == NULL)
    return(1);
  if(etalon[0] == '\0')
    return(1);
  if(rekvizit == NULL)
    return(1);
 }
int metka_srav=metka;

if(metka_srav == 2)
 metka_srav=0;
if(metka_srav == 3)
 metka_srav=1;

class iceb_u_str etalon1(etalon);
class iceb_u_str rekvizit1(rekvizit);

i1=strlen(etalon);
if(etalon[i1-1] == '*' && etalon[i1-2] == ',')
  metka=1;

if(etalon[0] != '-')
 {
  if(iceb_u_pole1(etalon,rekvizit,',',metka,&i1) == 0)
   return(0);


  if(metka == 4)
   {
    if(iceb_u_strstrm(rekvizit,etalon) == 1)
     return(0);
   }
  else
   {
    if(metka == 2 || metka == 3)
     {
      if(iceb_u_SRAV(etalon1.ravno_tolower(),rekvizit1.ravno_tolower(),metka_srav) == 0)
       return(0);
     }
    else
     if(iceb_u_SRAV(etalon,rekvizit,metka_srav) == 0)
      return(0);
   }
  /*подходит если реквизит не содержит значение*/
  if(iceb_u_SRAV(etalon,"''",0) == 0 && rekvizit[0] == '\0')
   return(0);
  if(iceb_u_pole1(etalon,"''",',',metka,&i1) == 0 && rekvizit[0] == '\0')
   return(0);

  /*подходит если реквизит содержит любое значение*/
  if(iceb_u_SRAV("'*'",etalon,0) == 0 && rekvizit[0] != '\0')
   return(0);

  if(iceb_u_pole1(etalon,"'*'",',',metka,&i1) == 0 && rekvizit[0] != '\0')
   return(0);
  return(1);
 }

if(etalon[0] == '-')
 {
  /*исключить если реквизит не имеет значение*/
  if(iceb_u_SRAV(etalon,"''",0) == 0 && rekvizit[0] == '\0')
   return(1);
  if(iceb_u_pole1(etalon,"''",',',metka,&i1) == 0 && rekvizit[0] == '\0')
   return(1);

  /*исключить если реквизит содержит любое значение*/
  if(iceb_u_SRAV("'*'",etalon,0) == 0 && rekvizit[0] != '\0')
   return(1);
  if(iceb_u_pole1(etalon,"'*'",',',metka,&i1) == 0 && rekvizit[0] != '\0')
   return(1);

  if(metka == 4) 
   {
    if(iceb_u_pole1(etalon,rekvizit,',',metka,&i1) == 0 || iceb_u_strstrm(rekvizit,etalon) == 1)
     return(1);
   }
  else
   {
    if(metka == 2 || metka == 3) /*с преобразованием*/
     {
      if(iceb_u_pole1(etalon,rekvizit,',',metka,&i1) == 0 || iceb_u_SRAV(etalon1.ravno_tolower(),rekvizit1.ravno_tolower(),metka_srav) == 0)
       return(1);
     }
    else
    if(iceb_u_pole1(etalon,rekvizit,',',metka,&i1) == 0 || iceb_u_SRAV(etalon,rekvizit,metka_srav) == 0)
      return(1);
   }
 }
return(0);
##########################################
#endif



int i1=0;

if(variant == 0)
 {
  if(etalon == NULL)
    return(0);
  if(etalon[0] == '\0')
    return(0);
  if(rekvizit == NULL)
    return(0);
 }
if(variant == 1)
 {
  if(etalon == NULL)
    return(1);
  if(etalon[0] == '\0')
    return(1);
  if(rekvizit == NULL)
    return(1);
 }
int metka_srav=metka;

if(metka_srav == 2)
 metka_srav=0;
if(metka_srav == 3)
 metka_srav=1;

class iceb_u_str etalon1(etalon);
class iceb_u_str rekvizit1(rekvizit);

i1=strlen(etalon);
if(etalon[i1-1] == '*' && etalon[i1-2] == ',')
  metka=1;

/*проверка задания способа сравнения в строке поиска*/
/*полное сравнение с преобразованием*/
if(iceb_u_pole1(etalon,"'='",',',0,&i1) == 0) /*в этом случае всегда должен быть задан через запятую как минимум один образец*/
   metka=2;

/*наличие цепочки символов в любом месте строки с преобразованием*/
if(iceb_u_pole1(etalon,"'%'",',',0,&i1) == 0) /*в этом случае всегда должен быть задан через запятую как минимум один образец*/
   metka=4;

if(etalon[0] != '-')
 {

  /*подходит если реквизит не содержит значение*/
  if(rekvizit[0] == '\0')
   {
    if(iceb_u_SRAV(etalon,"''",0) == 0)
     return(0);
    if(iceb_u_pole1(etalon,"''",',',metka,&i1) == 0)
     return(0);
   }

  /*подходит если реквизит содержит любое значение*/
  if(rekvizit[0] != '\0')
   {
    if(iceb_u_pole1(etalon,rekvizit,',',metka,&i1) == 0)
      return(0);
     
    if(metka == 4)
     {
      if(iceb_u_strstrm(rekvizit,etalon) == 1)
       return(0);
     }
    else
     {
      if(metka == 2 || metka == 3) /*С преобразованием*/
       {
        if(iceb_u_SRAV(etalon1.ravno_tolower(),rekvizit1.ravno_tolower(),metka_srav) == 0)
         return(0);
       }
      else
       if(iceb_u_SRAV(etalon,rekvizit,metka_srav) == 0)
        return(0);
     }
    if(iceb_u_SRAV("'*'",etalon,0) == 0)
     return(0);
    if(iceb_u_pole1(etalon,"'*'",',',0,&i1) == 0)
     return(0);
   }
  return(1);
 }

if(etalon[0] == '-')
 {
  /*исключить если реквизит не имеет значение*/
  if(rekvizit[0] == '\0')
   {
    if(iceb_u_SRAV(etalon,"''",0) == 0)
     return(1);
    if(iceb_u_pole1(etalon,"''",',',metka,&i1) == 0)
     return(1);
   }
  /*исключить если реквизит содержит любое значение*/
  if(rekvizit[0] != '\0')
   {
    if(iceb_u_SRAV("'*'",etalon,0) == 0)
     return(1);
    if(iceb_u_pole1(etalon,"'*'",',',0,&i1) == 0)
     return(1);

    if(metka == 4) 
     {
      if(iceb_u_pole1(etalon,rekvizit,',',metka,&i1) == 0 || iceb_u_strstrm(rekvizit,etalon) == 1)
       return(1);
     }
    else
     {
      if(metka == 2 || metka == 3) /*С преобразованием*/
       {
        if(iceb_u_pole1(etalon,rekvizit,',',metka,&i1) == 0 || iceb_u_SRAV(etalon1.ravno_tolower(),rekvizit1.ravno_tolower(),metka_srav) == 0)
         return(1);
       }
      else
       if(iceb_u_pole1(etalon,rekvizit,',',metka,&i1) == 0 || iceb_u_SRAV(etalon,rekvizit,metka_srav) == 0)
         return(1);
     }
   }
   

 }
return(0);




}
