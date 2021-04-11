/*$Id: iceb_get_text_str.c,v 1.5 2013/08/13 06:10:10 sasa Exp $*/
/*15.11.2011	09.09.2003	Белых А.И.	iceb_get_text_str.c
Получение текста из строки ввода
*/

#include "iceb_libbuh.h"


void iceb_get_text_str(GtkWidget *widget,iceb_u_str *text)
{
text->new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
//printf("\niceb_get_text-%s\n",text->ravno());
//gtk_editable_select_region(GTK_EDITABLE(widget),0,text->getdlinna());
gtk_editable_select_region(GTK_EDITABLE(widget),0,text->getdlinna());

}
