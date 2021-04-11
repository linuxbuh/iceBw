/*$Id: saldouplw_r.c,v 1.14 2013/09/26 09:46:55 sasa Exp $*/
/*23.05.2016	27.02.2009	Белых А.И.	saldouplw_r.c
Расчёт отчёта 
*/
#include "buhg_g.h"

class saldouplw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  short godk;
  short godn;  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  saldouplw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   saldouplw_r_key_press(GtkWidget *widget,GdkEventKey *event,class saldouplw_r_data *data);
gint saldouplw_r1(class saldouplw_r_data *data);
void  saldouplw_r_v_knopka(GtkWidget *widget,class saldouplw_r_data *data);


extern SQL_baza bd;
extern short startgodupl; //Стартовый год для "Учета путевых листов"

int saldouplw_r(short godn,short godk,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class saldouplw_r_data data;

data.godk=godk;
data.godn=godn;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Перенос сальдо для подсистемы \"Учёт путевых листов\""));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(saldouplw_r_key_press),&data);

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

repl.plus(gettext("Перенос сальдо для подсистемы \"Учёт путевых листов\""));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(saldouplw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)saldouplw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldouplw_r_v_knopka(GtkWidget *widget,class saldouplw_r_data *data)
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

gboolean   saldouplw_r_key_press(GtkWidget *widget,GdkEventKey *event,class saldouplw_r_data *data)
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


gint saldouplw_r1(class saldouplw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
SQLCURSOR cur,cur1;





short godk=data->godk;
short godn=startgodupl=data->godn; /*год начала может быть введён вручную а в подпрограммах эта дата используется для вычисления сальдо*/


int kolstr=0;

class iceb_u_spisok kv_ka;//Список кодов водителей-кодов автомобилей 

//Сальдо читаем так как могут быть остатки по которым небыло путевых листов
sprintf(strsql,"select distinct kv,ka from Uplsal where god=%d",godn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[0],row[1]);
  kv_ka.plus(strsql);
 }

//Узнаём все комбинации код водителя-код автомобиля
sprintf(strsql,"select distinct kv,ka from Upldok1 where datd >= '%d-01-01' \
and datd <= '%d-12-31'",godn,godk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  sprintf(strsql,"%s|%s",row[0],row[1]);
  if(kv_ka.find(strsql) < 0)
    kv_ka.plus(strsql);

 }



//Удаляем сальдо
sprintf(strsql,"delete from Uplsal where god=%d",godk);
iceb_sql_zapis(strsql,0,0,data->window);

char kodvod[64];
char kodavt[64];
//while(cur.read_cursor(&row) != 0)
kolstr1=0; 
kolstr=kv_ka.kolih();
class iceb_u_str fio("");
for(int in=0; in < kv_ka.kolih(); in++)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  iceb_u_polen(kv_ka.ravno(in),kodvod,sizeof(kodvod),1,'|');
  iceb_u_polen(kv_ka.ravno(in),kodavt,sizeof(kodavt),2,'|');
  
  class iceb_u_spisok marka_topl;
  class iceb_u_double ost;
  if(ostvaw(1,1,godk,kodvod,kodavt,0,&marka_topl,&ost,NULL,data->window) == 0.)
   continue;
  
  for(int ii=0; ii < marka_topl.kolih(); ii++)
   {
    if(ost.ravno(ii) == 0.)
     continue;

    fio.new_plus("");
    sprintf(strsql,"select naik from Uplouot where kod=%d",atoi(kodvod));
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     fio.new_plus(row1[0]);
          
    sprintf(strsql,"%d %-3s %-3s %-*.*s %-*s %10.3f\n",
    godk,kodavt,
    kodvod,
    iceb_u_kolbait(20,fio.ravno()),
    iceb_u_kolbait(20,fio.ravno()),
    fio.ravno(),
    iceb_u_kolbait(5,marka_topl.ravno(ii)),marka_topl.ravno(ii),ost.ravno(ii));

    iceb_printw(strsql,data->buffer,data->view);
    
    sprintf(strsql,"insert into Uplsal values (%d,%s,%s,'%s',%.2f,%d,%ld)",
    godk,kodavt,kodvod,marka_topl.ravno(ii),ost.ravno(ii),iceb_getuid(data->window),vremn);

//    iceb_printw(strsql,data->buffer,data->view);
//    iceb_printw("\n",data->buffer,data->view);

    iceb_sql_zapis(strsql,1,0,data->window);
    
   }
 }

short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
if(data->godk <= gt)
 {
  sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='uplnast.alx' and str like 'Стартовый год|%d%%'",
  data->godk,data->godn);

  if(iceb_sql_zapis(strsql,1,0,data->window) == 0)
   startgodupl=data->godk;
 }

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
