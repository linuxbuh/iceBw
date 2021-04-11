/*$Id: xdkgur_r.c,v 1.16 2013/09/26 09:46:59 sasa Exp $*/
/*23.05.2016	10.03.2006	Белых А.И.	xdkgur_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "xdkgur.h"

class xdkgur_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class xdkgur_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  xdkgur_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   xdkgur_r_key_press(GtkWidget *widget,GdkEventKey *event,class xdkgur_r_data *data);
gint xdkgur_r1(class xdkgur_r_data *data);
void  xdkgur_r_v_knopka(GtkWidget *widget,class xdkgur_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;
extern double   okrg1;
extern double	okrcn;

int xdkgur_r(class xdkgur_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class xdkgur_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка списка документов."));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(xdkgur_r_key_press),&data);

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
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне."));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(xdkgur_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)xdkgur_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  xdkgur_r_v_knopka(GtkWidget *widget,class xdkgur_r_data *data)
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

gboolean   xdkgur_r_key_press(GtkWidget *widget,GdkEventKey *event,class xdkgur_r_data *data)
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

/*************************************************/
void xdkgur_r_hap1(int nomstr,int *kolstr,FILE *ff,GtkWidget *wpredok)
{
*kolstr=4;
fprintf(ff,"%60s %s N%d\n","",gettext("Страница"),nomstr);
fprintf(ff,"\
--------------------------------------------------------------------------------\n");

fprintf(ff,gettext("Н.док.|К.о.|   Дата    |      Сумма     |  Наименование контрагента / Коментарий\n"));

fprintf(ff,"\
--------------------------------------------------------------------------------\n");
}
/******************************************/
/******************************************/

gint xdkgur_r1(class xdkgur_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(strsql,data->buffer,data->view);



SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr=0;

sprintf(strsql,"select datd,nomd,suma,uslb,polu,oper from %s where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by datd asc",
data->rk->tablica,gn,mn,dn,gk,mk,dk);
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
char imaf[30];
FILE *ff;

sprintf(imaf,"gp%d.lst",getpid());
data->rk->imaf.plus(imaf);

if(iceb_u_SRAV(data->rk->tablica,"Pltp",0) == 0)
  data->rk->naimf.plus(gettext("Журнал регистрации платёжных поручений."));
if(iceb_u_SRAV(data->rk->tablica,"Pltt",0) == 0)
  data->rk->naimf.plus(gettext("Журнал регистрации платёжных требований."));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff);


if(iceb_u_SRAV(data->rk->tablica,"Pltp",0) == 0)
  iceb_zagolov(gettext("Журнал регистрации платёжных поручений."),dn,mn,gn,dk,mk,gk,ff,data->window);
if(iceb_u_SRAV(data->rk->tablica,"Pltt",0) == 0)
  iceb_zagolov(gettext("Журнал регистрации платёжных требований."),dn,mn,gn,dk,mk,gk,ff,data->window);

if(data->rk->kontr.getdlinna() > 1)
  fprintf(ff,"\n%s %s\n",gettext("Код контрагента"),data->rk->kontr.ravno());

if(data->rk->kodop.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());

int kol=0;
xdkgur_r_hap1(1,&kol,ff,data->window);
 


double suma1=0.,suma=0.;
float kolstr1=0.;
int kolstr2=0;
SQL_str row;
SQL_str row1;
class iceb_u_str kodor1("");
class iceb_u_str naior1("");
short d,m,g;
double sum=0.,usl=0.;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  if(iceb_u_proverka(data->rk->kodop.ravno(),row[5],0,0) != 0)
    continue;

  iceb_u_pole(row[4],&kodor1,1,'#');
  if(iceb_u_proverka(data->rk->kontr.ravno(),kodor1.ravno(),0,0) != 0)
    continue;
    
  iceb_u_pole(row[4],&naior1,2,'#');

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  sum=atof(row[2]);
  usl=atof(row[3]);
  suma+=sum;
  suma1+=usl;
  
  fprintf(ff,"%-*s %-*s %02d.%02d.%d %10.2f/%4.2f   %s\n",
  iceb_u_kolbait(6,row[1]),row[1],
  iceb_u_kolbait(4,row[5]),row[5],
  d,m,g,sum,usl,naior1.ravno());

  sprintf(strsql,"select zapis from %sz where datd='%04d-%02d-%02d' and \
  nomd='%s' and mz=0 order by nz asc",data->rk->tablica,g,m,d,row[1]);
  /*printw("\n%s",strsql);*/
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 != 0)
  while(cur1.read_cursor(&row1) != 0)
   {
    if(row1[0][0] != '\0')
      fprintf(ff,"%37s %-*.*s\n"," ",
      iceb_u_kolbait(45,row1[0]),iceb_u_kolbait(45,row1[0]),row1[0]);

    if(iceb_u_strlen(row1[0]) > 45)
      fprintf(ff,"%37s %-*.*s\n"," ",
      iceb_u_kolbait(45,iceb_u_adrsimv(45,row1[0])),
      iceb_u_kolbait(45,iceb_u_adrsimv(45,row1[0])),
      iceb_u_adrsimv(45,row1[0]));
    
   }

 }

fprintf(ff,"ICEB_LST_END\n\
---------------------------------------------------------------------------\n\
%*s: %10.2f/%4.2f\n",iceb_u_kolbait(21,gettext("Итого")),gettext("Итого"),suma,suma1);

iceb_podpis(ff,data->window);

fclose(ff);

kol=iceb_ustpeh(data->rk->imaf.ravno(0),0,data->window);
iceb_rnl(data->rk->imaf.ravno(0),kol,NULL,&xdkgur_r_hap1,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
