/*$Id: iceb_dumpbazout.c,v 1.9 2014/05/07 10:58:10 sasa Exp $*/
/*19.06.2019	16.08.2012	Белых А.И.	iceb_dumpbazout.c
Выгрузка дампа базы
*/
#include <errno.h>
#include <sys/stat.h>
#include "iceb_libbuh.h"

class iceb_dumpbazout_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class iceb_u_str namebases;
  class iceb_u_str login;
  class iceb_u_str parol;
  class iceb_u_str hostname;
    
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  int prohod;
  iceb_dumpbazout_data()
   {
    namebases.plus("");
    voz=1;
    kon_ras=1;
    prohod=0;
   }
 };
gboolean   iceb_dumpbazout_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_dumpbazout_data *data);
gint iceb_dumpbazout1(class iceb_dumpbazout_data *data);
void  iceb_dumpbazout_v_knopka(GtkWidget *widget,class iceb_dumpbazout_data *data);


int iceb_dumpbazout(const char *namebases,const char *login,const char *parol,const char *hostname,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
char strsql[512];
iceb_u_str repl;
class iceb_dumpbazout_data data;
class iceb_u_spisok textmenu;
data.name_window.plus(__FUNCTION__);

sprintf(strsql,"%s %s",gettext("Выгрузка дампа базы данных"),namebases);

textmenu.plus(strsql);
textmenu.plus(gettext("Введите пароль"));

if(iceb_parol(0,&textmenu,wpredok) != 0)
 return(1);

if(namebases[0] != '\0')
  data.namebases.new_plus(namebases);
data.login.new_plus(login);
data.parol.new_plus(parol);
data.hostname.new_plus(hostname);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
 gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",__FUNCTION__,gettext("Выгрузка дампа базы"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_dumpbazout_key_press),&data);

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

repl.plus(gettext("Выгрузка дампа базы данных в файл"));

if(namebases[0] != '\0')
  sprintf(strsql,"%s:%s",gettext("База данных"),namebases);
else
  sprintf(strsql,"%s",gettext("Выгрузка всех баз данных"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(iceb_dumpbazout_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);



g_idle_add((GSourceFunc)iceb_dumpbazout1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_dumpbazout_v_knopka(GtkWidget *widget,class iceb_dumpbazout_data *data)
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

gboolean   iceb_dumpbazout_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_dumpbazout_data *data)
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

gint iceb_dumpbazout1(class iceb_dumpbazout_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_clock sss(data->window);
int kolih_bar=6;
float kolih=0.;

iceb_refresh();
sleep(1);

if(data->prohod  == 0) /*Для того чтобы отрисовалось окно программы*/
 {
  data->prohod++;
  return(TRUE);
 }
iceb_pbar(data->bar,kolih_bar,++kolih);    
iceb_refresh();
class iceb_u_str ima_arh("");

class iceb_u_str put_na_nastr(g_get_home_dir());
put_na_nastr.plus("/.iceB/lno.alx");

iceb_u_poldan("Путь на каталог для выгрузки дампа базы",&ima_arh,put_na_nastr.ravno());

if(ima_arh.ravno()[0] == '\0')
 ima_arh.new_plus(iceb_u_getcurrentdir());

ima_arh.plus("/mysqlbaza_");


if(data->namebases.ravno()[0] != '\0')
 {
  ima_arh.plus(data->namebases.ravno());


  if(data->prohod  == 1) /*Для того чтобы отрисовалось окно программы*/
   {
    sprintf(strsql,"%s %s %s %s\n",gettext("Выгружаем дамп базы данных"),data->namebases.ravno(),gettext("в файл"),ima_arh.ravno());
    
    iceb_printw(strsql,data->buffer,data->view);
    data->prohod++;
    return(TRUE);
   }

  sprintf(strsql,"mysqldump -u %s -p%s --databases %s --lock-tables -h %s > %s",
  data->login.ravno(),data->parol.ravno(),data->namebases.ravno(),data->hostname.ravno(),ima_arh.ravno());
 }
else
 {

  ima_arh.plus("all");


  if(data->prohod  == 1) /*Для того чтобы отрисовалось окно программы*/
   {
    sprintf(strsql,"%s %s %s\n",gettext("Выгружаем дампы всех баз данных"),gettext("в файл"),ima_arh.ravno());
    iceb_printw(strsql,data->buffer,data->view);
    data->prohod++;
    return(TRUE);
   }

  sprintf(strsql,"mysqldump -u %s -p%s --all-databases --lock-tables -h %s > %s",
  data->login.ravno(),data->parol.ravno(),data->hostname.ravno(),ima_arh.ravno());
 }
iceb_refresh();

FILE *ff;
if((ff = fopen(ima_arh.ravno(),"w")) == NULL)
 {
  iceb_er_op_fil(ima_arh.ravno(),"",errno,data->window);
  return(FALSE);
 }
fclose(ff);
unlink(ima_arh.ravno());

iceb_pbar(data->bar,kolih_bar,++kolih);    
iceb_refresh();

if(system(strsql) != 0)
 {
  char bros[2048];
  sprintf(bros,"%s\n%s",gettext("Ошибка при выполнении команды"),strsql);
  iceb_menu_soob(bros,data->window);
  
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_pbar(data->bar,kolih_bar,++kolih);    

struct stat bufstat;
char razmerfil[128];
memset(razmerfil,'\0',sizeof(razmerfil));

memset(razmerfil,'\0',sizeof(razmerfil));
if(stat(ima_arh.ravno(),&bufstat) == 0)
 {
  if(bufstat.st_size > 1000000)
   {
    double razm=bufstat.st_size/1000000.;
    razm=iceb_u_okrug(razm,0.01);
    sprintf(razmerfil,"%.2f %s",razm,gettext("мегабайт"));
   }
  else
   sprintf(razmerfil,"%ld %s",bufstat.st_size,gettext("байт"));
 }    

iceb_pbar(data->bar,kolih_bar,++kolih);    

sprintf(strsql,"%s %s %s\n",gettext("Размер файла дампа"),ima_arh.ravno(),razmerfil);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s %s %s bzip2\n",gettext("Сжимаем файл"),ima_arh.ravno(),gettext("программой"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"bzip2 -f %s",ima_arh.ravno());
class iceb_u_str ima_arh_bz2(ima_arh.ravno());
int metka_zip=0;
if(system(strsql) == 0)
 {
  metka_zip=1;   
  sprintf(strsql,"\n%s %s.bz2\n",gettext("Дамп базы выгружен в файл"),ima_arh.ravno());
  iceb_printw(strsql,data->buffer,data->view);
  ima_arh_bz2.plus(".bz2");
 }
else
 {
  sprintf(strsql,"\n%s %s\n",gettext("Дамп базы выгружен в файл"),ima_arh.ravno());
  iceb_printw(strsql,data->buffer,data->view);
 }

iceb_pbar(data->bar,kolih_bar,++kolih);    

if(metka_zip == 1)
 {
  memset(razmerfil,'\0',sizeof(razmerfil));
  if(stat(ima_arh_bz2.ravno(),&bufstat) == 0)
   {
    if(bufstat.st_size > 1000000)
     {
      double razm=bufstat.st_size/1000000.;
      razm=iceb_u_okrug(razm,0.01);
      sprintf(razmerfil,"%.2f %s",razm,gettext("мегабайт"));
     }
    else
     sprintf(razmerfil,"%ld %s",bufstat.st_size,gettext("байт"));
   }    
 }

iceb_pbar(data->bar,kolih_bar,++kolih);    

sprintf(strsql,"%s %s\n",gettext("Размер файла"),razmerfil);
iceb_printw(strsql,data->buffer,data->view);









iceb_beep();


gtk_label_set_text(GTK_LABEL(data->label),gettext("Выгрузка закончена"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
