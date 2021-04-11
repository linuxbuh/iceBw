/*$Id: iceb_u_getcurrentdir.c,v 1.2 2013/08/13 06:10:17 sasa Exp $*/
/*04.09.2012	04.09.2012	Белых А.И.	iceb_u_getcurrentdir.c
Получение текущего каталога
*/
#include <glib.h>
#include "iceb_util.h"
const char *iceb_u_getcurrentdir()
{

static class iceb_u_str current_dir("");

gchar *bros=g_get_current_dir();
current_dir.new_plus(bros);
g_free(bros);

return(current_dir.ravno());



}
