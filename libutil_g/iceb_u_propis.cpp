/*$Id: iceb_u_propis.c,v 1.2 2013/08/13 06:10:17 sasa Exp $*/
/*16.04.2017	05.07.2011	Белых А.И.	iceb_u_propis.c
получение целой части числа прописью
Подпрограмма преобразования числа в текстовую строку
				      |       |       |
			     12 11 10 | 9 8 7 | 6 5 4 | 3  2  1
				      |       |       |
*/
#include <glib.h>
#include <ctype.h>
#include	"iceb_util.h"

const char *iceb_u_propis(double hs,int metka_1s) /*0-первая буква маленькая 1-большая*/
{
static class iceb_u_str str("");

class iceb_u_str s;
class iceb_u_str s1("");
int             kls;
int             i;
short           md;  /*Метка десятка*/
short           mt;  /*Метка тысячи*/
short           mm;  /*Метка миллиона*/
short           mml; /*Метка миллиарда*/

if(str.getdlinna() > 1)
 str.new_plus("");
//sprintf(s,"%.2f",hs);
s.plus(hs,2);
iceb_u_polen(s.ravno(),&s1,1,'.');

mml=mm=mt=md=0;

kls=strlen(s1.ravno());
for(i=kls; i> 0 ;  i--)
 {
  switch (i)
   {
    case 1 :
     if(s1.ravno()[kls-i] == '0')
      {
       if(md == 1)
	{
         str.plus(gettext(" десять"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '1')
      {
       if(md == 1)
	{
         str.plus(gettext(" одиннадцать"));
	}
       else
	{
	 str.plus(gettext(" одна"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '2')
      {
       if(md == 1)
	{
         str.plus(gettext(" двенадцать"));
	}
       else
	{
         str.plus(gettext(" две"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '3')
      {
       if(md == 1)
	{
         str.plus(gettext(" тринадцать"));
	}
       else
	  str.plus(gettext(" три"));
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '4')
      {
       if(md == 1)
	{
         str.plus(gettext(" четырнадцать"));
	}
       else
	{
	   str.plus(gettext(" четыре"));
	}
       md=0;
       continue;

      }
     if(s1.ravno()[kls-i] == '5')
      {
       if(md == 1)
	{
	   str.plus(gettext(" пятнадцать"));
	}
       else
	{
	   str.plus(gettext(" пять"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '6')
      {
       if(md == 1)
	{
	   str.plus(gettext(" шестнадцать"));
	}
       else
	{
	   str.plus(gettext(" шесть"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '7')
      {
       if(md == 1)
	{
	   str.plus(gettext(" семнадцать"));
	}
       else
	{
	   str.plus(gettext(" семь"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '8')
      {
       if(md == 1)
	{
	   str.plus(gettext(" восемнадцать"));
	}
       else
	{
	   str.plus(gettext(" восемь"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '9')
      {
       if(md == 1)
	{
	   str.plus(gettext(" девятнадцать"));
	}
       else
	{
	   str.plus(gettext(" девять"));
	}
       md=0;
       continue;
      }

     break;
/*******************************/
    case 2 :
    case 5 :
    case 8 :
    case 11 :
     if(i == 5)
      if(s1.ravno()[kls-i] == '0')
	mt++;
     if(i == 8)
      if(s1.ravno()[kls-i] == '0')
	mm++;
     if(i == 11)
      if(s1.ravno()[kls-i] == '0')
	mml++;

     if(s1.ravno()[kls-i] == '1')
       md++;;
     if(s1.ravno()[kls-i] == '2')
      {
	 str.plus(gettext(" двадцать"));
       continue;
      }
     if(s1.ravno()[kls-i] == '3')
      {
	 str.plus(gettext(" тридцать"));
       continue;
      }
     if(s1.ravno()[kls-i] == '4')
      {
       str.plus(gettext(" сорок"));
       continue;
      }
     if(s1.ravno()[kls-i] == '5')
      {
	  str.plus(gettext(" пятьдесят"));
       continue;
      }
     if(s1.ravno()[kls-i] == '6')
      {
	 str.plus(gettext(" шестьдесят"));
       continue;
      }
     if(s1.ravno()[kls-i] == '7')
      {
	 str.plus(gettext(" семьдесят"));
       continue;
      }
     if(s1.ravno()[kls-i] == '8')
      {
	 str.plus(gettext(" восемьдесят"));
       continue;
      }
     if(s1.ravno()[kls-i] == '9')
      {
	 str.plus(gettext(" девяносто"));
       continue;
      }
     break;
/******************************/
    case 3 :
    case 6 :
    case 9 :
    case 12 :

     if(i == 6)
      if(s1.ravno()[kls-i] == '0')
       mt++;

     if(i == 9)
      if(s1.ravno()[kls-i] == '0')
	mm++;
     if(i == 12)
      if(s1.ravno()[kls-i] == '0')
	mml++;
     /*
     if(s1.ravno()[kls-i] == '0')
	mt++;
       */

     if(s1.ravno()[kls-i] == '1')
       str.plus(gettext(" сто"));
     if(s1.ravno()[kls-i] == '2')
      {
	 str.plus(gettext(" двести"));
       continue;
      }
     if(s1.ravno()[kls-i] == '3')
      {
       str.plus(gettext(" триста"));
       continue;
      }
     if(s1.ravno()[kls-i] == '4')
      {
	 str.plus(gettext(" четыреста"));
       continue;
      }
     if(s1.ravno()[kls-i] == '5')
      {
	 str.plus(gettext(" пятьсот"));
       continue;
      }
     if(s1.ravno()[kls-i] == '6')
      {
	 str.plus(gettext(" шестьсот"));
       continue;
      }
     if(s1.ravno()[kls-i] == '7')
      {
	 str.plus(gettext(" семьсот"));
       continue;
      }
     if(s1.ravno()[kls-i] == '8')
      {
	 str.plus(gettext(" восемьсот"));
       continue;
      }
     if(s1.ravno()[kls-i] == '9')
      {
	 str.plus(gettext(" девятьсот"));
       continue;
      }
     break;
/******************************/
    case 4 :
     if(s1.ravno()[kls-i] == '0')
      {
       if(md == 1)
	{
	   str.plus(gettext(" десять тысяч"));
	}
       else
	 if(mt < 2)
	  {
	     str.plus(gettext(" тысяч"));
	  }
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '1')
      {
       if(md == 1)
	{
	   str.plus(gettext(" одиннадцать тысяч"));
	}
       else
	{
	   str.plus(gettext(" одна тысяча"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '2')
      {
       if(md == 1)
	{
	   str.plus(gettext(" двенадцать тысяч"));
	}
       else
	{
	   str.plus(gettext(" две тысячи"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '3')
      {
       if(md == 1)
	{
	   str.plus(gettext(" тринадцать тысяч"));
	}
       else
	{
	  str.plus(gettext(" три тысячи"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '4')
      {
       if(md == 1)
	{
	  str.plus(gettext(" четырнадцать тысяч"));
	}
       else
	{
	   str.plus(gettext(" четыре тысячи"));
	}
       md=0;
       continue;

      }
     if(s1.ravno()[kls-i] == '5')
      {
       if(md == 1)
	{
	   str.plus(gettext(" пятнадцать тысяч"));
	}
       else
	{
	   str.plus(gettext(" пять тысяч"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '6')
      {
       if(md == 1)
	{
	   str.plus(gettext(" шестнадцать тысяч"));
	}
       else
	{
	   str.plus(gettext(" шесть тысяч"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '7')
      {
       if(md == 1)
	{
	   str.plus(gettext(" семнадцать тысяч"));
	}
       else
	{
	   str.plus(gettext(" семь тысяч"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '8')
      {
       if(md == 1)
	{
	   str.plus(gettext(" восемнадцать тысяч"));
	}
       else
	{
	   str.plus(gettext(" восемь тысяч"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '9')
      {
       if(md == 1)
	{
	   str.plus(gettext(" девятнадцать тысяч"));
	}
       else
	{
	   str.plus(gettext(" девять тысяч"));
	}
       md=0;
       continue;
      }
     break;

/****************************/
    case 7 :
     if(s1.ravno()[kls-i] == '0')
      {
       if(md == 1)
	{
	  str.plus(gettext(" десять миллионов"));
	}
       else
	 if(mm < 2)
	  {
	     str.plus(gettext(" миллионов"));
	  }
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '1')
      {
       if(md == 1)
	{
	   str.plus(gettext(" одиннадцать миллионов"));
	}
       else
	{
	   str.plus(gettext(" один миллион"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '2')
      {
       if(md == 1)
	{
	   str.plus(gettext(" двенадцать миллионов"));
	}
       else
	{
	   str.plus(gettext(" два миллиона"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '3')
      {
       if(md == 1)
	{
	  str.plus(gettext(" тринадцать миллионов"));
	}
       else
	{
	  str.plus(gettext(" три миллиона"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '4')
      {
       if(md == 1)
	{
	  str.plus(gettext(" четырнадцать миллионов"));
	}
       else
	{
	   str.plus(gettext(" четыре миллиона"));
	}
       md=0;
       continue;

      }
     if(s1.ravno()[kls-i] == '5')
      {
       if(md == 1)
	{
	   str.plus(gettext(" пятнадцать миллионов"));
	}
       else
	{
	   str.plus(gettext(" пять миллионов"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '6')
      {
       if(md == 1)
	{
	   str.plus(gettext(" шестнадцать миллионов"));
	}
       else
	{
	   str.plus(gettext(" шесть миллионов"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '7')
      {
       if(md == 1)
	{
	   str.plus(gettext(" семнадцать миллионов"));
	}
       else
	{
	   str.plus(gettext(" семь миллионов"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '8')
      {
       if(md == 1)
	{
	   str.plus(gettext(" восемнадцать миллионов"));
	}
       else
	{
	   str.plus(gettext(" восемь миллионов"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '9')
      {
       if(md == 1)
	{
	   str.plus(gettext(" девятнадцать миллионов"));
	}
       else
	{
	   str.plus(gettext(" девять миллионов"));
	}
       md=0;
       continue;
      }
     break;
/********************************/
    case 10 :
     if(s1.ravno()[kls-i] == '0')
      {
       if(md == 1)
	{
	   str.plus(gettext(" десять миллиардов"));
	}
       else
	 if(mml < 2 )
	  {
	     str.plus(gettext(" миллиард"));
	  }
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '1')
      {
       if(md == 1)
	{
	   str.plus(gettext(" одиннадцать миллиардов"));
	}
       else
	{
	   str.plus(gettext(" один миллиард"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '2')
      {
       if(md == 1)
	{
	   str.plus(gettext(" двенадцать миллиардов"));
	}
       else
	{
	   str.plus(gettext(" два миллиарда"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '3')
      {
       if(md == 1)
	{
	   str.plus(gettext(" тринадцать миллиардов"));
	}
       else
	{
	   str.plus(gettext(" три миллиарда"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '4')
      {
       if(md == 1)
	{
	   str.plus(gettext(" четырнадцать миллиардов"));
	}
       else
	{
	   str.plus(gettext(" четыре миллиарда"));
	}
       md=0;
       continue;

      }
     if(s1.ravno()[kls-i] == '5')
      {
       if(md == 1)
	{
	   str.plus(gettext(" пятнадцать миллиардов"));
	}
       else
	{
	   str.plus(gettext(" пять миллиардов"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '6')
      {
       if(md == 1)
	{
	   str.plus(gettext(" шестнадцать миллиардов"));
	}
       else
	{
	   str.plus(gettext(" шесть миллиардов"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '7')
      {
       if(md == 1)
	{
	   str.plus(gettext(" семнадцать миллиардов"));
	}
       else
	{
	   str.plus(gettext(" семь миллиардов"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '8')
      {
       if(md == 1)
	{
	   str.plus(gettext(" восемнадцать миллиардов"));
	}
       else
	{
	   str.plus(gettext(" восемь миллиардов"));
	}
       md=0;
       continue;
      }
     if(s1.ravno()[kls-i] == '9')
      {
       if(md == 1)
	{
	   str.plus(gettext(" девятнадцать миллиардов"));
	}
       else
	{
	   str.plus(gettext(" девять миллиардов"));
	}
       md=0;
       continue;
      }
     break;
   }
 }
/***********
if(str[0] == '\0')
  sprintf(str," %s",gettext("ноль "));
**********/
if(str.ravno()[0] == '\0')
 str.plus(" ",gettext("ноль"));
if(metka_1s == 0)
 return(iceb_u_adrsimv(1,str.ravno()));
// return(&str[1]); /*первый символ всегда пробел*/

/*Преобразование первой буквы из малой в большую*/
if(g_utf8_validate(str.ravno(),-1,NULL) == FALSE)
 {
  char simv=str.ravno()[1];
//  str[1]=toupper(simv);
  char tmp[str.getdlinna()+1];
  strcpy(tmp,str.ravno());
  tmp[1]=toupper(simv);
  str.new_plus(tmp);
 }
else
 {
  char ddd[16];
  memset(ddd,'\0',sizeof(ddd));
  g_utf8_strncpy(ddd,str.ravno(),2);

  s1.new_plus(g_utf8_strup(ddd,-1));
  gchar *strout=g_strdup_printf("%s%s",s1.ravno(),iceb_u_adrsimv(2,str.ravno()));
  str.new_plus(strout);
  g_free(strout);  
 }

//return(&str[1]); /*первый символ всегда пробел*/
return(iceb_u_adrsimv(1,str.ravno()));
}
