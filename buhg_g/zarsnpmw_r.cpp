/*$Id: zarsnpmw_r.c,v 1.6 2013/09/26 09:47:01 sasa Exp $*/
/*23.05.2016	20.01.2012	Белых А.И.	zarsnpmw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "zarsnpmw.h"
#define		KOLSTRHAP 9 //Количество строк в шапке

class zarsnpmw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zarsnpmw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zarsnpmw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zarsnpmw_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarsnpmw_r_data *data);
gint zarsnpmw_r1(class zarsnpmw_r_data *data);
void  zarsnpmw_r_v_knopka(GtkWidget *widget,class zarsnpmw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int zarsnpmw_r(class zarsnpmw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zarsnpmw_r_data data;
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zarsnpmw_r_key_press),&data);

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zarsnpmw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zarsnpmw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zarsnpmw_r_v_knopka(GtkWidget *widget,class zarsnpmw_r_data *data)
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

gboolean   zarsnpmw_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarsnpmw_r_data *data)
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

/**************************************/
/*Печать заголовка*/
/*************************************/
void zarsnpm_pz(short mn,short gn,short mk,short gk,FILE *ff)
{

int kolih_mes=iceb_u_period(1,mn,gn,1,mk,gk,1);
class iceb_u_str liniq("--------------------------------------");
for(int nom=0; nom <= kolih_mes; nom++)
 liniq.plus("-----------");

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,gettext("  Т/н |          Фамилия             |"));
short d=1,m=mn,g=gn;
for(int nom=0; nom < kolih_mes; nom++)
 {
  fprintf(ff," %02d.%04d  |",m,g);
  iceb_u_dpm(&d,&m,&g,3);
 }
fprintf(ff,"%-*.*s|\n",
iceb_u_kolbait(10,gettext("Итого")),
iceb_u_kolbait(10,gettext("Итого")),
gettext("Итого"));

fprintf(ff,"%s\n",liniq.ravno());

/*************
123456|123456789012345678901234567890|
*************/



}
/********************************************/
/*распечатка строки*/
/************************************/
void zarsnpm_rs(int kolih_mes,class iceb_u_double *suma,class iceb_u_double *suma_plus,FILE *ff)
{
for(int nom=0; nom < kolih_mes; nom++)
 {
  fprintf(ff,"%10.2f|",suma->ravno(nom));
  if(suma_plus != NULL)
    suma_plus->plus(suma->ravno(nom),nom);
 }
fprintf(ff,"%10.2f|\n",suma->suma());
}

/******************************************/
/******************************************/

