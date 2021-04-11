/*$Id: zarszkw_r.c,v 1.6 2013/09/26 09:47:01 sasa Exp $*/
/*23.05.2016	21.01.2012	Белых А.И.	zarszkw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "zarszkw.h"
#define		KOLSTRHAP 9 //Количество строк в шапке

class zarszkw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zarszkw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zarszkw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zarszkw_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarszkw_r_data *data);
gint zarszkw_r1(class zarszkw_r_data *data);
void  zarszkw_r_v_knopka(GtkWidget *widget,class zarszkw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int zarszkw_r(class zarszkw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zarszkw_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт свода начислений по месяцам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zarszkw_r_key_press),&data);

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

repl.plus(gettext("Расчёт свода начислений по месяцам"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zarszkw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zarszkw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zarszkw_r_v_knopka(GtkWidget *widget,class zarszkw_r_data *data)
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

gboolean   zarszkw_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarszkw_r_data *data)
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
/*Печать подчёркивающей строки*/
/******************************/
void zarszk_ps(int kolmes,FILE *ff)
{

fprintf(ff,"-------------------------------------");
//          1234567890123456789012345678901234567
for(int ii=0; ii <= kolmes; ii++)
 fprintf(ff,"-----------");
fprintf(ff,"\n");
}
/*************************/
/*Расчёт  суммы по колонке*/
/**************************/

double zarszk_sum_kol(int podr,class iceb_u_int *spis_podr,int nomer_kol,int kolih_mes,int kolih_tab_nom,class iceb_u_double *suma_m)
{
double itogo_kol=0.;
for(int ii=0; ii < kolih_tab_nom; ii++)
 {
  if(podr != spis_podr->ravno(ii))
   continue;
  itogo_kol+=suma_m->ravno(ii*kolih_mes+nomer_kol);  
 }
return(itogo_kol);
}
/**************************/
/*Печать горизонтальной строки с итогами по колонкам*/
/****************************************************/
void zarszk_itogo_gor(int podr,class iceb_u_int *spis_podr,int kolih_mes,int kolih_tab_nom,class iceb_u_double *suma_m,FILE *ff)
{
fprintf(ff,"%36s","");
double itogo_gor=0.;
double suma=0.;
for(int ii=0; ii < kolih_mes; ii++)
 {
  suma=zarszk_sum_kol(podr,spis_podr,ii,kolih_mes,kolih_tab_nom,suma_m);
  itogo_gor+=suma;
  fprintf(ff," %10.2f",suma);
 }
fprintf(ff," %10.2f\n",itogo_gor);
}

/******************************************/
/******************************************/

gint zarszkw_r1(class zarszkw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
SQLCURSOR cur,cur1;



short mn=0,gn=0;
short mk=0,gk=0;
if(iceb_u_rsdat1(&mn,&gn,data->rk->datan.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Не верно введена дата начала !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(data->rk->datak.ravno()[0] == '\0')
 {
  mk=mn; gk=gn;
 }
else
 if(iceb_u_rsdat1(&mk,&gk,data->rk->datak.ravno()) != 0)
  {
   iceb_menu_soob(gettext("Не верно введена дата конца !"),data->window);
   sss.clear_data();
   gtk_widget_destroy(data->window);
   return(FALSE);
  }

sprintf(strsql,"%d.%d%s => %d.%d%s\n",
mn,gn,gettext("г."),
mk,gk,gettext("г."));

iceb_printw(strsql,data->buffer,data->view);


int kolstr=0;
//Создаём список табельных номеров
sprintf(strsql,"select distinct tabn from Zarp where datz >= '%04d-%02d-01' \
and datz <= '%04d-%02d-31' and suma <> 0.",
gn,mn,gk,mk);

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


class iceb_u_int spis_tab_nom;
float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
 
  spis_tab_nom.plus(atoi(row[0]));
 }


sprintf(strsql,"select distinct kont from Ukrdok where datd >= '%04d-%02d-01' \
and datd <= '%04d-%02d-31'",gn,mn,gk,mk);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int dlina_prist=iceb_u_strlen(ICEB_ZAR_PKTN);
kolstr1=0;
int tabnom=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_srav_r(ICEB_ZAR_PKTN,row[0],1) != 0)
   continue;


  tabnom=atoi(iceb_u_adrsimv(dlina_prist,row[0]));

  if(iceb_u_proverka(data->rk->tabnom.ravno(),strsql,0,0) != 0)
   continue;
  if(spis_tab_nom.find(tabnom) < 0)
   spis_tab_nom.plus(tabnom);
 }
int kolih_tab_nom=spis_tab_nom.kolih();
int kolih_mes=iceb_u_period(1,mn,gn,1,mk,gk,1);

class iceb_u_double suma_zar;
class iceb_u_double suma_kom;

suma_zar.make_class(kolih_mes*kolih_tab_nom);
suma_kom.make_class(kolih_mes*kolih_tab_nom);



//читаем зарплату
sprintf(strsql,"select datz,tabn,suma,knah from Zarp where datz >= '%04d-%02d-01' \
and datz <= '%04d-%02d-31' and prn='1' and suma <> 0.",
gn,mn,gk,mk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }


