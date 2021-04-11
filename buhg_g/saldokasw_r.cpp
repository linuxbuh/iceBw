/*$Id: saldokasw_r.c,v 1.14 2013/09/26 09:46:55 sasa Exp $*/
/*23.05.2016	27.02.2009	Белых А.И.	saldokasw_r.c
Расчёт отчёта 
*/
#include "buhg_g.h"

class saldokasw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  short god;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  saldokasw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   saldokasw_r_key_press(GtkWidget *widget,GdkEventKey *event,class saldokasw_r_data *data);
gint saldokasw_r1(class saldokasw_r_data *data);
void  saldokasw_r_v_knopka(GtkWidget *widget,class saldokasw_r_data *data);


extern SQL_baza bd;

int saldokasw_r(short gods,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class saldokasw_r_data data;

data.god=gods;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Перенос сальдо для подсистемы \"Учёт кассовых ордеров\""));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(saldokasw_r_key_press),&data);

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

repl.plus(gettext("Перенос сальдо для подсистемы \"Учёт кассовых ордеров\""));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(saldokasw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)saldokasw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldokasw_r_v_knopka(GtkWidget *widget,class saldokasw_r_data *data)
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

gboolean   saldokasw_r_key_press(GtkWidget *widget,GdkEventKey *event,class saldokasw_r_data *data)
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


gint saldokasw_r1(class saldokasw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;






short dn=1;
short mn=1;
short gn=data->god-1;
short dk=31;
short mk=12;
short gk=data->god-1;


int kolstr=0;
SQL_str row1;

class SQLCURSOR cur1;

class iceb_u_spisok kas_kodcn; //Список код кассы-код целевого назначения
class iceb_u_double mscn; //массив сумм сальдо целевого назначения


//Читаем сальдо по кодам целевого назначения
sprintf(strsql,"select kod,ks,saldo from Kascnsl where god=%d",gn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[1],row[0]);
  if((nomer=kas_kodcn.find(strsql)) < 0)
   kas_kodcn.plus(strsql);

  mscn.plus(atof(row[2]),nomer);
 }


//читаем документы

sprintf(strsql,"select kassa,tp,datd,nomd,kodop from Kasord where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by datd,tp asc",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
float kolstr1=0;
double suma=0.,sumapod=0.;
short d,m,g;
short tipz;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  tipz=atoi(row[1]);
  //читаем код целевого назначения этой операции
  if(tipz == 1)
    sprintf(strsql,"select kcn from Kasop1 where kod='%s'",row[4]);
  if(tipz == 2)
    sprintf(strsql,"select kcn from Kasop2 where kod='%s'",row[4]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(atoi(row1[0]) == 0)
     continue;
   }
  else 
   continue;
  sprintf(strsql,"%s|%s",row[0],row1[0]);  
  if((nomer=kas_kodcn.find(strsql)) < 0)
    kas_kodcn.plus(strsql);
    
  iceb_u_rsdat(&d,&m,&g,row[2],2);
  
  sumkasorw(row[0],tipz,row[3],g,&suma,&sumapod,data->window);

  if(tipz == 2)  
    sumapod*=-1; 

  mscn.plus(sumapod,nomer);

 }

//удаляем записи если они есть

sprintf(strsql,"delete from Kascnsl where god=%d",data->god);
iceb_sql_zapis(strsql,0,0,data->window);

int kassa=0;
char kodcn[10];
time_t vrem;
time(&vrem);

for(int ii=0; ii < kas_kodcn.kolih() ; ii++)
 {
  iceb_u_polen(kas_kodcn.ravno(ii),&kassa,1,'|');
  iceb_u_polen(kas_kodcn.ravno(ii),kodcn,sizeof(kodcn),2,'|');

  sprintf(strsql,"Kacca:%d Код:%s Сумма:%.2f\n",kassa,kodcn,mscn.ravno(ii));
  iceb_printw(strsql,data->buffer,data->view);
   
  sprintf(strsql,"insert into Kascnsl values (%d,%s,%d,%.2f,%d,%ld)",
  data->god,kodcn,kassa,mscn.ravno(ii),iceb_getuid(data->window),vrem);
  iceb_sql_zapis(strsql,0,0,data->window);

   
 }

/*Создаём стартовый кассовый ордер*/

sprintf(strsql,"select kod from Kasop1 where kod='%s'",gettext("ОСТ"));
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"insert into Kasop1 (kod,naik,metkapr,ktoz,vrem) values('%s','%s',0,%d,%ld)",
  gettext("ОСТ"),
  gettext("Стартове сальдо"),
  iceb_getuid(data->window),
  time(NULL));

  iceb_sql_zapis(strsql,0,0,data->window);
 }

sprintf(strsql,"select kassa,tp,suma from Kasord1 where \
datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d'  order by datd,tp asc",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_int kod_kas;
class iceb_u_double suma_kas;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  kassa=atoi(row[0]);
  tipz=atoi(row[1]);

  suma=atof(row[2]);  

  if(tipz == 2)
   suma*=-1;
   
  if((nomer=kod_kas.find(kassa)) < 0)
   kod_kas.plus(kassa,nomer);   
  suma_kas.plus(suma,nomer);
 }

for(int nom=0; nom < kod_kas.kolih(); nom++)
 {
  sprintf(strsql,"delete from Kasord where datd='%d-01-01' and nomd='000' and kassa=%d",data->god,kod_kas.ravno(nom));
  printf("%s\n",strsql);
  iceb_sql_zapis(strsql,1,0,data->window);

  sprintf(strsql,"delete from Kasord1 where datd='%d-01-01' and nomd='000' and kassa=%d",data->god,kod_kas.ravno(nom));
  printf("%s\n",strsql);
  iceb_sql_zapis(strsql,1,0,data->window);
  
  sprintf(strsql,"insert into Kasord (kassa,tp,datd,nomd,kodop,podt,god,ktoi,vrem) values(%d,1,'%d-01-01','000','%s',1,%d,%d,%ld)",
  kod_kas.ravno(nom),data->god,gettext("ОСТ"),data->god,iceb_getuid(data->window),time(NULL));
  printf("%s\n",strsql);

  iceb_sql_zapis(strsql,1,0,data->window);

  sprintf(strsql,"insert into Kasord1 (kassa,tp,datd,datp,nomd,kontr,suma,god,ktoi,vrem) values(%d,1,'%d-01-01','%d-01-01','000','00',%.2f,%d,%d,%ld)",
  kod_kas.ravno(nom),data->god,data->god,suma_kas.ravno(nom),data->god,iceb_getuid(data->window),time(NULL));
  printf("%s\n",strsql);

  iceb_sql_zapis(strsql,1,0,data->window);

  sprintf(strsql,"%s:%d %s:%.2f\n",gettext("Касса"),kod_kas.ravno(nom),gettext("Сальдо"),suma_kas.ravno(nom));
  iceb_printw(strsql,data->buffer,data->view);
 }
sprintf(strsql,"%s\n",gettext("Перенос сальдо закончен"));
iceb_printw(strsql,data->buffer,data->view);





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}









