/*$Id: iceb_get_menu.c,v 1.8 2013/08/13 06:10:09 sasa Exp $*/
/*09.08.2004	09.08.2004	Белых А.И.	iceb_get_menu.c
Чтение выбранной в меню строки
*/
#include "iceb_libbuh.h"

void    iceb_get_menu(GtkWidget *widget,int *data)
{
int enter=atoi(gtk_widget_get_name(widget));
*data=enter;
}
