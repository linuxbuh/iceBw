/*$Id: iceb_menu_vibknop.c,v 1.5 2013/08/13 06:10:12 sasa Exp $*/
/*05.01.2004	05.01.2004	Белых А.И.	iceb_menu_vibknop.c
Выбор кнопки в простом меню выбора
*/
#include "iceb_libbuh.h"

void  iceb_menu_vibknop(GtkWidget *widget,struct vibknop_data *data)
{

int kn=atoi(gtk_widget_get_name(widget));

data->knopka=kn;

gtk_widget_destroy(data->window);

}
