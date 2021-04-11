/*$Id: impotvrw_r.c,v 1.15 2014/07/31 07:08:25 sasa Exp $*/
/*23.05.2016	17.03.2009	Белых А.И.	impotvrw_r.c
импорт отработанного времени
*/
#include  <errno.h>
#include  <sys/stat.h>
#include "buhg_g.h"

class impotvrw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
    
  class impotvrw_rek *rk;
  const char *imaf_imp;
          
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  impotvrw_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   impotvrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class impotvrw_r_data *data);
gint impotvrw_r1(class impotvrw_r_data *data);
void  impotvrw_r_v_knopka(GtkWidget *widget,class impotvrw_r_data *data);

extern SQL_baza bd;
extern double okrcn;

int impotvrw_r(const char *imaf_imp,
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_spisok repl_s;
int gor=0;
int ver=0;
class impotvrw_r_data data;

data.imaf_imp=imaf_imp;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт отработанного времени"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(impotvrw_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт отработанного времени"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(impotvrw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)impotvrw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  impotvrw_r_v_knopka(GtkWidget *widget,class impotvrw_r_data *data)
{
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   impotvrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class impotvrw_r_data *data)
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

gint impotvrw_r1(class impotvrw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_clock sss(data->window);
struct stat work;
class iceb_u_str strok("");
class iceb_u_str bros("");
SQL_str row;
class SQLCURSOR cur;
if(stat(data->imaf_imp,&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Нет найден файл"),data->imaf_imp);
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }


FILE *ff=NULL;

if((ff = fopen(data->imaf_imp,"r")) == NULL)
 {
  iceb_er_op_fil(data->imaf_imp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafprom[64];
FILE *ffprom=NULL;

sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  iceb_er_op_fil(imafprom,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
float		mkolrd=0.; //Количество рабочих дней в месяце
float		mkolrh=0.;  //Количество рабочих часов в месяце
float kolrd=0.,kolrh=0.,kolkd=0.;
int kodtb=0;
short mes=0,god=0;
float razmer=0;
int tabnom=0;
class iceb_u_str tabnom_str("");
while(iceb_u_fgets(&strok,ff) == 0)
 {
  razmer+=strlen(strok.ravno());

  if(strok.ravno()[0] == '#')
   continue;

  iceb_printw(strok.ravno(),data->buffer,data->view);
  iceb_pbar(data->bar,work.st_size,razmer);    

  if(iceb_u_polen(strok.ravno(),&bros,1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  if(bros.ravno()[0] == '"') /*Для Госстандарта*/
    tabnom_str.new_plus(iceb_u_adrsimv(1,bros.ravno()));
  else
    tabnom_str.new_plus(bros.ravno());
  if((tabnom=tabnom_str.ravno_atoi()) == 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
    
  sprintf(strsql,"select tabn from Kartb where tabn=%d",tabnom);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) <= 0)
   {

    sprintf(strsql,"%s:%d",gettext("Не найден табельный номер"),tabnom);
    iceb_menu_soob(strsql,data->window);

    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"%s:%d\n",\
    gettext("Не найден табельный номер"),tabnom);
    continue;
   }

  if(iceb_u_polen(strok.ravno(),&kodtb,2,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
//  kodtb=atoi(bros);

  sprintf(strsql,"select kod from Tabel where kod=%d",kodtb);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) <= 0)
   {

    sprintf(strsql,"%s:%d",gettext("Не найден код табеля"),kodtb);
    iceb_menu_soob(strsql,data->window);

    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"%s:%d\n",\
    gettext("Не найден код табеля"),kodtb);
    continue;
   }

  sprintf(strsql,"select god from Ztab where tabn=%d and god=%d and \
mes=%d and kodt=%d and nomz=%d",tabnom,god,mes,kodtb,0);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
   {
    iceb_menu_soob(gettext("Такая запись уже есть !"),data->window);

    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"%s\n",gettext("Такая запись уже есть !"));
    continue;
   }

  //Проверяем блокировку карточки
  sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
tabn=%d",god,mes,tabnom);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   if(atoi(row[0]) != 0)
    {
     sprintf(strsql,"%s !",gettext("Карточка заблокирована"));
     iceb_menu_soob(strsql,data->window);
     
     fprintf(ffprom,"%s",strok.ravno());
     fprintf(ffprom,"%s !\n",gettext("Карточка заблокирована"));
     continue;
    }
   

  if(iceb_u_polen(strok.ravno(),&kolrd,3,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
//  kolrd=atof(bros);

  if(iceb_u_polen(strok.ravno(),&kolrh,4,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
//  kolrh=atof(bros);

  if(iceb_u_polen(strok.ravno(),&kolkd,5,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
//  kolkd=atof(bros);

  if(iceb_u_polen(strok.ravno(),&mkolrd,6,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
//  mkolrd=atof(bros);

  if(iceb_u_polen(strok.ravno(),&mkolrh,7,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
//  mkolrh=atof(bros);

  zaptabelw(0,mes,god,tabnom,kodtb,kolrd,\
  kolrh,kolkd,0,0,0,0,0,0,mkolrd,mkolrh,"",0,0,data->window);

 }
iceb_pbar(data->bar,work.st_size,work.st_size);    

fclose(ff);
fclose(ffprom);
unlink(data->imaf_imp);

rename(imafprom,data->imaf_imp);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);



//repl.new_plus(gettext("Загрузка завершена"));
//repl.ps_plus(gettext("Количество документов"));
//repl.plus(":");
//repl.plus(koldok);

//iceb_menu_soob(&repl,data->window);


data->voz=0;

return(FALSE);
}
