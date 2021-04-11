/*$Id: plc_mu_r.c,v 1.21 2013/09/26 09:46:52 sasa Exp $*/
/*27.02.2017	18.11.2004	Белых А.И.	plc_mu_r.c
Проверка логической целосности базы материального учёта
*/
#include <errno.h>
#include "buhg_g.h"

class plc_mu_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
    
  int metka; //0-только проверка 1-проверка с удалением
  
  short kon_ras; //0-расчёт завершен 1-нет
  class iceb_u_str imaf_p;
  int kolerror;
  plc_mu_r_data()
   {
    kolerror=0;
    kon_ras=1;
   }
 };
gboolean   plc_mu_r_key_press(GtkWidget *widget,GdkEventKey *event,class plc_mu_r_data *data);
gint plc_mu_r1(class plc_mu_r_data *data);
void  plc_mu_r_v_knopka(GtkWidget *widget,class plc_mu_r_data *data);

extern SQL_baza bd;

int plc_mu_r(int metka,class iceb_u_str *imaf_p,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class plc_mu_r_data data;

data.metka=metka;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Проверка логической целосности базы"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(plc_mu_r_key_press),&data);

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

repl.plus(gettext("Проверка логической целосности базы"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(plc_mu_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)plc_mu_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

imaf_p->new_plus(data.imaf_p.ravno());

return(data.kolerror);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  plc_mu_r_v_knopka(GtkWidget *widget,class plc_mu_r_data *data)
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

gboolean   plc_mu_r_key_press(GtkWidget *widget,GdkEventKey *event,class plc_mu_r_data *data)
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

gint plc_mu_r1(class plc_mu_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_u_str repl;
iceb_clock sss(data->window);


sprintf(strsql,"matulk%d.lst",getpid());
data->imaf_p.new_plus(strsql);
FILE *ff;

if((ff = fopen(data->imaf_p.ravno(),"w")) == NULL)
 {
  iceb_er_op_fil(data->imaf_p.ravno(),"",errno,data->window);
  return(FALSE);
 }


iceb_zagolov(gettext("Проверка логической целосности базы данных"),0,0,0,0,0,0,ff,data->window);



fprintf(ff,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,data->window));


fprintf(ff,"\
--------------------------------------\n");
fprintf(ff,"%s\n",
gettext("  Дата    |Склад|+/-|Номер документа"));
fprintf(ff,"\
--------------------------------------\n");


sprintf(strsql,"\
--------------------------------------\n");
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s\n",gettext("  Дата    |Склад|+/-|Номер документа"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"\
--------------------------------------\n");
iceb_printw(strsql,data->buffer,data->view);

SQLCURSOR cur;
SQL_str row;
SQLCURSOR cur1;
SQL_str row1;
int kolstr;
float kolstr1=0;

sprintf(strsql,"%s Dokummat1\n",gettext("Проверка таблицы"));
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff,"%s\n",strsql);
fprintf(ff,"%s\n",
gettext("Проверка записей в документах не имеющих шапки документа"));


sprintf(strsql,"select distinct datd,sklad,nomd,tipz,nomkar from Dokummat1 \
order by datd asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);

sprintf(strsql,"%s %d\n",gettext("Количество записей"),kolstr);
iceb_printw(strsql,data->buffer,data->view);

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {

  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select tip from Dokummat where tip=%s and \
datd='%s' and sklad=%s and nomd='%s'",row[3],row[0],row[1],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    sprintf(strsql,"%s %-5s %-3s %s\n",iceb_u_datzap(row[0]),row[1],row[3],row[2]);
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",iceb_u_datzap(row[0]),row[1],row[3],row[2]);
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Dokummat1 where datd='%s' and sklad=%s and nomd='%s' and \
tipz=%s and nomkar=%s",row[0],row[1],row[2],row[3],row[4]);
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }
  if(row[4][0] == '0')
   continue;
  sprintf(strsql,"select nomk from Kart where sklad=%s and \
nomk=%s",row[1],row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    sprintf(strsql,"%s %-5s %-3s %s\n",iceb_u_datzap(row[0]),row[1],row[3],row[2]);
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql,"%s %s %s %s !\n",
    gettext("Не найдена карточка"),row[4],
    gettext("на складе"),row[3]);
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",iceb_u_datzap(row[0]),row[1],row[3],row[2]);
    fprintf(ff,"%s %s %s %s !",
    gettext("Не найдена карточка"),row[4],
    gettext("на складе"),row[3]);
    data->kolerror++;
   }
 }

sprintf(strsql,"%s Dokummat2\n",gettext("Проверка таблицы"));

iceb_printw(strsql,data->buffer,data->view);
fprintf(ff,"%s\n",strsql);


//sprintf(strsql,"select distinct god,sklad,nomd from Dokummat2 order by god asc"); с таким запросом выдаёт мусор
sprintf(strsql,"select god,sklad,nomd from Dokummat2 order by god asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);


sprintf(strsql,"%s %d\n",gettext("Количество записей"),kolstr);
iceb_printw(strsql,data->buffer,data->view);

kolstr1=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
//  sprintf(stsql,"%s %-5s %-3s %s\n",row[0],row[1],row[3],row[2]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select tip from Dokummat where  \
datd >= '%s-01-01' and datd <= '%s-12-31'and sklad=%s and nomd='%s'",row[0],row[0],row[1],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    iceb_beep();
    sprintf(strsql,"%10s %-5s %-3s %s\n",row[0],row[1],"",row[2]);
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",row[0],row[1],"",row[2]);
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Dokummat2 where god='%s' and sklad=%s and nomd='%s'",
      row[0],row[1],row[2]);
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }
 }

