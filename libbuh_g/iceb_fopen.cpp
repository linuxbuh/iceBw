/*$Id: iceb_fopen.c,v 1.3 2013/08/13 06:10:09 sasa Exp $*/
/*12.03.2013	12.03.2013	Белых А.И.	iceb_fopen.c
Отрыть файл 
*/
#include <errno.h>
#include "iceb_libbuh.h"

int iceb_fopen::start(const char *imaf,const char *wr,GtkWidget *wpredok)
{
 if((ff =fopen(imaf,wr)) == NULL)
  {
   iceb_er_op_fil(imaf,"",errno,wpredok);
   return(1);
  }
return(0);
}
