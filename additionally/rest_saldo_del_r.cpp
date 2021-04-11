/*$Id: rest_saldo_del_r.c,v 1.9 2013/08/13 06:10:25 sasa Exp $*/
/*25.05.2016	26.12.2004	Белых А.И.	rest_saldo_del_r.c
Удаление даных за год в подсистеме выписки счетов
*/
#include "i_rest.h"

class rest_saldo_del_r_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  iceb_u_str god;
  
  rest_saldo_del_r_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rest_saldo_del_r_r_key_press(GtkWidget *widget,GdkEventKey *event,class rest_saldo_del_r_r_data *data);
gint rest_saldo_del_r_r1(class rest_saldo_del_r_r_data *data);
void  rest_saldo_del_r_r_v_knopka(GtkWidget *widget,class rest_saldo_del_r_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *imabaz;
extern short start_god_rest;

int rest_saldo_del_r(GtkWidget *wpredok)
{
char strsql[500];
iceb_u_str repl;
class rest_saldo_del_r_r_data data;


repl.plus(gettext("Введите год, за который нужно удалить данные"));
repl.ps_plus(gettext("Внимание !!! Перед удалением обязательно должно быть перенесено сальдо !!!"));

if(iceb_menu_vvod1(&repl,&data.god,5,"",wpredok) != 0)
 return(1); 
sprintf(strsql,gettext("Удалить данные за %d год ? Вы уверены ?"),data.god.ravno_atoi());



if(iceb_menu_danet(strsql,2,wpredok) == 2)
 return(1);

if(iceb_parol(0,wpredok) != 0)
 return(1);



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",name_system,gettext("Удаление данных за год"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rest_saldo_del_r_r_key_press),&data);

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

repl.new_plus(gettext("Удаление данных за год"));
repl.plus(" ");
repl.plus(data.god.ravno());

sprintf(strsql,"%s:%s %s",gettext("База данных"),imabaz,iceb_get_pnk("00",0,NULL));
repl.ps_plus(strsql);
repl.ps_plus(gettext("Стартовый год"));
repl.plus(":");
repl.plus(start_god_rest);

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



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне."));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rest_saldo_del_r_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rest_saldo_del_r_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rest_saldo_del_r_r_v_knopka(GtkWidget *widget,class rest_saldo_del_r_r_data *data)
{
printf("rest_saldo_del_r_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rest_saldo_del_r_r_key_press(GtkWidget *widget,GdkEventKey *event,class rest_saldo_del_r_r_data *data)
{
 printf("rest_saldo_del_r_r_key_press\n");
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

gint rest_saldo_del_r_r1(class rest_saldo_del_r_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1000];
iceb_u_str repl;
iceb_clock sss(data->window);

int kolstr=3;
float kolstr1=0.;

sprintf(strsql,"%s\n",gettext("Удаляем записи по кассам"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Restkas where datz >= '%04d-01-01' and \
datz <= '%04d-12-31'",data->god.ravno_atoi(),data->god.ravno_atoi());

iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"%s\n",gettext("Удаляем счета"));
iceb_printw(strsql,data->buffer,data->view);
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select datd,nomd from Restdok where god=%d and mo=1",data->god.ravno_atoi());
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  restdok_uddok(d,m,g,row[1],data->window);
    
 }

//Удаляем шапки документов
sprintf(strsql,"delete from Restdok where god=%d and mo=1",data->god.ravno_atoi());
iceb_sql_zapis(strsql,1,0,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен."));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
