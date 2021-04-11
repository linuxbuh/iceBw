/*$Id: glkniw.c,v 1.5 2013/08/13 06:09:34 sasa Exp $*/
/*03.03.2009	03.03.2009	Белых А.И.	glkniw.c
Расчёт Главной книги
*/
#include "buhg_g.h"
#include "glkni.h"

int   glkni_m(class glkni_rr *rek_ras);
int glkniw_r(class glkni_rr *datark,GtkWidget *wpredok);




void glkniw()
{
static class glkni_rr data;
data.imaf.free_class();
data.naimf.free_class();



if(glkni_m(&data) != 0)
 return;

if(glkniw_r(&data,NULL) != 0)
  return;

iceb_rabfil(&data.imaf,&data.naimf,NULL);

}
