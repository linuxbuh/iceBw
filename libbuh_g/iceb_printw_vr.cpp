/*$Id: iceb_printw_vr.c,v 1.11 2013/12/31 11:49:19 sasa Exp $*/
/*22.05.2016	21.12.2003	Белых А.И.	iceb_printw_vr.c
Вывод в окно времени расчёта
*/
#include <math.h>
#include "iceb_libbuh.h"


void iceb_printw_vr(time_t vrem_n, //Время начала расчёта
GtkTextBuffer *buffer,GtkWidget *view)
{
char stroka[300];
time_t vrem_k;

time(&vrem_k);

double kii;
double dii=(vrem_k-vrem_n)/60.;

dii=modf(dii,&kii);

sprintf(stroka,"%s %.f%s %.f%s\n",
gettext("Время расчёта"),
kii,
gettext("мин."),
dii*60.,
gettext("сек."));

iceb_printw(stroka,buffer,view);

/*делаем для того чтобы показала нижнюю часть вывода на экран*/
GtkTextIter iter;
gtk_text_buffer_get_iter_at_offset(buffer,&iter,-1);
gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view),&iter,0.0,TRUE,0.,1.);

iceb_refresh();
}

