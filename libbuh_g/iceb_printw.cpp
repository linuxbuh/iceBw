/*$Id: iceb_printw.c,v 1.11 2013/12/31 11:49:19 sasa Exp $*/
/*24.05.2016	19.12.2003	Белых А.И.	iceb_printw.c
Вывод текста в приготовленное окно 
*/
#include "iceb_libbuh.h"

void iceb_printw(const char *stroka,GtkTextBuffer *buffer,GtkWidget *view)
{

GtkTextIter iter;

gtk_text_buffer_get_iter_at_offset(buffer,&iter,-1);
gtk_text_buffer_insert(buffer,&iter,stroka,-1);

gtk_text_buffer_get_iter_at_offset(buffer,&iter,-1);
gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view),&iter,0.0,TRUE,0.,1.);

iceb_refresh();
 
}
/******************************************/
void iceb_printw(const char *stroka,GtkTextBuffer *buffer,GtkWidget *view,const char *name_tag)
{
GtkTextIter iter;

gtk_text_buffer_get_iter_at_offset(buffer,&iter,-1);
gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,stroka,-1,name_tag,NULL);


gtk_text_buffer_get_iter_at_offset(buffer,&iter,-1);
gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view),&iter,0.0,TRUE,0.,1.);

iceb_refresh();
}
