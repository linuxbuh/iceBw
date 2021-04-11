/*$Id: upl_plc_r.c,v 1.14 2013/09/26 09:46:58 sasa Exp $*/
/*23.05.2016	30.03.2008	Белых А.И.	upl_plc_r.c
Проверка логической целосности для подсистемы "Учёт путевых листов"
*/
#include <errno.h>
#include "buhg_g.h"

class upl_plc_r_data
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

  upl_plc_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   upl_plc_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_plc_r_data *data);
gint upl_plc_r1(class upl_plc_r_data *data);
void  upl_plc_r_v_knopka(GtkWidget *widget,class upl_plc_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int upl_plc_r(GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class upl_plc_r_data data;
SQL_str row;
class SQLCURSOR cur;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

/*проверяем полномочия оператора*/
sprintf(strsql,"select krb,krn from icebuser where login='%s'",iceb_u_getlogin());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s %s %s icebuser",gettext("Не найден логин"),iceb_u_getlogin(),gettext("в таблице"));
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
if(atoi(row[0]) != 0 || atoi(row[1]) != 0)
 {
  iceb_menu_soob(gettext("У вас нет полномочий для этой операции!"),wpredok);
  return(1);
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Проверка логической целосности базы данных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(upl_plc_r_key_press),&data);

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

repl.plus(gettext("Проверка логической целосности базы данных"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,wpredok));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(upl_plc_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)upl_plc_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upl_plc_r_v_knopka(GtkWidget *widget,class upl_plc_r_data *data)
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

gboolean   upl_plc_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_plc_r_data *data)
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

gint upl_plc_r1(class upl_plc_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;

FILE *ff;
char imaf[56];

sprintf(imaf,"upl_plc%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"%s Upldok1\n",gettext("Проверяем записи в таблице"));
iceb_printw(strsql,data->buffer,data->view);

fprintf(ff,"%s\n",gettext("Проверка логической целосности базы данных"));
fprintf(ff,"%s Upldok1\n",gettext("Проверяем записи в таблице"));

sprintf(strsql,"select datd,kp,nomd from Upldok1");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
 }

sprintf(strsql,"%s %d\n",gettext("Количество записей в таблице"),kolstr);
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff,"%s %d\n",gettext("Количество записей в таблице"),kolstr);

int koloh=0;
float kolstr1=0;
SQL_str row1;
class SQLCURSOR cur1;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select kp from Upldok where datd='%s' and kp=%s and nomd='%s'",
  row[0],row[1],row[2]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    sprintf(strsql,"%s %-3s %s\n",row[0],row[1],row[2]);

    iceb_printw(strsql,data->buffer,data->view);

    fprintf(ff,"%s %-3s %s\n",row[0],row[1],row[2]);
    koloh++;
    sprintf(strsql,"delete from Upldok1 where datd='%s' and kp=%s and nomd='%s'",
    row[0],row[1],row[2]);
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  
 }

sprintf(strsql,"\n%s Upldok2a\n",gettext("Проверяем записи в таблице"));
iceb_printw(strsql,data->buffer,data->view);

fprintf(ff,"\n%s Upldok2a\n",gettext("Проверяем записи в таблице"));

sprintf(strsql,"select datd,kp,nomd from Upldok2a");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
 }
sprintf(strsql,"%s %d\n",gettext("Количество записей в таблице"),kolstr);
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff,"%s %d\n",gettext("Количество записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select kp from Upldok where datd='%s' and kp=%s and nomd='%s'",
  row[0],row[1],row[2]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    sprintf(strsql,"%s %-3s %s\n",row[0],row[1],row[2]);

    iceb_printw(strsql,data->buffer,data->view);
    fprintf(ff,"%s %-3s %s\n",row[0],row[1],row[2]);
    koloh++;

    sprintf(strsql,"delete from Upldok2a where datd='%s' and kp=%s and nomd='%s'",
    row[0],row[1],row[2]);
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  
 }

sprintf(strsql,"\n%s Upldok3\n",gettext("Проверяем записи в таблице"));
iceb_printw(strsql,data->buffer,data->view);

fprintf(ff,"\n%s Upldok3\n",gettext("Проверяем записи в таблице"));

sprintf(strsql,"select datd,kp,nomd from Upldok3");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
 }
sprintf(strsql,"%s %d\n",gettext("Количество записей в таблице"),kolstr);
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff,"%s %d\n",gettext("Количество записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select kp from Upldok where datd='%s' and kp=%s and nomd='%s'",
  row[0],row[1],row[2]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    sprintf(strsql,"%s %-3s %s\n",row[0],row[1],row[2]);

    iceb_printw(strsql,data->buffer,data->view);
    fprintf(ff,"%s %-3s %s\n",row[0],row[1],row[2]);
    koloh++;

    sprintf(strsql,"delete from Upldok3 where datd='%s' and kp=%s and nomd='%s'",
    row[0],row[1],row[2]);
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  
 }

fclose(ff);

iceb_printw_vr(vremn,data->buffer,data->view);

if(koloh == 0)
 {
  
  iceb_menu_soob(gettext("Порядок, ошибок не обнаружено!"),data->window);
  unlink(imaf);
 }
else
 {
  sprintf(strsql,"%s: %d",gettext("Количество исправленых ошибок"),koloh);
  repl.new_plus(strsql);
  
  sprintf(strsql,"%s %s",gettext("Распечатку выгружено в файл"),imaf);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,data->window);
  
  class iceb_u_spisok imafil;
  class iceb_u_spisok naimf;
  imafil.plus(imaf);
  naimf.plus(gettext("Протокол ошибок"));
  iceb_rabfil(&imafil,&naimf,data->window);
 }




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);


data->voz=0;
return(FALSE);

}