gint zarsnpmw_r1(class zarsnpmw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1,row2;
SQLCURSOR cur,cur1,cur2;



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

int kolih_mes=iceb_u_period(1,mn,gn,1,mk,gk,1);

class iceb_u_double suma_mes;
suma_mes.make_class(kolih_mes);

class iceb_u_double suma_mes_itogo_podr;
suma_mes_itogo_podr.make_class(kolih_mes);

class iceb_u_double suma_mes_itogo_all;
suma_mes_itogo_all.make_class(kolih_mes);

class iceb_u_spisok mesac;
short d,m,g;
d=1;
m=mn;
g=gn;
for(int nom=0; nom < kolih_mes; nom++)
 {
  sprintf(strsql,"%d.%d",m,g);
  mesac.plus(strsql);
  iceb_u_dpm(&d,&m,&g,3);  
 }

int kolstr=0;

sprintf(strsql,"select tabn,fio,podr,kateg from Kartb order by podr asc,kateg asc,fio asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной карточки !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
FILE *ff;
char imaf[64];
sprintf(imaf,"spm%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_zagolov(gettext("Свод начислений по месяцам"),1,mn,gn,1,mk,gk,ff,data->window);

if(data->rk->tabnom.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),data->rk->tabnom.ravno());

if(data->rk->podr.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код подразделения"),data->rk->podr.ravno());

if(data->rk->kod_kateg.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код категории"),data->rk->kod_kateg.ravno());

if(data->rk->kod_nah.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код начисления"),data->rk->kod_nah.ravno());
 
class iceb_u_str liniq("--------------------------------------");
for(int nom=0; nom <= kolih_mes; nom++)
 liniq.plus("-----------");

float kolstr1=0;
int kolstr2=0;
double suma=0.;
int podr=0,podr1=0;
int kateg=0,kateg1=0;
int nomer=0;
class iceb_u_str naim_podr("");
class iceb_u_str naim_kateg("");
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->podr.ravno(),row[2],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->kod_kateg.ravno(),row[3],0,0) != 0)
   continue;

  sprintf(strsql,"select datz,knah,suma from Zarp where tabn=%s and datz >= '%04d-%02d-1' and datz <= '%04d-%02d-31' and prn='1' and suma <> 0.",row[0],gn,mn,gk,mk);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(1);
   }
  if(kolstr2 == 0)
   continue;  

  podr=atoi(row[2]);
  kateg=atoi(row[3]);


  if(podr != podr1)
   {
    if(podr1 != 0)
     {
      fprintf(ff,"%s\n",liniq.ravno());
      fprintf(ff,"%*s|",iceb_u_kolbait(37,gettext("Итого по подразделению")),gettext("Итого по подразделению"));
      zarsnpm_rs(kolih_mes,&suma_mes_itogo_podr,&suma_mes_itogo_all,ff); /*распечатываем строку*/
      suma_mes_itogo_podr.clear_class();
     }
    naim_podr.new_plus("");
    sprintf(strsql,"select naik from Podr where kod=%s",row[2]);
    if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) == 1)
     naim_podr.new_plus(row2[0]);

    fprintf(ff,"\n%s:%s %s\n",gettext("Подразделение"),row[2],naim_podr.ravno());

    zarsnpm_pz(mn,gn,mk,gk,ff); /*шапка*/

    naim_kateg.new_plus("");
    sprintf(strsql,"select naik from Kateg where kod=%s",row[3]);
    if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) == 1)
     naim_kateg.new_plus(row2[0]);    
    fprintf(ff,"%s:%s %s\n",gettext("Категория"),row[3],naim_kateg.ravno());

    podr1=podr;
    kateg1=kateg;
   }

  if(kateg1 != kateg)
   {
    naim_kateg.new_plus("");
    sprintf(strsql,"select naik from Kateg where kod=%s",row[3]);
    if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) == 1)
     naim_kateg.new_plus(row2[0]);    
    fprintf(ff,"%s:%s %s\n",gettext("Категория"),row[3],naim_kateg.ravno());
    kateg1=kateg;
   }




  suma_mes.clear_class();
  
  while(cur1.read_cursor(&row1) != 0)
   {
    if(iceb_u_proverka(data->rk->kod_nah.ravno(),row1[1],0,0) != 0)
     continue;

    suma=atof(row1[2]);

    iceb_u_rsdat(&d,&m,&g,row1[0],2);

    sprintf(strsql,"%d.%d",m,g);

    if((nomer=mesac.find(strsql)) < 0)
     {
      sprintf(strsql,"%s %d.%d",gettext("Не найдена дата"),m,g);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    suma_mes.plus(suma,nomer);
   }
  fprintf(ff,"%6s|%-*.*s|",
  row[0],
  iceb_u_kolbait(30,row[1]),
  iceb_u_kolbait(30,row[1]),
  row[1]);  

  zarsnpm_rs(kolih_mes,&suma_mes,&suma_mes_itogo_podr,ff); /*распечатываем строку*/

     
 }
fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*s|",iceb_u_kolbait(37,gettext("Итого по подразделению")),gettext("Итого по подразделению"));
zarsnpm_rs(kolih_mes,&suma_mes_itogo_podr,&suma_mes_itogo_all,ff); /*распечатываем строку*/

fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*s|",iceb_u_kolbait(37,gettext("Общий итог")),gettext("Общий итог"));
zarsnpm_rs(kolih_mes,&suma_mes_itogo_all,NULL,ff); /*распечатываем строку*/

iceb_podpis(ff,data->window);

fclose(ff);




data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Cвод начислений по месяцам"));


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
