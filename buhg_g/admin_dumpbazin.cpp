/*$Id: admin_dumpbazin.c,v 1.10 2014/05/07 10:58:06 sasa Exp $*/
/*23.05.2016	16.08.2012	Белых А.И.	admin_dumpbazin.c
Загрузка дампа базы
*/
#include <errno.h>
#include <sys/stat.h>
#include "iceb_libbuh.h"

class admin_dumpbazin_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class iceb_u_str login;
  class iceb_u_str parol;
  class iceb_u_str hostname;
  int metka_zag;    
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  class iceb_u_str imaf;
  
  admin_dumpbazin_data()
   {
    imaf.plus("");
    voz=1;
    kon_ras=1;
    metka_zag=0;
   }
 };
gboolean   admin_dumpbazin_key_press(GtkWidget *widget,GdkEventKey *event,class admin_dumpbazin_data *data);
gint admin_dumpbazin1(class admin_dumpbazin_data *data);
void  admin_dumpbazin_v_knopka(GtkWidget *widget,class admin_dumpbazin_data *data);

int admin_dumpbazin_m(class iceb_u_str *imafil,int *metka_zag,GtkWidget *wpredok);

int admin_dumpbazin(const char *login,const char *parol,const char *hostname,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class admin_dumpbazin_data data;
class iceb_u_spisok textmenu;
int gor=0;
int ver=0;

if(admin_dumpbazin_m(&data.imaf,&data.metka_zag,wpredok) != 0)
 return(1);

data.login.new_plus(login);
data.parol.new_plus(parol);
data.hostname.new_plus(hostname);
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",__FUNCTION__,gettext("Загрузка дампа базы"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(admin_dumpbazin_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Загрузка дампа базы из файла"));

GtkWidget *label=gtk_label_new(repl.ravno());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(admin_dumpbazin_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)admin_dumpbazin1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_dumpbazin_v_knopka(GtkWidget *widget,class admin_dumpbazin_data *data)
{
//printf("admin_dumpbazin_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_dumpbazin_key_press(GtkWidget *widget,GdkEventKey *event,class admin_dumpbazin_data *data)
{
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}
/******************************************/
/******************************************/

gint admin_dumpbazin1(class admin_dumpbazin_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_clock sss(data->window);
int kolih_bar=4;
float kolih=0.;
class iceb_u_str imafin;
class iceb_u_str syfix;
int voz=0;

iceb_pbar(data->bar,kolih_bar,++kolih);    
iceb_refresh();
sleep(1);/*даём задержку чтобы отрисовалось окно программы*/


sprintf(strsql,"%s\n",gettext("Загружаем дамп базы данных"));
iceb_printw(strsql,data->buffer,data->view);


struct stat bufstat;
if(stat(data->imaf.ravno(),&bufstat) != 0)
 {
  iceb_er_op_fil(data->imaf.ravno(),"",errno,data->window);
  goto kon;
 }

imafin.new_plus(data->imaf.ravno());

iceb_pbar(data->bar,kolih_bar,++kolih);    
iceb_refresh();

if(iceb_u_polen(data->imaf.ravno(),&syfix,2,'.') == 0)
 {

  if(iceb_u_SRAV("bz2",syfix.ravno(),0) == 0 || iceb_u_SRAV("BZ2",syfix.ravno(),0) == 0)
   {
    iceb_u_polen(data->imaf.ravno(),&imafin,1,'.');

    sprintf(strsql,"%s bunzip2\n",gettext("Разжимаем файл программой"));
    iceb_printw(strsql,data->buffer,data->view);
    
    
    sprintf(strsql,"bunzip2 %s 2>&1",data->imaf.ravno_shell_quote());
    iceb_printw(strsql,data->buffer,data->view);
    iceb_printw("\n",data->buffer,data->view);

//    if(system(strsql) != 0)
    if(iceb_system(strsql,data->buffer,data->view) != 0)
     {
      sprintf(strsql,"%s!\n",gettext("Ошибка выполнения команды"));
      iceb_printw(strsql,data->buffer,data->view);
     }
   }
 }

iceb_pbar(data->bar,kolih_bar,++kolih);    
iceb_refresh();

sprintf(strsql,"%s %s\n",gettext("Загружаем дамп базы из файла"),imafin.ravno_shell_quote());
iceb_printw(strsql,data->buffer,data->view);
iceb_refresh();

if(data->metka_zag == 0)
  sprintf(strsql,"mysql -u %s -p'%s' -h %s 2>&1 < %s",data->login.ravno(),data->parol.ravno(),data->hostname.ravno(),imafin.ravno_shell_quote());
else
  sprintf(strsql,"mysql -u %s -p'%s' -h %s -f 2>&1 < %s",data->login.ravno(),data->parol.ravno(),data->hostname.ravno(),imafin.ravno_shell_quote());

//printf("%s-%s\n",__FUNCTION__,strsql);

if((voz=iceb_system(strsql,data->buffer,data->view)) != 0)
 {
  char bros[1024];
  sprintf(bros,"%s:\n",gettext("Ошибка выполнения команды"));
  iceb_printw(bros,data->buffer,data->view);

  iceb_printw(strsql,data->buffer,data->view);
  iceb_printw("\n",data->buffer,data->view);

 }

iceb_pbar(data->bar,kolih_bar,++kolih);    


kon:;

iceb_beep();

gtk_label_set_text(GTK_LABEL(data->label),gettext("Загрузка закончена"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}


