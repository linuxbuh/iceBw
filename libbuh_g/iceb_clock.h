/*$Id: iceb_clock.h,v 1.6 2013/08/13 06:10:09 sasa Exp $*/
/*20.05.2016	23.04.2005	Белых А.И.	iceb_clock.c
Установка курсора 
*/

class iceb_clock
 {
  GtkWidget *www;
  GdkDisplay *disp;
  GdkWindow *cwindow;
  public:
  
  //Конструктор    
  iceb_clock(GtkWidget *kkk)
   {
    disp=gtk_widget_get_display(kkk);
    cwindow=gtk_widget_get_window(kkk);
    www=kkk;
    if(www != NULL)
     {
//      printf("iceb_clock-установили курсор Ждите !!!\n");
  
 //      gdk_window_set_cursor(gtk_widget_get_window(www),gdk_cursor_new(ICEB_CURSOR_GDITE));
       gdk_window_set_cursor(cwindow,gdk_cursor_new_for_display(disp,ICEB_CURSOR_GDITE));

      while(gtk_events_pending())
       gtk_main_iteration();
     }
   } 
  //Деструктор
  ~iceb_clock()
   {
    clear_data();
   }
  void clear_data()
   {
    if(www != NULL)
     {
//      gdk_window_set_cursor(www->window,gdk_cursor_new(ICEB_CURSOR));
//      gdk_window_set_cursor(gtk_widget_get_window(www),gdk_cursor_new(ICEB_CURSOR));
      gdk_window_set_cursor(cwindow,gdk_cursor_new_for_display(disp,ICEB_CURSOR));


//      printf("iceb_clock-Сняли курсор Ждите !!!\n");
     }
    www=NULL;
   }
 };
