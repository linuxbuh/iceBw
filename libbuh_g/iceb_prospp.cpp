/*$Id: iceb_prospp.c,v 1.9 2013/05/17 14:58:36 sasa Exp $*/
/*16.11.2010	26.10.2005	Белых А.И.	iceb_prospp.c
Просмотр перед печатью файла
*/
#include	"iceb_libbuh.h"


int iceb_prospp(const char *imaf,GtkWidget *wpredok)
{
int voz=0;

iceb_print_operation(GTK_PRINT_OPERATION_ACTION_PREVIEW,imaf,wpredok);

return(voz);

}
