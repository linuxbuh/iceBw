/*$Id: xdkbanksw_r.c,v 1.4 2013/09/26 09:46:59 sasa Exp $*/
/*24.05.2016	18.07.2013	Белых А.И.	xdkbanksw_r.c
Расчёт отчёта 
*/
#include "buhg_g.h"
#include "xdkbanksw.h"
#define		KOLSTRHAP 9 //Количество строк в шапке

class xdkbanksw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class xdkbanksw_rr *rk;
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  xdkbanksw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   xdkbanksw_r_key_press(GtkWidget *widget,GdkEventKey *event,class xdkbanksw_r_data *data);
gint xdkbanksw_r1(class xdkbanksw_r_data *data);
void  xdkbanksw_r_v_knopka(GtkWidget *widget,class xdkbanksw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int xdkbanksw_r(class xdkbanksw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class xdkbanksw_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.rk=datark;
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка списка документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(xdkbanksw_r_key_press),&data);

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

repl.plus(gettext("Распечатка списка документов"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(xdkbanksw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)xdkbanksw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  xdkbanksw_r_v_knopka(GtkWidget *widget,class xdkbanksw_r_data *data)
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

gboolean   xdkbanksw_r_key_press(GtkWidget *widget,GdkEventKey *event,class xdkbanksw_r_data *data)
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

/****************************/
/*определяем код банка для заданного кода контрагента*/
/****************************/
int xdkbanks_kk(const char *kontr,GtkWidget *wpredok)
{
class iceb_u_str kod_kontr("");
class SQLCURSOR cur_alx;
SQL_str row_alx;
int kolstr=0;
char strsql[512];
int kod_banka=0;
sprintf(strsql,"select str from Alx where fil='zarbanks.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(-1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s zarbanks.alx",gettext("Не найдены настройки")); 
  iceb_menu_soob(strsql,wpredok);
  return(-1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],&kod_kontr,3,'|') != 0)
   continue;
  if(iceb_u_SRAV(kod_kontr.ravno(),kontr,0) != 0)
   continue;
  int voz=0;
  if((voz=iceb_u_polen(row_alx[0],&kod_banka,4,'|')) == 0)
   return(kod_banka);
   
 }

return(-1);
}

/******************************************/
/******************************************/

gint xdkbanksw_r1(class xdkbanksw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;
int kolstr=0;
short dk,mk,gk;
short dr,mr,gr;

if(iceb_u_rsdat(&dk,&mk,&gk,data->rk->datas.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели дату сальдо!"),data->window);
   sss.clear_data();
   gtk_widget_destroy(data->window);
   return(FALSE);

 }

if(iceb_u_rsdat(&dr,&mr,&gr,data->rk->datap.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели дату проводки!"),data->window);
   sss.clear_data();
   gtk_widget_destroy(data->window);
   return(FALSE);

 }

/*проверяем список банков в таблице. Если их больше одного то нужно проверять есть ли карточка 
этого банка у работкика или нет*/
sprintf(strsql,"select kod from Zarsb");
int kolbank=iceb_sql_readkey(strsql,data->window);
int kod_banka=0;

if(kolbank > 1)
 if((kod_banka=xdkbanks_kk(data->rk->kodor.ravno(),data->window)) < 0)
  {
   sprintf(strsql,"%s %s %s zarbanks.alx!",gettext("Не найден код банка для кода контрагента"),data->rk->kodor.ravno(),gettext("в настройках"));
   iceb_menu_soob(strsql,data->window);
   sss.clear_data();
   gtk_widget_destroy(data->window);
   return(FALSE);
  }


//Составляем список всех контрагентов
sprintf(strsql,"select ns,kodkon from Skontr where ns='%s'",data->rk->shet.ravno());

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
class iceb_u_spisok KONT;

int kolkontr=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->shet.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kodkontr.ravno(),row[1],0,0) != 0)
    continue;
  if(KONT.find_r(row[1]) < 0)
   {
    kolkontr++;
    KONT.plus(row[1]);
   }
 }

double deb[kolkontr];
double kre[kolkontr];
memset(&deb,'\0',sizeof(deb));
memset(&kre,'\0',sizeof(kre));

extern short	startgodb; /*Стартовый год*/
short       gods=startgodb;
if(gods == 0)
 gods=gk;

//Читаем сальдо
sprintf(strsql,"select ns,kodkon,deb,kre from Saldo where kkk=1 and gs=%d and ns='%s'",gods,data->rk->shet.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int nomer=0;
kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->shet.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kodkontr.ravno(),row[1],0,0) != 0)
    continue;
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);

  nomer=KONT.find_r(row[1]);
  deb[nomer]+=atof(row[2]);
  kre[nomer]+=atof(row[3]);
 }

//Читаем все проводки
sprintf(strsql,"select sh,kodkon,deb,kre from Prov where datp >= '%d-%d-%d' \
and datp <= '%d-%d-%d' and kodkon <> '' and sh='%s'",gods,1,1,gk,mk,dk,data->rk->shet.ravno());

//printw("%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->shet.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kodkontr.ravno(),row[1],0,0) != 0)
    continue;

  nomer=KONT.find_r(row[1]);
  deb[nomer]+=atof(row[2]);
  kre[nomer]+=atof(row[3]);
 }



int dlpris=iceb_u_strlen(ICEB_ZAR_PKTN);

class iceb_u_str bankshet("");
class iceb_u_str fio("");
class iceb_u_str tabnom("");
class iceb_u_str inn("");
double sum=0.;
double debp=0.;
double krep=0.;
class iceb_u_str kontr1("");
class iceb_u_str kontr2("");
class iceb_u_str koment("");
class iceb_u_str podsyst("");

int tipz=0;
if(iceb_u_SRAV(data->rk->tabl.ravno(),"Pltp",0) == 0)
 {
  podsyst.new_plus(ICEB_MP_PPOR);
  tipz=2;
 }
if(iceb_u_SRAV(data->rk->tabl.ravno(),"Pltt",0) == 0)
 {
  podsyst.new_plus(ICEB_MP_PTRE);
  tipz=1;
 }

for(int snom=0 ; snom < kolkontr; snom++)
 {
  
  if(pvbanks_kprovw(&KONT,snom,ICEB_ZAR_PKTN,data->rk->metka,&sum,&tabnom,deb,kre,dlpris,&fio,&bankshet,&inn,data->rk->kodgrkontr.ravno(),kod_banka,data->window) != 0)
   continue;

  debp=0.;
  krep=0.;
  kontr1.new_plus(KONT.ravno(snom));
  kontr2.new_plus(KONT.ravno(snom));

  if(data->rk->metka == 0) /*Дебет*/
   krep=sum;
  if(data->rk->metka == 1) /*кредит*/
   debp=sum;

  koment.new_plus(gettext("Перечислено"));
  koment.plus(" ");
  koment.plus(fio.ravno());   
  iceb_zapprov(0,gr,mr,dr,data->rk->shet.ravno(),data->rk->shetk.ravno(),kontr1.ravno(),kontr2.ravno(),podsyst.ravno(),
  data->rk->nomdok.ravno(),data->rk->kodop.ravno(),debp,krep,koment.ravno_filtr(),2,0,time(NULL),data->rk->gd,data->rk->md,data->rk->dd,tipz,0,data->window);

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