sprintf(strsql,"%s Dokummat3\n",gettext("Проверка таблицы"));
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff,"%s\n",strsql);


sprintf(strsql,"select distinct datd,sklad,nomd from Dokummat3 order by datd asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);


sprintf(strsql,"%s %d\n",gettext("Количество записей"),kolstr);
iceb_printw(strsql,data->buffer,data->view);

kolstr1=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
//  sprintf(stsql,"%s %-5s %-3s %s\n",row[0],row[1],row[3],row[2]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select tip from Dokummat where \
datd='%s' and sklad=%s and nomd='%s'",row[0],row[1],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    iceb_beep();
    sprintf(strsql,"%s %-5s %-3s %s\n",iceb_u_datzap(row[0]),row[1],"",row[2]);
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",iceb_u_datzap(row[0]),row[1],"",row[2]);
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Dokummat3 where datd='%s' and sklad=%s and nomd='%s'",
      row[0],row[1],row[2]);
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }
 }



sprintf(strsql,"\n%s Dokummat4\n",gettext("Проверка таблицы"));
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff,"%s Dokummat4\n",gettext("Проверка таблицы"));

fprintf(ff,"\
--------------------------------------\n");
fprintf(ff,"%s\n",
gettext("  Дата    |Склад|Номер документа"));
fprintf(ff,"\
--------------------------------------\n");


sprintf(strsql,"select distinct datd,skl,nomd from Dokummat4 order by datd asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);

sprintf(strsql,"%s %d\n",gettext("Количество записей"),kolstr);
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select tip from Dokummat where tip=2 and datd='%s' and sklad=%s and nomd='%s'",row[0],row[1],row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) <= 0)
   {
    iceb_beep();
    sprintf(strsql,"%s %-5s %s\n",iceb_u_datzap(row[0]),row[1],row[2]);
    iceb_printw(strsql,data->buffer,data->view);
    fprintf(ff,"%s %-5s %s\n",iceb_u_datzap(row[0]),row[1],row[2]);
    fprintf(ff,"%s !\n",gettext("Не найдена шапка документа"));

    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Dokummat4 where datd='%s' and skl=%s and nomd='%s'",row[0],row[1],row[2]);
      iceb_sql_zapis(strsql,1,0,data->window);
     }
   }

 }


sprintf(strsql,"\n%s Zkart\n",gettext("Проверка таблицы"));
iceb_printw(strsql,data->buffer,data->view);

fprintf(ff,"\n%s Zkart\n",gettext("Проверка таблицы"));

sprintf(strsql,"select distinct sklad,nomk,nomd,datd,tipz from Zkart \
order by datd asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);

sprintf(strsql,"%s %d\n",gettext("Количество записей"),kolstr);
iceb_printw(strsql,data->buffer,data->view);

kolstr1=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
//  wprintw(win1,"%s %-5s %-3s %s\n",row[3],row[0],row[4],row[2]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select tip from Dokummat where tip=%s and \
datd='%s' and sklad=%s and nomd='%s'",row[4],row[3],row[0],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    iceb_beep();
    sprintf(strsql,"%s %-5s %-3s %s\n",iceb_u_datzap(row[3]),row[0],row[4],row[2]);
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql,"%s\n",gettext("Не найдена шапка документа !"));
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",iceb_u_datzap(row[3]),row[0],row[4],row[2]);
    fprintf(ff,"%s !\n",gettext("Не найдена шапка документа"));
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Zkart where datd='%s' and sklad=%s and nomd='%s' and \
tipz=%s and nomk=%s",row[3],row[0],row[2],row[4],row[1]);
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }

  sprintf(strsql,"select tipz from Dokummat1 where tipz=%s and \
datd='%s' and sklad=%s and nomd='%s'",row[4],row[3],row[0],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    iceb_beep();
    sprintf(strsql,"%s %-5s %-3s %s\n",iceb_u_datzap(row[3]),row[0],row[4],row[2]);
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql,"%s !\n",gettext("Не найдена запись в документе"));
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",iceb_u_datzap(row[3]),row[0],row[4],row[2]);
    fprintf(ff,"%s !\n",gettext("Не найдена запись в документе"));
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Zkart where datd='%s' and sklad=%s and nomd='%s' and \
tipz=%s and nomk=%s",row[3],row[0],row[2],row[4],row[1]);
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }
  sprintf(strsql,"select sklad from Kart where sklad=%s and nomk=%s",
  row[0],row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    iceb_beep();
    sprintf(strsql,"%s %-5s %-3s %s\n",iceb_u_datzap(row[3]),row[0],row[4],row[2]);
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql,"%s !\n",gettext("Не найдена карточка"));
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(ff,"%s %-5s %-3s %s\n",iceb_u_datzap(row[3]),row[0],row[4],row[2]);
    fprintf(ff,"%s !\n",gettext("Не найдена карточка"));
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Zkart where datd='%s' and sklad=%s and nomd='%s' and \
tipz=%s and nomk=%s",row[3],row[0],row[2],row[4],row[1]);
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }
 }

