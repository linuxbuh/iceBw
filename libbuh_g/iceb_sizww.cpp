/*$Id:$*/
/*24.02.2015	22.01.2014	Белых А.И.	iceb_sizww.c
Запись размеров окна в файл
*/
#include <errno.h>
#include "iceb_libbuh.h"
int iceb_sizww(const char *name_ok,GtkWidget *wpredok)
{
gint gor=0;
gint vert=0;

gtk_window_get_size(GTK_WINDOW(wpredok),&gor,&vert);

return(iceb_sizw(name_ok,gor,vert,wpredok));

}