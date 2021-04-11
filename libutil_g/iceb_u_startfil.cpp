/*$Id: iceb_u_startfil.c,v 1.4 2013/05/17 14:58:43 sasa Exp $*/
/*05.07.2002	05.07.2002	Белых А.И.	iceb_u_startfil.c
Выдача в файл начальной команды для определения формата файла
*/
#include        <stdio.h>


void   iceb_u_startfil(FILE *ff)
{

fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/

}
