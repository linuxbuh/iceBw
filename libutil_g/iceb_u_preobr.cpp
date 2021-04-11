/*$Id: iceb_u_preobr.c,v 1.12 2013/08/13 06:10:17 sasa Exp $*/
/*26.05.2013   19.11.1992      Белых А.И.      iceb_u_preobr.c
Подпрограмма преобразования числа в текстовую строку
				      |       |       |
			     12 11 10 | 9 8 7 | 6 5 4 | 3  2  1
				      |       |       |
*/
#include	"iceb_util.h"



void iceb_u_preobr(double hs, //Число
class iceb_u_str *str, //Строка
short gri)  //0-грн 1-гривень 2-только целую часть без грн.
{
class iceb_u_str s;
 class iceb_u_str s1("");
class iceb_u_str s2("");
int             i;
class iceb_u_str sss("");;

 
s.plus(hs,2);
iceb_u_polen(s.ravno(),&s2,2,'.');

str->new_plus(iceb_u_propis(hs,1));

if(gri == 0)
 {
/************
  sprintf(sss,"%s %s %s %s",str,gettext("грн."),
  s2.ravno(),gettext("коп."));
*****************/
  sss.new_plus(str->ravno());
  sss.plus(" ",gettext("грн."));
  sss.plus(" ",s2.ravno());
  sss.plus(" ",gettext("коп."));
 }
if(gri == 1)
 {
  i=iceb_u_pole2(str->ravno(),' ');
  iceb_u_polen(str->ravno(),&s,i,' ');

  s1.new_plus(gettext("гривень"));

  if(iceb_u_SRAV(s.ravno(),gettext("одна"),0) == 0)
   s1.new_plus(gettext("гривня"));

  if(iceb_u_SRAV(s.ravno(),gettext("две"),0) == 0)
    s1.new_plus(gettext("гривни"));

  if(iceb_u_SRAV(s.ravno(),gettext("три"),0) == 0)
      s1.new_plus(gettext("гривни"));

  if(iceb_u_SRAV(s.ravno(),gettext("четыре"),0) == 0)
   s1.new_plus(gettext("гривни"));

  switch (s2.ravno()[1])
   {
    case '1' :
      if(s2.ravno()[0] == '1')
        s.new_plus(gettext("копеек"));
     else
        s.new_plus(gettext("копейка"));
     break;
    case '2' :
    case '3' :
    case '4' :
     s.new_plus(gettext("копейки"));
     break;
    default:
     s.new_plus(gettext("копеек"));
     break;
   }
/*************
  if(str[0] != '\0')
    sprintf(sss,"%s %s %s %s",str,s1.ravno(),s2.ravno(),s.ravno());
  else
    sprintf(sss,"%s %s",s2.ravno(),s.ravno());
****************/
  if(str->ravno()[0] != '\0')
   {
   sss.new_plus(str->ravno());
   sss.plus(" ",s1.ravno());
   sss.plus(" ",s2.ravno());
   sss.plus(" ",s.ravno());
   }
  else
   {
    sss.new_plus(s2.ravno());
    sss.plus(" ",s.ravno());
   }
 }
if(gri == 2)
  sss.new_plus(str->ravno());

str->new_plus(sss.ravno());
}

/***********************************/

void iceb_u_preobr(double hs, //Число
char *str, //Строка
short gri)  //0-грн 1-гривень 2-только целую часть без грн.
{
class iceb_u_str stroka("");
iceb_u_preobr(hs,&stroka,gri);
strcpy(str,stroka.ravno());
}