sprintf(strsql,"\n%s Kart\n",gettext("Проверка таблицы"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"\
--------------------\n");
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s\n",
gettext("Склад|Номер карточки"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"\
--------------------\n");
iceb_printw(strsql,data->buffer,data->view);


fprintf(ff,"\n%s Kart\n",gettext("Проверка таблицы"));

fprintf(ff,"\
--------------------\n");

fprintf(ff,"%s\n",
gettext("Склад|Номер карточки"));

fprintf(ff,"\
--------------------\n");


sprintf(strsql,"select sklad,nomk from Kart \
order by sklad asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);

sprintf(strsql,"%s %d\n",gettext("Количество записей"),kolstr);
iceb_printw(strsql,data->buffer,data->view);


kolstr1=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
//  wprintw(win1,"%-5s %s\n",row[0],row[1]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select tipz from Dokummat1 where \
sklad=%s and nomkar=%s limit 1",row[0],row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    iceb_beep();
    sprintf(strsql,"%-5s %s\n",row[0],row[1]);
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(ff,"%-5s %s\n",row[0],row[1]);
    fprintf(ff,"%s\n",gettext("Не найдены записи в документах!"));
    data->kolerror++;

    if(data->metka == 1) 
     {
      sprintf(strsql,"delete from Kart where sklad=%s and nomk=%s",row[0],row[1]); 
      if(sql_zap(&bd,strsql) != 0)      
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
   }
 }

/*проверяем проводки сделанные к документам*/
sprintf(strsql,"\nПроверяем таблицу Prov\n");
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff,"\nПроверяем таблицу Prov\n");

sprintf(strsql,"select * from Prov where kto='%s' order by datp asc",ICEB_MP_MATU);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
 }

sprintf(strsql,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  /*Проверяем наличие записей в документе*/
  sprintf(strsql,"select datd from Dokummat1 where datd='%s' and sklad=%d and nomd='%s' limit 1",row[14],atoi(row[7]),row[6]);

  if(iceb_sql_readkey(strsql,data->window) == 0)
   {
    iceb_beep();
    data->kolerror++;

    fprintf(ff,"\n%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13],row[14],row[15],row[16]);
    fprintf(ff,"%s\n",strsql);
    fprintf(ff,"%s!\n",gettext("Не найдены записи в документе"));

    sprintf(strsql,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13],row[14],row[15],row[16]);
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql,"%s!\n",gettext("Не найдены записи в документе"));
    iceb_printw(strsql,data->buffer,data->view);



    if(data->metka == 1)
     {
      /*проводки парные и они будут удалены по одной так как и к парной проводке тоже не будут найдены записи в документе*/

      sprintf(strsql,"delete from Prov where kto='%s' and pod=%d and nomd='%s' and datd='%s'",
      ICEB_MP_MATU,atoi(row[7]),row[6],row[14]);
      

      iceb_sql_zapis(strsql,1,0,data->window);

     }
   }  

 }

fclose(ff);
iceb_ustpeh(data->imaf_p.ravno(),3,data->window);
iceb_printw_vr(vremn,data->buffer,data->view);



if(data->kolerror == 0)
 {
  iceb_menu_soob(gettext("Порядок, ошибок не обнаружено !"),data->window);
  unlink(data->imaf_p.ravno());
 }
else
 {
  sprintf(strsql,"%s: %d",gettext("Количество ошибок"),data->kolerror);
  repl.new_plus(strsql);
  iceb_menu_soob(&repl,data->window);
 }






data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

return(FALSE);

}
