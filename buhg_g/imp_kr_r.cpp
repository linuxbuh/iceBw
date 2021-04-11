/*$Id: imp_kr_r.c,v 1.20 2013/09/26 09:46:48 sasa Exp $*/
/*23.05.2016	09.10.2006	Белых А.И.	imp_kr_r.c
импорт начислений/удержаний из подсистемы Учёт командировочных расходов
*/
#include  <errno.h>
#include "buhg_g.h"
#include "imp_kr.h"

class imp_kr_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
    
  class imp_kr_rek *rk;
        
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  imp_kr_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   imp_kr_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_kr_r_data *data);
gint imp_kr_r1(class imp_kr_r_data *data);
void  imp_kr_r_v_knopka(GtkWidget *widget,class imp_kr_r_data *data);

extern SQL_baza bd;
extern double okrcn;

int imp_kr_r(class imp_kr_rek *rek,
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_spisok repl_s;
int gor=0;
int ver=0;
class imp_kr_r_data data;

data.rk=rek;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(imp_kr_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт документов"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(imp_kr_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)imp_kr_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_kr_r_v_knopka(GtkWidget *widget,class imp_kr_r_data *data)
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

gboolean   imp_kr_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_kr_r_data *data)
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

gint imp_kr_r1(class imp_kr_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_clock sss(data->window);


short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);

 }


int dlina_prist=strlen(ICEB_ZAR_PKTN);


SQL_str row,row1;
SQLCURSOR cur,cur1;
int kolstr=0;

sprintf(strsql,"select nomd,datd,kont from Ukrdok where \
datao >= '%04d-%02d-%02d' and datao <= '%04d-%02d-%02d'",
gn,mn,dn,gk,mk,dk);


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
//Смотрим есть ли счёт в начислении
class iceb_u_str shet_nah("");

sprintf(strsql,"select shet from Nash where kod=%d",data->rk->kod_nah_ud);
if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
 shet_nah.new_plus(row1[0]); 

class iceb_u_str tabnom("");
short dnr,mnr,gnr;
short dkr,mkr,gkr;
class iceb_u_str shetkar("");
int podrkar;
int kateg;
int sovm;
int zvan;
class iceb_u_str lgot("");
class iceb_u_str dolg("");
class ZARP     zp;



int nz=0;
int vernuli;
double suma=0;

class iceb_u_str shet("");

shet.new_plus(shet_nah.ravno());
int tabn;
int kolstr2=0;
float kolstr1=0.;

while(cur.read_cursor(&row) != 0)
 {

  iceb_pbar(data->bar,kolstr,++kolstr1);    
    
  if(iceb_u_strstrm(row[2],ICEB_ZAR_PKTN) != 1)
   continue;

  tabnom.new_plus(iceb_u_adrsimv(dlina_prist,row[2]));
  tabn=atol(tabnom.ravno());
  
  sprintf(strsql,"tabnom=%s\n",tabnom.ravno());
  iceb_printw(strsql,data->buffer,data->view);
  
  //Проверяем есть ли такой табельный номер  
  sprintf(strsql,"select datn,datk,shet,podr,kodg,kateg,\
sovm,zvan,lgoti,dolg from Kartb where tabn=%d",tabn);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    sprintf(strsql,"%s %d !\n",gettext("Не найден табельный номер"),tabn);
    iceb_menu_soob(strsql,data->window);
    continue;
   }

  iceb_u_rsdat(&dnr,&mnr,&gnr,row1[0],2);
  iceb_u_rsdat(&dkr,&mkr,&gkr,row1[1],2);
  shetkar.new_plus(row1[2]);

  podrkar=atoi(row1[3]);
  kateg=atoi(row1[5]);
  sovm=atoi(row1[6]);
  zvan=atoi(row1[7]);
  lgot.new_plus(row1[8]);
  dolg.new_plus(row1[9]);

  if(shet_nah.ravno()[0] == '\0')
   shet.new_plus(shetkar.ravno());
   

  sprintf(strsql,"select kodr,cena from Ukrdok1 where \
datd='%s' and nomd='%s'",row[1],row[0]);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  if(kolstr2 == 0)
   continue;

  while(cur1.read_cursor(&row1) != 0)
   {

    if(iceb_u_proverka(data->rk->kod_zat.ravno(),row1[0],0,0) != 0)
     continue;
    suma=atof(row1[1]);
    if(data->rk->prn == 2)
     suma*=-1;    
    nz=0;

    zp.tabnom=tabn;
    zp.prn=data->rk->prn;
    zp.knu=data->rk->kod_nah_ud;
    zp.dz=0;   //Если день равен нулю, то удаление старой записи перед вставкой новой не происходит
    zp.mz=data->rk->mz;
    zp.gz=data->rk->gz;
    zp.mesn=data->rk->mz; zp.godn=data->rk->gz;
    zp.nomz=0;
    zp.podr=podrkar;
    strcpy(zp.shet,shet.ravno());

povtor:;

    if((vernuli=zapzarpw(&zp,suma,data->rk->denz.ravno_atoi(),data->rk->mz,data->rk->gz,0,shet.ravno(),"",nz++,podrkar,"",data->window)) == 1)
      goto povtor;

    if(vernuli != 0)
     continue;
     
    zapzarn1w(tabn,data->rk->prn,data->rk->kod_nah_ud,0,0,0,shet.ravno(),data->window);

    zarsocw(data->rk->mz,data->rk->gz,tabn,data->window);
    zaravprw(tabn,data->rk->mz,data->rk->gz,NULL,data->window);

    //Проверяем есть ли запись карточки по этой дате и если нет делаем запись
    sprintf(strsql,"select god from Zarn where tabn=%d and god=%d and mes=%d",tabn,data->rk->gz,data->rk->mz);
    if(iceb_sql_readkey(strsql,data->window) == 0)  
      zapzarnw(data->rk->mz,data->rk->gz,podrkar,tabn,kateg,sovm,zvan,shetkar.ravno(),lgot.ravno(),dnr,mnr,gnr,dkr,mkr,gkr,0,dolg.ravno(),data->window);
    
   }

 }




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