kolstr1=0;
int nomer_tabnom=0;
int nomer_mes=0;
double suma=0.;
short d,m,g;
short d1=1,m1=0,g1=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kod_nah.ravno(),row[3],0,0) != 0)
   continue;  
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  tabnom=atoi(row[1]);
  suma=atof(row[2]);
  nomer_mes=0;  
  d1=1;
  m1=mn;
  g1=gn;
  while(iceb_u_sravmydat(1,m,g,1,m1,g1) > 0)
   {
    nomer_mes++;
    iceb_u_dpm(&d1,&m1,&g1,3);
   } 
  nomer_tabnom=spis_tab_nom.find(tabnom);  
  suma_zar.plus(suma,nomer_tabnom*kolih_mes+nomer_mes);
    
 }


//читаем командировочные расходы

sprintf(strsql,"select nomd,datd,kont from Ukrdok where datd >= '%04d-%02d-01' \
and datd <= '%04d-%02d-31'",gn,mn,gk,mk);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
kolstr1=0;
int kolstr2=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_srav_r(ICEB_ZAR_PKTN,row[2],1) != 0)
   continue;
  //Читаем содержимое документа
  sprintf(strsql,"select kodr,cena from Ukrdok1 where datd='%s' and nomd='%s'",row[1],row[0]);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  suma=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    if(iceb_u_proverka(data->rk->kod_kr.ravno(),row1[0],0,0) != 0)
     continue;
    suma+=atof(row1[1]);
   }

  tabnom=atoi(iceb_u_adrsimv(dlina_prist,row[2]));
//  printw("row=%s strsql=%s tabnom=%d\n",row[2],strsql,tabnom);
//  OSTANOV();
  iceb_u_rsdat(&d,&m,&g,row[1],2);
  nomer_mes=0;  
  d1=1;
  m1=mn;
  g1=gn;
  while(iceb_u_sravmydat(1,m,g,1,m1,g1) > 0)
   {
    nomer_mes++;
    iceb_u_dpm(&d1,&m1,&g1,3);
   } 
  nomer_tabnom=spis_tab_nom.find(tabnom);  
  suma_kom.plus(suma,nomer_tabnom*kolih_mes+nomer_mes);
  
 }

//Узнаём в каких подразделениях числятся работники
class iceb_u_int spis_podr;
spis_podr.make_class(kolih_tab_nom);
for(int ii=0; ii < kolih_tab_nom; ii++)
 {
  sprintf(strsql,"select podr from Kartb where tabn=%d",spis_tab_nom.ravno(ii));
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   spis_podr.plus(atoi(row[0]),ii);
 }

//распечатка по подразделениям
sprintf(strsql,"select kod,naik from Podr order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

FILE *ff;
char imaf[64];
sprintf(imaf,"zarkom%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  sss.clear_data();
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

short dkm=1;
iceb_u_dpm(&dkm,&mk,&gk,5);

iceb_zagolov(gettext("Расчёт общей суммы доходов"),1,mn,gn,dkm,mk,gk,ff,data->window);
if(data->rk->podr.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код подразделения"),data->rk->podr.ravno());
if(data->rk->kod_kr.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код командировочных расходов"),data->rk->kod_kr.ravno());
if(data->rk->kod_nah.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код начисления"),data->rk->kod_nah.ravno());
if(data->rk->tabnom.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),data->rk->tabnom.ravno());
 
int podrr=0;
class iceb_u_str fio("");
double itogo_gor=0.;

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rk->podr.ravno(),row[0],0,0) != 0)
   continue;
    
  podrr=atoi(row[0]);  
  if(spis_podr.find(podrr) < 0) //Подразделения по которым нет начислений пропускаем
   continue;
  /*двойная ширина символа до конца строки*/
  fprintf(ff,"\x0E%s %s\n",row[0],row[1]);

  zarszk_ps(kolih_mes,ff);
  //распечатываем строку с месяцами  
  
  fprintf(ff,"Таб.н|       Фамилия                |");
  d=1;
  m=mn;
  g=gn;
  for(int ii=0; ii < kolih_mes; ii++)
   {
    fprintf(ff," %02d.%04d  |",m,g);
    iceb_u_dpm(&d,&m,&g,3);  
   }
  fprintf(ff,"%*s|\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));
  zarszk_ps(kolih_mes,ff);
  
  for(int ii=0; ii < kolih_tab_nom; ii++)
   {
    if(podrr != spis_podr.ravno(ii))
     continue;


    //читаем фамилию
    sprintf(strsql,"select fio from Kartb where tabn=%d",spis_tab_nom.ravno(ii));
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     fio.new_plus(row1[0]);
    else
     fio.new_plus("");

    fprintf(ff,"%5d %-*.*s",spis_tab_nom.ravno(ii),iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno());
    itogo_gor=0.;
    for(int mes=0; mes < kolih_mes; mes++)
     {
      suma=suma_zar.ravno(ii*kolih_mes+mes);
      fprintf(ff," %10.2f",suma);
      itogo_gor+=suma;
     }
    fprintf(ff," %10.2f\n",itogo_gor);

    fprintf(ff,"%5s %-30.30s","","");

    itogo_gor=0.;
    for(int mes=0; mes < kolih_mes; mes++)
     {
      suma=suma_kom.ravno(ii*kolih_mes+mes);
      fprintf(ff," %10.2f",suma);
      itogo_gor+=suma;
     }
    fprintf(ff," %10.2f\n",itogo_gor);
   }
  zarszk_ps(kolih_mes,ff);

  zarszk_itogo_gor(podrr,&spis_podr,kolih_mes,kolih_tab_nom,&suma_zar,ff);
  zarszk_itogo_gor(podrr,&spis_podr,kolih_mes,kolih_tab_nom,&suma_kom,ff);

 }

iceb_podpis(ff,data->window);
fclose(ff);

data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Расчёт общей суммы доходов"));


for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),0,data->window);




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
