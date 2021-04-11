/*$Id: iceb_calendar.c,v 1.13 2013/08/13 06:10:09 sasa Exp $*/
/*04.02.2005	03.02.2005	Белых А.И.	iceb_calendar.c
Меню для выбора даты
*/

#include "iceb_libbuh.h"

class iceb_calendar_data
 {
  public:
  GtkWidget *window;
  GtkWidget *calendar;
  short dv;
  short mv;
  short gv;
  int voz;

  iceb_calendar_data()
   {
    voz=1;
   }
 };
void iceb_calendar_read_data(GtkCalendar *calendar,iceb_calendar_data *data);

int iceb_calendar(iceb_u_str *get_data,GtkWidget *wpredok)
{
short dv=0,mv=0,gv=0;
int voz=0;
if((voz=iceb_calendar(&dv,&mv,&gv,wpredok)) == 0)
 {
  char bros[56];
  sprintf(bros,"%d.%d.%d",dv,mv,gv);
  get_data->new_plus(bros);
 }
return(voz);
} 

int iceb_calendar(short *dv,short *mv,short *gv,GtkWidget *wpredok)
{
class iceb_calendar_data data;
char bros[512];

data.window= gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/

sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Календарь"));
gtk_window_set_title(GTK_WINDOW(data.window),bros);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

data.calendar=gtk_calendar_new();
gtk_container_add (GTK_CONTAINER (data.window), data.calendar);

g_signal_connect(data.calendar,"day_selected_double_click",G_CALLBACK(iceb_calendar_read_data),&data);

//показывать в шапке месяц и год
//gtk_calendar_display_options(GTK_CALENDAR(data.calendar),GTK_CALENDAR_SHOW_HEADING);

gtk_widget_show_all(data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

//printf("iceb_calendar %d.%d.%d\n",data.dv,data.mv,data.gv);
*dv=data.dv;
*mv=data.mv;
*gv=data.gv;

return(data.voz);

}
/***************************/
/*Чтение выбранной даты    */
/***************************/

void iceb_calendar_read_data(GtkCalendar *calendar,iceb_calendar_data *data)
{

guint dv=0;
guint mv=0;
guint gv=0;

gtk_calendar_get_date(calendar,&gv,&mv,&dv);
data->dv=dv;
data->mv=mv+1;
data->gv=gv;

data->voz=0;

gtk_widget_destroy(data->window);

}
