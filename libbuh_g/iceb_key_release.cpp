/*$Id: iceb_key_release.c,v 1.10 2013/05/17 14:58:34 sasa Exp $*/
/*28.09.2003	28.09.2003	Белых А.И.	iceb_key_release.c
Обработчик отжатия клавиш 
*/
#include "iceb_libbuh.h"

gboolean iceb_key_release(GtkWidget *widget,GdkEventKey *event,int *data)
{
//printf("iceb_key_release-Отжата клавиша\n");

switch(event->keyval)
 {

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("%s-Отжата клавиша переключения регистра\n",__FUNCTION__);

    *data=0;
    return(TRUE);
 }

//printf("iceb_key_release-fime\n");

return(TRUE);

}
