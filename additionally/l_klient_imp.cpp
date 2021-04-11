/*$Id: l_klient_imp.c,v 1.9 2013/08/13 06:10:23 sasa Exp $*/
/*25.05.2016	24.10.2006	Белых А.И.	l_klient_imp.c
импорт карточек из файла
*/
#include  <stdlib.h>
#include  <math.h>
#include  <errno.h>
#include  <unistd.h>
#include        <sys/stat.h>
#include "i_rest.h"

class l_klient_imp_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  class iceb_u_str imafz;
          
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчет завершен 1-нет
  l_klient_imp_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   l_klient_imp_key_press(GtkWidget *widget,GdkEventKey *event,class l_klient_imp_data *data);
gint l_klient_imp1(class l_klient_imp_data *data);
void  l_klient_imp_v_knopka(GtkWidget *widget,class l_klient_imp_data *data);

extern SQL_baza bd;
extern char *name_system;
extern double okrcn;

int l_klient_imp(const char *imaf,
GtkWidget *wpredok)
{
char strsql[500];
iceb_u_spisok repl_s;
class l_klient_imp_data data;

data.imafz.new_plus(imaf);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);
sprintf(strsql,"%s %s",name_system,gettext("Импорт карточек из файла"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_klient_imp_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

GtkWidget *label=NULL;

label=gtk_label_new(gettext("Импорт карточек из файла"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(l_klient_imp_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)l_klient_imp1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_klient_imp_v_knopka(GtkWidget *widget,class l_klient_imp_data *data)
{
printf("l_klient_imp_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_klient_imp_key_press(GtkWidget *widget,GdkEventKey *event,class l_klient_imp_data *data)
{
 printf("l_klient_imp_key_press\n");
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

gint l_klient_imp1(class l_klient_imp_data *data)
{
time_t vremn;
time(&vremn);
char strsql[500];
iceb_clock sss(data->window);
struct stat work;


if(stat(data->imafz.ravno(),&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Не найден файл"),data->imafz.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }



FILE *ff;

if((ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafprom[50];
FILE *ffprom;
sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  iceb_er_op_fil(imafprom,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char kod[20];
char grup[20];
int pl=0;
char pol[50];

int  kolkart=0;
float razmer=0.;
char strok[1024];
int i=0;

while(fgets(strok,sizeof(strok),ff) != NULL)
 {
  i=strlen(strok);
  razmer+=i;
  if(strok[i-2] == '\r') //Возврат каретки меняем на пробел
   strok[i-2]=' ';

  iceb_printw(strok,data->buffer,data->view);
 
  iceb_pbar(data->bar,work.st_size,razmer);    

  if(iceb_u_polen(strok,kod,sizeof(kod),1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  
  //Проверяем на отсутсвие такого табельного номера в базе данных
  sprintf(strsql,"select kod from Taxiklient where kod='%s'",kod);
  if(iceb_sql_readkey(strsql,data->window) > 0)
   {
    
    sprintf(strsql,gettext("Карточка %s уже введена !"),kod);
    iceb_menu_soob(strsql,data->window);
    fprintf(ffprom,"%s\n%s\n",strok,strsql);
    continue;
   } 

  iceb_u_polen(strok,grup,sizeof(grup),2,'|');
  //проверяем группу клиента
  sprintf(strsql,"select kod from Grupklient where kod='%s'",grup);
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    
    sprintf(strsql,"%s %s !",gettext("Не найдено код группы"),grup);
    iceb_menu_soob(strsql,data->window);
    fprintf(ffprom,"%s\n%s\n",strok,strsql);
    continue;
   } 

  iceb_u_polen(strok,pol,sizeof(pol),3,'|');
  pl=0;
  if(pol[0] == 'W' || pol[0] == 'w')
   pl=1;  
   
  sprintf(strsql,"insert into Taxiklient set kod='%s', grup='%s', pl=%d",kod,grup,pl);
  
  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
    continue;
  kolkart++;  
 }

fclose(ff);
fclose(ffprom);
unlink(data->imafz.ravno());

rename(imafprom,data->imafz.ravno());


class iceb_u_str repl;

repl.new_plus(gettext("Загрузка завершена"));
repl.ps_plus(gettext("Количество загруженых карточек"));
repl.plus(":");
repl.plus(kolkart);

iceb_menu_soob(&repl,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);
}
