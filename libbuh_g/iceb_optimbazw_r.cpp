/*$Id: iceb_optimbazw_r.c,v 1.13 2013/09/26 09:47:04 sasa Exp $*/
/*23.05.2016	09.05.2005	Белых А.И. 	iceb_optimbazw_r.c
Оптимизазия базы данных
*/
#include "iceb_libbuh.h"

class iceb_optimbazw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  class iceb_u_str imabaz;
  iceb_optimbazw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   iceb_optimbazw_r_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_optimbazw_r_data *data);
gint iceb_optimbazw_r1(class iceb_optimbazw_r_data *data);
void  iceb_optimbazw_r_v_knopka(GtkWidget *widget,class iceb_optimbazw_r_data *data);


extern SQL_baza bd;
extern iceb_u_str shrift_ravnohir;
extern double	nds1;
extern int kol_strok_na_liste;

int iceb_optimbazw_r(const char *imabaz,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class iceb_optimbazw_r_data data;
int gor=0,ver=0;

data.imabaz.new_plus(imabaz); 
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Оптимизация базы данных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_optimbazw_r_key_press),&data);

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

repl.plus(gettext("Оптимизация базы данных"));
sprintf(strsql,"%s:%s\n",gettext("База данных"),data.imabaz.ravno());
repl.ps_plus(strsql);

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(iceb_optimbazw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)iceb_optimbazw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_optimbazw_r_v_knopka(GtkWidget *widget,class iceb_optimbazw_r_data *data)
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

gboolean   iceb_optimbazw_r_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_optimbazw_r_data *data)
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

gint iceb_optimbazw_r1(class iceb_optimbazw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];

SQL_str row,row1;
SQLCURSOR cur,cur1;

strcpy(strsql,"select VERSION()");
iceb_sql_readkey(strsql,&row,&cur,data->window);

int metkazap=0;
if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace",NULL); /*назначаем шрифт*/

sprintf(strsql,"VERSION:%s\n",row[0]);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");


sprintf(strsql,"SHOW TABLES FROM %s",data->imabaz.ravno());
float kolstr1=0;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  goto vper;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  goto vper;
 }

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);

  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table %s",row[0]);
  if(metkazap == 1)
   {
    sprintf(strsql,"%s %s !\n",gettext("Оптимизация таблицы"),row[0]);
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
    if(sql_zap(&bd,strsql) < 0)
      iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
   }
  else
   {
//    iceb_sql_readkey(strsql,data->window);
    if(cur1.make_cursor(&bd,strsql) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
      continue;
     }

    while(cur1.read_cursor(&row1) != 0)
     {
      sprintf(strsql,"%-20s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
      iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
   
     }
   }


 }

sprintf(strsql,"\n%s:%d\n",gettext("Количество таблиц"),kolstr);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"\n%s.\n",gettext("Оптимизация базы закончена"));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");


vper:;
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,gettext("Расчёт закончен"));
//gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));
gtk_label_set_text(GTK_LABEL(data->label),strsql);

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);


data->voz=0;

return(FALSE);

}
