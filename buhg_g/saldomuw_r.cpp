/*$Id:$*/
/*22.03.2017	10.03.2006	Белых А.И.	saldomuw_r.c
Расчёт отчёта 
*/
#include "buhg_g.h"

class saldomuw_r_data
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

  saldomuw_r_data()
   {
    voz=1;
    kon_ras=1;
    godn=godk=0;
   }
 };
gboolean   saldomuw_r_key_press(GtkWidget *widget,GdkEventKey *event,class saldomuw_r_data *data);
gint saldomuw_r1(class saldomuw_r_data *data);
void  saldomuw_r_v_knopka(GtkWidget *widget,class saldomuw_r_data *data);

extern short startgod;

extern SQL_baza bd;

int saldomuw_r(short godn,short godk,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class saldomuw_r_data data;

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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Перенос сальдо для подсистемы \"Материальный учёт\""));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(saldomuw_r_key_press),&data);

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

repl.plus(gettext("Перенос сальдо для подсистемы \"Материальный учёт\""));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(saldomuw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)saldomuw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldomuw_r_v_knopka(GtkWidget *widget,class saldomuw_r_data *data)
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

gboolean   saldomuw_r_key_press(GtkWidget *widget,GdkEventKey *event,class saldomuw_r_data *data)
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

/***********************************/
/**********************************/

gint saldomuw_r1(class saldomuw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;






short godk=data->godk;
short godn=data->godn;


if(matnastw() != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
sprintf(strsql,"Год %d => %d\n",godn,godk);
iceb_printw(strsql,data->buffer,data->view);

char ostmat[10];
memset(ostmat,'\0',sizeof(ostmat));
strcpy(ostmat,gettext("ОСТ"));
/*Проверяем есть ли операция стартовый остаток и если нет то записываем ее*/
sprintf(strsql,"select kod from Prihod where kod='%s'",ostmat);

if(iceb_sql_readkey(strsql,data->window) == 0)
 {
  sprintf(strsql,"insert into Prihod \
values ('%s','%s',%d,%ld,0,1)",ostmat,gettext("Стартовый остаток"),\
  iceb_getuid(data->window),vremn);   

  iceb_sql_zapis(strsql,0,0,data->window);
 }
int kolstr=0;
sprintf(strsql,"select sklad,nomk,kodm,cena,nds,mnds,ei from Kart \
order by sklad asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

float kolstr1=0.;
int sklz=0;
int skl=0;
int nkk=0;
int kodm=0;
double ccn=0.;
double nds=0.;
int mnds=0;
class iceb_u_str ei("");
float pnds=iceb_pnds(1,1,godk,data->window);

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  skl=atoi(row[0]);
  nkk=atoi(row[1]);  
  class ostatok ostm;
  ostkarw(1,1,godn,31,12,godn,skl,nkk,&ostm);

  sprintf(strsql,"Склад - %-5d Карточка - %-5d Остаток - %.2f\n",skl,nkk,ostm.ostg[3]);

  iceb_printw(strsql,data->buffer,data->view);

  if(ostm.ostg[3] > 0.0000001 )
   {
    if(sklz != skl)
     {
      //На всякий случай удаляем, возможно это повторный перенос остатков
      sprintf(strsql,"delete from Zkart where datd='%04d-01-01' and nomd='000' and sklad=%d",godk,skl);
      iceb_sql_zapis(strsql,0,0,data->window);

      sprintf(strsql,"delete from Dokummat1 where datd='%04d-01-01' and nomd='000' and sklad=%d",godk,skl);
      iceb_sql_zapis(strsql,0,0,data->window);

      sprintf(strsql,"delete from Dokummat where datd='%04d-01-01' and nomd='000' and sklad=%d",godk,skl);
      iceb_sql_zapis(strsql,0,0,data->window);
      

      if(zap_s_mudokw(1,1,1,godk,skl,"00","000","",ostmat,1,1,"",0,0,0,1,"00",pnds,data->window) != 0)
       return(FALSE);

      sklz=skl;
     }
    kodm=atoi(row[2]);
    ccn=atof(row[3]);
    nds=atof(row[4]);
    mnds=atoi(row[5]);
    ei.new_plus(row[6]);
    
    zapvdokw(1,1,godk,skl,nkk,kodm,"000",ostm.ostg[3],ccn,ei.ravno(),nds,mnds,0,1,0,"","",0,"",data->window);
    zapvkrw(1,1,godk,"000",skl,nkk,1,1,godk,1,ostm.ostg[3],ccn,data->window);

   }

 }


short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
if(data->godk <= gt)
 {
  sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='matnast.alx' and str like 'Стартовый год|%d%%'",
  data->godk,data->godn);

  if(iceb_sql_zapis(strsql,1,0,data->window) == 0)
   startgod=data->godk;
  
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
