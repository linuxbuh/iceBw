/*$Id: uddusl_r.c,v 1.15 2013/08/25 08:26:56 sasa Exp $*/
/*23.05.2016	19.10.2004	Белых А.И.	uddusl_r.c
удаление документов в подсистеме "Учёт услуг"
*/
#include "buhg_g.h"

class uddusl_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  short prkk;

  iceb_u_str god;
  uddusl_r_data()
   {
    god.plus("");
   }

 };

gboolean   uddusl_r_key_press(GtkWidget *widget,GdkEventKey *event,class uddusl_r_data *data);
gint uddusl_r1(class uddusl_r_data *data);
void  uddusl_r_v_knopka(GtkWidget *widget,class uddusl_r_data *data);

extern SQL_baza bd;
 
void uddusl_r(GtkWidget *wpredok)
{
char strsql[512];
SQL_str		row;
class SQLCURSOR       cur;

class iceb_u_spisok repl_s;
class uddusl_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);




/*проверяем полномочия оператора*/
sprintf(strsql,"select uub,uun from icebuser where login='%s'",iceb_u_getlogin());
if(iceb_sql_readkey(strsql,&row,&cur,NULL) != 1)
 {
  sprintf(strsql,"%s %s %s icebuser",gettext("Не найден логин"),iceb_u_getlogin(),gettext("в таблице"));
  iceb_menu_soob(strsql,NULL);
  return;
 }
if(atoi(row[0]) != 0 || atoi(row[1]) != 0)
 {
  iceb_menu_soob(gettext("У вас нет полномочий для этой операции!"),NULL);
  return;
 }


repl_s.plus(gettext("Удаление документов за год"));
repl_s.plus(gettext("Введите год"));
if(iceb_menu_vvod1(&repl_s,&data.god,5,"",wpredok) != 0)
 return;

sprintf(strsql,gettext("Удалить документы за %d год ? Вы уверены ?"),data.god.ravno_atoi());

if(iceb_menu_danet(strsql,2,wpredok) == 2)
 return;
 
if(iceb_parol(0,wpredok) != 0)
 return;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Копировать записи из других документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uddusl_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Копировать записи из других документов"));
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


gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(uddusl_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)uddusl_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uddusl_r_v_knopka(GtkWidget *widget,class uddusl_r_data *data)
{
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uddusl_r_key_press(GtkWidget *widget,GdkEventKey *event,class uddusl_r_data *data)
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

gint uddusl_r1(class uddusl_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];

sprintf(strsql,"%s %d.\n",gettext("Удаление всех документов за год"),data->god.ravno_atoi());

sprintf(strsql,"%s.\n",gettext("Удаление подтверждающих записей"));
iceb_printw(strsql,data->buffer,data->view);

int		metkazap=0;
SQL_str row;
SQLCURSOR cur;

strcpy(strsql,"select VERSION()");
iceb_sql_readkey(strsql,&row,&cur,data->window);

if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  

sprintf(strsql,"VERSION:%s\n",row[0]);
iceb_printw(strsql,data->buffer,data->view);

int god=data->god.ravno_atoi();

sprintf(strsql,"delete from Usldokum2 where datd >= '%d-1-1' and \
datd <= '%d-12-31'",god,god);

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 
int kolud=sql_rows(&bd);
int ikolud=kolud;
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(strsql,data->buffer,data->view);
if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);

  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Usldokum2");
  if(metkazap == 1)
    iceb_sql_zapis(strsql,1,0,data->window);
  else
    iceb_sql_readkey(strsql,data->window);
 }


sprintf(strsql,"%s.\n",gettext("Удаление приложений к документам"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Usldokum3 where god=%d",god);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);
ikolud+=kolud;
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(strsql,data->buffer,data->view);
if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);

  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Usldokum3");
  if(metkazap == 1)
    iceb_sql_zapis(strsql,1,0,data->window);
  else
    iceb_sql_readkey(strsql,data->window);
 }


sprintf(strsql,"%s.\n",gettext("Удаление записей в документах"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Usldokum1 where datd >= '%d-1-1' and \
datd <= '%d-12-31'",god,god);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);
ikolud+=kolud;
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(strsql,data->buffer,data->view);
if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Usldokum1");
  if(metkazap == 1)
    iceb_sql_zapis(strsql,1,0,data->window);
  else
    iceb_sql_readkey(strsql,data->window);
 }

sprintf(strsql,"%s.\n",gettext("Удаление заголовков документов"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Usldokum where datd >= '%d-1-1' and \
datd <= '%d-12-31'",god,god);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);
ikolud+=kolud;
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(strsql,data->buffer,data->view);

if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Usldokum");
  if(metkazap == 1)
    iceb_sql_zapis(strsql,1,0,data->window);
  else
    iceb_sql_readkey(strsql,data->window);
 }


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

sprintf(strsql,"%s:%d",gettext("Общее количество удалённых записей"),ikolud);
iceb_menu_soob(strsql,data->window);

return(FALSE);
}
