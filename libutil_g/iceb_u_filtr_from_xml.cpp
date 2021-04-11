/*$Id: iceb_u_filtr_xml.c,v 1.3 2013/08/13 06:10:17 sasa Exp $*/
/*02.02.2015	29.01.2015	Белых А.И.	iceb_u_filtr_from_xml.c
Преобразование символьных строк по требованию налоговиков для символьных строк

> - &gt;
< - &lt;
" - &quot;
' - &apos;
& - &amp;

*/
#include "iceb_util.h"

const char *iceb_u_filtr_from_xml(const char *stroka)
{
static class iceb_u_str str_out;
str_out.new_plus(stroka);

iceb_u_zvstr(&str_out,"&amp;","&");
iceb_u_zvstr(&str_out,"&gt;",">");
iceb_u_zvstr(&str_out,"&lt;","<");
iceb_u_zvstr(&str_out,"&quot;","\"");
iceb_u_zvstr(&str_out,"&apos;","'");

return(str_out.ravno());

}
