/*$Id:$*/
/*08.05.2015	08.05.2015	Белых А.И.	iceb_beep.c
Звуковой сигнал
*/
#include <gdk/gdk.h>
void iceb_beep()
{
gdk_display_beep(gdk_display_get_default());

}
