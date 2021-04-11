/*$Id: iceb_mesc.c,v 1.5 2013/05/17 14:58:35 sasa Exp $*/
/*08.07.2004    21.10.1993      Белых А.И.     iceb_mesc.c
Подпрограмма получения названия месяца по номеру месяца
*/
#include	"iceb_libbuh.h"


void iceb_mesc(short mes,  //Номер месяца
short skl,  //Склонение названия 0 -не склонять 1-склонять
class iceb_u_str *naz) //Название месяца
{

if(skl == 0)
 {
  if(mes == 1)
   naz->new_plus(gettext("январь"));
  if(mes == 2)
   naz->new_plus(gettext("февраль"));
  if(mes == 3)
   naz->new_plus(gettext("март"));
  if(mes == 4)
   naz->new_plus(gettext("апрель"));
  if(mes == 5)
   naz->new_plus(gettext("май"));
  if(mes == 6)
   naz->new_plus(gettext("июнь"));
  if(mes == 7)
   naz->new_plus(gettext("июль"));
  if(mes == 8)
   naz->new_plus(gettext("август"));
  if(mes == 9)
   naz->new_plus(gettext("сентябрь"));
  if(mes == 10)
   naz->new_plus(gettext("октябрь"));
  if(mes == 11)
   naz->new_plus(gettext("ноябрь"));
  if(mes == 12)
   naz->new_plus(gettext("декабрь"));
 }
if(skl == 1)
 {
  if(mes == 1)
   naz->new_plus(gettext("января"));
  if(mes == 2)
   naz->new_plus(gettext("февраля"));
  if(mes == 3)
   naz->new_plus(gettext("марта"));
  if(mes == 4)
   naz->new_plus(gettext("апреля"));
  if(mes == 5)
   naz->new_plus(gettext("мая"));
  if(mes == 6)
   naz->new_plus(gettext("июня"));
  if(mes == 7)
   naz->new_plus(gettext("июля"));
  if(mes == 8)
   naz->new_plus(gettext("августа"));
  if(mes == 9)
   naz->new_plus(gettext("сентября"));
  if(mes == 10)
   naz->new_plus(gettext("октября"));
  if(mes == 11)
   naz->new_plus(gettext("ноября"));
  if(mes == 12)
   naz->new_plus(gettext("декабря"));
 }

}
/********************************/
void iceb_mesc(short mes,  //Номер месяца
short skl,  //Склонение названия 0 -не склонять 1-склонять
char *naz) //Название месяца
{

if(skl == 0)
 {
  if(mes == 1)
   strcpy(naz,gettext("январь"));
  if(mes == 2)
   strcpy(naz,gettext("февраль"));
  if(mes == 3)
   strcpy(naz,gettext("март"));
  if(mes == 4)
   strcpy(naz,gettext("апрель"));
  if(mes == 5)
   strcpy(naz,gettext("май"));
  if(mes == 6)
   strcpy(naz,gettext("июнь"));
  if(mes == 7)
   strcpy(naz,gettext("июль"));
  if(mes == 8)
   strcpy(naz,gettext("август"));
  if(mes == 9)
   strcpy(naz,gettext("сентябрь"));
  if(mes == 10)
   strcpy(naz,gettext("октябрь"));
  if(mes == 11)
   strcpy(naz,gettext("ноябрь"));
  if(mes == 12)
   strcpy(naz,gettext("декабрь"));
 }
if(skl == 1)
 {
  if(mes == 1)
   strcpy(naz,gettext("января"));
  if(mes == 2)
   strcpy(naz,gettext("февраля"));
  if(mes == 3)
   strcpy(naz,gettext("марта"));
  if(mes == 4)
   strcpy(naz,gettext("апреля"));
  if(mes == 5)
   strcpy(naz,gettext("мая"));
  if(mes == 6)
   strcpy(naz,gettext("июня"));
  if(mes == 7)
   strcpy(naz,gettext("июля"));
  if(mes == 8)
   strcpy(naz,gettext("августа"));
  if(mes == 9)
   strcpy(naz,gettext("сентября"));
  if(mes == 10)
   strcpy(naz,gettext("октября"));
  if(mes == 11)
   strcpy(naz,gettext("ноября"));
  if(mes == 12)
   strcpy(naz,gettext("декабря"));
 }

}
