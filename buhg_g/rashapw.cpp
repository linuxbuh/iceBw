/*$Id: rashapw.c,v 1.8 2013/08/13 06:09:50 sasa Exp $*/
/*12.07.2015	02.02.2007	Белых А.И.	rashapw.c
Распечатка реквизитов контрагентов в заголовке дукумента
*/
#include "buhg_g.h"

extern SQL_baza bd;

void rashapw(int tipz,int *kolstrok,const char *kodkontr00,const char *kontr,int ots,int kor,FILE *ffdok,
GtkWidget *wpredok)
{
class iceb_u_str nmo(""),nmo1("");  /*Наименование организации*/
class iceb_u_str gor(""),gor1(""); /*Адрес*/
class iceb_u_str pkod(""),pkod1(""); /*Почтовый код*/
class iceb_u_str nmb(""),nmb1("");  /*Наименование банка*/
class iceb_u_str mfo(""),mfo1("");  /*МФО*/
class iceb_u_str nsh(""),nsh1("");  /*Номер счета*/
class iceb_u_str inn(""),inn1("");  /*Индивидуальный налоговый номер*/
class iceb_u_str grb(""),grb1("");  /*Город банка*/
class iceb_u_str npnds(""),npnds1(""); /*Номер плательщика НДС*/
class iceb_u_str tel(""),tel1("");    /*Номер телефона*/
class iceb_u_str regnom(""); /*Регистрационный номер частного предпринимателя*/
char bros[1024];
char bros1[1024];
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
/*Читаем реквизиты организации свои */


sprintf(bros,"select * from Kontragent where kodkon='%s'",kodkontr00);
if(sql_readkey(&bd,bros,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",
  gettext("Не найден код контрагента"),kodkontr00);
  iceb_menu_soob(strsql,wpredok);
 }
else
 {
  nmo.new_plus(row[1]);
  gor.new_plus(row[3]);
  pkod.new_plus(row[5]);
  nmb.new_plus(row[2]);
  mfo.new_plus(row[6]);
  nsh.new_plus(row[7]);
  inn.new_plus(row[8]);
  grb.new_plus(row[4]);
  npnds.new_plus(row[9]);
  tel.new_plus(row[10]);
 } 


/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/
if(atof(kontr) != 0. || kontr[0] != '0')
 {

  sprintf(bros,"select * from Kontragent where kodkon='%s'",kontr);
  if(sql_readkey(&bd,bros,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",
    gettext("Не найден код контрагента"),kontr);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    nmo1.new_plus(row[1]);
    gor1.new_plus(row[3]);
    pkod1.new_plus(row[5]);
    nmb1.new_plus(row[2]);
    mfo1.new_plus(row[6]);
    nsh1.new_plus(row[7]);
    inn1.new_plus(row[8]);
    grb1.new_plus(row[4]);
    npnds1.new_plus(row[9]);
    tel1.new_plus(row[10]);
    regnom.new_plus(row[15]);
    
   }
 }

//if(ots != 0)
// fprintf(ffdok,"\x1b\x6C%c",ots); /*Установка левого поля*/
fprintf(ffdok,"\x1B\x33%c",30-kor); /*Уменьшаем межстрочный интервал*/
//fprintf(ffdok,"\x12"); /*Отмена ужатого режима*/
//fprintf(ffdok,"\x1B\x4D"); /*12-знаков*/

if(tipz == 1)
 fprintf(ffdok,gettext("\
  Кому                                 От кого\n"));
if(tipz == 2)
 fprintf(ffdok,gettext("\
   От кого                             Кому\n"));
*kolstrok+=1;

fprintf(ffdok,"%-*s ",iceb_u_kolbait(40,nmo.ravno()),nmo.ravno());
if(iceb_u_strlen(nmo1.ravno()) <= 35)
  fprintf(ffdok,"\x1B\x45N%s %s\x1B\x46\n",kontr,nmo1.ravno());
else
  fprintf(ffdok,"\x1B\x45\x0FN%s %s\x1B\x46\x12\n",kontr,nmo1.ravno());
*kolstrok+=1;

memset(bros,'\0',sizeof(bros));
sprintf(bros,"%s %s",gettext("Адрес"),gor.ravno());
sprintf(bros1,"%s %s",gettext("Адрес"),gor1.ravno());
fprintf(ffdok,"%-*.*s %s\n",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

sprintf(bros,"%s %s",gettext("Код ЕГРПОУ"),pkod.ravno());
sprintf(bros1,"%s %s",gettext("Код ЕГРПОУ"),pkod1.ravno());
fprintf(ffdok,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

sprintf(bros,"%s %s %s %s",gettext("Р/С"),nsh.ravno(),
gettext("МФО"),mfo.ravno());

sprintf(bros1,"%s %s %s %s",gettext("Р/С"),nsh1.ravno(),
gettext("МФО"),mfo1.ravno());
fprintf(ffdok,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

sprintf(bros,"%s %s %s %s",gettext("в"),nmb.ravno(),
gettext("г."),grb.ravno());
sprintf(bros1,"%s %s %s %s",gettext("в"),nmb1.ravno(),
gettext("г."),grb1.ravno());
fprintf(ffdok,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

sprintf(bros,"%s %s",
gettext("Налоговый номер"),inn.ravno());
sprintf(bros1,"%s %s",
gettext("Налоговый номер"),inn1.ravno());

fprintf(ffdok,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

sprintf(bros,"%s %s",gettext("Номер сви-ства плательщика НДС"),npnds.ravno());
sprintf(bros1,"%s %s",gettext("Номер сви-ства плательщика НДС"),npnds1.ravno());
fprintf(ffdok,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

sprintf(bros,"%s %s",gettext("Телефон"),tel.ravno());
sprintf(bros1,"%s %s",gettext("Телефон"),tel1.ravno());
fprintf(ffdok,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
*kolstrok+=1;

if(tipz == 2)
if(regnom.ravno()[0] != '\0')
 {
  fprintf(ffdok,"%s: %s\n",gettext("Регистрационный номер"),regnom.ravno());
  *kolstrok+=1;
 }  

}
