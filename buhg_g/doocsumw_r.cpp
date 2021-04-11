/*$Id: doocsumw_r.c,v 1.24 2014/02/28 05:20:58 sasa Exp $*/
/*23.05.2016	06.05.2005	Белых А.И. 	doocsumw_r.c
Расчёт сумм выполненых дооценок
*/
#include <errno.h>
#include "buhg_g.h"
#include "doocsumw.h"

class doocsumw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;


  class doocsumw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  doocsumw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   doocsumw_r_key_press(GtkWidget *widget,GdkEventKey *event,class doocsumw_r_data *data);
gint doocsumw_r1(class doocsumw_r_data *data);
void  doocsumw_r_v_knopka(GtkWidget *widget,class doocsumw_r_data *data);

extern SQL_baza bd;

int doocsumw_r(class doocsumw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class doocsumw_r_data data;
int gor=0;
int ver=0;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать суммы выполненных автоматических дооценок"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(doocsumw_r_key_press),&data);

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

repl.plus(gettext("Распечатать суммы выполненных автоматических дооценок"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(doocsumw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)doocsumw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  doocsumw_r_v_knopka(GtkWidget *widget,class doocsumw_r_data *data)
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

gboolean   doocsumw_r_key_press(GtkWidget *widget,GdkEventKey *event,class doocsumw_r_data *data)
{
// printf("doocsumw_r_key_press\n");
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

gint doocsumw_r1(class doocsumw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."));
iceb_printw(strsql,data->buffer,data->view);

class iceb_u_str imafn("matnast.alx");


sprintf(strsql,"select datd,sklad,nomd,nomon,pn,kodop from Dokummat where \
datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d'  and tip=1",gn,mn,dn,gk,mk,dk);

SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr;
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


int vt=0;
memset(strsql,'\0',sizeof(strsql));
iceb_poldan("Код группы возвратная тара",strsql,imafn.ravno(),data->window);
vt=atoi(strsql);
char imaf[64];
FILE *ff;

sprintf(imaf,"dooc%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт сумм выполненых дооценок"));
iceb_u_startfil(ff);
iceb_zagolov(gettext("Расчёт сумм выполненых дооценок"),dn,mn,gn,dk,mk,gk,ff,data->window);

fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());

if(data->rk->sklad.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());

fprintf(ff,"\
-------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Склад|   Дата   |Номер док.|Номер док.|        Сумма        |  Сумма   |Основание\n"));
fprintf(ff,gettext("\
     |          | приход   |  расход  |  приход  |  расход  | дооценки |\n"));
fprintf(ff,"\
-------------------------------------------------------------------------------------\n");
double	sumi[3];
memset(&sumi,'\0',sizeof(sumi));
SQL_str row;
SQL_str row1;
float kolstr1=0.;
short d,m,g;
int mnds;
class iceb_u_str osnov("");
double suma=0.,sumkor=0.,sumabn=0.;
double nds=0.,sum=0.,sum1=0.;
float pnds=0.;
double kolih=0.;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s %s\n",row[0],row[1],row[2]);
  iceb_printw(strsql,data->buffer,data->view);
  iceb_pbar(data->bar,kolstr,++kolstr1);

  if(iceb_u_proverka(data->rk->kodop.ravno(),row[5],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[1],0,0) != 0)
    continue;
    
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  pnds=atof(row[4]);
  /*Узнаем НДС документа*/
  mnds=0;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=11",g,row[2],row[1]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    mnds=atoi(row1[0]);
   }  
  //Узнаем основание документа
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%s \
and nomd='%s' and nomerz=3",g,row[1],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    osnov.new_plus(row1[0]);
  else
    osnov.new_plus("");    
  sumzap1w(d,m,g,row[2],atoi(row[1]),&suma,&sumabn,mnds,vt,&sumkor,1,&kolih,data->window);

  if(mnds == 0)
    nds=(suma+sumkor)*pnds/100.;
  else
   nds=0.;

  sum=suma+sumkor+nds+sumabn;
  
  sumzap1w(d,m,g,row[3],atoi(row[1]),&suma,&sumabn,mnds,vt,&sumkor,2,&kolih,data->window);

  if(mnds == 0)
    nds=(suma+sumkor)*pnds/100.;
  else
   nds=0.;

  sum1=suma+sumkor+nds+sumabn;
  sumi[0]+=sum;
  sumi[1]+=sum1;
  sumi[2]+=sum-sum1;
  fprintf(ff,"%-5s %02d.%02d.%d %-10s %-*s %10.2f %10.2f %10.2f %s\n",
  row[1],
  d,m,g,
  row[2],
  iceb_u_kolbait(10,row[3]),row[3],
  sum,sum1,sum-sum1,osnov.ravno());
 }
fprintf(ff,"\
-------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f %10.2f\n",
iceb_u_kolbait(38,gettext("Итого")),gettext("Итого"),sumi[0],sumi[1],sumi[2]);

iceb_podpis(ff,data->window);
fclose(ff);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
